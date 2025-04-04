/*
 * scd41.c
 *
 *  Created on: 19 мар. 2025 г.
 *      Author: pvvx
 */

#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"

#if USE_SENSOR_SCD41
#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pm.h"

#include "i2c.h"
#include "sensor.h"
#include "app.h"

#if SENSOR_SLEEP_MEASURE
#error "Set SENSOR_SLEEP_MEASURE = 0!"
#endif

#define SCD41_I2C_ADDR		0x62

typedef enum {
    SCD4X_START_PERIODIC_MEASUREMENT_CMD_ID = 0x21b1,
    SCD4X_READ_MEASUREMENT_RAW_CMD_ID = 0xec05,
    SCD4X_STOP_PERIODIC_MEASUREMENT_CMD_ID = 0x3f86,
    SCD4X_SET_TEMPERATURE_OFFSET_RAW_CMD_ID = 0x241d,
    SCD4X_GET_TEMPERATURE_OFFSET_RAW_CMD_ID = 0x2318,
    SCD4X_SET_SENSOR_ALTITUDE_CMD_ID = 0x2427,
    SCD4X_GET_SENSOR_ALTITUDE_CMD_ID = 0x2322,
    SCD4X_SET_AMBIENT_PRESSURE_RAW_CMD_ID = 0xe000,
    SCD4X_GET_AMBIENT_PRESSURE_RAW_CMD_ID = 0xe000,
    SCD4X_PERFORM_FORCED_RECALIBRATION_CMD_ID = 0x362f,
    SCD4X_SET_AUTOMATIC_SELF_CALIBRATION_ENABLED_CMD_ID = 0x2416,
    SCD4X_GET_AUTOMATIC_SELF_CALIBRATION_ENABLED_CMD_ID = 0x2313,
    SCD4X_SET_AUTOMATIC_SELF_CALIBRATION_TARGET_CMD_ID = 0x243a,
    SCD4X_GET_AUTOMATIC_SELF_CALIBRATION_TARGET_CMD_ID = 0x233f,
    SCD4X_START_LOW_POWER_PERIODIC_MEASUREMENT_CMD_ID = 0x21ac,
    SCD4X_GET_DATA_READY_STATUS_RAW_CMD_ID = 0xe4b8,
    SCD4X_PERSIST_SETTINGS_CMD_ID = 0x3615,
    SCD4X_GET_SERIAL_NUMBER_CMD_ID = 0x3682,
    SCD4X_PERFORM_SELF_TEST_CMD_ID = 0x3639,
    SCD4X_PERFORM_FACTORY_RESET_CMD_ID = 0x3632,
    SCD4X_REINIT_CMD_ID = 0x3646,
    SCD4X_GET_SENSOR_VARIANT_RAW_CMD_ID = 0x202f,
    SCD4X_MEASURE_SINGLE_SHOT_CMD_ID = 0x219d,
    SCD4X_MEASURE_SINGLE_SHOT_RHT_ONLY_CMD_ID = 0x2196,
    SCD4X_POWER_DOWN_CMD_ID = 0x36e0,
    SCD4X_WAKE_UP_CMD_ID = 0x36f6,
    SCD4X_SET_AUTOMATIC_SELF_CALIBRATION_INITIAL_PERIOD_CMD_ID = 0x2445,
    SCD4X_GET_AUTOMATIC_SELF_CALIBRATION_INITIAL_PERIOD_CMD_ID = 0x2340,
    SCD4X_SET_AUTOMATIC_SELF_CALIBRATION_STANDARD_PERIOD_CMD_ID = 0x244e,
    SCD4X_GET_AUTOMATIC_SELF_CALIBRATION_STANDARD_PERIOD_CMD_ID = 0x234b,
} SCD4xCmdId;

const sensor_def_cfg_t def_thcoef_scd41 = {
		.coef.val1_k = 17500, // temp_k
		.coef.val1_z = -4500, // temp_z
		.coef.val2_k = 10000, // humi_k
		.coef.val2_z = 0, // humi_z
#if SENSOR_SLEEP_MEASURE
		.measure_timeout = 5000000 * CLOCK_16M_SYS_TIMER_CLK_1US,
#endif
		.sensor_type = IU_SENSOR_SCD41
};

RAM sensor_cfg_t sensor_cfg;

#define CRC_POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

_attribute_ram_code_
static u8 sensor_crc(u8 crc) {
	int i;
	for(i = 8; i > 0; i--) {
		if (crc & 0x80)
			crc = (crc << 1) ^ (CRC_POLYNOMIAL & 0xff);
		else
			crc = (crc << 1);
	}
	return crc;
}

