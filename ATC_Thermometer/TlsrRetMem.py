#!/usr/bin/env python
 
### TlsrMemInfo.py ###
###  Autor: pvvx   ###
 
import sys
import signal
import struct
import platform
import time
import argparse
import subprocess
import os
import io

__progname__ = "TLSR825x RetentionMemInfo"
__filename__ = "TlsrRetMemInfo"
__version__ = "12.11.20"

SRAM_BASE_ADDR = 0x840000

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
	
class ELFFile:

	def __init__(self, name):
		self.name = name
		self.symbols = {}
		try:
			tool_nm = "tc32-elf-nm"
			#if sys.platform == 'linux2':
			#	tool_nm = "tc32-elf-nm"
			proc = subprocess.Popen([tool_nm, self.name], stdout=subprocess.PIPE)
		except OSError:
			print("Error calling " + tool_nm + ", do you have toolchain in PATH?")
			sys.exit(1)
		for l in proc.stdout:
			fields = l.strip().split()
			try:
				if fields[0] == b"U":
					#print("Warning: Undefined symbol '%s'!" %(fields[1].decode('ASCII')))
					continue
				if fields[0] == b"w":
					continue  # can skip weak symbols
				self.symbols[fields[2]] = int(fields[0], 16)
			except ValueError:
				raise FatalError("Failed to strip symbol output from nm: %s" % fields)

	def get_symbol_addr(self, sym, default = 0):
		try:
			x = self.symbols[sym]
		except:
			return default
		return x

def main():

	signal.signal(signal.SIGINT, signal_handler)
	parser = argparse.ArgumentParser(description='%s version %s' % (__progname__, __version__), prog=__filename__)
	parser.add_argument('elffname', help='Name of elf file')		
	args = parser.parse_args()

	e = ELFFile(args.elffname);
	rrs = e.get_symbol_addr(b"_retention_data_end_");
	if rrs > 0:
		rrs = (rrs + 255) & 0x0001ff00;
		print(". = 0x%x;" % rrs);
	else:
		print(". = 0x8000;");
	sys.exit(0);
	

if __name__ == '__main__':
	main()
