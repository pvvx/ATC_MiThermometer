#!/usr/bin/env python3
"""Update pvvx ATC_MiThermometer custom firmware over BLE from Linux (e.g. a Raspberry Pi).

Port of the Telink OTA of https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html,
for devices already running pvvx custom firmware (the first flash from stock firmware
still needs the web flasher). The image is chosen by the hardware id the device reports,
as the web flasher does. Needs `pip install bleak`. Stop other BLE scanners first.

  ./ble_ota.py --check A4:C1:38:0B:F8:E1 [...]   # print model and installed version
  ./ble_ota.py A4:C1:38:0B:F8:E1 [...]           # flash the latest release if older
  ./ble_ota.py -df sensors.ini                   # MACs from an ini file ([AA:BB:..] sections)
  ./ble_ota.py --file ATC_v59.bin --force MAC    # flash a given image
"""
import argparse
import asyncio
import configparser
import json
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


async def update(mac, release, image, check, force):
    from bleak import BleakClient, BleakScanner
    # pvvx advertises every few seconds and is connectable only part of the time: scan first
    rssi = []
    dev = await BleakScanner.find_device_by_filter(
        lambda d, adv: d.address.upper() == mac.upper() and not rssi.append(adv.rssi), timeout=60)
    if not dev:
        raise TimeoutError("not heard in 60s")
    async with BleakClient(dev, timeout=30) as c:
        reply = asyncio.get_running_loop().create_future()
        await c.start_notify(CFG_CHAR, lambda _, d: d[:1] == b"\x55" and not reply.done() and reply.set_result(bytes(d)))
        await c.write_gatt_char(CFG_CHAR, b"\x55", response=True)
        ver, hw = parse_cfg(await asyncio.wait_for(reply, 10))
        await c.stop_notify(CFG_CHAR)
        print(f"  rssi {rssi[-1]} dBm, hardware id {hw}, installed {fmt(ver)}")
        if check:
            return
        if image:
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
        await c.write_gatt_char(OTA_CHAR, bytes.fromhex("00ff"), response=True)
        await c.write_gatt_char(OTA_CHAR, bytes.fromhex("01ff"), response=True)
        await asyncio.sleep(0.3)
        start, total = time.time(), -(-len(fw) // 16)
        for i, p in ota_packets(fw):
            if i == total:  # end marker: the device reboots at once, so never gets to answer
                await c.write_gatt_char(OTA_CHAR, p, response=False)
                break
            await c.write_gatt_char(OTA_CHAR, p, response=True)
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
    a = ap.parse_args()

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
                asyncio.run(update(mac, release, image, a.check, a.force))
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


if __name__ == "__main__":
    selftest()
    main()
