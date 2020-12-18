#!/usr/bin/env python

### ComSwireWriter.py ###
###    Autor: pvvx    ###
###    Edited: Aaron Christophel ATCnetz.de    ###
###    Edit : Pila    ###

import sys
import signal
import struct
import serial
import platform
import time
import argparse
import os
import io
import serial.tools.list_ports

__progname__ = 'TLSR825x Flasher'
__version__ = "00.00.04"

COMPORT_MIN_BAUD_RATE=340000
COMPORT_DEF_BAUD_RATE=921600
USBCOMPORT_BAD_BAUD_RATE=460800

FLASH_SECTOR_SIZE = 4096

debug = False
bit8mask = 0x20

class FatalError(RuntimeError):
	def __init__(self, message):
		RuntimeError.__init__(self, message)
	@staticmethod
	def WithResult(message, result):
		message += " (result was %s)" % hexify(result)
		return FatalError(message)
def signal_handler(signal, frame):
	print()
	print('Keyboard Break!')
	sys.exit(0)
def arg_auto_int(x):
	return int(x, 0)
def hex_dump(addr, blk):
	print('%06x: ' % addr, end='')
	for i in range(len(blk)):
		if (i+1) % 16 == 0:
			print('%02x ' % blk[i])
			if i < len(blk) - 1:
				print('%06x: ' % (addr + i + 1), end='')
		else:
			print('%02x ' % blk[i], end='')
	if len(blk) % 16 != 0:
		print('')
# encode data (blk) into 10-bit swire words
def sws_encode_blk(blk):
	pkt=[]
	d = bytearray(10) # word swire 10 bits
	d[0] = 0x80 # start bit byte cmd swire = 1
	for el in blk:
		m = 0x80 # mask bit
		idx = 1
		while m != 0:
			if (el & m) != 0:
				d[idx] = 0x80
			else:
				d[idx] = 0xfe
			idx += 1
			m >>= 1
		d[9] = 0xfe # stop bit swire = 0
		pkt += d
		d[0] = 0xfe # start bit next byte swire = 0
	return pkt
# decode 9 bit swire response to byte (blk)
def sws_decode_blk(blk):
	if (len(blk) == 9) and ((blk[8] & 0xfe) == 0xfe):
		bitmask = bit8mask
		data = 0;
		for el in range(8):
			data <<= 1
			if (blk[el] & bitmask) == 0:
				data |= 1
			bitmask = 0x10
		#print('0x%02x' % data)
		return data
	#print('Error blk:', blk)
	return None
# encode a part of the read-by-address command (before the data read start bit) into 10-bit swire words
def sws_rd_addr(addr):
	return sws_encode_blk(bytearray([0x5a, (addr>>16)&0xff, (addr>>8)&0xff, addr & 0xff, 0x80]))
# encode command stop into 10-bit swire words
def sws_code_end():
	return sws_encode_blk([0xff])
# encode the command for writing data into 10-bit swire words
def sws_wr_addr(addr, data):
	return sws_encode_blk(bytearray([0x5a, (addr>>16)&0xff, (addr>>8)&0xff, addr & 0xff, 0x00]) + bytearray(data)) + sws_encode_blk([0xff])
# send block to USB-COM
def wr_usbcom_blk(serialPort, blk):
	# USB-COM chips throttle the stream into blocks at high speed!
	# Swire is transmitted by 10 bytes of UART.
	# The packet must be a multiple of these 10 bytes.
	# Max block USB2.0 64 bytes -> the packet will be 60 bytes.
	if serialPort.baudrate > USBCOMPORT_BAD_BAUD_RATE:
		i = 0
		s = 60
		l = len(blk)
		while i < l:
			if l - i < s:
				s = l - i
			i += serialPort.write(blk[i:i+s])
			serialPort.flush()
		return i
	return serialPort.write(blk)
# send and receive block to USB-COM
def	rd_wr_usbcom_blk(serialPort, blk):
	i = wr_usbcom_blk(serialPort, blk)
	return i == len(serialPort.read(i))
# send swire command write to USB-COM
def sws_wr_addr_usbcom(serialPort, addr, data):
	return wr_usbcom_blk(serialPort, sws_wr_addr(addr, data))
