/**
 * @file xbr818.c
 * @author pvvx
 */
#include "tl_common.h"
#include "app_config.h"
#if USE_SENSOR_XBR818
#include "drivers.h"
#include "xbr818.h"
#include "flash_eep.h"

#define XBR818_I2C_ADDR (0x71<<1)

#define I2C_TCLK_US	4	//

RAM xbr818_cfg_t xbr818_cfg;

const xbr818_cfg_t def_xbr818_cfg = {
		250, 35
};

static void drv_i2c_start(void) {
	gpio_set_output_en(XBR818_SCL, 0); // SCL set "1"
	gpio_set_output_en(XBR818_SDA, 0); // SDA set "1"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(XBR818_SDA, 1); // SDA set "0"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(XBR818_SCL, 1); // SCL set "0"
	//sleep_us(10);
}

static void drv_i2c_stop(void) {
	gpio_set_output_en(XBR818_SDA, 1); // SDA set "0"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(XBR818_SCL, 0); // SCL set "1"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(XBR818_SDA, 0); // SDA set "1"
	sleep_us(I2C_TCLK_US);
}

static int drv_i2c_wr_byte(u8 b) {
	int ret, i = 8;
	while(i--) {
		sleep_us(I2C_TCLK_US/2);
		if(b & 0x80)
			gpio_set_output_en(XBR818_SDA, 0); // SDA set "1"
		else
			gpio_set_output_en(XBR818_SDA, 1); // SDA set "0"
		sleep_us(I2C_TCLK_US/2);
		gpio_set_output_en(XBR818_SCL, 0); // SCL set "1"
		sleep_us(I2C_TCLK_US);
		gpio_set_output_en(XBR818_SCL, 1); // SCL set "0"
		b <<= 1;
	}
	sleep_us(I2C_TCLK_US/2);
	gpio_set_output_en(XBR818_SDA, 0); // SDA set "1"
	sleep_us(I2C_TCLK_US/2);
	gpio_set_output_en(XBR818_SCL, 0); // SCL set "1"
	sleep_us(I2C_TCLK_US);
	ret = gpio_read(XBR818_SDA);
	gpio_set_output_en(XBR818_SCL, 1); // SCL set "0"
	return ret;
}

static u8 drv_i2c_rd_byte(int ack) {
	u8 ret = 0, i = 8;
	gpio_set_output_en(XBR818_SDA, 0); // SDA set "1"
	while(i--) {
		sleep_us(I2C_TCLK_US);
		gpio_set_output_en(XBR818_SCL, 0); // SCL set "1"
		sleep_us(I2C_TCLK_US);
		ret <<= 1;
		if(gpio_read(XBR818_SDA)) {
			ret |= 1;
		}
		gpio_set_output_en(XBR818_SCL, 1); // SCL set "0"
	}
	sleep_us(I2C_TCLK_US/2);
	gpio_set_output_en(XBR818_SDA, ack); // SDA set "0"
	sleep_us(I2C_TCLK_US/2);
	gpio_set_output_en(XBR818_SCL, 0); // SCL set "1"
	sleep_us(I2C_TCLK_US);
	gpio_set_output_en(XBR818_SCL, 1); // SCL set "0"
	gpio_set_output_en(XBR818_SDA, 0); // SDA set "1"
	return ret;
}

int xbr818_write_reg(u8 raddr, u8 data) {
	int ret = 0;
	drv_i2c_start();
	ret = drv_i2c_wr_byte(XBR818_I2C_ADDR);
	if(ret == 0) {
		ret = drv_i2c_wr_byte(raddr);
		if(ret == 0) {
			ret = drv_i2c_wr_byte(data);
		}
	}
	drv_i2c_stop();
	return ret;
}

int xbr818_write_regs(u8 raddr, u8 * pdata, int size) {
	int ret = 0;
	drv_i2c_start();
	ret = drv_i2c_wr_byte(XBR818_I2C_ADDR);
	if(ret == 0) {
		ret = drv_i2c_wr_byte(raddr);
		while(ret == 0 && size--) {
			ret = drv_i2c_wr_byte(*pdata++);
		}
	}
	drv_i2c_stop();
	return ret;
}


int xbr818_read_regs(u8 raddr, u8 *pdata, int size) {
	int ret = -1;
	drv_i2c_start();
	ret = drv_i2c_wr_byte(XBR818_I2C_ADDR);
	if(ret == 0) {
		ret = drv_i2c_wr_byte(raddr);
		if(ret == 0) {
			drv_i2c_start();
			ret = drv_i2c_wr_byte(XBR818_I2C_ADDR | 1);
			if(ret == 0) {
				while(size--) {
					*pdata = drv_i2c_rd_byte(size);
					pdata++;
				}
			}
		}
	}
	drv_i2c_stop();
	return ret;
}

void xbr818_activate(void) {
	drv_i2c_start();
	drv_i2c_stop();
}

// XBR818 Registers:
//     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
// 00: 48 b4 8d 35 20 87 4c 5a a8 86 30 12 00 00 aa 94
// 10: 20 10 10 61 00 80 00 01 2c 01 01 00 55 00 fa 00
// 20: 00 fa 00 c0 60 70 ac 0d bc 02 0e 00 00 00 00 00
// 30: af 3e bb 88 af 3e 48 76 c5 c2 af 3e bb 88 af 3e
// 40: 35 78 71 c2 33 3f 7b 99 33 3f a3 67 52 c2 c6 22
// 50: 57 ce c6 22 10 5b 0f d1 85 0e 85 0e 00 00 8b 20
// 60: 00 00 00 40 00 00 00 00 00 00 00 00 00 40 00 00
// 70: 00 00 00 00 00 00 41 10 08 80 02 00 00 00 00 00
// 80: 00 30 00 00 53 13 00 00 00 00 00 00 00 00 00 00
// 90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// ...
// f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

