/*
 * ens160.c
 *
 *  Created on: 4 мар. 2025 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app_config.h"

#if defined(USE_SENSOR_ENS160) && USE_SENSOR_ENS160

#include "drivers.h"
#include "app.h"
#include "i2c.h"
#include "ens160.h"

#define ENS160_ADDRESS_1 0x52
#define ENS160_ADDRESS_2 0x53

// ENS160 registers address
#define ENS160_RA_PART_ID	0x00
#define ENS160_RA_OP_MODE	0x10
#define ENS160_RA_CONFIG	0x11
#define ENS160_RA_COMMAND	0x12
#define ENS160_RA_TEMP_IN	0x13
#define ENS160_RA_RH_IN		0x15
#define ENS160_RA_STATUS	0x20
#define ENS160_RA_DATA_AQI	0x21
#define ENS160_RA_DATA_TVOC	0x22
#define ENS160_RA_DATA_ETOH	0x22
#define ENS160_RA_DATA_ECO2	0x24
#define ENS160_RA_DATA_T	0x30
#define ENS160_RA_DATA_RH	0x32
#define ENS160_RA_DATA_MISR	0x38

// General Purpose Write registers
#define ENS160_RA_GPR_WRITE0 0x40
#define ENS160_RA_GPR_WRITE1 0x41
#define ENS160_RA_GPR_WRITE2 0x42
#define ENS160_RA_GPR_WRITE3 0x43
#define ENS160_RA_GPR_WRITE4 0x44
#define ENS160_RA_GPR_WRITE5 0x45
#define ENS160_RA_GPR_WRITE6 0x46
#define ENS160_RA_GPR_WRITE7 0x47

// General Purpose Read registers
#define ENS160_RA_GPR_READ0 0x48
#define ENS160_RA_GPR_READ1 0x49
#define ENS160_RA_GPR_READ2 0x4A
#define ENS160_RA_GPR_READ3 0x4B
#define ENS160_RA_GPR_READ4 0x4C
#define ENS160_RA_GPR_READ5 0x4D
#define ENS160_RA_GPR_READ6 0x4E
#define ENS160_RA_GPR_READ7 0x4F

#define ENS160_DEVICE_ID 0x0160

#define ENS160_CONFIG 			0x43 // INTn pin is enabled when new data is presented in the DATA_XXX, Push / Pull
typedef struct {
    u8 int_en : 1;
    u8 int_dat : 1;
    u8 reserved_one : 1;
    u8 int_gpr : 1;
    u8 reserved_two : 1;
    u8 int_cfg : 1;
    u8 int_pol : 1;
    u8 reserved_three : 1;
} ens160_reg_config_t;

// All commands must be issued when device is idle.
#define ENS160_COMMAND_NOP			0x00
// Get Firwmware App Version - version is placed in General Purpose Read Registers as follows:
// GPR_READ04 - Version (Major)
// GPR_READ05 - Version (Minor)
// GPR_READ06 - Version (Release)
#define ENS160_COMMAND_GET_APPVER	0x0E
// Clear General Purpose Read Register
#define ENS160_COMMAND_CLRGPR		0xCC

typedef struct {
    u8 stat_as : 1;
    u8 stat_er : 1;
    u8 reserved_two : 1;
    u8 reserved_one : 1;
    u8 validity_flag : 2;
    u8 new_dat : 1;
    u8 new_gpr : 1;
} ens160_reg_device_status_t;


// -----------------

RAM ens160_wrk_t ens160;

inline int write_regs8_ens160(u8 reg_addr, u8 data) {
	return send_i2c_word(ens160.i2c_address, (data << 8) | reg_addr);
}
inline int write_regs16_ens160(u8 reg_addr, u16 data) {
	return send_i2c_addr_word(ens160.i2c_address, (data << 8) | reg_addr);
}
inline int read_regs8_ens160(u8 reg_addr, u8 * pdata) {
	return read_i2c_byte_addr(ens160.i2c_address, reg_addr, (u8 *)pdata, 1);
}
inline int read_regs16_ens160(u8 reg_addr, u16 * pdata) {
	return read_i2c_byte_addr(ens160.i2c_address, reg_addr, (u8 *)pdata, 2);
}

inline int read_all_data_ens160(void) {
	return read_i2c_byte_addr(ens160.i2c_address, ENS160_RA_DATA_AQI,  (u8 *)&ens160.aqi, 1+2+2);
}

int init_ens160(void) {
    u16 id = 0;
    ens160.mode = ENS160_MODE_DEEP_SLEEP;
	ens160.i2c_address = (u8) scan_i2c_addr(ENS160_ADDRESS_1 << 1);
	if (!ens160.i2c_address)
		ens160.i2c_address = (u8) scan_i2c_addr(ENS160_ADDRESS_2 << 1);
	if (ens160.i2c_address
		&& (!read_regs16_ens160(ENS160_RA_PART_ID, &id))
		&& id == ENS160_DEVICE_ID) {
		if(!write_regs8_ens160(ENS160_RA_OP_MODE, ENS160_MODE_RESET))
			ens160.mode = ENS160_MODE_RESET;
	}
	return (!ens160.i2c_address);
}

int set_th_ens160(s16 tx100, u16 hx100) {
	int ret = 1;
	if (ens160.i2c_address) {
		// rt = (t + 273.15) * 64
		// rt = (tx100 + 27315) * 64/100
		// 65536*(64/100) = 41943.04
		// test: ((2500 + 27315) * 64) >> 16 = 19081 = 0x4a89
		ret = write_regs16_ens160(ENS160_RA_TEMP_IN, ((tx100 + 27315) * 41943) >> 16);
		// rh = h * 512
		// rh = hx100 * 512/100
		// 65536*(512/100) = 335544.32
		// test: (5000 * 335544) >> 16 = 25599 = 0x63ff
		if(!ret)
			ret = write_regs16_ens160(ENS160_RA_RH_IN, (hx100 * 335544) >> 16);
	}
	return ret;
}

#define ENS160_AVERAGE_COUNT_SHL	5 // 4,5,6,7,8,9,10,11,12 -> 16,32,64,128,256,512,1024,2048,4096

int read_ens160(void) {
	if (ens160.i2c_address) {
		if(ens160.mode == ENS160_MODE_RESET) {
			read_regs8_ens160(ENS160_RA_STATUS, &ens160.status);
			if(!write_regs8_ens160(ENS160_RA_OP_MODE, ENS160_MODE_STANDARD)
			&& !write_regs8_ens160(ENS160_RA_CONFIG, ENS160_CONFIG)) {
				ens160.mode = ENS160_MODE_STANDARD;
				return 0;
			}
		} else if(ens160.mode == ENS160_MODE_STANDARD) {
			read_regs8_ens160(ENS160_RA_STATUS, &ens160.status);
			if(ens160.status & 0x40) {
				// Invalid Operating Mode has been selected
				init_ens160();
			} else if ((ens160.status & 0x0c) == 0x0c) {
				// Invalid output
			} else if ((ens160.status & 0x82) == 0x82) {
#if 0
				read_regs8_ens160(ENS160_RA_DATA_AQI, &ens160.aqi)
				read_regs16_ens160(ENS160_RA_DATA_TVOC, &ens160.tvoc);
				read_regs16_ens160(ENS160_RA_DATA_ECO2, &ens160.co2);
#else
				read_all_data_ens160();

#endif
#if USE_ENS160_INT
				ens160.co2_sum += ens160.co2;
				ens160.tvoc_sum += ens160.tvoc;
				if(ens160.cnt < (1 << ENS160_AVERAGE_COUNT_SHL)) {
					ens160.cnt++;
					ens160.co2 = ens160.co2_sum / ens160.cnt;
					ens160.tvoc = ens160.tvoc_sum / ens160.cnt;
				} else {
					ens160.co2 = ens160.co2_sum >> ENS160_AVERAGE_COUNT_SHL;
					ens160.tvoc = ens160.tvoc_sum >> ENS160_AVERAGE_COUNT_SHL;
					ens160.co2_sum -= ens160.co2;
					ens160.tvoc_sum -= ens160.tvoc;
				}
#endif
				ens160.flg = 0xff;
#ifdef GPIO_COOLER
				gpio_setup_up_down_resistor(GPIO_COOLER, (ens160.aqi < 3) ? PM_PIN_PULLDOWN_100K : PM_PIN_PULLUP_10K);
#endif
				cpu_set_gpio_wakeup(GPIO_ENS160_INT, Level_High, 1);  // pad wakeup deepsleep enable
				return 1;
			}
		}
	} else {
		init_ens160();
	}
	return 0;
}

#endif // defined(USE_SENSOR_ENS160) && USE_SENSOR_ENS160
