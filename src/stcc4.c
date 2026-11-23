/*
 * stcc4.c
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: pvvx
 */

#include "tl_common.h"
#include "drivers.h"
//#include "vendor/common/user_config.h"
#include "app_config.h"

#if USE_SENSOR_STCC4
#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pm.h"
#include "stack/ble/ble.h"

#include "i2c.h"
#include "sensor.h"
#include "stcc4.h"
#include "app.h"

#define STCC4_ADV_COUNT		5

#define STCC4_I2C_ADDR		0x64 // 0x64, 0x65

typedef enum {
    STCC4_START_CONTINUOUS_MEASUREMENT_CMD_ID = 0x218b,
    STCC4_READ_MEASUREMENT_RAW_CMD_ID = 0xec05,
    STCC4_STOP_CONTINUOUS_MEASUREMENT_CMD_ID = 0x3f86,
    STCC4_MEASURE_SINGLE_SHOT_CMD_ID = 0x219d,
    STCC4_PERFORM_FORCED_RECALIBRATION_CMD_ID = 0x362f,
    STCC4_GET_PRODUCT_ID_CMD_ID = 0x365b,
    STCC4_SET_RHT_COMPENSATION_CMD_ID = 0xe000,
    STCC4_SET_PRESSURE_COMPENSATION_RAW_CMD_ID = 0xe016,
    STCC4_PERFORM_SELF_TEST_CMD_ID = 0x278c,
    STCC4_PERFORM_CONDITIONING_CMD_ID = 0x29bc,
    STCC4_ENTER_SLEEP_MODE_CMD_ID = 0x3650,
    STCC4_EXIT_SLEEP_MODE_CMD_ID = 0x0,
    STCC4_ENABLE_TESTING_MODE_CMD_ID = 0x3fbc,
    STCC4_DISABLE_TESTING_MODE_CMD_ID = 0x3f3d,
    STCC4_PERFORM_FACTORY_RESET_CMD_ID = 0x3632,
} STCC4_CMD_ID;

RAM sensor_co2_t sensor_co2;

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

/*
_attribute_ram_code_
static int write_regs16_stcc4(u16 raddr, u16 rdata) {
	u8 	b[5];
	b[0] = (u8)(raddr >> 8);
	b[1] = (u8)raddr;
	b[2] = (u8)(rdata >> 8);
	b[3] = (u8)rdata;
	b[4] = sensor_crc(sensor_crc(0xff ^ b[2]) ^ b[3]);
	return send_i2c_buf(sensor_cfg.i2c_addr, b, 5);
}
*/

/* return 0 -> ok */
_attribute_ram_code_
static int read_regs16_stcc4(u16 raddr, u8 * pdata, int cnt) {
	u8 b[4*3];
	b[0] = (u8)(raddr >> 8);
	b[1] = (u8)raddr;
	if(!send_i2c_buf(STCC4_I2C_ADDR << 1, b, 2)) {
		// STCC4: Execution Time 1 ms ?
		// sleep_us(1000);
		if(!read_i2c_buf(STCC4_I2C_ADDR << 1, b, (cnt << 1) + cnt)) {
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
			return 0; // ok
		}
	}
	return 1; // nack
}

/* return 0 -> ok */
inline int write_cmd16_stcc4(u16 cmd) {
	return send_i2c_word(STCC4_I2C_ADDR << 1, (cmd << 8) | (cmd >> 8));
}

/* return 0 -> ok */
inline int exit_sleep_stcc4(void) {
//	if(!send_i2c_byte(STCC4_I2C_ADDR << 1, STCC4_EXIT_SLEEP_MODE_CMD_ID))
//		return 0;
	return send_i2c_byte(STCC4_I2C_ADDR << 1, STCC4_EXIT_SLEEP_MODE_CMD_ID);
	// STCC4: Execution Time 5 ms
}