_attribute_ram_code_
int write_regs16_scd41(u16 raddr, u16 rdata) {
	u8 	b[5];
	b[0] = (u8)(raddr >> 8);
	b[1] = (u8)raddr;
	b[2] = (u8)(rdata >> 8);
	b[3] = (u8)rdata;
	b[4] = sensor_crc(sensor_crc(0xff ^ b[2]) ^ b[3]);
	return send_i2c_buf(sensor_cfg.i2c_addr, b, 5);
}

_attribute_ram_code_
int read_regs16_scd41(u16 raddr, u8 * pdata, int cnt) {
	u8 b[4*3];
	b[0] = (u8)(raddr >> 8);
	b[1] = (u8)raddr;
	if(!send_i2c_buf(sensor_cfg.i2c_addr, b, 2)) {
		sleep_us(1000);
		if(!read_i2c_buf(sensor_cfg.i2c_addr, b, (cnt << 1) + cnt)) {
			u8 * p = b;
			while(cnt--) {
				if(p[2] == sensor_crc(sensor_crc(0xff ^ p[0]) ^ p[1])) {
					pdata[1] = p[0];
					pdata[0] = p[1];
					pdata += 2;
				} else {
					return 1;
				}
				p += 3;
			}
			return 0;
		}
	}
	return 1;
}

inline int write_cmd_scd41(u16 cmd) {
	return send_i2c_word(sensor_cfg.i2c_addr, (cmd << 8) | (cmd >> 8));
}



void init_sensor(void) {
	sensor_cfg.id = 0;
	sensor_cfg.sensor_type = TH_SENSOR_NONE;
	sensor_cfg.i2c_addr = (u8) scan_i2c_addr(SCD41_I2C_ADDR << 1);
	if (sensor_cfg.i2c_addr) {
		if (!read_regs16_scd41(SCD4X_GET_SENSOR_VARIANT_RAW_CMD_ID, (u8 *)&sensor_cfg.id, 1)) {
			int ret = 0;
			if(cfg.flg.lp_measures)
				ret = write_cmd_scd41(SCD4X_START_LOW_POWER_PERIODIC_MEASUREMENT_CMD_ID); // measurement 30 sec, Average 3.4 mA 3.3V
			else
				ret = write_cmd_scd41(SCD4X_START_PERIODIC_MEASUREMENT_CMD_ID); // measurement 5 sec, Average 17.5 mA 3.3V
			if(!ret) {
				sensor_cfg.wait_tik = clock_time();
				sensor_def_cfg_t * ptabinit = (sensor_def_cfg_t *)&def_thcoef_scd41;
				if(sensor_cfg.coef.val1_k == 0)
					memcpy(&sensor_cfg.coef, ptabinit, sizeof(sensor_cfg.coef));
				sensor_cfg.sensor_type = ptabinit->sensor_type;
				return;
			} else {
				write_cmd_scd41(SCD4X_STOP_PERIODIC_MEASUREMENT_CMD_ID);
			}
		} else {
			write_cmd_scd41(SCD4X_STOP_PERIODIC_MEASUREMENT_CMD_ID);
		}
	}
	sensor_cfg.i2c_addr = 0;
}

#define SCD4X_WAIT_TIME 	45*CLOCK_16M_SYS_TIMER_CLK_1S

_attribute_ram_code_
int read_sensor_cb(void) {
	struct __attribute__((packed)) {
		u16 co2;
		u16 temp;
		u16 humi;
	}m;
	u16 status;
	if(sensor_cfg.i2c_addr) {
		if(!read_regs16_scd41(SCD4X_GET_DATA_READY_STATUS_RAW_CMD_ID, (u8 *)&status, 1)) {
			if((status & 0x7ff) != 0) {
				sensor_cfg.wait_tik = clock_time();
				if(!read_regs16_scd41(SCD4X_READ_MEASUREMENT_RAW_CMD_ID, (u8 *)&m.co2, 3)) {
					measured_data.co2 = m.co2;
					measured_data.temp = ((s32)(m.temp * sensor_cfg.coef.val1_k) >> 16) + sensor_cfg.coef.val1_z; // x 0.01 C //17500 - 4500
					measured_data.humi = ((u32)(m.humi * sensor_cfg.coef.val2_k) >> 16) + sensor_cfg.coef.val2_z; // x 0.01 %	   // 10000 -0
					if (measured_data.humi < 0)
						measured_data.humi = 0;
					else if (measured_data.humi > 9999)
						measured_data.humi = 9999;
					measured_data.count++;
					return 1;
				}
			} else if(clock_time() - sensor_cfg.wait_tik < SCD4X_WAIT_TIME)
				return 0;
		}
		write_cmd_scd41(SCD4X_STOP_PERIODIC_MEASUREMENT_CMD_ID);
		sensor_cfg.i2c_addr = 0;
	} else
		init_sensor();
	return 0;
}

#endif // USE_SENSOR_SCD41
