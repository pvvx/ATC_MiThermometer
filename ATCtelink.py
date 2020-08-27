#!/usr/bin/env python
 
###   Test.py                ###
###    Autor: pvvx           ###
###    Edited: Aaron Christophel ATCnetz.de    ###
 
import sys
import struct
import serial
import platform
import time
import argparse
import os
import io
import serial.tools.list_ports

ports = serial.tools.list_ports.comports()

usedCom = ports[0].device#select the first available comport
usedBaud =230400 #921600
resetTime = 1
print('Using port: ' + usedCom)

serialPort = serial.Serial(usedCom, usedBaud, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
serialPort.timeout = 100*12/usedBaud   

def sws_code_blk(blk):
	pkt=[]
	d = bytearray([0xd8,0xdf,0xdf,0xdf,0xdf])
	for el in blk:
		if (el & 0x80) != 0:
			d[0] &= 0x1f
		if (el & 0x40) != 0:
			d[1] &= 0xf8
		if (el & 0x20) != 0:
			d[1] &= 0x1f
		if (el & 0x10) != 0:
			d[2] &= 0xf8
		if (el & 0x08) != 0:
			d[2] &= 0x1f
		if (el & 0x04) != 0:
			d[3] &= 0xf8
		if (el & 0x02) != 0:
			d[3] &= 0x1f
		if (el & 0x01) != 0:
			d[4] &= 0xf8
		pkt += d 
		d = bytearray([0xdf,0xdf,0xdf,0xdf,0xdf])
	return pkt
	
def sws_rd_addr(addr):
	return sws_code_blk(bytearray([0x5a,0x00, (addr>>8)&0xff, addr & 0xff, 0x80]))
def sws_code_end():
	return sws_code_blk([0xff])
def sws_wr_addr(addr, data):
	return sws_code_blk(bytearray([0x5a,0x00, (addr>>8)&0xff, addr & 0xff, 0x00]) + bytearray(data)) + sws_code_blk([0xff])

def flash_send_cmds(data):
    serialPort.write(sws_wr_addr(0x0d, bytearray([0x00])))#cns low
    for x in range(len(data)):
        serialPort.write(sws_wr_addr(0x0c, bytearray([data[x]])))
    serialPort.write(sws_wr_addr(0x0d, bytearray([0x01])))#cns high
	
def flash_write_enable():
    flash_send_cmds([0x06])
    
def flash_write_addr(addr, data):
    flash_send_cmds(bytearray([0x02,(addr>>16)&0xffff,(addr>>8)&0xff,addr & 0xff])+ bytearray(data))  

def flash_erase_sector(addr):
    flash_send_cmds([0x20,(addr>>16)&0xffff,(addr>>8)&0xff,addr & 0xff])
	
def flash_erase_all():
    flash_send_cmds([0x60])
	
serialPort.setDTR(True)
serialPort.setRTS(True)
time.sleep(1)
serialPort.setDTR(False)
serialPort.setRTS(False)


blk = sws_wr_addr(0x0602, bytearray([0x85]))# Stop CPU
t1 = time.time()

while time.time()-t1 < resetTime:#Seding Activate MSG
	for i in range(5):
		serialPort.write(blk)
	serialPort.reset_input_buffer()
time.sleep(10*12/usedBaud)
while len(serialPort.read(1000)):
    continue
    
serialPort.write(sws_wr_addr(0x00b2, [55]))# Set SWS Speed


while len(serialPort.read(1000)):
    continue

serialPort.write(sws_rd_addr(0x00b2))
while len(serialPort.read(1000)):
    continue
serialPort.write([0xff])
read = serialPort.read(500)
serialPort.write(sws_code_end())

print(len(read)/5)
print(read)
#to this point its just getting the connecting and testing it

#From here on the flashing starts

flash_write_enable()
#flash_erase_sector(0x000000)
flash_erase_all()
time.sleep(15)

bytes = open(sys.argv[1], 'rb').read()

i = len(bytes)
curr_addr = 0
pkt_length = 0
while curr_addr < len(bytes):
    outbuffer = bytearray(0x100)
    if i >= 0x100:
        pkt_length = 0x100
    else:
        pkt_length = i        
    for x in range(pkt_length):
        outbuffer[x] = bytes[curr_addr + x]
    flash_write_enable()
    flash_write_addr(curr_addr, outbuffer)
    print('now at: '+hex(curr_addr)+' from: '+hex(len(bytes)))
    curr_addr += pkt_length
    i -= pkt_length
    
#serialPort.write(sws_wr_addr(0x0602, [0x88]))  # Run CPU
serialPort.write(sws_wr_addr(0x006f, [0x22]))  # Reset CPU

print('This took: '+ str(time.time()-t1))

serialPort.close