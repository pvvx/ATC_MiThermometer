#!/usr/bin/env python3
"""Update pvvx ATC_MiThermometer custom firmware over BLE from Linux (e.g. a Raspberry Pi) or macOS.

Port of the Telink OTA of https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html.
The image is chosen by the hardware id the device reports, as the web flasher does.
Needs `pip install bleak` (and `cryptography` for --stock). Stop other BLE scanners first.

  ./ble_ota.py --check A4:C1:38:0B:F8:E1 [...]   # print model and installed version
  ./ble_ota.py A4:C1:38:0B:F8:E1 [...]           # flash the latest release if older
  ./ble_ota.py -df sensors.ini                   # MACs from an ini file ([AA:BB:..] sections)
  ./ble_ota.py --file ATC_v59.bin --force MAC    # flash a given image
  ./ble_ota.py --stock --file ATC_v59.bin MAC    # first flash from the original Xiaomi firmware

--stock does what "Do Activation" does in the web flasher (Mi standard auth: register, then
login), then flashes in the same connection. On macOS give the CoreBluetooth UUID instead of the MAC.
"""
import argparse
import asyncio
import configparser
import hashlib
import hmac
import json
import os
import random
import string
import struct
import sys
import time
import urllib.request

RAW = "https://raw.githubusercontent.com/pvvx/ATC_MiThermometer/master/"
OTA_CHAR = "00010203-0405-0607-0809-0a0b0c0d2b12"
CFG_CHAR = "00001f1f-0000-1000-8000-00805f9b34fb"
MAX_BLE_OTA = 0x20000  # bigger images need the web flasher's Ext.OTA
OTA_ERRORS = ["Success", "Lost one or more packets", "CRC error in data", "Writing data to flash",
              "Lost last one or more packets", "Timeout", "Firmware CRC check"]


def crc16_modbus(data):
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return crc


def ota_packets(fw):
    """Telink OTA: 16-byte blocks as [index LE16][data][crc16 LE16], then the end marker."""
    fw += b"\xff" * (-len(fw) % 16)
    n = len(fw) // 16
    for i in range(n):
        p = struct.pack("<H", i) + fw[i * 16:i * 16 + 16]
        yield i, p + struct.pack("<H", crc16_modbus(p))
    yield n, bytes.fromhex("02ff") + struct.pack("<HH", n - 1, ~(n - 1) & 0xFFFF)


async def write(c, uuid, data):
    """Write with response only when the characteristic allows it: BlueZ quietly falls back,
    CoreBluetooth (macOS) refuses with "Write Not Permitted"."""
    ch = c.services.get_characteristic(uuid)
    await c.write_gatt_char(ch, data, response="write" in ch.properties)


MI_CTRL = "00000010-0000-1000-8000-00805f9b34fb"  # service FE95, control point
MI_DATA = "00000019-0000-1000-8000-00805f9b34fb"  # service FE95, framed data
ACK_READY, ACK_OK = bytes.fromhex("00000101"), bytes.fromhex("00000100")


