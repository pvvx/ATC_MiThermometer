/*
 * BME280.c
 *
 *  Created on: 26 янв. 2016 г.
 *      Author: PVV
 */
#include "tl_common.h"
#include "app_config.h"

#if defined(USE_SENSOR_BME280) && USE_SENSOR_BME280

#include "drivers.h"
#include "i2c.h"
#include "sensor.h"
#include "app.h"

#define ENABLE_TEST_REG_STATUS		0

RAM sensor_cfg_t sensor_cfg;

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
_attribute_ram_code_
s32 BME280_compensate_T(s32 adc_T, sensor_cfg_t *p) {
	s32 var1, var2;
	var1 = ((((adc_T >> 3) - ((s32)p->dig.T1 << 1))) * ((s32)p->dig.T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((s32)p->dig.T1)) * ((adc_T >> 4) - ((s32)p->dig.T1))) >> 12) * ((s32)p->dig.T3)) >> 14;
	p->t_fine = var1 + var2;
	return (p->t_fine * 5 + 128) >> 8;
}

// Returns humidity in %RH, resolution is 0.01.
// Output value of “4633”represents 4633/100 = 46.33%RH
_attribute_ram_code_
u32 BME280_compensate_H(s32 adc_H, sensor_cfg_t *p)
{
  s32 h;
  h = (p->t_fine - ((s32)76800));
  h = (((((adc_H << 14) - (((s32)p->dig.H4) << 20) - (((s32)p->dig.H5) * h)) +
    ((s32)16384)) >> 15) * (((((((h * ((s32)p->dig.H6)) >> 10) * (((h *
    ((s32)p->dig.H3)) >> 11) + ((s32)32768))) >> 10) + ((s32)2097152)) * ((s32)p->dig.H2) + 8192) >> 14));
  h = (h - (((((h >> 15) * (h >> 15)) >> 7) * ((s32)p->dig.H1)) >> 4));
  h = ((h < 0) ? 0 : h);
  h = ((h > 0x18FFAE14)? 0x18FFAE14 : h); // = ((9999.5*65536)/100)*(2pow6)
  return (((u32)h>>6)*((u32)100))>>16;
}

// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
_attribute_ram_code_
u32 BME280_compensate_P(s32 adc_P, sensor_cfg_t *p)
{
	s32 var1, var2;
	u32 v;
	var1 = (((s32)p->t_fine) >> 1) - (s32)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11) * ((s32)p->dig.P6);
	var2 = var2 + ((var1 * ((s32)p->dig.P5)) << 1);
	var2 = (var2 >> 2)+(((s32)p->dig.P4) << 16);
	var1 = (((p->dig.P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((s32)p->dig.P2) * var1) >> 1)) >> 18;
	var1 =((((32768 + var1)) * ((s32)p->dig.P1)) >> 15);
	if (var1 == 0)
		return 0; // avoid exception caused by division by zero
	v = (((u32)(((s32)1048576) - adc_P) - (var2 >> 12))) * 3125;
	if (v < 0x80000000)
		v = (v << 1) / ((u32)var1);
	else
		v = (v / (u32)var1) * 2;
	var1 = (((s32)p->dig.P9) * ((s32)(((v >> 3) * (v >> 3)) >> 13))) >>12;
	var2 = (((s32)(v >> 2)) * ((s32)p->dig.P8)) >> 13;
	v = (u32)((s32)v + ((var1 + var2 + p->dig.P7) >> 4));
	return v;
}

