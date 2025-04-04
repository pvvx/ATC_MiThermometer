#include "tl_common.h"
#ifdef I2C_GROUP
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "i2c.h"
#include "app.h"
#include "drivers/8258/gpio_8258.h"

_attribute_ram_code_
void init_i2c(void) {
	i2c_gpio_set(I2C_GROUP); // I2C_GPIO_GROUP_C0C1, I2C_GPIO_GROUP_C2C3, I2C_GPIO_GROUP_B6D7, I2C_GPIO_GROUP_A3A4
#ifdef  I2C_MAX_SPEED
#if I2C_MAX_SPEED < 400000
	reg_i2c_speed = CLOCK_SYS_CLOCK_HZ/(4*I2C_MAX_SPEED); // ? kHz
#else
	reg_i2c_speed = CLOCK_SYS_CLOCK_HZ/(4*400000); // 400 kHz
#endif
#endif
    //reg_i2c_id  = slave address
    reg_i2c_mode |= FLD_I2C_MASTER_EN; //enable master mode
	reg_i2c_mode &= ~FLD_I2C_HOLD_MASTER; // Disable clock stretching for Sensor

    reg_clk_en0 |= FLD_CLK0_I2C_EN;    //enable i2c clock
    reg_spi_sp  &= ~FLD_SPI_ENABLE;   //force PADs act as I2C; i2c and spi share the hardware of IC
}

int scan_i2c_addr(int address) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
		init_i2c();
	u8 r = reg_i2c_speed;
	reg_i2c_speed = (u8)(CLOCK_SYS_CLOCK_HZ/(4*100000)); // 100 kHz
	reg_i2c_id = (u8) address;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	reg_i2c_speed = r;
	return ((reg_i2c_status & FLD_I2C_NAK)? 0 : address);
}

/* send_i2c_byte() return: NAK (=0 - send ok) */
int send_i2c_byte(u8 i2c_addr, u8 cmd) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = i2c_addr;
	reg_i2c_adr = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	return (reg_i2c_status & FLD_I2C_NAK);
}

/* send_i2c_word() return: NAK (=0 - send ok)
* cmd - [0:7] addr, [8:16] cmd -> send: [addr], [7:0], [15:8] */
int send_i2c_word(u8 i2c_addr, u16 cmd) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = i2c_addr;
	reg_i2c_adr_dat = cmd;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_DO | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	return (reg_i2c_status & FLD_I2C_NAK);
}

/* send_i2c_addr_word() return: NAK (=0 - send ok)
 * ra_w - [0:7] addr, [8:24] word -> send: [addr], [7:0], [15:8], [23:16] */
int send_i2c_addr_word(u8 i2c_addr, u32 ra_w) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	reg_i2c_id = i2c_addr;
	reg_i2c_dat_ctrl = (ra_w & 0x00ffffff)
	| ((FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR | FLD_I2C_CMD_DO | FLD_I2C_CMD_DI | FLD_I2C_CMD_STOP) << 24);
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	return (reg_i2c_status & FLD_I2C_NAK);
}


/* send_i2c_buf() return: NAK (=0 - send ok) */
int send_i2c_buf(u8 i2c_addr, u8 * dataBuf, u32 dataLen) {
	int err = 0;
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	u8 * p = dataBuf;
	reg_i2c_id = i2c_addr;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	err = reg_i2c_status & FLD_I2C_NAK;
	if(!err) {
		while (dataLen--) {
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
			err = reg_i2c_status & FLD_I2C_NAK;
			if(err)
				break;
		}
	}
	reg_i2c_ctrl = FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	return err;
}