# send and receive swire command write to USB-COM
def rd_sws_wr_addr_usbcom(serialPort, addr, data):
	i = wr_usbcom_blk(serialPort, sws_wr_addr(addr, data))
	return i == len(serialPort.read(i))
# send swire data in fifo mode
def rd_sws_fifo_wr_usbcom(serialPort, addr, data):
	rd_sws_wr_addr_usbcom(serialPort, 0x00b3, bytearray([0x80])) # [0xb3]=0x80 ext.SWS into fifo mode
	rd_sws_wr_addr_usbcom(serialPort, addr, data) # send all data to one register (no increment address - fifo mode)
	rd_sws_wr_addr_usbcom(serialPort, 0x00b3, bytearray([0x00])) # [0xb3]=0x00 ext.SWS into normal(ram) mode
# send and receive swire command read to USB-COM
def sws_read_data(serialPort, addr, size = 1):
	time.sleep(0.05)
	serialPort.reset_input_buffer()
	# send addr and flag read
	rd_wr_usbcom_blk(serialPort, sws_rd_addr(addr))
	out = []
	# read size bytes
	for i in range(size):
		# send bit start read byte
		serialPort.write([0xfe])
		# read 9 bits swire, decode read byte
		blk = serialPort.read(9)
		# Added retry reading for Prolific PL-2303HX and ...
		if len(blk) < 9:
			blk += serialPort.read(10-len(blk))
		x = sws_decode_blk(blk)
		if x != None:
			out += [x]
		else:
			if debug:
				print('\r\nDebug: read swire byte:')
				hex_dump(addr+i, blk)
			# send stop read
			rd_wr_usbcom_blk(serialPort, sws_code_end())
			out = None
			break
	# send stop read
	rd_wr_usbcom_blk(serialPort, sws_code_end())
	return out
# set sws speed according to clk frequency and serialPort baud
def set_sws_speed(serialPort, clk):
	#--------------------------------
	# Set register[0x00b2]
	print('SWire speed for CLK %.1f MHz... ' % (clk/1000000), end='')
	swsdiv = int(round(clk*2/serialPort.baudrate))
	if swsdiv > 0x7f:
		print('Low UART baud rate!')
		return False
	byteSent = sws_wr_addr_usbcom(serialPort, 0x00b2, [swsdiv])
	# print('Test SWM/SWS %d/%d baud...' % (int(serialPort.baudrate/5),int(clk/5/swsbaud)))
	read = serialPort.read(byteSent)
	if len(read) != byteSent:
		if serialPort.baudrate > USBCOMPORT_BAD_BAUD_RATE and byteSent > 64 and len(read) >= 64 and len(read) < byteSent:
			print('\n\r!!!!!!!!!!!!!!!!!!!BAD USB-UART Chip!!!!!!!!!!!!!!!!!!!')
			print('UART Output:')
			hex_dump(0,sws_wr_addr(0x00b2, [swsdiv]))
			print('UART Input:')
			hex_dump(0,read)
			print('!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!')
			return False
		print('\n\rError: Wrong RX-TX connection!')
		return False
	#--------------------------------
	# Test read register[0x00b2]
	x = sws_read_data(serialPort, 0x00b2)
	#print(x)
	if x != None and x[0] == swsdiv:
		print('ok.')
		if debug:
			print('Debug: UART-SWS %d baud. SW-CLK ~%.1f MHz' % (int(serialPort.baudrate/10), serialPort.baudrate*swsdiv/2000000))
			print('Debug: swdiv = 0x%02x' % (swsdiv))
		return True
	#--------------------------------
	# Set default register[0x00b2]
	rd_sws_wr_addr_usbcom(serialPort, 0x00b2, 0x05)
	print('no')
	return False
