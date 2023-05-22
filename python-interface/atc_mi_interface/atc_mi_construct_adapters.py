# Library module used by atc_mi_construct.py

from construct import *  # pip3 install construct
from Crypto.Cipher import AES  # pip3 install pycryptodome
import re

MacVendor = Switch(
    this.MAC[:9],
    {
        "A4:C1:38:": Computed("Telink Semiconductor (Taipei) Co. Ltd."),
        "54:EF:44:": Computed("Lumi United Technology Co., Ltd"),
    },
    default=Computed("Unknown vendor"),
)


class BtHomeCodec(Tunnel):
    def __init__(self, subcon, size_payload, bindkey=b'', mac_address=b''):
        super().__init__(subcon)
        self.default_bindkey = bindkey
        self.size_payload = size_payload
        self.def_mac = mac_address

    def bindkey(self, ctx):
        try:
            return ctx._params.bindkey or self.default_bindkey
        except Exception:
            return self.default_bindkey

    def mac(self, ctx):
        try:
            return ctx._params.mac_address or self.def_mac
        except Exception:
            return self.def_mac

    def decrypt(self, ctx, nonce, encrypted_data, mic):
        bindkey = self.bindkey(ctx)
        if not bindkey:
            raise ValueError('Missing bindkey during decrypt().')
        cipher = AES.new(bindkey, AES.MODE_CCM, nonce=nonce, mac_len=4)
        cipher.update(b"\x11")
        try:
            return cipher.decrypt_and_verify(encrypted_data, mic), None
        except Exception as e:
            return None, "Decryption error: " + str(e)

    def encrypt(self, ctx, nonce, msg):
        bindkey = self.bindkey(ctx)
        if not bindkey:
            raise ValueError('Missing bindkey during encrypt().')
        cipher = AES.new(bindkey, AES.MODE_CCM, nonce=nonce, mac_len=4)
        cipher.update(b"\x11")
        return cipher.encrypt_and_digest(msg)

    def _decode(self, obj, ctx, path):
        mac = self.mac(ctx)
        if not mac:
            raise ValueError('Missing MAC address during _decode().')
        pkt = ctx._io.getvalue()[2:]
        uuid = pkt[0:2]
        encrypted_data = pkt[2:-8]
        count_id = pkt[-8:-4]  # Int32ul
        mic = pkt[-4:]
        nonce = mac + uuid + count_id
        msg, error = self.decrypt(ctx, nonce, encrypted_data, mic)
        if error:
            return error
        return count_id + msg

    def _encode(self, obj, ctx, path):
        mac = self.mac(ctx)
        if not mac:
            raise ValueError('Missing MAC address during _encode().')
        count_id = bytes(obj)[:4]  # Int32ul
        uuid16 = b"\x1e\x18"
        nonce = mac + uuid16 + count_id
        ciphertext, mic = self.encrypt(
            ctx, nonce, bytes(obj)[4:self.size_payload + 4])
        return ciphertext + count_id + mic


class AtcMiCodec(BtHomeCodec):
    def _decode(self, obj, ctx, path):
        mac = self.mac(ctx)
        if not mac:
            raise ValueError('Missing MAC address during _decode().')
        payload = bytes(obj)[1:]
        cipherpayload = payload[:-4]
        header_bytes = ctx._io.getvalue()[:4]  # b'\x0e\x16\x1a\x18' (custom_enc) or b'\x0b\x16\x1a\x18' (atc1441_enc)
        nonce = mac[::-1] + header_bytes + bytes(obj)[:1]
        token = payload[-4:] #mic
        msg, error = self.decrypt(ctx, nonce, cipherpayload, token)
        if error:
            return error
        return msg + bytes(len(obj) - len(msg))

    def _encode(self, obj, ctx, path):
        mac = self.mac(ctx)
        if not mac:
            raise ValueError('Missing MAC address during _encode().')
        header_bytes = ctx._io.getvalue()[:4] + b'\xbd'  # b'\x0e\x16\x1a\x18\xbd' (custom_enc) or b'\x0b\x16\x1a\x18\xbd' (atc1441_enc)
        nonce = mac[::-1] + header_bytes
        ciphertext, mic = self.encrypt(
            ctx, nonce, bytes(obj)[:self.size_payload])
        return b'\xbd' + ciphertext + mic


