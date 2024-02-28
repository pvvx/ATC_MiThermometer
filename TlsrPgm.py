#!/usr/bin/env python

##### TlsrPgm.py #####
###  Autor: pvvx   ###
## https://github.com/pvvx/TLSRPGM ##

import sys
import signal
import struct
import serial
import platform
import time
import argparse
import os
import io

__progname__ = 'TLSR82xx TlsrPgm'
__filename__ = 'TlsrPgm'
__version__ = '31.10.23'

DEFAULT_UART_BAUD = 230400

FLASH_SECTOR_SIZE = 4096

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
	sys.exit(1)

crctable = (
0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 )
def crc16(data: bytearray, length):
	crc = 0xFFFF
	for i in range(0, length):
		crc = (crc >> 8) ^ crctable[(crc ^ data[i]) & 0xFF]
	return bytearray([crc&0xFF,(crc>>8)&0xFF])
def crc_blk(data: bytearray):
	return data+crc16(data, len(data))
def crc_chk(data: bytearray):
	c = data[len(data)-2:];
	if c == crc16(data, len(data)-2):
		return 1
	return 0
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
def hex2str(blk):
	s = ''
	for i in range(len(blk)):
		s += '%02x' % (blk[i])
		if i < len(blk)-1:
			s += ' '
	return s
def bcd2str(blk):
	s = ''
	for i in range(len(blk)):
		s += '%d.%d' % ((blk[i]>>8)&0x0f, blk[i]&0x0f)
		if i < len(blk)-1:
			s += '.'
	return s

