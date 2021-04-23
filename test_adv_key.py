import asyncio
import struct
import binascii
from Crypto.Cipher import AES

def parse_value(hexvalue, typecode):
	vlength = len(hexvalue)
	# print("vlength:", vlength, "hexvalue", hexvalue.hex(), "typecode", typecode)
	if vlength == 4:
		if typecode == 0x0D:
			(temp, humi) = struct.unpack("<hH", hexvalue)
			print("temperature:", temp / 10, "humidity", humi / 10)
			return 1
	if vlength == 2:
		if typecode == 0x06:
			(humi,) = struct.unpack("<H", hexvalue)
			print("humidity:", humi / 10)
			return 1
		if typecode == 0x04:
			(temp,) = struct.unpack("<h", hexvalue)
			print("temperature:", temp / 10)
			return 1
		if typecode == 0x09:
			(cond,) = struct.unpack("<H", hexvalue)
			print("conductivity:", cond)
			return 1
	if vlength == 1:
		if typecode == 0x0A:
			print("battery:", hexvalue[0])
			return 1
		if typecode == 0x08:
			print("moisture:", hexvalue[0])
			return 1
	if vlength == 3:
		if typecode == 0x07:
			(illum,) = struct.unpack("<I", hexvalue + b'\x00')
			print("illuminance:", illum)
			return 1
	print("vlength:", vlength, "hexvalue:", hexvalue.hex(), "typecode:", typecode)
	return None

def decrypt_payload(payload, key, nonce):
	token = payload[-4:] #mic
	payload_counter = payload[-7:-4] #value
	nonce = b"".join([nonce, payload_counter])
	cipherpayload = payload[:-7]
	cipher = AES.new(key, AES.MODE_CCM, nonce=nonce, mac_len=4)
	cipher.update(b"\x11")
	data = None
	try:
		data = cipher.decrypt_and_verify(cipherpayload, token)
	except ValueError as error:
		print("Decryption failed: %s" % error)
		print("token: %s" % token.hex())
		print("nonce: %s" % nonce.hex())
		print("payload: %s" % payload.hex())
		print("cipherpayload: %s" % cipherpayload.hex())
		return None
	print("decrypt:", data.hex(), end = ', ')
	if len(data) == data[2] + 3:
		return parse_value(data[3:], data[0])
	print('??')
	return None

def decrypt_aes_ccm(key, data):
	print("Packet:", data.hex())
	#                     mac      dev_id       cnt
	nonce = b"".join([data[9:15], data[6:8], data[8:9]])
	#                      payload   bind  nonce
	return decrypt_payload(data[15:], key, nonce)

#=============================
# main()
#=============================
def main():
	print()
	print("====== Test encode -----------------------------------------")
	binkey = binascii.unhexlify('43CA100B38FB9E7299DDE3538ED45D68')
	print("binkey:", binkey.hex())
	mac = binascii.unhexlify('ED5E0B38C1A4')
	pid = binascii.unhexlify('5b05') #PRODUCT_ID
	cnt = binascii.unhexlify('01') # encode frame cnt
	ext_cnt = binascii.unhexlify('000000') # ext_cnt
	beacon_nonce = b"".join([mac, pid, cnt, ext_cnt])
	data = binascii.unhexlify('0410021901')

	cipher = AES.new(binkey, AES.MODE_CCM, nonce=beacon_nonce, mac_len=4)
	cipher.update(b"\x11")
	ciphertext, tag = cipher.encrypt_and_digest(data)
	print('ciphertext:', ciphertext.hex())
	print('tag:', tag.hex())
	pkt1 = binascii.unhexlify('1A1695FE58585B05')
	pkt = b"".join([pkt1, cnt, mac, ciphertext, ext_cnt, tag])
	decrypt_aes_ccm(binkey, pkt);

	print()
	print("====== Test decode -------------------------------------------")
	# 0  1  2 3  4 5  6 7  8  9         14 15                    26
	# 1a 16 95fe 5858 5b05 a8 ed5e0b38c1a4 0239ff0e350000002f044957
	#                 pid  cnt mac         crypt data      mic
	pkt = binascii.unhexlify('191695FE58585B050CED5E0B38C1A4E4F0DA76020000893216B7')
	decrypt_aes_ccm(binkey, pkt);
	pkt = binascii.unhexlify('1A1695FE58585B050DED5E0B38C1A4C69401D90F0200008078F409')
	decrypt_aes_ccm(binkey, pkt);
	pkt = binascii.unhexlify('1A1695FE58585B0502ED5E0B38C1A47B092DF3240200002878FAAC')
	decrypt_aes_ccm(binkey, pkt);

if __name__ == '__main__':
	main()