/* read_i2c_byte_addr() return: NAK (=0 - read ok) */
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
_attribute_ram_code_
#endif
int read_i2c_byte_addr(u8 i2c_addr, u8 reg_addr, u8 * dataBuf, u32 dataLen) {
	int ret = -1;
	int size = dataLen;
	u8 *p = dataBuf;
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	//u8 r = irq_disable();
	reg_i2c_id = i2c_addr;
	reg_i2c_adr = reg_addr;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_ADDR;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	ret = reg_i2c_status & FLD_I2C_NAK;
	if (ret == 0 && size) {
		// Start By Master Read, Write Slave Address, Read data first byte
		reg_rst0 = FLD_RST0_I2C;
		reg_rst0 = 0;
		reg_i2c_id = i2c_addr | FLD_I2C_WRITE_READ_BIT;
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID; // | FLD_I2C_CMD_READ_ID;
	    while(reg_i2c_status & FLD_I2C_CMD_BUSY);
		ret = reg_i2c_status & FLD_I2C_NAK;
		if(ret == 0) {
			while(size--) {
				if(!size)
					reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
				else
					reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
				while(reg_i2c_status & FLD_I2C_CMD_BUSY);
				*p++ = reg_i2c_di;
			}
		}
	}
	reg_i2c_ctrl = FLD_I2C_CMD_STOP;
    while(reg_i2c_status & FLD_I2C_CMD_BUSY);
    //irq_restore(r);
    return ret;
}

_attribute_ram_code_
int read_i2c_buf(u8 i2c_addr, u8 * dataBuf, u32 dataLen) {
	int ret = -1;
	int size = dataLen;
	u8 *p = dataBuf;
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	//u8 r = irq_disable();
	reg_i2c_id = i2c_addr | FLD_I2C_WRITE_READ_BIT;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID; // | FLD_I2C_CMD_READ_ID;
    while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	ret = reg_i2c_status & FLD_I2C_NAK;
	if(ret == 0) {
		while(size--) {
			if(!size)
				reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
			else
				reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while(reg_i2c_status & FLD_I2C_CMD_BUSY);
			*p++ = reg_i2c_di;
		}
	}
	reg_i2c_ctrl = FLD_I2C_CMD_STOP;
    while(reg_i2c_status & FLD_I2C_CMD_BUSY);
    //irq_restore(r);
    return ret;
}


/* Universal I2C/SMBUS read-write transaction
 * wrlen = 0..127 !
 * return NAK (=0 - ok) */
int I2CBusUtr(void * outdata, i2c_utr_t * tr, u32 wrlen) {
	u8 * pwrdata = (u8 *) &tr->wrdata;
	u8 * poutdata = (u8 *) outdata;
	u32 cntstart = wrlen - (tr->mode & 0x7f);
	u32 rdlen = tr->rdlen & 0x7f;

	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
	u8 r = irq_disable();

	reg_i2c_id = *pwrdata++; // or (wrlen == 0 && rdlen)
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);

	int ret = reg_i2c_status & FLD_I2C_NAK;
	while(ret == 0 && wrlen) {
		// write data
		reg_i2c_do = *pwrdata++;
		reg_i2c_ctrl = FLD_I2C_CMD_DO;
		while(reg_i2c_status & FLD_I2C_CMD_BUSY);
		ret = reg_i2c_status & FLD_I2C_NAK;

		if(--wrlen == cntstart && ret == 0) { // + send start & id
			if(tr->mode & 0x80) {
				reg_i2c_ctrl = FLD_I2C_CMD_STOP;
			} else {
				// hw reset I2C
				reg_rst0 = FLD_RST0_I2C;
				reg_rst0 = 0;
			}
			while(reg_i2c_status & FLD_I2C_CMD_BUSY);
			reg_i2c_id = tr->wrdata[0] |  FLD_I2C_WRITE_READ_BIT;
			// start + id
			reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID;
			while(reg_i2c_status & FLD_I2C_CMD_BUSY);
			ret = reg_i2c_status & FLD_I2C_NAK;
		}
	}
	if(ret == 0) {
		while(rdlen) {
			if(--rdlen == 0 && (tr->rdlen & 0x80) == 0)
				reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID | FLD_I2C_CMD_ACK;
			else
				reg_i2c_ctrl = FLD_I2C_CMD_DI | FLD_I2C_CMD_READ_ID;
			while(reg_i2c_status & FLD_I2C_CMD_BUSY);
			*poutdata++ = reg_i2c_di;
		}
	}
	reg_i2c_ctrl = FLD_I2C_CMD_STOP; // launch start/stop cycle
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	irq_restore(r);
	return ret;
}
#else