class TLSRPGM:
	MAX_BUF_READ_SIZE = 1024
	MAX_RESPONSE_SIZE = 1026
	MIN_RESPONSE_SIZE = 6

	FLASH_SECTOR_SIZE = FLASH_SECTOR_SIZE

	CMD_FUNCS = 0
	CMD_FLASH_READ = 1
	CMD_FLASH_WRITE = 2
	CMD_FLASH_SECT_ERASE = 3
	CMD_FLASH_ALL_ERASE = 4
	CMD_FLASH_GET_JEDEC_ID = 5
	CMD_FLASH_GET_STATUS = 6
	CMD_SWIRE_READ = 7
	CMD_SWIRE_WRITE = 8
	CMD_SWIRE_AREAD = 9
	CMD_SWIRE_AWRITE = 10
	CMD_SWIRE_FIFO_READ = 11
	CMD_SWIRE_FIFO_FWRITE = 12
	CMD_FLASH_WRRD = 13

	CMDF_GET_VERSION = 0
	CMDF_MCU_REBOOT = 1
	CMDF_SWIRE_CFG = 2
	CMDF_EXT_POWER = 3
	CMDF_SWIRE_ACTIVATE = 4
	CMDF_UART_BAUD = 5

	ERR_NONE = 0
	ERR_FUNC = 1
	ERR_LEN  = 2
	ERR_READ = 3
	ERR_BUSY = 4
	ERR_CRC  = 5
	ERR_BAUD = 6
	ERR_MAX  = 7

	ERR_RETRY_COUNT = 3

	port = None
	err = 0
	wcnt = 0

	tab_err = [ \
		"Errors None", \
		"Function number error!", \
		"Data length error!", \
		"SWire read timeout!", \
		"Timeout flag while reading analog register!" \
		"Error CRC!"
		"Invalid baud rate number!", \
	]

	tab_uart_baud = [ \
		115200, \
		230400, \
		460800, \
		500000, \
		921600, \
		1000000, \
		1500000, \
		2000000, \
		3000000, \
	]

	pgm_version = 0
	pgm_cid = 0
	pgm_pwr = 1

	pgm_swsps = 0.96
	pgm_clk = 24
	pgm_swdiv = 5
	pgm_swaddrlen = 3
	pgm_swbuf = [b'\x5a\x00\x06\x02\x00\x05']
	
	pgm_chip = '?'
	#-----------------------------------------------
	# Functions for an PGM device
	#-----------------------------------------------
	def __init__(self, port, baud = DEFAULT_UART_BAUD):
		self.reopen(baud, port)
	# Open/ReOpen com-port
	def reopen(self, baud, port = None):
		if port == None:
			if self.port == None:
				print ('ReOpen ?')
				return False
			else:
				self._port.close();
				port = self.port
			print ('ReOpen %s, %d bit/s...' % (port, baud), end = ' ')
		else:
			print ('Open %s, %d bit/s...' % (port, baud), end = ' ')
		try:
			self._port = serial.Serial(port, baud, \
									   serial.EIGHTBITS,\
									   serial.PARITY_NONE, \
									   serial.STOPBITS_ONE)
			self._port.setRTS(False)
			self._port.setDTR(False)
			time.sleep(0.05)
			self._port.flushOutput()
			self._port.flushInput()
			self._port.reset_output_buffer()
			self._port.reset_input_buffer()
			self._port.timeout = 0.1
		except:
			print('Error!')
			return False
		print('ok')
		self.port = port
		return True
	# Send to usb-com-port
	def write(self, wblk):
		self._port.reset_input_buffer()
		return self._port.write(wblk)
	# Read usb-com-port
	def read(self, rdlen):
		try:
			rblk = self._port.read(rdlen)
			#if len(rblk) < rdlen:
			#	rblk += self._port.read(rdlen - len(rblk))
		except:
			print('\rError read %s!' % (self.port))
			return None
		return rblk
	def command(self, wblk, rdlen = None):
		w = self.write(crc_blk(wblk))
		if w != len(wblk)+2:
			self.err = 100
			return None
		if rdlen == None:
			rdlen = self.MAX_RESPONSE_SIZE
			chklen = self.MIN_RESPONSE_SIZE
		else:
			chklen = rdlen
		self.wcnt = 0
		rblk = self.read(rdlen)
		if rblk == None or len(rblk) < chklen:
			self.err =  self.ERR_LEN +100
			return None;
		if not crc_chk(rblk):
			self.err =  self.ERR_CRC +100
			return None;
		if rblk[0] != wblk[0]:
			self.err =  self.ERR_FUNC +100
			return None;
		self.err = rblk[1];
		self.wcnt = rblk[2] | (rblk[3]<<8)
		if self.err != self.ERR_NONE:
			return None;
		return rblk
	# Set Default UART speed on PGM board
	def close(self):
		self.command(struct.pack('<BBH', self.CMD_FUNCS, self.CMDF_UART_BAUD, 1), 6)
		self._port.close()
	# Set UART speed on PGM board
	def SetUartBaud(self, baud = DEFAULT_UART_BAUD):
		for i in range(len(self.tab_uart_baud)):
			if self.tab_uart_baud[i] == baud:
				print('Set Speed UART PGM board %d bits/s...' % (baud), end = ' ')
				data = self.command(struct.pack('<BBH', self.CMD_FUNCS, self.CMDF_UART_BAUD, i), 6)
				if data == None:
					print('Error(%d)', self.err)
					return False
				print('ok')
				if self._port.baudrate != baud:
					if not self.reopen(baud):
						print('Plese reset PGM board!')
						return False
				return True
		print('Invalid PGM baud rate (%d)!' % (baud))
		return False
	# Reset/Power pin On/Off
	def SetPinRST(self, out = 1):
		data = self.command(struct.pack('<BBH', self.CMD_FUNCS, self.CMDF_EXT_POWER, out), 7)
		if data == None:
			print('\rError[%d] Set pin RST/Power!' % self.err)
			return False
		return True
	# Get version pgm board
	def GetVersion(self):
		data = self.command(struct.pack('<BBH', self.CMD_FUNCS, self.CMDF_GET_VERSION, 0), 19)
		if data == None:
			print('\rError[%d] Read PGM Version and Config!' % self.err)
			return False
		self.pgm_version = [data[5], data[4]]
		(self.pgm_cid, self.pgm_pwr, self.pgm_swdiv, self.pgm_swaddrlen) = struct.unpack('<HBBB', data[6:11])
		self.pgm_swbuf = data[11:17]
		if self.pgm_cid == 0 and self.pgm_ver == 0:
			print('Connection to the PGM board?')
			return False
		if self.pgm_cid == 0x5562:
			self.pgm_chip = 'TLSR825x'
			self.pgm_clk = 24
		elif self.pgm_cid == 0x5325:
			self.pgm_chip = 'TLSR8266'
			self.pgm_clk = 32
		elif self.pgm_cid == 0x5326:
			self.pgm_chip = 'TLSR8267'
			self.pgm_clk = 32
		elif self.pgm_cid == 0x5327:
			self.pgm_chip = 'TLSR8269'
			self.pgm_clk = 32
		else:
			self.pgm_clk = 32
			self.pgm_chip = '?'
		print('PGM: ChipID: 0x%04x (%s), ver:' % (self.pgm_cid, self.pgm_chip), bcd2str(self.pgm_version))
		self.pgm_swsps = self.pgm_clk/5/self.pgm_swdiv
		print('swdiv %d, addrlen %d, swbuf [%s], ' % (self.pgm_swdiv, self.pgm_swaddrlen, hex2str(self.pgm_swbuf)), end = '')
		if self.pgm_pwr != 0:
			print('pwr On')
		else:
			print('pfr Off')
		print('SWire bit rate: %.4f Mbits/s' % self.pgm_swsps)
		return True
	# Set Speed SWM pgm board
	def SetPgmConfig(self, swdiv = None, swaddrlen = None, swbuf = None):
		if swdiv == None:
			swdiv = self.pgm_swdiv;
		if swaddrlen == None:
			swaddrlen = self.pgm_swaddrlen;
		if swbuf == None:
			swbuf = self.pgm_swbuf;
		data = self.command(struct.pack('<BBHBB', self.CMD_FUNCS, self.CMDF_SWIRE_CFG, 0, swdiv, swaddrlen) + swbuf, 14)
		if data == None:
			print('Error[%d] Set PGM Config!' % self.err)
			return False
		(self.pgm_swdiv, self.pgm_swaddrlen) = struct.unpack('<BB', data[4:6])
		self.pgm_swbuf = data[6:12]
		print('PGM: swdiv %d, addrlen %d, swbuf [%s]' % (self.pgm_swdiv, self.pgm_swaddrlen, hex2str(self.pgm_swbuf)))
		return True
	# Reset/Power pin On, Activate, get PC
	def Activate(self, tms):
		count = 0
		if tms > 0:
			count = int(tms/((self.pgm_swaddrlen + 5.7)*5*self.pgm_swdiv/2400))
			if count > 0xffff:
				count = 0xffff
		t = self._port.timeout
		self._port.timeout = tms/1000 + 0.5
		#print('Count = %d, timeout = %.6f' % (count, self._port.timeout))
		print('Activate %d ms... ' % tms, end = '')
		# simple version of the function:
		''' 
		data = self.command(struct.pack('<BBH', self.CMD_FUNCS, self.CMDF_SWIRE_ACTIVATE, count), 10)
		self._port.timeout = t
		if data == None:
			print('Activate Error[%d]!' % self.err)
			return False
		print('ok')
		self.ext_pc = struct.unpack('<I', data[4:8])
		print('CPU PC=0x%08x' % self.ext_pc)
		return True
		'''
		# multiple choice function
		wblk = struct.pack('<BBH', self.CMD_FUNCS, self.CMDF_SWIRE_ACTIVATE, count)
		self.write(crc_blk(wblk))
		rblk = self.read(6)
		self._port.timeout = t
		if rblk == None or len(rblk) < 6:
			self.err =  self.ERR_LEN +100
			print('\r\nTimeout response - Check PGM connected!') 
			return False
		if len(rblk) != 6 or rblk[0] != wblk[0]:
			self.err =  self.ERR_FUNC +100
			print('\r\nIncorrect response - Check PGM connected!') 
			return False
		self.err = rblk[1];
		self.wcnt = rblk[2] | (rblk[3]<<8)
		if self.wcnt > 0 and self.wcnt <= 4:
			rblk += self.read(self.wcnt)
		if not crc_chk(rblk):
			self.err =  self.ERR_CRC +100
			print('\r\nIncorrect response - BAD CRC!') 
			return False
		if self.err != self.ERR_NONE:
			if self.err >= len(self.tab_err):
				print('\r\nError[%d]: Unknown error!' % (self.err))
			else:
				if self.err == self.ERR_READ:
					print('Error!')
					print('Check SWM<->SWS connection or Reset/Activation time!')
				print('\r\nError[%d]: %s' % (self.err, self.tab_err[self.err]))
			return False
		if len(rblk) >= 10:
			self.ext_pc = struct.unpack('<I', rblk[4:8])
			print('ok')
			print('CPU PC=0x%08x' % (self.ext_pc))
		else:
			print('\r\nUnknown response!') 
			return False
		return True
	#-----------------------------------------------
	# Functions for an external device
	#-----------------------------------------------
	# Read Flash Status
	def ReadFlashStatus(self):
		data = self.command(struct.pack('<BBH', self.CMD_FLASH_GET_STATUS, 0, 0), 7)
		if data == None or self.wcnt != 1:
			print('\rError get Flash Status! (%d)' % self.err) 
			return None
		print('Flash Status Register: 0x%02x' % data[4])
		return data[4]
	# Waiting for Flash to be ready
	def WaitingFlashReady(self, count = 300):
		while count > 0:
			data = self.command(struct.pack('<BBH', self.CMD_FLASH_GET_STATUS, 0, 0), 7)
			if data == None or self.wcnt != 1:
				print('\rError get Flash Status! (%d)' % self.err) 
				return False
			if (data[4] & 0x01) == 0:
				#print('Flash status 0x%02x, cnt %d' % (data[4], count))
				return True
			count -= 1
		print('\rTimeout! Flash Status 0x%02x!' % data[4]) 
		return False
	# Write Flash Status
	def WriteFlashStatus(self, fstatus = 0):
		data = self.command(struct.pack('<BBHHB', self.CMD_FLASH_WRRD, 0, 0, 0, 6), 6)
		if data == None:
			print('\rError Write Flash Status! (%d)' % self.err) 
			return None
		data = self.command(struct.pack('<BBHHBB', self.CMD_FLASH_WRRD, 0, 0, 0, 1, fstatus&0xff), 6)
		if data == None:
			print('\rError Write Flash Status! (%d)' % self.err) 
			return None
		if not self.WaitingFlashReady(5):
			return None
		#data = self.command(struct.pack('<BBHHB', self.CMD_FLASH_WRRD, 0, 0, 1, 5), 7)
		#if data == None:
		#	print('\rError get Flash Status! (%d)' % self.err) 
		#	return None
		#print('Flash Status Register 0x%02x' % data[4])
		return True
	# CPcpu Ext. Chip
	def ReadPCcpu(self):
		rdsize = 0x4
		offset = 0x6bc
		data = self.command(struct.pack('<BBHH', self.CMD_SWIRE_READ, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
		if data == None or self.wcnt != rdsize:
			print('\rError Read SWire data! (%d)' % self.err) 
			return False
		self.ext_pc = struct.unpack('<I', data[4:8])
		rdsize = 0x1
		offset = 0x602
		data = self.command(struct.pack('<BBHH', self.CMD_SWIRE_READ, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
		if data == None or self.wcnt != rdsize:
			print('\rError Read SWire data! (%d)' % self.err) 
			return False
		print('CPU PC=0x%08x' % self.ext_pc, '([0x0602] = 0x%02x)' % data[4])
		return True
	# Dump Ext. Chip regs
	def DumpChipRegs(self, offset = 0x60, rdsize = 0x20):
		data = self.command(struct.pack('<BBHH', self.CMD_SWIRE_READ, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
		if data == None or self.wcnt != rdsize:
			print('\rError Read SWire data! (%d)' % self.err) 
			return False
		print('-------------------------------------------------------')
		print('\rREGISTERS:')
		hex_dump(offset, data[4:rdsize+4])
		return True
	# Dump Ext. Chip Flash
	def DumpChipFlash(self, offset = 0x0, rdsize = 0x20):
		data = self.command(struct.pack('<BBHH', self.CMD_FLASH_READ, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
		if data == None or self.wcnt != rdsize:
			print('\rError Read SWire data! (%d)' % self.err) 
			return False
		print('-------------------------------------------------------')
		print('\rFLASH:')
		hex_dump(offset, data[4:rdsize+4])
		return True
	# Dump Ext. Chip Ana regs
	def DumpChipARegs(self, offset = 0x0, rdsize = None):
		if not self.EnableClkALGM():
			return False
		if rdsize == None:
			if self.pgm_swaddrlen == 3:
				rdsize = 0x100
			else:
				rdsize = 0x80
		data = self.command(struct.pack('<BBHH', self.CMD_SWIRE_AREAD, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
		if data == None or self.wcnt != rdsize:
			print('\rError Read SWire data! (%d)' % self.err) 
			return False
		print('-------------------------------------------------------')
		print('\rANALOG REGISTERS:')
		hex_dump(offset, data[4:rdsize+4])
		return True
	# Dump Ext. Chip FlashUID
	def DumpChipFlashUID(self, offset = 0, rdsize = 0x20):
		rdsize +=4 # +4 dimmy
		data = self.command(struct.pack('<BBHHB', self.CMD_FLASH_WRRD, offset & 0xff, (offset>>8) & 0xffff, rdsize, 0x4B), rdsize+6)
		if data == None or self.wcnt != rdsize + 1:
			print('\rError Read Flash UID! (%d)' % self.err)
			return False
		print('-------------------------------------------------------')
		print('\rFLASH UID:')
		hex_dump(offset, data[8:rdsize+4])
		return True
	# Read Ext. Chip ID
	def ReadChipID(self):
		rdsize = 3
		offset = 0x7d
		data = self.command(struct.pack('<BBHH', self.CMD_SWIRE_READ, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
		if data == None or self.wcnt != rdsize:
			print('\rError get Chip ID! (%d)' % self.err) 
			return False
		self.ext_cver = data[4]
		self.ext_cid = data[5] | (data[6]<<8)
		if self.ext_cid == 0x5562:
			self.ext_chip = 'TLSR825x'
		elif self.ext_cid == 0x5325:
			self.ext_chip = 'TLSR8266'
		elif self.ext_cid == 0x5326:
			self.ext_chip = 'TLSR8267'
		elif self.ext_cid == 0x5327:
			self.ext_chip = 'TLSR8269'
		else:
			self.ext_chip = None
		if self.ext_chip == None:
			print('Chip ID: 0x%04X, rev: 0x%02X' % (self.ext_cid, self.ext_cver))
		else:
			print('Chip %s ID: 0x%04X, rev: 0x%02X' % (self.ext_chip, self.ext_cid, self.ext_cver))
		return True
	# Read Flash JEDEC ID
	def ReadFlashJEDECID(self):
		data = self.command(struct.pack('<BBH', self.CMD_FLASH_GET_JEDEC_ID, 0, 0), 6+3)
		if data == None or self.wcnt != 3:
			print('\rError get Flash JEDEC ID! (%d)' % self.err) 
			return False
		self.jedecid = (data[4]<<16) | (data[5]<<8) | data[6]
		print('Flash JEDEC ID: 0x%06X, Size: %d kbytes' % (self.jedecid, ((1<<data[6])>>10)))
		return True
	# Read Blocks Flash to stream
	def ReadBlockFlash(self, stream, offset = 0, size = 0x80000):
		offset &= 0xffffff
		rdsize = self.MAX_BUF_READ_SIZE
		while size > 0:
			if rdsize > size:
				rdsize = size
			print('\rRead from 0x%06x...' % offset, end = '')
			data = self.command(struct.pack('<BBHH', self.CMD_FLASH_READ, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
			if data == None or self.wcnt != rdsize:
				print('\rError Read Flash data at 0x%06x! ' % offset, end = '')
				return False
			stream.write(data[4:rdsize+4]);
			size -= rdsize
			offset += rdsize
		print('\r                               \r',  end = '')
		return True
	# Read Blocks Reg/Sram to stream
	def ReadBlockRegs(self, stream, offset = 0x40000, size = 0x10000):
		offset &= 0xffffff
		rdsize = self.MAX_BUF_READ_SIZE
		while size > 0:
			if rdsize > size:
				rdsize = size
			print('\rRead from 0x%06x...' % offset, end = '')
			data = self.command(struct.pack('<BBHH', self.CMD_SWIRE_READ, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
			if data == None or self.wcnt != rdsize:
				print('\rError Read SWire data at 0x%06x! ' % offset, end = '')
				return False
			stream.write(data[4:rdsize+4]);
			size -= rdsize
			offset += rdsize
		print('\r                               \r',  end = '')
		return True
	# Write Reg/Sram Data 
	def WriteRegsData(self, offset = 0x602, wdata = b'\x05'):
		offset &= 0xffffff
		wrsize = len(wdata)
		data = self.command(struct.pack('<BBH', self.CMD_SWIRE_WRITE, offset & 0xff, (offset>>8) & 0xffff) + wdata, 6)
		if data == None or self.wcnt != wrsize:
			print('\rError Write SWire data at 0x%06x! ' % offset, end = '')
			return False
		return True
	# Write Blocks Reg/Sram from stream
	def WriteBlockRegs(self, stream, offset = 0, size = 0):
		wrsize = self.MAX_BUF_READ_SIZE
		cnt_err = self.ERR_RETRY_COUNT
		while size > 0:
			offset &= 0xffffff
			data = stream.read(wrsize)
			wrsize = len(data)
			if not data or wrsize == 0: # end of stream
				print(' Error Read file!')
				return False
			while 1:
				print('\rWrite to 0x%06x...' % offset, end = '')
				rdata = self.command(struct.pack('<BBH', self.CMD_SWIRE_WRITE, offset & 0xff, (offset>>8) & 0xffff)+data, 6)
				if rdata == None or self.wcnt != wrsize:
					cnt_err -= 1
					if cnt_err == 0:
						print('\rError Write Swire data at 0x%06x!' % offset)
						return False
					print(' Error', cnt_err)
				else:
					cnt_err = self.ERR_RETRY_COUNT
				break
			offset += wrsize
			size -= wrsize
		print('\r                               \r',  end = '')
		return True
	# Read Blocks Analog Reg to stream
	def ReadBlockAnalogRegs(self, stream, offset = 0x000, size = None):
		if not self.EnableClkALGM():
			return False
		if size == None:
			if self.pgm_swaddrlen == 3:
				size = 0x100
			else:
				size = 0x80
		offset &= 0xff
		rdsize = self.MAX_BUF_READ_SIZE
		cnt_err = self.ERR_RETRY_COUNT
		while size > 0:
			if rdsize > size:
				rdsize = size
			print('\rRead from 0x%06x...' % offset, end = '')
			data = self.command(struct.pack('<BBHH', self.CMD_SWIRE_AREAD, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
			if data == None  or self.wcnt != rdsize:
				print('\rError Read SWire data at 0x%06x! ' % offset, end = '')
				return False
			stream.write(data[4:rdsize+4]);
			size -= rdsize
			offset += rdsize
		print('\r                               \r',  end = '')
		return True
	# Write Blocks Analog Reg from stream
	def WriteBlockAnalogRegs(self, stream, offset = 0x000, size = 0x100):
		if not self.EnableClkALGM():
			return False
		wrsize = 0x100
		cnt_err = self.ERR_RETRY_COUNT
		while size > 0:
			offset &= 0x0ff
			data = stream.read(wrsize)
			wrsize = len(data)
			if not data or wrsize == 0: # end of stream
				print(' Error Read file!')
				return False
			while 1:
				print('\rWrite to 0x%06x...' % offset, end = '')
				rdata = self.command(struct.pack('<BBH', self.CMD_SWIRE_AWRITE, offset & 0xff, (offset>>8) & 0xffff)+data, 6)
				if rdata == None or self.wcnt != wrsize:
					cnt_err -= 1
					if cnt_err == 0:
						print('\rError Write aregs data at 0x%06x!' % offset)
						return False
					print(' Error', cnt_err)
				else:
					cnt_err = self.ERR_RETRY_COUNT
				break
			offset += wrsize
			size -= wrsize
		print('\r                               \r',  end = '')
		return True
	# Read FlashUID
	def ReadFlashUID(self):
		offset = 0
		rdsize = 256+4 # 8+4
		data = self.command(struct.pack('<BBHHB', self.CMD_FLASH_WRRD, offset & 0xff, (offset>>8) & 0xffff, rdsize, 0x4B), rdsize+6)
		if data == None: # or self.wcnt+1 != rdsize:
			print('\rError Read Flash UID! (%d)' % self.err)
			return False
		self.FlashUID = data[8:16]
		print('Flash UID:' , self.FlashUID)
		hex_dump(0, data[8:264])
		return True
	# Erase Sectors Flash
	def EraseSectorsFlash(self, offset = 0, count = 1):
		cnt_err = self.ERR_RETRY_COUNT
		size = count * self.FLASH_SECTOR_SIZE
		for i in range(count):
			offset = offset & (0xffffff^(self.FLASH_SECTOR_SIZE-1))
			print('\rErase sector at 0x%06x...' % offset, end = '')
			while 1:
				data = self.command(struct.pack('<BBH', self.CMD_FLASH_SECT_ERASE, offset & 0xff, (offset>>8) & 0xffff), 6)
				if data == None:
					if not self.WaitingFlashReady():
						return False
					cnt_err -= 1
					if cnt_err == 0:
						print('\rError Erase sector at 0x%06x!' % offset)
						return False
					print(' Error', cnt_err)
				else:
					if not self.WaitingFlashReady():
						return False
					cnt_err = self.ERR_RETRY_COUNT
				break
			offset += self.FLASH_SECTOR_SIZE
		print('\r                               \r',  end = '')
		return True
	# Erase All Blocks Flash
	def EraseAllFlash(self):
		data = self.command(struct.pack('<BBH', self.CMD_FLASH_ALL_ERASE, 0, 0))
		if data == None:
			print('\rError Erase All Flash! (%d)' % self.err)
			return False
		return self.WaitingFlashReady(3000)
	# Write Blocks Flash from stream
	def WriteBlockFlash(self, stream, offset = 0, size = 0, erase = True):
		wrsize = 0x100
		cnt_err = self.ERR_RETRY_COUNT
		if erase and (offset & (self.FLASH_SECTOR_SIZE-1)) != 0:
			erasec = offset & (0xffffff^(self.FLASH_SECTOR_SIZE-1))
		else:
			erasec = 0xffffffff # = flag
		while size > 0:
			offset &= 0xffffff
			if erase:
				wrsec = offset & (0xffffff^(self.FLASH_SECTOR_SIZE-1))
				if erasec != wrsec:
					print('\rErase at 0x%06x...' % offset, end = '')
					while 1:
						data = self.command(struct.pack('<BBH', self.CMD_FLASH_SECT_ERASE, offset & 0xff, (offset>>8) & 0xffff), 6)
						if data == None:
							if not self.WaitingFlashReady():
								return False
							cnt_err -= 1
							if cnt_err == 0:
								print('\rError Erase sector at 0x%06x!' % offset)
								return 0
							print(' Error', cnt_err)
						else:
							if not self.WaitingFlashReady():
								return False
							cnt_err = self.ERR_RETRY_COUNT
						break
					erasec = wrsec
			data = stream.read(wrsize)
			wrsize = len(data)
			if not data or wrsize == 0: # end of stream
				print(' Error Read file!')
				return False
			for e in data:
				if e != 0xff:
					while 1:
						print('\rWrite to 0x%06x...' % offset, end = '')
						rdata = self.command(struct.pack('<BBH', self.CMD_FLASH_WRITE, offset & 0xff, (offset>>8) & 0xffff)+data, 6)
						if rdata == None or self.wcnt != wrsize:
							if not self.WaitingFlashReady():
								return False
							cnt_err -= 1
							if cnt_err == 0:
								print('\rError Write Flash data at 0x%06x!' % offset)
								return False
							print(' Error', cnt_err)
						else:
							if not self.WaitingFlashReady():
								return False
							cnt_err = self.ERR_RETRY_COUNT
						break
					break
			offset += wrsize
			size -= wrsize
		print('\r                               \r',  end = '')
		return True
	# Enable CLK ALGM on 825x
	def EnableClkALGM(self):
		rdsize = 4
		offset = 0x61
		data = self.command(struct.pack('<BBHH', self.CMD_SWIRE_READ, offset & 0xff, (offset>>8) & 0xffff, rdsize), rdsize+6)
		if data == None or self.wcnt != rdsize:
			print('Error Read SWire data! (%d)' % self.err) 
			return False
		x = bytearray(data[4:8])
		# TLSR825x - bit 3, TLSR826x - bit 1
		if self.pgm_swaddrlen == 3:
			msk_bit = 0x08
		else:
			msk_bit = 0x02
		if (x[0] & msk_bit) != 0 or (x[3] & msk_bit) == 0:
			x[0] &= ~msk_bit
			x[3] |= msk_bit # reg 0x64
			data = self.command(struct.pack('<BBH', self.CMD_SWIRE_WRITE, offset & 0xff, (offset>>8) & 0xffff) + x, 6)
			if data == None or self.wcnt != rdsize:
				#print('Error!') 
				print('Error Write SWire data! (%d)' % self.err) 
				print('Error Activate ALGM!')
				return False
		return True
	# Test
	def TestDebugPC(self, ttime = 1, offset = 0x6bc):
		if ttime < 1:
			ttime = 1
		flgsleep = False
		flgrun = False
		i = 0
		print('Read register addres 0x%06x:' % offset)
		wblk = struct.pack('<BBHH', self.CMD_SWIRE_READ, offset & 0xff, (offset>>8) & 0xffff, 4) 
		t1 = time.time()
		t2 = t1
		te = t1 + ttime
		while t2 < te:
			self.write(crc_blk(wblk))
			rblk = self.read(6)
			t2 = time.time()
			if rblk == None or len(rblk) < 6 or rblk[0] != wblk[0]:
				print('\r\nError Read response!') 
				return False
			self.err = rblk[1];
			self.wcnt = rblk[2] | (rblk[3]<<8)
			if self.wcnt == 4 and self.err == 0:
				rblk += self.read(4)
				if crc_chk(rblk):
					if flgsleep:
						t1 = t2
						print()
					self.ext_reg = struct.unpack('<I', rblk[4:8])
					print('\r0x%08x' % self.ext_reg, end = '')
					if flgrun:
						print(' (%.3f)' % (t2-t1), end = '')
					flgsleep = False
					flgrun = True
				else:
					print('\r\nError Read response!') 
					return False
			else:
				if flgrun:
					print()
				print('\rCPU sleep? (%.3f sec)' % (t2-t1), end = '')
				if not flgsleep:
					t1 = t2
				flgsleep = True
				flgrun = False
		print()
		return True
	# Wait CPU run
	def WaitCPU(self, twait_sec = 1, offset = 0x6bc):
		if twait_sec < 1:
			twait_sec = 1
		flgsleep = False
		wblk = struct.pack('<BBHH', self.CMD_SWIRE_READ, offset & 0xff, (offset>>8) & 0xffff, 4) 
		t1 = time.time()
		t2 = t1
		te = t1 + twait_sec
		while t2 < te:
			self.write(crc_blk(wblk))
			rblk = self.read(6)
			t2 = time.time()
			if rblk == None or len(rblk) < 6 or rblk[0] != wblk[0]:
				print('\r\nError Read response!') 
				return False
			self.err = rblk[1];
			self.wcnt = rblk[2] | (rblk[3]<<8)
			if self.wcnt == 4 and self.err == 0:
				rblk += self.read(4)
				if crc_chk(rblk):
					if flgsleep:
						t1 = t2
						print()
					self.ext_pc = struct.unpack('<I', rblk[4:8])
					print('\rCPU PC=0x%08x' % self.ext_pc)
					return True
				else:
					print('\r\nError Read response!') 
					return False
			else:
				print('\rCPU sleep? (%.3f sec)' % (t2-t1), end = '')
				if not flgsleep:
					t1 = t2
				flgsleep = True
		print()
		return False
	def FlashUnlock(self):
		print('Write 0 to Flash Status Register...')
		if self.WriteFlashStatus(0) == None:
			return None
		ret = self.ReadFlashStatus()
		if ret == None or ret != 0:
			print('\r\nError unlock Flash!') 
			return None
		return ret
#============================= 
# main()
#============================= 
def main():
	# First time slice
	t1 = time.time()
	signal.signal(signal.SIGINT, signal_handler)
	comport_def_name='COM1'
	if sys.platform == "linux" or sys.platform == "linux2":
		comport_def_name = '/dev/ttyS0'
	elif sys.platform == "win32":
		comport_def_name='COM1'
	#elif sys.platform == "darwin":
	#else:
	#	print(sys.platform)
	#
	parser = argparse.ArgumentParser(description='%s version %s' % (__progname__, __version__), prog=__filename__)
	parser.add_argument(
		'-p', '--port', 
		help='Serial port device (default: %s)' % comport_def_name,
		default=comport_def_name)
	parser.add_argument(
		'-b', '--baud',
		help='UART Baud Rate (default: %d)' % DEFAULT_UART_BAUD,
		type=arg_auto_int,
		default=DEFAULT_UART_BAUD)
	parser.add_argument(
		'-t', '--trst',
		help='Hard Reset Time ms (default: 0 ms)',
		type=arg_auto_int,
		default = 0)
	parser.add_argument(
		'-a', '--act',
		help='Activation Time ms (0-off, default: 0 ms)',
		type=arg_auto_int,
		default = 0)
	parser.add_argument(
		'-z', '--zw',
		help='Wait Start CPU (default: 0 sec) (before main processing)',
		type=arg_auto_int,
		default = 0)
	parser.add_argument(
		'-s', '--stopcpu',
		help='CPU Stop (before main processing)',
		action="store_true")
	parser.add_argument(
		'-c', '--cpustall',
		help='CPU Stall (before main processing)',
		action="store_true")
	parser.add_argument(
		'-r', '--run',
		help='CPU Run (post main processing)',
		action="store_true")
	parser.add_argument(
		'-g', '--go',
		help='CPU Go (post main processing)',
		action="store_true")
	parser.add_argument(
		'-m', '--mrst',
		help='Soft Reset (MCU Reboot) (post main processing)',
		action="store_true")
	parser.add_argument(
		'-w', '--wrktime',
		help='Show Worked Time',
		action="store_true")
	parser.add_argument(
		'-u', '--u2b',
		help='Use 2 bytes swire address (TLSR826x)',
		action="store_true")
	subparsers = parser.add_subparsers(
			dest='operation',
			help='Run '+__filename__+' {command} -h for additional help')

	parser_read_flash = subparsers.add_parser(
			'rf',
			help='Read Flash to binary file')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('size', help='Size of region', type=arg_auto_int)
	parser_read_flash.add_argument('filename', help='Name of binary file')

	parser_burn_flash = subparsers.add_parser(
			'we',
			help='Write file to Flash with sectors erases')
	parser_burn_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_burn_flash.add_argument('filename', help='Name of binary file')

	parser_write_flash = subparsers.add_parser(
			'wf',
			help='Write file to Flash without sectors erases')
	parser_write_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_write_flash.add_argument('filename', help='Name of binary file')

	parser_erase_sec_flash = subparsers.add_parser(
			'es',
			help='Erase Region (sectors) of Flash')
	parser_erase_sec_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_erase_sec_flash.add_argument('size', help='Size of region', type=arg_auto_int)

	parser_erase_all_flash = subparsers.add_parser(
			'ea',
			help='Erase All Flash')
	parser_read_fstatus = subparsers.add_parser(
			'fsr',
			help='Read Flash Status Register')
	parser_write_fstatus = subparsers.add_parser(
			'fsw',
			help='Write Flash Status Register')
	parser_write_fstatus.add_argument('fsreg', help='Register Value (byte)', type=arg_auto_int)

	parser_read_flash = subparsers.add_parser(
			'rs',
			help='Read SWire address to binary file')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('size', help='Size of region', type=arg_auto_int)
	parser_read_flash.add_argument('filename', help='Name of binary file')

	parser_write_flash = subparsers.add_parser(
			'ws',
			help='Write file to Swire addres')
	parser_write_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_write_flash.add_argument('filename', help='Name of binary file')

	parser_read_flash = subparsers.add_parser(
			'ra',
			help='Read Analog Registers to binary file')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('size', help='Size of region', type=arg_auto_int)
	parser_read_flash.add_argument('filename', help='Name of binary file')

	parser_read_flash = subparsers.add_parser(
			'wa',
			help='Write file to Analog Registers')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('filename', help='Name of binary file')

	parser_read_flash = subparsers.add_parser(
			'i', help='External Chip Information')
	parser_read_flash = subparsers.add_parser(
			'ds',
			help='Dump Swire (Registers or SRAM)')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('size', help='Size of region', type=arg_auto_int)
	parser_read_flash = subparsers.add_parser(
			'df',
			help='Dump Flash')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('size', help='Size of region', type=arg_auto_int)
	parser_read_flash = subparsers.add_parser(
			'da',
			help='Dump Analog Registers')
	parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
	parser_read_flash.add_argument('size', help='Size of region', type=arg_auto_int)
	parser_read_flash = subparsers.add_parser(
			'dc',
			help='Show uit32 register or SRAM addres')
	parser_read_flash.add_argument('address', help='address (PC - 0x6bc)', type=arg_auto_int)
	parser_read_flash.add_argument('time', help='time (sec)', type=arg_auto_int)

	args = parser.parse_args()
	print('=======================================================')
	print('%s version %s' % (__progname__, __version__))
	print('-------------------------------------------------------')
	pgm = TLSRPGM(args.port, DEFAULT_UART_BAUD)
	if pgm.port == None:
		sys.exit(1)
	if not pgm.GetVersion():
		sys.exit(1)
	if args.u2b:
		if pgm.pgm_swaddrlen != 2:
			if not pgm.SetPgmConfig(swdiv = int(round(pgm.pgm_clk/4.8,0)), swaddrlen=2, swbuf = b'\x5a\x00\x06\x02\x00\x05'):
				pgm.close()
				sys.exit(1)
	else:
		if pgm.pgm_swaddrlen != 3:
			if not pgm.SetPgmConfig(swdiv = int(round(pgm.pgm_clk/4.8,0)), swaddrlen=3, swbuf = b'\x5a\x00\x06\x02\x00\x05'):
				pgm.close()
				sys.exit(1)
	# set speed up?
	if args.baud != DEFAULT_UART_BAUD:
		if not pgm.SetUartBaud(args.baud):
			sys.exit(1)
	if args.trst > 0 or args.act != 0 or args.stopcpu or args.cpustall or args.zw:
		print('=== PreProcess ========================================')
	if args.zw: # Wait CPU run
		if not pgm.WaitCPU(args.zw):
			pgm.close()
			sys.exit(1)
	if args.trst > 0: # Hard reset (Pin RST set '0')?	
		if args.trst < 5: # min 5 ms
			args.trst = 5
		print('Hard reset Ext.MCU %d ms...' % (args.trst), end = ' ')
		if not pgm.SetPinRST(0): # Hard reset (Pin RST set '0')
			pgm.close()
			sys.exit(1)
		time.sleep(args.trst/1000) # Wait trst ms
		print('ok')
		if args.act > 0: # activate ?
			if not pgm.Activate(args.act):
				pgm.close()
				sys.exit(1)
		else: # no activate
			if not pgm.SetPinRST(1):
				pgm.close()
				sys.exit(1)
			time.sleep(0.07) # wait ext. MCU init
	else:
		if args.act > 0:
			print('Hard reset Ext.MCU 50 ms...', end = ' ')
			if not pgm.SetPinRST(0):
				pgm.close()
				sys.exit(1)
			time.sleep(0.05)
			print('ok')
			if not pgm.Activate(args.act):
				pgm.close()
				sys.exit(1)
	if args.act < 0: # No Hard reset (Pin RST set '0'), MCU Soft Reset + Activate 70 ms
		print('MCU Reboot...', end = ' ')
		if not pgm.WriteRegsData(0x6f, b'\x20'):
			pgm.close()
			sys.exit(1)
		print('ok')
		if not pgm.Activate(70): # Activate 70 ms
			pgm.close()
			sys.exit(1)
	if args.stopcpu:	# CPU Stop ?
		print('CPU Stop...', end = ' ')
		if not pgm.WriteRegsData(0x602, b'\x05'):
			pgm.close()
			sys.exit(1)
		print('ok')
	if args.cpustall:	# CPU Stall ?
		print('CPU Stall...', end = ' ')
		if not pgm.WriteRegsData(0x602, b'\x06'):
			pgm.close()
			sys.exit(1)
		print('ok')
	# Command Read to File
	print('=== Process ===========================================')
	if args.operation == 'rs' \
	or args.operation == 'rf' \
	or args.operation == 'ra':
		offset = args.address & 0x00ffffff
		size = args.size & 0x00ffffff
		if size == 0:
			print('\rError: Read size = %d!' % size)
			pgm.close()
			sys.exit(1)
		print('Outfile: %s' % args.filename)
		try:
			stream = open(args.filename, 'wb')
		except:
			print('Error: Not open Outfile file <%s>!' % args.filename)
			pgm.close()
			sys.exit(1)
		ret = False
		if args.operation == 'rs':
			print('Read SWire from 0x%06x to 0x%06x...' % (offset, offset + size))
			ret = pgm.ReadBlockRegs(stream, offset, size)
		elif args.operation == 'rf':
			print('Read Flash from 0x%06x to 0x%06x...' % (offset, offset + size))
			ret = pgm.ReadBlockFlash(stream, offset, size)
		elif args.operation == 'ra':
			if size > 0x100 or offset + size > 0x100:
				print('Warning: Maximum number of analog registers 256!')
				if size > 0x100:
					size = 0x100
			offset &= 0x00ff
			print('Read Analog Registers from 0x%06x to 0x%06x...' % (offset, offset + size))
			ret = pgm.ReadBlockAnalogRegs(stream, offset, size)
		stream.close
		if not ret:
			pgm.close()
			sys.exit(1)
	# Command Write from File
	elif args.operation == 'ws' \
	  or args.operation == 'wf' \
	  or args.operation == 'we' \
	  or args.operation == 'wa':
		offset = args.address & 0x00ffffff
		print('Inputfile: %s' % (args.filename))
		try:
			stream = open(args.filename, 'rb')
			size = os.path.getsize(args.filename)
		except:
			print('Error: Not open input file <%s>!' % args.fldr)
			pgm.close()
			sys.exit(1)
		ret = False
		if size < 1:
			print('Error: File size = %d!'% size)
		elif args.operation == 'ws':
			print('Write Swire register 0x%08x to 0x%08x...' % (offset, offset + size))
			ret = pgm.WriteBlockRegs(stream, offset, size)
		elif args.operation == 'wf' or args.operation == 'we':
			if pgm.FlashUnlock() == None:
				pgm.close()
				sys.exit(1)
			print('Write Flash data 0x%08x to 0x%08x...' % (offset, offset + size))
			ret = pgm.WriteBlockFlash(stream, offset, size, bool(args.operation == 'we'))
		elif args.operation == 'wa':
			if size > 0x100 or offset + size > 0x100:
				print('Warning: Maximum number of analog registers 256!')
				if size > 0x100:
					size = 0x100
			offset &= 0x00ff
			print('Write Analog register 0x%08x to 0x%08x...' % (offset, offset + size))
			ret = pgm.WriteBlockAnalogRegs(stream, offset, size)
		stream.close
		if not ret:
			pgm.close()
			sys.exit(1)
	elif args.operation == 'es':
		if pgm.FlashUnlock() == None:
			pgm.close()
			sys.exit(1)
		count = int((args.size + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE)
		size = (count * FLASH_SECTOR_SIZE)
		offset = args.address & (0xffffff^(FLASH_SECTOR_SIZE-1))
		print('Erase Flash %d sectors,\r\ndata from 0x%06x to 0x%06x ...' % (count, offset, offset + size))
		if not pgm.EraseSectorsFlash(offset,count):
			pgm.close()			
			sys.exit(1)
	elif args.operation == 'ea':
		if pgm.FlashUnlock() == None:
			pgm.close()
			sys.exit(1)
		print('Erase All Flash ...')
		if not pgm.EraseAllFlash():
			pgm.close()
			sys.exit(1)
	elif args.operation == 'fsr':
		print('Read Flash Status Register...')
		ret = pgm.ReadFlashStatus()
		if ret == None:
			pgm.close()
			sys.exit(1)
	elif args.operation == 'fsw':
		print('Write 0x%02x to Flash Status Register...' % args.fsreg)
		ret = pgm.WriteFlashStatus(args.fsreg)
		if ret == None:
			pgm.close()
			sys.exit(1)
		ret = pgm.ReadFlashStatus()
		if ret == None:
			pgm.close()
			sys.exit(1)
	elif args.operation == 'i':
		if not pgm.ReadChipID() \
		or not pgm.ReadPCcpu() \
		or not pgm.ReadFlashJEDECID() \
		or pgm.ReadFlashStatus() == None \
		or not pgm.DumpChipFlashUID() \
		or not pgm.DumpChipFlash() \
		or not pgm.DumpChipARegs() \
		or not pgm.DumpChipRegs():
			pgm.close()
			sys.exit(1)
	elif args.operation == 'ds':
		if not pgm.DumpChipRegs(args.address, args.size):
			pgm.close()
			sys.exit(1)
	elif args.operation == 'df':
		if not pgm.DumpChipFlash(args.address, args.size):
			pgm.close()
			sys.exit(1)
	elif args.operation == 'da':
		if not pgm.DumpChipARegs(args.address, args.size):
			pgm.close()
			sys.exit(1)
	elif args.operation == 'dc':
		#print('CPU Run...', end = ' ')
		#if not pgm.WriteRegsData(0x602, b'\x88'):
		#	pgm.close()
		#	sys.exit(1)
		#print('ok')
		if not pgm.TestDebugPC(args.time, args.address):
			pgm.close()
			sys.exit(1)
	else:
		print('No action assigned.')
	if args.run or args.go or args.mrst:
		print('=== Post-Process ======================================')
	# Commands / flags post main processing
	if args.run:
		print('CPU Run...', end = ' ')
		if not pgm.WriteRegsData(0x602, b'\x88'):
			pgm.close()
			sys.exit(1)
		print('ok')
	if args.go:
		print('CPU Go...', end = ' ')
		if not pgm.WriteRegsData(0x602, b'\x08'):
			pgm.close()
			sys.exit(1)
		print('ok')
	if args.mrst:
		print('MCU Reboot...', end = ' ')
		if not pgm.WriteRegsData(0x6f, b'\x20'):
			pgm.close()
			sys.exit(1)
		print('ok')
	if args.wrktime:
		# Second time slice
		t2 = time.time()
		print(" Worked Time: %.3f sec" % (t2-t1))
	pgm.close()
	sys.exit(0)

if __name__ == '__main__':
	main()