_attribute_ram_code_
int set_comp_sensor(s16 temp, u16 humi) {
	struct {
		u16 id;
		u8 temp[2];
		u8 temp_crc;
		u8 humi[2];
		u8 humi_crc;
	} set_comp;
	u32 tmp;
	set_comp.id = (u16)((STCC4_SET_RHT_COMPENSATION_CMD_ID >> 8) | (STCC4_SET_RHT_COMPENSATION_CMD_ID << 8));
	// Tс = (T+45)*65535/175;
	tmp = (temp + 4500) * 0x3BEAF;
	set_comp.temp[0] = (tmp >> 24) & 0xff;
	set_comp.temp[1] = (tmp >> 16) & 0xff;
	set_comp.temp_crc = sensor_crc(sensor_crc(0xff ^ set_comp.temp[0]) ^ set_comp.temp[1]);
	// RHc = (RH+6)*65535/125;
	tmp = (humi + 600) * 0x53E28;
	set_comp.humi[0] = (tmp >> 24) & 0xff;
	set_comp.humi[1] = (tmp >> 16) & 0xff;
	set_comp.humi_crc = sensor_crc(sensor_crc(0xff ^ set_comp.humi[0]) ^ set_comp.humi[1]);
	return send_i2c_buf(STCC4_I2C_ADDR << 1, (u8 *)&set_comp, sizeof(set_comp));
}


void init_co2_sensor(void) {
	write_cmd16_stcc4(STCC4_ENTER_SLEEP_MODE_CMD_ID);
	sensor_co2.measure_timeout = 0;
	sensor_co2.th_measure_count = STCC4_ADV_COUNT - 1;
}

extern void WakeupLowPowerCb(int par);

_attribute_ram_code_
int task_measure_co2(void) {
	if(sensor_co2.measure_timeout) {
		if(sensor_co2.measure_timeout == 5*CLOCK_16M_SYS_TIMER_CLK_1MS) {
			if(set_comp_sensor(measured_data.temp, measured_data.humi) == 0) {
				sleep_us(1000);
				if(write_cmd16_stcc4(STCC4_MEASURE_SINGLE_SHOT_CMD_ID) == 0) { // 0x219d
					// STCC4: Execution Time 500 ms
					//bls_pm_registerAppWakeupLowPowerCb(WakeupLowPowerCb);
					sensor_co2.time_measure = clock_time();
					sensor_co2.measure_timeout = 500*CLOCK_16M_SYS_TIMER_CLK_1MS;
					bls_pm_setAppWakeupLowPower(sensor_co2.time_measure + sensor_co2.measure_timeout, 1);
					return 1;
				}
			}
		} else if(!read_regs16_stcc4(STCC4_READ_MEASUREMENT_RAW_CMD_ID, // 0xec05
				(u8 *)&measured_data.co2, 1)) {
			sleep_us(1000);
			wrk.msc.b.co2_sensor_read = 1;
			write_cmd16_stcc4(STCC4_ENTER_SLEEP_MODE_CMD_ID); // 0x3650
			sensor_co2.measure_timeout = 0;
			return 1;
		}
		sleep_us(1000);
		write_cmd16_stcc4(STCC4_ENTER_SLEEP_MODE_CMD_ID); // 0x3650
		sensor_co2.measure_timeout = 0;
	} else if(++sensor_co2.th_measure_count > STCC4_ADV_COUNT) {
		exit_sleep_stcc4();
		//bls_pm_registerAppWakeupLowPowerCb(WakeupLowPowerCb);
		sensor_co2.th_measure_count = 0;
		sensor_co2.time_measure = clock_time();
		sensor_co2.measure_timeout = 5*CLOCK_16M_SYS_TIMER_CLK_1MS;
		bls_pm_setAppWakeupLowPower(sensor_co2.time_measure + sensor_co2.measure_timeout, 1);
	}
	return 0;
}

void start_measure_co2(void) {
	if(sensor_co2.measure_timeout == 0) {
		if(++sensor_co2.th_measure_count > STCC4_ADV_COUNT) {
			exit_sleep_stcc4();
			//bls_pm_registerAppWakeupLowPowerCb(WakeupLowPowerCb);
			sensor_co2.th_measure_count = 0;
			//sensor_co2.time_measure = clock_time();
			//sensor_co2.measure_timeout = 5*CLOCK_16M_SYS_TIMER_CLK_1MS;
			//bls_pm_setAppWakeupLowPower(sensor_co2.time_measure + sensor_co2.measure_timeout, 1);
		}
	}
}

#endif // USE_SENSOR_STCC4

