#ifndef _I2C_H_
#define _I2C_H_

#ifndef I2C_GROUP
#define init_i2c() soft_i2c_stop()
#else
void init_i2c();
int send_i2c_addr_word(u8 i2c_addr, u32 ra_w);
#endif
//void send_i2c(u8 device_id, u8 *buffer, int dataLen);
int scan_i2c_addr(int address);
int send_i2c_byte(u8 i2c_addr, u8 cmd);
int send_i2c_word(u8 i2c_addr, u16 cmd);
int send_i2c_buf(u8 i2c_addr, u8 * dataBuf, u32 dataLen);
int read_i2c_buf(u8 i2c_addr, u8 * dataBuf, u32 dataLen);
int read_i2c_byte_addr(u8 i2c_addr, u8 reg_addr, u8 * dataBuf, u32 dataLen);

/* Universal I2C/SMBUS read-write transaction struct */
typedef struct _i2c_utr_t {
	unsigned char mode;  // bit0..6: The byte number of the record for the new START (bit7: =1 - generate STOP/START)
	unsigned char rdlen; // bit0..6: Number of bytes read (bit7: =0 - the last byte read generates NACK, =1 - ACK)
	unsigned char wrdata[1]; // Array, the first byte is the address, then the bytes to write to the bus: i2c_addr_wr, wr_byte1, wr_byte2, wr_byte3, ... wr_byte126
} i2c_utr_t;
int I2CBusUtr(void * outdata, i2c_utr_t * tr, u32 wrlen);

#endif //_I2C_H_