def mi_frames(data):
    """Frames of a Mi auth transfer: [seq LE16, from 1][up to 18 bytes]."""
    return [struct.pack("<H", i // 18 + 1) + data[i:i + 18] for i in range(0, len(data), 18)]


class MiChannel:
    """Transfers on characteristic 0x0019: start 00 00 00 <type> <frames LE16>, ACK ready, frames, ACK ok."""

    def __init__(self, client):
        self.c, self.data, self.ctrl = client, asyncio.Queue(), asyncio.Queue()

    async def start(self):
        await self.c.start_notify(MI_DATA, lambda _, d: self.data.put_nowait(bytes(d)))
        await self.c.start_notify(MI_CTRL, lambda _, d: self.ctrl.put_nowait(bytes(d)))
        await asyncio.sleep(1)  # macOS drops a write without response sent right after connecting

    async def command(self, hexcode):
        await self.c.write_gatt_char(MI_CTRL, bytes.fromhex(hexcode), response=False)

    async def put(self, b):
        await self.c.write_gatt_char(MI_DATA, b, response=False)

    async def get(self):
        return await asyncio.wait_for(self.data.get(), 10)

    async def recv(self):
        p = await self.get()
        if p[:3] == b"\0\0\x02":  # whole payload in one packet
            await self.put(bytes.fromhex("00000300"))
            return p[3], p[4:]
        if p[:3] != b"\0\0\0":
            raise RuntimeError(f"Mi auth: unexpected packet {p.hex()}")
        typ, n = p[3], struct.unpack("<H", p[4:6])[0]
        await self.put(ACK_READY)
        parts = {}
        while len(parts) < n:
            f = await self.get()
            if f[:2] != b"\0\0":
                parts[struct.unpack("<H", f[:2])[0]] = f[2:]
        await self.put(ACK_OK)
        return typ, b"".join(parts[i] for i in range(1, n + 1))

    async def send(self, typ, data):
        frames = mi_frames(data)
        await self.put(bytes([0, 0, 0, typ]) + struct.pack("<H", len(frames)))
        if await self.get() != ACK_READY:
            raise RuntimeError(f"Mi auth: device not ready for package {typ}")
        for f in frames:
            await self.put(f)
        if await self.get() != ACK_OK:
            raise RuntimeError(f"Mi auth: package {typ} rejected")

    async def wait_ctrl(self, *codes):
        while True:
            v = await asyncio.wait_for(self.ctrl.get(), 10)
            if v in codes:
                return v
            if v[:1] in (b"\x12", b"\x14", b"\x22", b"\x23", b"\xe0", b"\xe1", b"\xe3"):
                raise RuntimeError(f"Mi auth failed: {v.hex()}")


async def mi_activate(c):
    """Register, then log in with Mi standard auth, as "Do Activation" in the web flasher.
    The original firmware accepts the Telink OTA only after this, in the same connection."""
    from cryptography.hazmat.primitives import hashes
    from cryptography.hazmat.primitives.asymmetric import ec
    from cryptography.hazmat.primitives.ciphers.aead import AESCCM
    from cryptography.hazmat.primitives.kdf.hkdf import HKDF
    from cryptography.hazmat.primitives.serialization import Encoding, PublicFormat

    ch = MiChannel(c)
    await ch.start()
    await ch.command("a2000000")  # device info: 4-byte header + device id
    _, info = await ch.recv()
    did = info[4:]
    if not did.strip(b"\0"):  # never bound: make up an id, as the web flasher does
        rnd = "".join(random.choice(string.ascii_letters + string.digits) for _ in range(6))
        did = b"\0blt.3.129v" + rnd.encode() + b"g00"
    await ch.command("15000000")  # register without PKI
    key = ec.generate_private_key(ec.SECP256R1())
    await ch.send(3, key.public_key().public_bytes(Encoding.X962, PublicFormat.UncompressedPoint)[1:])
    typ, dev_pub = await ch.recv()
    if typ != 3 or len(dev_pub) != 64:
        raise RuntimeError("Mi auth: no device public key")
    shared = key.exchange(ec.ECDH(), ec.EllipticCurvePublicKey.from_encoded_point(ec.SECP256R1(), b"\x04" + dev_pub))
    k = HKDF(hashes.SHA256(), 64, None, b"mible-setup-info").derive(shared)
    token, a_key = k[0:12], k[28:44]  # k[12:28] is the MiBeacon bind key
    await ch.send(0, AESCCM(a_key, tag_length=4).encrypt(bytes(range(0x10, 0x1C)), did, b"devID"))
    await ch.command("13000000")
    await ch.wait_ctrl(bytes.fromhex("11000000"))  # registered
    await ch.command("24000000")  # login
    app_rand = os.urandom(16)
    await ch.send(0x0B, app_rand)
    _, dev_rand = await ch.recv()
    e = HKDF(hashes.SHA256(), 64, app_rand + dev_rand, b"mible-login-info").derive(token)
    _, dev_confirm = await ch.recv()
    if dev_confirm != hmac.new(e[0:16], dev_rand + app_rand, hashlib.sha256).digest():
        raise RuntimeError("Mi auth: device confirmation does not match")
    await ch.send(0x0A, hmac.new(e[16:32], app_rand + dev_rand, hashlib.sha256).digest())
    await ch.wait_ctrl(bytes.fromhex("21000000"), bytes.fromhex("e2000000"))  # logged in
    print("  activated (Mi register + login)")


def parse_cfg(r):
    """Reply to command 0x55 -> (firmware version, hardware id), as CustomBlkParse() in the web flasher."""
    if len(r) < 10 or r[0] != 0x55:
        return None, None
    ver = r[1]
    if len(r) < 12:
        return ver, None
    hver = r[11] & 0x87 if ver < 0x36 else r[11]
    if ver >= 0x48:
        return ver, hver
    hw = hver & 0x0F
    return ver, 16 + (r[10] & 0x7F) if hw == 0x0F else hw


def fmt(v):
    return f"V{v >> 4}.{v & 15}" if v is not None else "unknown"


async def update(mac, release, image, check, force, stock=False):
    from bleak import BleakClient, BleakScanner
    # pvvx advertises every few seconds and is connectable only part of the time: scan first
    rssi = []
    dev = await BleakScanner.find_device_by_filter(
        lambda d, adv: d.address.upper() == mac.upper() and not rssi.append(adv.rssi), timeout=60)
    if not dev:
        raise TimeoutError("not heard in 60s")
    async with BleakClient(dev, timeout=30) as c:
        if stock:  # original firmware: no config characteristic, activation first
            print(f"  rssi {rssi[-1]} dBm, original firmware")
            await mi_activate(c)
            fw = image
        else:
            reply = asyncio.get_running_loop().create_future()
            await c.start_notify(CFG_CHAR, lambda _, d: d[:1] == b"\x55" and not reply.done() and reply.set_result(bytes(d)))
            for _ in range(3):  # macOS drops a write without response sent right after connecting: wait, retry
                await asyncio.sleep(1)
                await write(c, CFG_CHAR, b"\x55")
                try:
                    r = await asyncio.wait_for(asyncio.shield(reply), 3)
                    break
                except asyncio.TimeoutError:
                    continue
            else:
                raise TimeoutError("no reply to the config request")
            ver, hw = parse_cfg(r)
            await c.stop_notify(CFG_CHAR)
            print(f"  rssi {rssi[-1]} dBm, hardware id {hw}, installed {fmt(ver)}")
        if check:
            return
        if stock:
            pass
        elif image:
            fw = image
        else:
            want, files = release
            path = files[hw] if hw is not None and hw < len(files) else "?"
            if not path.endswith(".bin"):
                raise RuntimeError(f"no BLE image for hardware id {hw} in firmware.json, use the web flasher")
            if ver is not None and ver >= want and not force:
                print("  up to date")
                return
            fw = fetch(path)
            print(f"  flashing {path} ({fmt(want)})")
        # The original firmware's OTA characteristic only allows write without response; without
        # flow control the blocks must be paced, or the image is silently lost and not applied.
        with_response = "write" in c.services.get_characteristic(OTA_CHAR).properties
        await c.write_gatt_char(OTA_CHAR, bytes.fromhex("00ff"), response=with_response)
        await c.write_gatt_char(OTA_CHAR, bytes.fromhex("01ff"), response=with_response)
        await asyncio.sleep(0.3)
        start, total = time.time(), -(-len(fw) // 16)
        for i, p in ota_packets(fw):
            if i == total:  # end marker: the device reboots at once, so never gets to answer
                if not with_response:  # a read returns only after every queued block went out
                    st = await c.read_gatt_char(OTA_CHAR)
                    if st and st[0]:
                        raise RuntimeError(f"OTA error before the end marker: {OTA_ERRORS[st[0]] if st[0] < len(OTA_ERRORS) else st[0]}")
                await c.write_gatt_char(OTA_CHAR, p, response=False)
                break
            await c.write_gatt_char(OTA_CHAR, p, response=with_response)
            if not with_response:
                await asyncio.sleep(0.02)
            if (i + 1) % 8 == 0:  # same status poll as the web flasher
                st = await c.read_gatt_char(OTA_CHAR)
                if st and st[0]:
                    raise RuntimeError(f"OTA error at block {i}: {OTA_ERRORS[st[0]] if st[0] < len(OTA_ERRORS) else st[0]}")
                if i % 400 == 399:
                    print(f"  {i * 100 // total}%", flush=True)
        print(f"  sent in {time.time() - start:.0f}s, the device checks the CRC and reboots")


_cache = {}


def fetch(path):
    if path not in _cache:
        fw = urllib.request.urlopen(RAW + path, timeout=60).read()
        check_image(fw)
        _cache[path] = fw
    return _cache[path]


def check_image(fw):
    if fw[8:12] != b"KNLT" or len(fw) > MAX_BLE_OTA:  # Telink image signature
        sys.exit("not a Telink firmware image, or too big for BLE OTA")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("mac", nargs="*")
    ap.add_argument("--devicelistfile", "-df", help="ini file whose sections are MAC addresses")
    ap.add_argument("--check", action="store_true", help="only print model and installed version")
    ap.add_argument("--file", help="local firmware image instead of the latest release for the model")
    ap.add_argument("--force", action="store_true", help="flash even if already up to date")
    ap.add_argument("--stock", action="store_true",
                    help="the device runs the original Xiaomi firmware: activate, then flash --file")
    a = ap.parse_args()
    if a.stock and not a.file:
        ap.error("--stock needs --file (e.g. bin/ATC_v59.bin for LYWSD03MMC)")

    names = {m.upper(): "" for m in a.mac}
    if a.devicelistfile:
        ini = configparser.ConfigParser()
        if not ini.read(a.devicelistfile):
            sys.exit(f"cannot read {a.devicelistfile}")
        names.update({s.upper(): ini[s].get("sensorname", "") for s in ini.sections() if ":" in s})
    if not names:
        ap.error("give MAC addresses or --devicelistfile")

    release, image = None, None
    if a.file:
        image = open(a.file, "rb").read()
        check_image(image)
    elif not a.check:
        j = json.load(urllib.request.urlopen(RAW + "firmware.json", timeout=20))
        release = j["version"], j["custom"]
        print(f"latest release: {fmt(j['version'])}")

    failed = 0
    for mac, name in names.items():
        print(mac, name)
        for attempt in range(3):  # BlueZ connections time out now and then
            try:
                asyncio.run(update(mac, release, image, a.check, a.force, a.stock))
                break
            except RuntimeError as e:  # not worth retrying
                print(f"  {e}")
                break
            except Exception as e:
                print(f"  attempt {attempt + 1} failed: {e.__class__.__name__}: {e}")
        else:
            failed += 1
    sys.exit(1 if failed else 0)


def selftest():
    assert crc16_modbus(b"123456789") == 0x4B37  # standard Modbus check value
    pk = list(ota_packets(b"\x01" * 20))
    assert len(pk) == 3 and len(pk[0][1]) == 20 and pk[1][1][2:18] == b"\x01" * 4 + b"\xff" * 12
    assert pk[2][1] == bytes.fromhex("02ff0100feff")
    cfg = bytes([0x55, 0x59, 0, 0, 0, 0, 10, 4, 0x8B, 0, 55, 0])
    assert parse_cfg(cfg) == (0x59, 0)
    assert parse_cfg(bytes([0x55, 0x40] + [0] * 8 + [0x83, 0x0F])) == (0x40, 16 + 3)  # old fw, extended id
    assert parse_cfg(b"\x55\x59") == (None, None)
    fr = mi_frames(bytes(64))  # a public key goes in 4 frames: 18 + 18 + 18 + 10
    assert [f[:2] for f in fr] == [b"\x01\x00", b"\x02\x00", b"\x03\x00", b"\x04\x00"] and len(fr[3]) == 12


if __name__ == "__main__":
    selftest()
    main()