# auto set sws speed according to serialport baud
def set_sws_auto_speed(serialPort):
	#---------------------------------------------------
	# swsbaud = Fclk/5/register[0x00b2]
	# register[0x00b2] = Fclk/5/swsbaud
	# swsbaud = serialPort.baudrate/10
	# register[0x00b2] = Fclk*2/serialPort.baudrate
	# Fclk = 16000000..48000000 Hz
	# serialPort.baudrate = 460800..3000000 bits/s
	# register[0x00b2] = swsdiv = 10..208
	#---------------------------------------------------
	#serialPort.timeout = 0.01 # A serialPort.timeout must be set !
	if debug:
		swsdiv_def = int(round(24000000*2/serialPort.baudrate))
		print('Debug: default swdiv for 24 MHz = %d (0x%02x)' % (swsdiv_def, swsdiv_def))
	swsdiv = int(round(16000000*2/serialPort.baudrate))
	if swsdiv > 0x7f:
		print('Low UART baud rate!')
		return False
	swsdiv_max = int(round(48000000*2/serialPort.baudrate))
	#bit8m = (bit8mask + (bit8mask<<1) + (bit8mask<<2))&0xff
	bit8m = ((~(bit8mask-1))<<1)&0xff
	while swsdiv <= swsdiv_max:
		# register[0x00b2] = swsdiv
		rd_sws_wr_addr_usbcom(serialPort, 0x00b2, bytearray([swsdiv]))
		# send addr and flag read
		rd_wr_usbcom_blk(serialPort, sws_rd_addr(0x00b2))
		# start read data
		serialPort.write([0xfe])
		# read 9 bits data
		blk = serialPort.read(9)
		# Added retry reading for Prolific PL-2303HX and ...
		if len(blk) < 9:
			blk += serialPort.read(9-len(blk))
		# send stop read
		rd_wr_usbcom_blk(serialPort, sws_code_end())
		if debug:
			print('Debug (read data):')
			hex_dump(swsdiv, blk)
		if len(blk) == 9 and blk[8] == 0xfe:
			cmp = sws_encode_blk([swsdiv])
			if debug:
				print('Debug (check data):')
				hex_dump(swsdiv+0xccc00, sws_encode_blk([swsdiv]))
				print('bit mask: 0x%02x' % (bit8m))
			if (blk[0]&bit8m) == bit8m and blk[1] == cmp[2] and blk[2] == cmp[3] and blk[4] == cmp[5] and blk[6] == cmp[7] and blk[7] == cmp[8]:
				'''
				swsdiv += 1	
				rd_sws_wr_addr_usbcom(serialPort, 0x00b2, bytearray([swsdiv]))
				data = sws_read_data(serialPort, 0x00b2, 1)
				if data == None or data[0] != swsdiv:
					swsdiv -= 1
					if debug:
						print('swsdiv:', swsdiv)
					break
				rd_sws_wr_addr_usbcom(serialPort, 0x00b2, bytearray([swsdiv]))
				'''
				print('UART-SWS %d baud. SW-CLK ~%.1f MHz(?)' % (int(serialPort.baudrate/10), serialPort.baudrate*swsdiv/2000000))
				return True
		swsdiv += 1
		if swsdiv > 0x7f:
			print('Low UART baud rate!')
			break
	#--------------------------------
	# Set default register[0x00b2]
	rd_sws_wr_addr_usbcom(serialPort, 0x00b2, bytearray([0x05]))
	return False
def activate(serialPort, tact_ms):
	#--------------------------------
	# issue reset-to-bootloader:
	# RTS = either RESET (active low = chip in reset)
	# DTR = active low
	print('Reset module (RTS low)...')
	serialPort.setDTR(True)
	serialPort.setRTS(True)
	time.sleep(0.05)
	serialPort.setDTR(False)
	serialPort.setRTS(False)
	#--------------------------------
	# Stop CPU|: [0x0602]=5
	print('Activate (%d ms)...' % tact_ms)
	sws_wr_addr_usbcom(serialPort, 0x06f, bytearray([0x20])) # soft reset mcu
	blk = sws_wr_addr(0x0602, bytearray([0x05]))
	if tact_ms > 0:
		tact = tact_ms/1000.0
		t1 = time.time()
		while time.time()-t1 < tact:
			for i in range(5):
				wr_usbcom_blk(serialPort, blk)
			serialPort.reset_input_buffer()
	#--------------------------------
	# Duplication with syncronization
	time.sleep(0.01)
	serialPort.reset_input_buffer()
	rd_wr_usbcom_blk(serialPort, sws_code_end())
	rd_wr_usbcom_blk(serialPort, blk)
	time.sleep(0.01)
	serialPort.reset_input_buffer()