const u8 xbr818_regs_init[] = {
	0x23, 0xc0,	// pin_ctl[7:0] INT_IRQ pin: io_value_out & light_flag inverted
				// b[7:4] 0xc io_value_out & light_flag inverted :pin_ctl[11]
				// b[3:0] IO_VAL pin output select 0x0:io_value_out & light_flag
	0x1c, 0x55, // io_timer_ctl
				// b[7:6] Reserved
				// b[5] t1 value select: 0: configure by pin, 1:configure by CPU register
				// b[4:3] timer interrupt output select: 10: 1 hour
				// b[2:1] light sensor triger timer select: 10: 1 Minute
				// b[0] s/m/h/d counter enable
	0x0b, 0x12, // adc_vinbais_enb[6] disable adc input 5u bias (0->5uA	bias enable),ADC1,ADC2,ADC3
				// ls_trim[5:4] light sensor threshold setting
				// ls_rstn[3] light sensor reset_n
				// pga_gain[2:0] PGA gain control(0~7 x1/x1.5/x2/x3/x4/x6/x8/x12)
	0x2a, 0x0e, // ?
	0x11, 0x10, // bb_ctl[15:8]
				// b[7] noise detect low threshold 0:1/4 bb_thresh2
				// b[6:4] total sum samples setting 1:2x128
				// b[3:2] dc average dynamic caculate time select 00:4 cycle
				// b[1:0] dc average initial caculate time select 00:4 cycle
	0x12, 0x10, // bb_ctl[23:0]
				// b[7] noise value select 0: auto detect
				// b[6] dc value select 0: auto detect
				// b[5:4] noise variation threshold: 01: 1/4
				// b[3:2] nosie update select 00: 1/2
				// b[1:0] dc update select 00: 1/2

//	0x18, 0x2C, // bb_thresh1[7:0]	threshold for ac detection
//	0x19, 0x01, // bb_thresh1[15:8]	threshold for ac detection

	0x02, 0x8d, // vco_cnt[7:4] frequency tune
				// rf1_en[3] rf ldo1 enable
				// rfldo1_trim[2:0] rf ldo1 trim

//	0x03, 0x47, // mix_swdc [6:4] mixer dc trim
				// vco_sw [2:0] Transmission power 0x40..0x47 124.1uA..108.5uA

	0x1a, 0x01, // bb_thresh2[7:0]	threshold for noise detection
	0x1b, 0x00, // bb_thresh2[15:8]	threshold for nosie detection

	0x13, 0x61, // bb_ctl[31:24]
				// b[7] I2C read only data update enable for 0x26~0x29
				// b[6:5] Reserved
				// b[4] bb proc threshold mode: 0: auto by configure pin
				// b[3] bb read only data update enable
				// b[2:1] read only data select: 00: det_dc_sum
				// b[0] bb proc enable: 1: enable
	// Задает время активности после обнаружения
//	0x1f, 0x00, // t1_value[23:16]	t1 value, io output high cnt @32KHz
//	0x1e, 0xfa, // t1_value[15:8]	t1 value, io output high cnt @32KHz
//	0x1d, 0x00, // t1_value[7:0]	t1 value, io output high cnt @32KHz
	// Задает время блокировки
	0x20, 0x00, // t2_value[7:0]	t2 value, io output low cnt @32KHz
	0x21, 0xfa, // t2_value[15:8]	t2 value, io output low cnt @32KHz
	0x22, 0x00, // t2_value[23:16]	t2 value, io output low cnt @32KHz

	0x24, 0x60, // pin_ctl[11:10]
				// b[7:4] Reserved
				// b[3] INT_IRQ GPIO out
				// b[2] IO_VAL GPIO out
				// b[1] power mode control select 0:by P1_5, 1:by CPU control register
				// b[0] ADC1 sample enable for VCO tuning 0:enable
	0,0
};

/**
 * @brief xrb818 set times
 *
 */
int xbr818_set_cfg(void) {
	u32 t32k;
	if(!xbr818_cfg.time_s || xbr818_cfg.time_s > 524) { // 1..524 sec ?
		xbr818_cfg.time_s = 524;
	}
	if(!xbr818_cfg.trg_ac) {
		xbr818_cfg.trg_ac = 1;
	}
	xbr818_activate();
	// set threshold for ac detection
	if(!xbr818_write_regs(0x18, (u8 *)&xbr818_cfg.trg_ac, 2) ) {
		// set t1 value, io output high cnt @32KHz
		t32k = xbr818_cfg.time_s * 32000;
		return xbr818_write_regs(0x1d, (u8 *)&t32k, 3);
	} else
		return 1;
}

/**
 * @brief xrb818 init
 *
 */
int xbr818_init(void) {
	const u8 * p = xbr818_regs_init;
	if (flash_read_cfg(&xbr818_cfg, EEP_ID_MTN, sizeof(xbr818_cfg))
			!= sizeof(xbr818_cfg)) {
		memcpy(&xbr818_cfg, &def_xbr818_cfg, sizeof(xbr818_cfg));
	}
	xbr818_activate();
	while(p[0]) {
		if(xbr818_write_regs(p[0], (u8 *)&p[1], 1))
			return 1;
		p += 2;
	}
	return xbr818_set_cfg();
}

/**
 * @brief xbr818 go to sleep
 *
 */
void xbr818_go_sleep(void) {
	xbr818_activate();
	xbr818_write_reg(0x02, 0x85); // rf ldo1 disable
}

#endif // USE_SENSOR_XBR818