#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "i2c.h"
#include "app.h"
#include "drivers/8258/gpio_8258.h"


#if I2C_MAX_SPEED < 200000
#define I2C_TCLK_US	32 // 2 us (in 1/16 us)
#else
#define I2C_TCLK_US	10 // 0.625 us (in 1/16 us)
#endif

_attribute_ram_code_ void sleep_us16(unsigned int us16)
{
	unsigned int t = reg_system_tick;
	while((unsigned int)(reg_system_tick - t) < us16) {
	}
}


void soft_i2c_start(void) {
	gpio_set_output_en(I2C_SCL, 0); // SCL set "1"
	gpio_set_output_en(I2C_SDA, 0); // SDA set "1"
	sleep_us16(I2C_TCLK_US);
	gpio_set_output_en(I2C_SDA, 1); // SDA set "0"
	sleep_us16(I2C_TCLK_US);
	gpio_set_output_en(I2C_SCL, 1); // SCL set "0"
	//sleep_us16(10);
}

void soft_i2c_stop(void) {
	gpio_set_output_en(I2C_SDA, 1); // SDA set "0"
	sleep_us16(I2C_TCLK_US);
	gpio_set_output_en(I2C_SCL, 0); // SCL set "1"
	sleep_us16(I2C_TCLK_US);
	gpio_set_output_en(I2C_SDA, 0); // SDA set "1"
}

int soft_i2c_wr_byte(u8 b) {
	int ret, i = 8;
	while(i--) {
		sleep_us16(I2C_TCLK_US/2);
		if(b & 0x80)
			gpio_set_output_en(I2C_SDA, 0); // SDA set "1"
		else
			gpio_set_output_en(I2C_SDA, 1); // SDA set "0"
		sleep_us16(I2C_TCLK_US/2);
		gpio_set_output_en(I2C_SCL, 0); // SCL set "1"
		sleep_us16(I2C_TCLK_US);
		gpio_set_output_en(I2C_SCL, 1); // SCL set "0"
		b <<= 1;
	}
	sleep_us16(I2C_TCLK_US/2);
	gpio_set_output_en(I2C_SDA, 0); // SDA set "1"
	sleep_us16(I2C_TCLK_US/2);
	gpio_set_output_en(I2C_SCL, 0); // SCL set "1"
	sleep_us16(I2C_TCLK_US);
	ret = gpio_read(I2C_SDA);
	gpio_set_output_en(I2C_SCL, 1); // SCL set "0"
	return ret;
}

u8 soft_i2c_rd_byte(int ack) {
	u8 ret = 0, i = 8;
	gpio_set_output_en(I2C_SDA, 0); // SDA set "1"
	while(i--) {
		sleep_us16(I2C_TCLK_US);
		gpio_set_output_en(I2C_SCL, 0); // SCL set "1"
		sleep_us16(I2C_TCLK_US);
		ret <<= 1;
		if(gpio_read(I2C_SDA)) {
			ret |= 1;
		}
		gpio_set_output_en(I2C_SCL, 1); // SCL set "0"
	}
	sleep_us16(I2C_TCLK_US/2);
	gpio_set_output_en(I2C_SDA, ack); // SDA set "0"
	sleep_us16(I2C_TCLK_US/2);
	gpio_set_output_en(I2C_SCL, 0); // SCL set "1"
	sleep_us16(I2C_TCLK_US);
	gpio_set_output_en(I2C_SCL, 1); // SCL set "0"
	gpio_set_output_en(I2C_SDA, 0); // SDA set "1"
	return ret;
}