def FlashReadBlock(serialPort, stream, offset = 0, size = 0x80000):
	offset &= 0x00ffffff
	rdsize = 0x10
	while size > 0:
		if rdsize > size:
			rdsize = size
		print('\rRead from 0x%06x...' % offset, end = '')
		rd_sws_wr_addr_usbcom(serialPort, 0x0b3, bytearray([0x80])) # [0xb3]=0x80 ext.SWS into fifo mode
		rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x00]))  # SPI set cns low
		# send all data to one register (not increment address - fifo mode), cmd flash rd, addr, + launch first read
		rd_sws_wr_addr_usbcom(serialPort, 0x0c, bytearray([0x03, (offset >> 16) & 0xffff, (offset >> 8) & 0xff, offset & 0xff, 0]))
		rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x0A]))  # [0x0d]=0x0a SPI set auto read mode & cns low
		# read all data from one register (not increment address - fifo mode)
		data = sws_read_data(serialPort, 0x0c, rdsize)
		rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x01]))  # SPI set cns high
		rd_sws_wr_addr_usbcom(serialPort, 0x0b3, bytearray([0x00])) # [0xb3]=0x00 ext.SWS into normal(ram) mode
		if data == None or len(data) != rdsize:
			print('\rError Read Flash data at 0x%06x! ' % offset)
			return False
		stream.write(bytearray(data))
		size -= rdsize
		offset += rdsize
	print('\r                               \r',  end = '')
	return True
def FlashReady(serialPort, count = 33):
	for _ in range(count):
		rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x00]))  # SPI set cns low
		rd_sws_wr_addr_usbcom(serialPort, 0x0c, bytearray([0x05]))  # Flash cmd rd status
		data = sws_read_data(serialPort, 0x0c)
		rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x01]))  # SPI set cns high
		if data == None:
			print('\rError Read Flash Status! (%d)  ' %(_))
			return False
		if (data[0] & 0x01) == 0:
			return True
	print('\rTimeout! Flash status 0x%02x!     ' % data[0])
	return False
def FlashWriteEnable(serialPort):
	rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x00]))  # cns low
	rd_sws_wr_addr_usbcom(serialPort, 0x0c, bytearray([0x06]))  # Flash cmd write enable
	rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x01]))  # cns high
def FlashWriteAddr(serialPort, addr, data):
	FlashWriteEnable(serialPort)
	rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x00]))  # cns low
	# Flash cmd write + addr + data
	rd_sws_fifo_wr_usbcom(serialPort, 0x0c, bytearray([0x02, (addr >> 16) & 0xffff, (addr >> 8) & 0xff, addr & 0xff]) + bytearray(data))
	rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x01]))  # cns high
	return FlashReady(serialPort)
def FlashEraseAll(serialPort):
	FlashWriteEnable(serialPort)
	rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x00]))  # cns low
	rd_sws_fifo_wr_usbcom(serialPort, 0x0c, bytearray([0x60])) # Flash cmd erase all
	rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x01]))  # cns high
	return FlashReady(serialPort, 1000)
def FlashEraseSectors(serialPort, offset = 0, size = 1):
	offset &= ~(FLASH_SECTOR_SIZE-1)
	size = (size + FLASH_SECTOR_SIZE-1) & (~(FLASH_SECTOR_SIZE-1))
	while size > 0:
		print('\rErase Sector at 0x%06x...' % offset, end = '')
		FlashWriteEnable(serialPort)
		rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x00]))  # cns low
		rd_sws_wr_addr_usbcom(serialPort, 0x0c, bytearray([0x20]))  # Flash cmd erase sector
		rd_sws_wr_addr_usbcom(serialPort, 0x0c, bytearray([(offset >> 16) & 0xffff]))  # Faddr hi
		rd_sws_wr_addr_usbcom(serialPort, 0x0c, bytearray([(offset >> 8) & 0xff]))  # Faddr mi
		rd_sws_wr_addr_usbcom(serialPort, 0x0c, bytearray([offset & 0xff]))  # Faddr lo
		rd_sws_wr_addr_usbcom(serialPort, 0x0d, bytearray([0x01]))  # cns high
		offset += FLASH_SECTOR_SIZE
		size -= FLASH_SECTOR_SIZE
		if not FlashReady(serialPort):
			return False
	print('\r                               \r',  end = '')
	return True
