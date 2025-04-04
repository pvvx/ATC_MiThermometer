/*
 * ina216.c
 *
 *  Created on: 21 июл. 2024 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"

#if (DEV_SERVICES & SERVICE_IUS) && USE_SENSOR_INA3221
#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pm.h"

#include "i2c.h"
#include "sensor.h"
#include "app.h"

#if SENSOR_SLEEP_MEASURE
#error "Set SENSOR_SLEEP_MEASURE = 0!"
#endif

RAM sensor_cfg_t sensor_cfg;
const sensor_def_cfg_t sensor_ina226_def_cfg = {
		.coef[0].val1_k = 3277, // current_k (163.8/0.1)*65536/0x7fff = 3276.8
		.coef[0].val2_k = 65535, // voltage_k 32760*65536/0x7ff8 = 65536
		.coef[0].val1_z = 0, // current_z
		.coef[0].val2_z = 0, // voltage_z
		.coef[1].val1_k = 3277, // current_k (81.92/0.1)*65536/0x7fff = 1638.4500
		.coef[1].val2_k = 65535, // voltage_k 40960*65536/0x7fff = 81922.500
		.coef[1].val1_z = 0, // current_z
		.coef[1].val2_z = 0, // voltage_z
		.coef[2].val1_k = 3277, // current_k (81.92/0.1)*65536/0x7fff = 1638.4500
		.coef[2].val2_k = 65535, // voltage_k 40960*65536/0x7fff = 81922.500
		.coef[2].val1_z = 0, // current_z
		.coef[2].val2_z = 0, // voltage_z

		.sensor_type = IU_SENSOR_INA3221
};

#define INA3221_I2C_ADDR		0x40
#define INA3221_I2C_ADDR_MAX	0x43

#define INA3221_REG_CFG		0x00
#define INA3221_REG_SHT1	0x01
#define INA3221_REG_BUS1	0x02
#define INA3221_REG_SHT2	0x03
#define INA3221_REG_BUS2	0x04
#define INA3221_REG_SHT3	0x05
#define INA3221_REG_BUS3	0x06

#define INA3221_REG_ALR1	0x07
#define INA3221_REG_WRN1	0x08
#define INA3221_REG_ALR2	0x09
#define INA3221_REG_WRN2	0x0a
#define INA3221_REG_ALR3	0x0b
#define INA3221_REG_WRN3	0x0c

#define INA3221_REG_VSM		0x0d
#define INA3221_REG_VSL		0x0e

#define INA3221_REG_MSK		0x0f

#define INA3221_REG_PVU		0x10
#define INA3221_REG_PVL		0x11

#define INA3221_REG_MID		0xfe	// = 0x5449
#define INA3221_REG_VID		0xff	// = 0x3220

//#define DEF_INA3221_CFG	0b0111110110110111		// 3 x 2 x 4.156 ms, Shunt and Bus, Continuous, 512 Averages (12767.232 ms)
#define DEF_INA3221_CFG	0b0111111100100111		// 3 x 2 x 1.1 ms, Shunt and Bus, Continuous, 1024 Averages (6758.4 ms)
#define DEF_INA3221_RES (DEF_INA3221_CFG | 0x8000) // reset

#define INA3221_ID  0x20324954


void init_sensor(void) {
	int test_addr = INA3221_I2C_ADDR << 1;
	union {
		u8 ub[4];
		u16 us[2];
		u32 ud;
	} buf;
//	sensor_cfg.stage = 0;
	sensor_cfg.i2c_addr = 0;
	sensor_cfg.sensor_type = TH_SENSOR_NONE;

	// send_i2c_byte(0, 0x06); // Reset command using the general call address

	do {
		if(((sensor_cfg.i2c_addr = (u8) scan_i2c_addr(test_addr)) != 0)
			 // Get ID
			&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, INA3221_REG_MID, buf.ub, 2)
			&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, INA3221_REG_VID, (u8 *) &buf.ub[2], 2)
			&& buf.ud == INA3221_ID) {
			sensor_cfg.id = buf.ud;
			send_i2c_addr_word(sensor_cfg.i2c_addr, INA3221_REG_CFG | (U16_LO(DEF_INA3221_RES) << 16) | (U16_HI(DEF_INA3221_RES) << 8));
			sleep_us(256);
			send_i2c_addr_word(sensor_cfg.i2c_addr, INA3221_REG_CFG | (U16_LO(DEF_INA3221_CFG) << 16) | (U16_HI(DEF_INA3221_CFG) << 8));
			send_i2c_addr_word(sensor_cfg.i2c_addr, INA3221_REG_PVL | (U16_LO(4800) << 16) | (U16_HI(4800) << 8));
			send_i2c_addr_word(sensor_cfg.i2c_addr, INA3221_REG_PVU | (U16_LO(4900) << 16) | (U16_HI(4900) << 8));
			if(!sensor_cfg.coef[0].val1_k) {
				memcpy(&sensor_cfg.coef, &sensor_ina226_def_cfg.coef, sizeof(sensor_cfg.coef));
			}
			sensor_cfg.sensor_type = sensor_ina226_def_cfg.sensor_type;
			break;
		}
		test_addr++;
	} while(test_addr <= (INA3221_I2C_ADDR_MAX << 1));
}


_attribute_ram_code_ __attribute__((optimize("-Os")))
int read_sensor_cb(void) {
	int ret = 0;
	int stage = 0;
	u8 ub[4];
	if(sensor_cfg.i2c_addr && sensor_cfg.sensor_type) {
		do {
			if (!read_i2c_byte_addr(sensor_cfg.i2c_addr, INA3221_REG_SHT1 + (stage << 1), ub, 2)
			&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, INA3221_REG_BUS1 + (stage << 1), &ub[2], 2)) {
				s16 itmp = (ub[0] << 8) | ub[1];
				u16 utmp = (ub[2] << 8) | ub[3];
#if 1			// Correct - Rfilter 10 Om
				itmp -= (utmp + (utmp >> 1)) >> 9; // div 416.67
//				itmp -= (utmp - (utmp >> 2)) >> 8; // div 344.83
#endif
				measured_data.current[stage] = ((s32)(itmp * sensor_cfg.coef[stage].val1_k) >> 16) + sensor_cfg.coef[stage].val1_z;
				measured_data.voltage[stage] = ((utmp * sensor_cfg.coef[stage].val2_k) >> 16) + sensor_cfg.coef[stage].val2_z;
			} else {
				init_sensor();
				return ret;
			}
		} while(++stage < 3);
		measured_data.count++;
		//sensor_cfg.stage = 0;
		//wrk.msc.all_flgs = 0xff;
		ret = 1;
	} else {
		init_sensor();
	}
	return ret;
}

#endif // (DEV_SERVICES & SERVICE_IUS)