class MiLikeCodec(BtHomeCodec):
    def _decode(self, obj, ctx, path):
        payload = obj
        cipherpayload = payload[:-7]
        #mac = ctx._io.getvalue()[9:15:-1]
        mac = self.mac(ctx)
        dev_id = ctx._io.getvalue()[6:8]  # pid, PRODUCT_ID
        cnt = ctx._io.getvalue()[8:9]  # encode frame cnt
        count_id = payload[-7:-4]  # Int24ul
        nonce = mac[::-1] + dev_id + cnt + count_id
        token = payload[-4:] #mic
        msg, error = self.decrypt(ctx, nonce, cipherpayload, token)
        if error:
            return error
        return count_id + msg

    def _encode(self, obj, ctx, path):
        #mac = ctx._io.getvalue()[9:15:-1]
        mac = self.mac(ctx)
        dev_id = ctx._io.getvalue()[6:8]  # pid, PRODUCT_ID
        cnt = ctx._io.getvalue()[8:9]  # encode frame cnt
        count_id = bytes(obj)[:3]  # Int24ul
        nonce = mac[::-1] + dev_id + cnt + count_id
        ciphertext, mic = self.encrypt(
            ctx, nonce, bytes(obj)[3:self.size_payload + 3])
        return ciphertext + count_id + mic


MacAddress = ExprAdapter(Byte[6],
    decoder = lambda obj, ctx: ":".join("%02x" % b for b in obj).upper(),
    encoder = lambda obj, ctx: bytes.fromhex(re.sub(r'[.:\- ]', '', obj))
)
ReversedMacAddress = ExprAdapter(Byte[6],
    decoder = lambda obj, ctx: ":".join("%02x" % b for b in obj[::-1]).upper(),
    encoder = lambda obj, ctx: bytes.fromhex(re.sub(r'[.:\- ]', '', obj))[::-1]
)
Int16ub_x1000 = ExprAdapter(Int16ub,
        obj_ / 1000, lambda obj, ctx: int(float(obj) * 1000))
Int16ul_x1000 = ExprAdapter(Int16ul,
        obj_ / 1000, lambda obj, ctx: int(float(obj) * 1000))
Int16ul_x100 = ExprAdapter(
    Int16ul, obj_ / 100, lambda obj, ctx: int(float(obj) * 100))
Int16sl_x100 = ExprAdapter(
    Int16sl, obj_ / 100, lambda obj, ctx: int(float(obj) * 100))
Int16ub_x10 = ExprAdapter(
    Int16ub, obj_ / 10, lambda obj, ctx: int(float(obj) * 10))
Int16ul_x10 = ExprAdapter(
    Int16ul, obj_ / 10, lambda obj, ctx: int(float(obj) * 10))
Int16sb_x10 = ExprAdapter(
    Int16sb, obj_ / 10, lambda obj, ctx: int(float(obj) * 10))
Int16sl_x10 = ExprAdapter(
    Int16sl, obj_ / 10, lambda obj, ctx: int(float(obj) * 10))

def normalize_report(report):
    report = re.sub(r"\n\s*Container:\n?", "\n", report, flags=re.DOTALL)
    report = re.sub(r"\n\s*version =[^\n]*\n", "\n", report, flags=re.DOTALL)
    report = re.sub(r" = Container:\s*\n", ":\n", report, flags=re.DOTALL)
    report = re.sub(r" = ListContainer:\s*\n", ":\n", report, flags=re.DOTALL)
    report = re.sub(r" = u'", " = '", report, flags=re.DOTALL)
    report = re.sub(r"\n\s*\n", "\n", report, flags=re.DOTALL)
    report = re.sub(r"unhexlify\('([A-Fa-f0-9]*)'\)",
        lambda m: f"    {m.group(1).upper()}", report, flags=re.DOTALL)
    return report