def FlashWriteBlock(serialPort, stream, offset = 0, size = 0, erase = True):
	wrsize = 0x100
	if erase and (offset & (FLASH_SECTOR_SIZE-1)) != 0:
		erasec = offset & (0xffffff^(FLASH_SECTOR_SIZE-1))
	else:
		erasec = 0xffffffff # = flag
	fa = 0
	while size > 0:
		offset &= 0xffffff
		if erase:
			wrsec = offset & (0xffffff^(FLASH_SECTOR_SIZE-1))
			if erasec != wrsec:
				# send sector erase command + faddr
				if not FlashEraseSectors(serialPort, offset):
					print('\rError Erase sector at 0x%06x!' % offset)
					return False
				erasec = wrsec
		data = stream.read(wrsize)
		wrsize = len(data)
		if not data or wrsize == 0: # end of stream
			print('\rError Read file at 0x%06x!          ' % (fa+wrsize))
			return False
		for e in data:
			if e != 0xff:
				print('\rWrite to 0x%06x...' % offset, end = '')
				if not FlashWriteAddr(serialPort, offset, data):
					print('\rError write sector at 0x%06x!' % offset)
					return False
				break
		offset += wrsize
		size -= wrsize
		fa + wrsize
	print('\r                               \r',  end = '')
	return True

