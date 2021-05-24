import asyncio
import struct
import binascii
from Crypto.Cipher import AES

def parse_value(hexvalue):
	vlength = len(hexvalue)
	# print("vlength:", vlength, "hexvalue", hexvalue.hex(), "typecode", typecode)
	if vlength == 3:
		temp = hexvalue[0]/2 - 40
		humi = hexvalue[1]/2
		batt = hexvalue[2] & 0x7F
		trg =  hexvalue[2] >> 7
		print("Temperature:", temp, "Humidity:", humi, "Battery:", batt, "Trg:", trg)
		return 1
	if vlength == 6:
		(temp, humi, batt, trg) = struct.unpack("<hHBB", hexvalue)
		print("Temperature:", temp/100, "Humidity:", humi/100, "Battery:", batt, "Trg:", trg)
		return 1
	print("MsgLength:", vlength, "HexValue:", hexvalue.hex())
	return None

def decrypt_payload(payload, key, nonce):
	token = payload[-4:] # mic
	cipherpayload = payload[:-4] # EncodeData
	print("Nonce: %s" % nonce.hex())
	print("CryptData: %s" % cipherpayload.hex(), "Mic: %s" % token.hex())
	cipher = AES.new(key, AES.MODE_CCM, nonce=nonce, mac_len=4)
	cipher.update(b"\x11")
	data = None
	try:
		data = cipher.decrypt_and_verify(cipherpayload, token)
	except ValueError as error:
		print("Decryption failed: %s" % error)
		return None
	print("DecryptData:", data.hex())
	print()
	if parse_value(data) != None:
		return 1
	print('??')
	return None

def decrypt_aes_ccm(key, mac, data):
	print("MAC:", mac.hex(), "Binkey:", key.hex())
	print()
	adslength = len(data)
	if adslength > 8 and data[0] <= adslength and data[0] > 7 and data[1] == 0x16 and data[2] == 0x1a and data[3] == 0x18:
		pkt = data[:data[0]+1]
		# nonce: mac[6] + head[4] + cnt[1]
		nonce = b"".join([mac, pkt[:5]])
		return decrypt_payload(pkt[5:], key, nonce)
	else:
		print("Error: format packet!")
	return None

#=============================
# main()
#=============================
def main():
	print()
	temp = 22.53
	humi = 50.12
	batt = 99
	trg  = 135
	print("Temperature:", temp, "Humidity:", humi, "Battery:", batt, "Trg:", trg)
	print()

	mac = binascii.unhexlify('ed5e0b38c1a4') # MAC
	binkey = binascii.unhexlify('12345678901234567890123456789012')
	print("MAC:", mac.hex(), "Binkey:", binkey.hex())
	print()

	print("====== Test1 ATC encode ----------------------------------------")

	data = struct.pack("<BBB", int((temp + 40)*2), int(humi*2), batt + (trg&2 << 6)) 
	adshead = struct.pack(">BBHB", len(data) + 8, 0x16, 0x1a18, 0xbd) # ad struct head: len, id, uuid16, cnt
	beacon_nonce = b"".join([mac, adshead])
	cipher = AES.new(binkey, AES.MODE_CCM, nonce=beacon_nonce, mac_len=4)
	cipher.update(b"\x11")
	ciphertext, mic = cipher.encrypt_and_digest(data)
	print("Data:", data.hex())
	print("Nonce:", beacon_nonce.hex())
	print("CryptData:", ciphertext.hex(), "Mic:", mic.hex())
	adstruct = b"".join([adshead, ciphertext, mic])
	print()
	print("AdStruct:", adstruct.hex())
	print()
	print("====== Test1 ATC decode ----------------------------------------")
	decrypt_aes_ccm(binkey, mac, adstruct);

	print()
	print("====== Test2 PVVX encode ----------------------------------------")
	data = struct.pack("<hHBB", int(temp*100), int(humi*100), batt, trg) 
	adshead = struct.pack(">BBHB", len(data) + 8, 0x16, 0x1a18, 0xbd) # ad struct head: len, id, uuid16, cnt
	beacon_nonce = b"".join([mac, adshead])
	cipher = AES.new(binkey, AES.MODE_CCM, nonce=beacon_nonce, mac_len=4)
	cipher.update(b"\x11")
	ciphertext, mic = cipher.encrypt_and_digest(data)
	print("Data:", data.hex())
	print("Nonce:", beacon_nonce.hex())
	print("CryptData:", ciphertext.hex(), "Mic:", mic.hex())
	adstruct = b"".join([adshead, ciphertext, mic])
	print()
	print("AdStruct:", adstruct.hex())
	print()
	print("====== Test2 PVVX decode ----------------------------------------")
	#mac = binascii.unhexlify('a4c1380b5eed')
	#adstruct = binascii.unhexlify('0e161a1821a3dbe71940006994b078')
	print("AdStruct:", adstruct.hex())
	decrypt_aes_ccm(binkey, mac, adstruct);


if __name__ == '__main__':
	main()
