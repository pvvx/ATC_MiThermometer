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

#if (DEV_SERVICES & SERVICE_IUS) && USE_SENSOR_INA226
#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pm.h"
//#include "stack/ble/ll/ll_pm.h"

#include "i2c.h"
#include "sensor.h"
#include "app.h"

RAM sensor_cfg_t sensor_cfg;
const sensor_def_cfg_t sensor_ina226_def_cfg = {
		.coef.val1_k = 16385, // current_k (81.92/0.1)*65536/0x7fff = 1638.4500
		.coef.val2_k = 81923, // voltage_k 40960*65536/0x7fff = 81922.500
		.coef.val1_z = 0, // current_z
		.coef.val2_z = 0, // voltage_z
		.sensor_type = IU_SENSOR_INA226
};

#define INA226_I2C_ADDR	0x40
#define INA226_I2C_ADDR_MAX	0x4F

#define INA226_REG_CFG	0x00
#define INA226_REG_SHT	0x01
#define INA226_REG_BUS	0x02
#define INA226_REG_MID	0xfe	// = 0x5449
#define INA226_REG_VID	0xff	// = 2260

//#define DEF_INA226_CFG	0b0100110100100111		// 2 x 1.1 ms, Shunt and Bus, Continuous, 512 Averages (1126.4 ms)
#define DEF_INA226_CFG	0b0100111100100111		// 2 x 1.1 ms, Shunt and Bus, Continuous, 1024 Averages (2252.8 ms)
#define DEF_INA226_RES (DEF_INA226_CFG | 0x8000) // reset

#define INA226_ID  0x60224954


void init_sensor(void) {
	int test_addr = INA226_I2C_ADDR << 1;
	union {
		u8 ub[4];
		u16 us[2];
		u32 ud;
	} buf;
	sensor_cfg.i2c_addr = 0;
	sensor_cfg.sensor_type = TH_SENSOR_NONE;

	// send_i2c_byte(0, 0x06); // Reset command using the general call address

	do {
		if(((sensor_cfg.i2c_addr = (u8) scan_i2c_addr(test_addr)) != 0)
			 // Get ID
			&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, INA226_REG_MID, buf.ub, 2)
			&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, INA226_REG_VID, (u8 *) &buf.ub[2], 2)
			&& buf.ud == INA226_ID) {
			sensor_cfg.id = buf.ud;
			send_i2c_addr_word(sensor_cfg.i2c_addr, INA226_REG_CFG | (U16_LO(DEF_INA226_RES) << 16) | (U16_HI(DEF_INA226_RES) << 8));
			sleep_us(256);
			send_i2c_addr_word(sensor_cfg.i2c_addr, INA226_REG_CFG | (U16_LO(DEF_INA226_CFG) << 16) | (U16_HI(DEF_INA226_CFG) << 8));
			if(!sensor_cfg.coef.val1_k) {
				sensor_cfg.coef = sensor_ina226_def_cfg.coef;
			}
			sensor_cfg.sensor_type = sensor_ina226_def_cfg.sensor_type;
			break;
		}
		test_addr++;
	} while(test_addr <= (INA226_I2C_ADDR_MAX << 1));
}

_attribute_ram_code_ __attribute__((optimize("-Os")))
int read_sensor_cb(void) {
	int ret = 0;
	u8 ub[4];
	if(sensor_cfg.i2c_addr && sensor_cfg.sensor_type
		&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, INA226_REG_SHT, ub, 2)
		&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, INA226_REG_BUS, &ub[2], 2)) {
		s16 itmp = (ub[0] << 8) | ub[1];
		measured_data.current = ((s32)(itmp * sensor_cfg.coef.val1_k) >> 16) + sensor_cfg.coef.val1_z;
		u16 utmp = (ub[2] << 8) | ub[3];
		measured_data.voltage = ((utmp * sensor_cfg.coef.val2_k) >> 16) + sensor_cfg.coef.val2_z;
		measured_data.energy = measured_data.voltage * measured_data.current;
		measured_data.count++;
		ret = 1;
	} else {
		init_sensor();
	}
	return ret;
}

#endif // (DEV_SERVICES & SERVICE_IUS)