//-------------------------------------------------------------------------------
// init_BME280
//-------------------------------------------------------------------------------
void init_sensor(void) {
	u8 buf[7];
	if(((sensor_cfg.i2c_addr = (u8) scan_i2c_addr(BME280_I2C_ADDR<<1)) != 0)
	&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, BME280_RA_CHIPID, (u8 *)&sensor_cfg.id, 2)
	&& (sensor_cfg.id & 0xff) == BME280_ID
	&& !send_i2c_word(sensor_cfg.i2c_addr, (0xB6 << 8) | BME280_RA_SOFTRESET)) {
#if ENABLE_TEST_REG_STATUS
		int i = 100;
		while(i--) {
			sleep_us(128); // pm_wait_ms(2)?
			if(read_i2c_byte_addr(sensor_cfg.i2c_addr, BME280_RA_STATUS, buf, 1)) {
				break;
			} else if((buf[0] & 1) == 0) {
#else
				pm_wait_ms(2); //?
#endif
				if(!send_i2c_word(sensor_cfg.i2c_addr, (BME280_SET_CTRL_HUMI << 8) | BME280_RA_CTRL_HUMI)
				&& !send_i2c_word(sensor_cfg.i2c_addr, (BME280_SET_CTRL_MEAS << 8) | BME280_RA_CTRL_MEAS)
				&& !send_i2c_word(sensor_cfg.i2c_addr, (BME280_SET_CONFIG << 8) | BME280_RA_CONFIG)
				// read calibrates values
				&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, BME280_RA_DIG_T1, (u8 *)&sensor_cfg.dig.T1, 28)
				&& !read_i2c_byte_addr(sensor_cfg.i2c_addr, BME280_RA_DIG_H2, buf, sizeof(buf))) {
					sensor_cfg.dig.H2 = (s16)(((s16)buf[1] << 8) | buf[0]);
					sensor_cfg.dig.H3 = buf[2];
					sensor_cfg.dig.H4 = (s16)((buf[3] << 8) | (buf[4] & 0x0F) << 4) >> 4;
					sensor_cfg.dig.H5 = (s16)((buf[5] << 8) | (0xF0 & buf[4])) >>  4;
					sensor_cfg.dig.H6 = buf[6];
					sensor_cfg.sensor_type = IU_SENSOR_BME280;
					// MaxMeasureTimeOut: 2.3*(OVERSAMP P+T+H)+1.25+0.575*2 = 9.3 ms
					sensor_cfg.measure_timeout = 10 * CLOCK_16M_SYS_TIMER_CLK_1MS;
					sensor_cfg.time_measure = clock_time() | 1;
					return;
				}
#if ENABLE_TEST_REG_STATUS
				else
					break;
			}
		}
#endif
	}
	sensor_cfg.i2c_addr = 0;
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
int read_sensor_cb(void) {
	bme280_reg_t reg;
	if(sensor_cfg.i2c_addr) {
#if ENABLE_TEST_REG_STATUS
		int i = 16;
		while(i--) {
			u8 status;
			if(read_i2c_byte_addr(sensor_cfg.i2c_addr, BME280_RA_STATUS, &status, 1)) {
				sleep_us(128); // pm_wait_ms(2)?
			} else if((status & 8) == 0) {
#endif
				if(!read_i2c_byte_addr(sensor_cfg.i2c_addr, BME280_RA_PRESSURE, (u8 *)&reg, sizeof(reg))) {
					// test cmd: 040108ECF7 -> 653c00 803900 5a35
					u32 adc = (reg.temp[0] << 12) | (reg.temp[1] << 4) | (reg.temp[2] >> 4);
					measured_data.temp = (s16)BME280_compensate_T(adc, &sensor_cfg) + sensor_cfg.coef.val1_z;
					adc = (reg.humi[0] << 8) | reg.humi[1];
					measured_data.humi = (u16)BME280_compensate_H(adc, &sensor_cfg) + sensor_cfg.coef.val2_z;
					adc = (reg.press[0] << 12) | (reg.press[1] << 4) | (reg.press[2] >> 4);
					measured_data.pressure = BME280_compensate_P(adc, &sensor_cfg);
					return 1;
				}
#if ENABLE_TEST_REG_STATUS
			}
		};
#endif
	} else
		init_sensor();
	return 0;
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
void start_measure_sensor_deep_sleep(void) {
	if(sensor_cfg.i2c_addr) {
		send_i2c_word(sensor_cfg.i2c_addr, (BME280_SET_CTRL_MEAS << 8) | BME280_RA_CTRL_MEAS);
	}
}

#endif //  defined(USE_SENSOR_BME280) && USE_SENSOR_BME280