def main():
	signal.signal(signal.SIGINT, signal_handler)
	t1 = time.time()
	#ports = serial.tools.list_ports.comports() #(win10) execution time more than 30 sec!
	#comport_def_name = ports[0].device
	comport_def_name='COM1'
	if sys.platform == 'linux' or sys.platform == 'linux2':
		comport_def_name = '/dev/ttyS0'
	elif sys.platform == 'win32':
		comport_def_name='COM1'
	#elif sys.platform == "darwin":
	#else:
	#	print(sys.platform)
	parser = argparse.ArgumentParser(description='%s version %s' % (__progname__, __version__))
	parser.add_argument(
		'-p', '--port',
		help='Serial port device (default: '+comport_def_name+')',
		default=comport_def_name)
	parser.add_argument(
		'-t', '--tact',
		help='Time Activation ms (0-off, default: 0 ms)',
		type=arg_auto_int,
		default=0)
	parser.add_argument(
		'-c', '--clk',
		help='SWire CLK (default: auto, 0 - auto)',
		type=arg_auto_int,
		default=0)
	parser.add_argument(
		'-b', '--baud',
		help='UART Baud Rate (default: '+str(COMPORT_DEF_BAUD_RATE)+', min: '+str(COMPORT_MIN_BAUD_RATE)+')',
		type=arg_auto_int,
		default=COMPORT_DEF_BAUD_RATE)
	parser.add_argument(
		'-r', '--run',
		help='CPU Run (post main processing)',
		action='store_true')
	parser.add_argument(
		'-d', '--debug',
		help='Debug info',
		action='store_true')
	subparsers = parser.add_subparsers(
		dest='operation',
		help=os.path.splitext(os.path.basename(__file__))[0]+' {command} -h for additional help')

	parser_read_flash = subparsers.add_parser(
		'rf',
		help='Read Flash to binary file')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('size', help='Size of region', type=arg_auto_int)
	parser_read_flash.add_argument('filename', help='Name of binary file')

	parser_burn_flash = subparsers.add_parser(
		'wf',
		help='Write file to Flash with sectors erases')
	parser_burn_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_burn_flash.add_argument('filename', help='Name of binary file')

	parser_erase_sec_flash = subparsers.add_parser(
		'es',
		help='Erase Region (sectors) of Flash')
	parser_erase_sec_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_erase_sec_flash.add_argument('size', help='Size of region', type=arg_auto_int)

	parser_erase_all_flash = subparsers.add_parser(
		'ea',
		help='Erase All Flash')

	args = parser.parse_args()
	print('=======================================================')
	print('%s version %s' % (__progname__, __version__))
	print('-------------------------------------------------------')
	global debug
	debug = args.debug
	if(args.baud < COMPORT_MIN_BAUD_RATE):
		print ('The minimum speed of the COM port is %d baud!' % COMPORT_MIN_BAUD_RATE)
		sys.exit(1)
	print ('Open %s, %d baud...' % (args.port, args.baud))
	try:
		serialPort = serial.Serial(args.port,args.baud)
		serialPort.reset_input_buffer()
		serialPort.timeout = 0.01
	except:
		print ('Error: Open %s, %d baud!' % (args.port, args.baud))
		sys.exit(1)
	if args.tact != 0:
		# activate
		activate(serialPort, args.tact)
	if args.clk == 0:
		# auto speed
		if not set_sws_auto_speed(serialPort):
			print('Chip sleep? -> Use reset chip (RTS-RST): see option --tact')
			sys.exit(1)
	else:
		# Set SWS Speed = CLK/5/[0xb2] bits/s
		if not set_sws_speed(serialPort, args.clk * 1000000):
			if not set_sws_speed(serialPort, 16000000):
				if not set_sws_speed(serialPort, 24000000):
					if not set_sws_speed(serialPort, 32000000):
						if not set_sws_speed(serialPort, 48000000):
							print('Chip sleep? -> Use reset chip (RTS-RST): see option --tact')
							sys.exit(1)
	#serialPort.timeout = 0.01 # SerialPort.timeout must be set for the following operations!
	if args.operation == 'rf':
		offset = args.address & 0x00ffffff
		size = args.size & 0x00ffffff
		if size == 0:
			print('\rError: Read size = %d!' % size)
			sys.exit(1)
		print('Outfile: %s' % args.filename)
		try:
			stream = open(args.filename, 'wb')
		except:
			print('Error: Not open Outfile file <%s>!' % args.filename)
			sys.exit(1)
		print('Read Flash from 0x%06x to 0x%06x...' % (offset, offset + size))
		if not FlashReadBlock(serialPort, stream, offset, size):
			sys.exit(1)
	elif args.operation == 'wf':
		offset = args.address & 0x00ffffff
		print('Inputfile: %s' % (args.filename))
		try:
			stream = open(args.filename, 'rb')
			size = os.path.getsize(args.filename)
		except:
			print('Error: Not open input file <%s>!' % args.fldr)
			sys.exit(1)
		if size < 1:
			print('Error: File size = %d!'% size)
		else:
			print('Write Flash data 0x%08x to 0x%08x...' % (offset, offset + size))
			if not FlashWriteBlock(serialPort, stream, offset, size):
				sys.exit(1)
	elif args.operation == 'es':
		count = int((args.size + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE)
		size = (count * FLASH_SECTOR_SIZE)
		offset = args.address & (0xffffff^(FLASH_SECTOR_SIZE-1))
		print('Erase Flash %d sectors,\r\ndata from 0x%06x to 0x%06x...' % (count, offset, offset + size))
		if not FlashEraseSectors(serialPort, offset, size):
			sys.exit(1)
	elif args.operation == 'ea':
		print('Erase All Flash ...')
		if not FlashEraseAll(serialPort):
			print('Error Erase All Flash!')
			sys.exit(1)
	else:
		pc = sws_read_data(serialPort, 0x06bc, 4)
		if pc == None or len(pc) != 4:
			print('Error read PC!')
			sys.exit(1)
		x = pc[0] + (pc[1]<<8) + (pc[2]<<16) + (pc[3]<<24)
		print('PC = 0x%06x' % (x))
	if args.run != 0:
		print('Reset CPU...')		
		sws_wr_addr_usbcom(serialPort, 0x006f, bytearray([0x22]))  # Reset CPU
	print('-------------------------------------------------------')
	# Second time slice
	t2 = time.time()
	print("Worked Time: %.3f sec" % (t2-t1))
	print('Done!')
	#--------------------------------
	# Set default register[0x00b2]
	# sws_wr_addr_usbcom(serialPort, 0x00b2, bytearray([5]))
	sys.exit(0)

if __name__ == '__main__':
	main()
