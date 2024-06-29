#pragma once

#include <stdint.h>

void init_i2c();
//void send_i2c(uint8_t device_id, uint8_t *buffer, int dataLen);
int scan_i2c_addr(int address);
int send_i2c_byte(uint8_t i2c_addr, uint8_t cmd);
int send_i2c_word(uint8_t i2c_addr, uint16_t cmd);
int send_i2c_buf(uint8_t i2c_addr, uint8_t * dataBuf, uint32_t dataLen);
int read_i2c_buf(uint8_t i2c_addr, uint8_t * dataBuf, uint32_t dataLen);
int read_i2c_byte_addr(uint8_t i2c_addr, uint8_t reg_addr, uint8_t * dataBuf, uint32_t dataLen);

/* Universal I2C/SMBUS read-write transaction struct */
typedef struct _i2c_utr_t {
	unsigned char mode;  // bit0..6: The byte number of the record for the new START (bit7: =1 - generate STOP/START)
	unsigned char rdlen; // bit0..6: Number of bytes read (bit7: =0 - the last byte read generates NACK, =1 - ACK)
	unsigned char wrdata[1]; // Array, the first byte is the address, then the bytes to write to the bus: i2c_addr_wr, wr_byte1, wr_byte2, wr_byte3, ... wr_byte126
} i2c_utr_t;
int I2CBusUtr(void * outdata, i2c_utr_t *tr, unsigned int wrlen);




