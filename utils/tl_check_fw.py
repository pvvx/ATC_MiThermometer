#! /usr/bin/env python3
import binascii
import sys
# https://github.com/pvvx/ATC_MiThermometer/issues/186#issuecomment-1030410603
with open(sys.argv[1], 'rb') as f:
    firmware = bytearray(f.read(-1))

if firmware[6:8] != b'\x5d\x02':
    # Ensure FW size is multiple of 16
    padding = 16 - len(firmware) % 16
    if padding < 16:
        firmware += b'\xFF' * padding
    # Fix FW length
    firmware[0x18:0x1c] = (len(firmware)+4).to_bytes(4, byteorder='little')
    # Add magic constant
    firmware[6:8] = b'\x5d\x02'
    # Add CRC
    crc = binascii.crc32(firmware) ^ 0xffffffff
    firmware += crc.to_bytes(4, byteorder='little')
    # Write the new firwmare back to the file
    with open(sys.argv[1], 'wb') as f:
        f.write(firmware)
    print(f"Firmware patched!")
else:
    print(f"Firmware already patched!")