int scan_i2c_addr(int addr) {
	int ret = 0;
	soft_i2c_start();
	if(soft_i2c_wr_byte(addr) == 0)
		ret = addr;
	soft_i2c_stop();
	return ret;
}

int send_i2c_byte(u8 addr, u8 b) {
	int ret;
	soft_i2c_start();
	ret = soft_i2c_wr_byte(addr);
	if(ret == 0)
		ret = soft_i2c_wr_byte(b);
	soft_i2c_stop();
	return ret;
}

int send_i2c_word(u8 addr, unsigned short w) {
	int ret;
	soft_i2c_start();
	ret = soft_i2c_wr_byte(addr);
	if(ret == 0) {
		ret = soft_i2c_wr_byte((u8)(w));
		if(ret == 0)
			ret = soft_i2c_wr_byte((u8) (w >> 8));
	}
	soft_i2c_stop();
	return ret;
}


int read_i2c_byte(u8 addr) {
	int ret = -1;
	soft_i2c_start();
	if(soft_i2c_wr_byte(addr | 1) == 0)
		ret = soft_i2c_rd_byte(0);
	soft_i2c_stop();
	return ret;
}

int send_i2c_buf(u8 addr, u8 * pbuf, u32 size) {
	int ret = 0;
	soft_i2c_start();
	ret = soft_i2c_wr_byte(addr);
	if(ret == 0) {
		while(size--) {
			ret = soft_i2c_wr_byte(*pbuf);
			if(ret)
				break;
			pbuf++;
		}
	}
	soft_i2c_stop();
	return ret;
}

int read_i2c_buf(u8 addr, u8 * pbuf, u32 size) {
	int ret = -1;
	soft_i2c_start();
	if(soft_i2c_wr_byte(addr | 1) == 0) {
		while(size--) {
			*pbuf = soft_i2c_rd_byte(size);
			pbuf++;
		}
		ret = 0;
	}
	soft_i2c_stop();
	return ret;
}

int read_i2c_byte_addr(u8 addr, u8 reg_addr, u8 * pbuf, u32 size) {
	int ret = -1;
	soft_i2c_start();
	if(soft_i2c_wr_byte(addr | 1) == 0) {
		if(soft_i2c_wr_byte(reg_addr) == 0) {
			while(size--) {
				*pbuf = soft_i2c_rd_byte(size);
				pbuf++;
			}
			ret = 0;
		}
	}
	soft_i2c_stop();
	return ret;
}

/* Universal I2C/SMBUS read-write transaction
 * wrlen = 0..127 !
 * return NAK (=0 - ok) */
int I2CBusUtr(void * outdata, i2c_utr_t * tr, u32 wrlen) {
	u8 * pwrdata = (u8 *) &tr->wrdata;
	u8 * poutdata = (u8 *) outdata;
	u32 cntstart = wrlen - (tr->mode & 0x7f);
	u32 rdlen = tr->rdlen & 0x7f;
	int ret;
	soft_i2c_start();
	ret = soft_i2c_wr_byte(*pwrdata++);
	while(ret == 0 && wrlen) {
		// write data
		ret = soft_i2c_wr_byte(*pwrdata++);

		if(--wrlen == cntstart && ret == 0) { // + send start & id
			if(tr->mode & 0x80) {
				soft_i2c_stop();
				sleep_us16(I2C_TCLK_US);
			}
			soft_i2c_start();
			ret = soft_i2c_wr_byte(tr->wrdata[0] | 1);
		}
	}
	if(ret == 0) {
		while(rdlen) {
			if(--rdlen == 0 && (tr->rdlen & 0x80) == 0)
				*poutdata = soft_i2c_rd_byte(1);
			else
				*poutdata = soft_i2c_rd_byte(0);
			poutdata++;
		}
	}
	soft_i2c_stop();
	return ret;
}

#endif // def I2C_GROUP
