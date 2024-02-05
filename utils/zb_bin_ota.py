#!/usr/bin/env python3

import argparse
import binascii
import os
import struct

def create(output, filename, manufacturer_id, image_type, file_version, header_string):
	hs = str.encode(header_string)
	if len(hs) > 31:
		print("Error: header_string size is too long!");
		exit(2);
	with (open(filename, 'rb')) as bin_file:
		bin_file.seek(0, 0)
		firmware = bytearray(bin_file.read(-1))
		ota_hdr_s = struct.Struct('<I5HIH32sI')
		header_size = 56
		firmware_len = len(firmware)
		total_image_size = firmware_len + header_size + 6
		ota_hdr = ota_hdr_s.pack(
			0xbeef11e,
			0x100,  # header version is 0x0100
			header_size,
			0,  # ota_ext_hdr_value if ota_ext_hdr else 0,
			manufacturer_id,  # args.manufacturer,
			image_type,  # args.image_type,
			file_version,  # options.File_Version
			2,  # options.stack_version,
			hs + b'\x00' * (32-len(hs)),
			#b'\x00' * 32,  # OTA_Header_String.encode(),
			total_image_size,
		)
		# add chunk header: 0 - firmware type
		ota_hdr += struct.pack('<HI', 0, firmware_len)
		
		head, tail = os.path.split(output)
		name, _ = os.path.splitext(tail)
		out_filename = os.path.join(head, '{:04x}-{:04x}-{:08x}-{}.zigbee'.format(
			manufacturer_id,
			image_type,
			file_version,
			name,
		))
		with open(out_filename, 'wb') as output:
			bin_file.seek(0, 0)
			output.write(ota_hdr)
			output.write(firmware)
		print("%s was created with ZCL OTA Header." % out_filename)

def arg_auto_int(x):
	return int(x, 0)

def main():
	parser = argparse.ArgumentParser(description="Create Zigbee OTA file",
		epilog="Reads a firmware image file and outputs an Zigbee OTA file")
	parser.add_argument("filename", metavar="INPUT", type=str, help="Firmware image filename")
	parser.add_argument("output", metavar="OUTPUT", type=str, help="OTA filename")
	parser.add_argument("-m", "--manufacturer_id", metavar="MANUFACTURER_ID", type=arg_auto_int, required=True, help="Manufacturer ID")
	parser.add_argument("-i", "--image_type", metavar="IMAGE_ID", type=arg_auto_int, required=True, help="Image ID")
	parser.add_argument("-v", "--file_version", metavar="VERSION", type=arg_auto_int, required=True, help="File version")
	parser.add_argument("-s", "--header_string", metavar="HEADER_STRING", type=str, default="", help="Header String")

	args = parser.parse_args()

	create(**vars(args))
	exit(0);
		
if __name__ == '__main__':
	main()
			