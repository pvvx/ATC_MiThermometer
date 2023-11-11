#!/usr/bin/env python3

import argparse
import binascii
import os
import struct

OTA_MAGIC = b'\x5d\x02'


def main(args):
    assert args.input_file != args.output

    with (open(args.input_file, 'rb') as bin_file):
        bin_file.seek(0, 0)
        firmware = bytearray(bin_file.read(-1))
        if firmware[6:8] != OTA_MAGIC:
            # Ensure FW size is multiple of 16
            padding = 16 - len(firmware) % 16
            if padding < 16:
                firmware += b'\xFF' * padding
            # Fix FW length
            firmware[0x18:0x1c] = (len(firmware) + 4).to_bytes(4, byteorder='little')
            # Add magic constant
            firmware[6:8] = OTA_MAGIC
            # Add CRC
            crc = binascii.crc32(firmware) ^ 0xffffffff
            firmware += crc.to_bytes(4, byteorder='little')

        ota_hdr_s = struct.Struct('<I5HIH32sI')
        header_size = 56
        firmware_len = len(firmware)
        total_image_size = firmware_len + header_size + 6
        manufacturer_code = int.from_bytes(firmware[18:20], byteorder='little')
        image_type = int.from_bytes(firmware[20:22], byteorder='little')
        file_version = args.set_version or int.from_bytes(firmware[2:6], byteorder='little')
        hs = str.encode('Telink OTA BLE device')
        ota_hdr = ota_hdr_s.pack(
            0xbeef11e,
            0x100,  # header version is 0x0100
            header_size,
            0,  # ota_ext_hdr_value if ota_ext_hdr else 0,
            manufacturer_code,  # args.manufacturer,
            image_type,  # args.image_type,
            file_version,  # options.File_Version
            args.ota_version,  # options.stack_version,
            hs + b'\x00' * (32-len(hs)),
            #b'\x00' * 32,  # OTA_Header_String.encode(),
            total_image_size,
        )
        # add chunk header: 0 - firmware type
        ota_hdr += struct.pack('<HI', 0, firmware_len)

        out_filename = args.output
        if not out_filename:
            head, tail = os.path.split(args.input_file)
            if args.path:
                head = args.path
            if args.output_title:
                name = args.output_title
            else:
                name, _ = os.path.splitext(tail)
            out_filename = os.path.join(head, '{:04x}-{:04x}-{:08x}-{}.zigbee'.format(
                manufacturer_code,
                image_type,
                file_version,
                name,
            ))
        with open(out_filename, 'wb') as output:
            bin_file.seek(0, 0)
            output.write(ota_hdr)
            output.write(firmware)
        print("%s was created with ZCL OTA Header." % out_filename)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("input_file", help="input file")
    parser.add_argument("-ot", '--output-title', help="replace original file name with the string")
    parser.add_argument("-o", '--output', help="output file")
    parser.add_argument("-p", '--path', help="path to output file")
    # sync with g_zcl_basicAttrs.stackVersion
    parser.add_argument("-s", '--ota-version', type=int, help="OTA stack version", default=2)
    parser.add_argument("-v", '--set-version', type=lambda x: int(x, 0), help="Override version from BIN")
    _args = parser.parse_args()
    main(_args)
