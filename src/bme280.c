/*
 * BME280.c
 *
 *  Created on: 26 янв. 2016 г.
 *      Author: PVV
 */
#include "tl_common.h"
#include "app_config.h"

#if (USE_SENSOR_BME280 || USE_SENSOR_BMP280)

#include "drivers.h"
#include "i2c.h"
#include "sensor.h"
#include "app.h"

#define ENABLE_TEST_REG_STATUS		0

extern u64 mul32x32_64(u32 a, u32 b); // hard function (in div_mod.S)

#if USE_SENSOR_BME280
const sensor_coef_t def_bme_cfg = {
		.val1_k = 8192,   // temp_k
		.val2_k = 102400, // 1024*100 humi_k
		.val1_z = 0, // temp_z
		.val2_z = 0 // humi_z
};
RAM sensor_cfg_t sensor_cfg;
#define bmx_struct sensor_cfg
#else
RAM sensor_bmx280_t sensor_bmx280;
#define bmx_struct sensor_bmx280
#endif

// 2048-(-4000) = 6048

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
static _attribute_ram_code_
s32 BMx280_compensate_T(s32 adc_T, bmx280_dig_t *p) {
	s32 var1, var2;
	var1 = ((((adc_T >> 3) - ((s32)p->T1 << 1))) * ((s32)p->T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((s32)p->T1)) * ((adc_T >> 4) - ((s32)p->T1))) >> 12) * ((s32)p->T3)) >> 14;
	p->t_fine = var1 + var2;
	return (p->t_fine * 5 + 128) >> 8;
}

#if USE_SENSOR_BME280
// Returns humidity in %RH, resolution is 0.01.
// Output value of “4633”represents 4633/100 = 46.33%RH
static _attribute_ram_code_
u32 BME280_compensate_H(s32 adc_H, bmx280_dig_t *p, u32 k, s32 z)
{
  s32 h;
  h = (p->t_fine - ((s32)76800));
  h = (((((adc_H << 14) - (((s32)p->H4) << 20) - (((s32)p->H5) * h)) +
    ((s32)16384)) >> 15) * (((((((h * ((s32)p->H6)) >> 10) * (((h *
    ((s32)p->H3)) >> 11) + ((s32)32768))) >> 10) + ((s32)2097152)) * ((s32)p->H2) + 8192) >> 14));
  h = (h - (((((h >> 15) * (h >> 15)) >> 7) * ((s32)p->H1)) >> 4));
  // h in %RH as unsigned 32 bit integer in 10.22 format (4194304 = 1%)
  h += z * 41943; // (1<<22)/100
  if(h < 0)
	  h = 0;
  else {
	  u8 r = irq_disable();
	  h = mul32x32_64((u32)h, k) >> 32;
	  irq_restore(r);
	  if(h > 9999)
		h = 9999;
  }
  return (u32)h;
}
#endif

// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
static _attribute_ram_code_
u32 BMx280_compensate_P(s32 adc_P, bmx280_dig_t *p)
{
	s32 var1, var2;
	u32 v;
	var1 = (((s32)p->t_fine) >> 1) - (s32)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11) * ((s32)p->P6);
	var2 = var2 + ((var1 * ((s32)p->P5)) << 1);
	var2 = (var2 >> 2)+(((s32)p->P4) << 16);
	var1 = (((p->P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((s32)p->P2) * var1) >> 1)) >> 18;
	var1 =((((32768 + var1)) * ((s32)p->P1)) >> 15);
	if (var1 == 0)
		return 0; // avoid exception caused by division by zero
	v = (((u32)(((s32)1048576) - adc_P) - (var2 >> 12))) * 3125;
	if (v < 0x80000000)
		v = (v << 1) / ((u32)var1);
	else
		v = (v / (u32)var1) * 2;
	var1 = (((s32)p->P9) * ((s32)(((v >> 3) * (v >> 3)) >> 13))) >>12;
	var2 = (((s32)(v >> 2)) * ((s32)p->P8)) >> 13;
	v = (u32)((s32)v + ((var1 + var2 + p->P7) >> 4));
	return v;
}

//-------------------------------------------------------------------------------
// init BMP280/BME280
//-------------------------------------------------------------------------------
void init_sensor_bmx280(void) {
#if USE_SENSOR_BME280
	u8 buf[7];
#endif
	if((((bmx_struct.i2c_addr = (u8) scan_i2c_addr(BMx280_I2C_ADDR1<<1)) != 0)
	|| ((bmx_struct.i2c_addr = (u8) scan_i2c_addr(BMx280_I2C_ADDR2<<1)) != 0))
	&& !read_i2c_byte_addr(bmx_struct.i2c_addr, BMx280_RA_CHIPID, (u8 *)&bmx_struct.id, 2)
#if USE_SENSOR_BME280
	&& (bmx_struct.id & 0xff) == BME280_ID
#else
	&& (bmx_struct.id & 0xff) == BMP280_ID
#endif
	&& !send_i2c_word(bmx_struct.i2c_addr, (BMx280_RESET << 8) | BMx280_RA_SOFTRESET)) {
#if ENABLE_TEST_REG_STATUS
		int i = 100;
		while(i--) {
			sleep_us(128); // pm_wait_ms(2)?
			if(read_i2c_byte_addr(bmx_struct.i2c_addr, BMx280_RA_STATUS, buf, 1)) {
				break;
			} else if((buf[0] & 1) == 0) {
#else
				pm_wait_ms(2); //?
#endif
#if USE_SENSOR_BME280
				if(!send_i2c_word(bmx_struct.i2c_addr, (BME280_SET_CTRL_HUMI << 8) | BME280_RA_CTRL_HUMI)
				&& !send_i2c_word(bmx_struct.i2c_addr, (BMx280_SET_CTRL_MEAS << 8) | BMx280_RA_CTRL_MEAS)
				&& !send_i2c_word(bmx_struct.i2c_addr, (BMx280_SET_CONFIG << 8) | BMx280_RA_CONFIG)
				// read calibrates values
				&& !read_i2c_byte_addr(bmx_struct.i2c_addr, BMx280_RA_DIG_T1, (u8 *)&bmx_struct.dig.T1, (3+10)*2)
				&& !read_i2c_byte_addr(bmx_struct.i2c_addr, BME280_RA_DIG_H2, buf, sizeof(buf))) {
					bmx_struct.dig.H2 = (s16)(((s16)buf[1] << 8) | buf[0]);
					bmx_struct.dig.H3 = buf[2];
					bmx_struct.dig.H4 = (s16)((buf[3] << 8) | (buf[4] & 0x0F) << 4) >> 4;
					bmx_struct.dig.H5 = (s16)((buf[5] << 8) | (0xF0 & buf[4])) >>  4;
					bmx_struct.dig.H6 = buf[6];
					bmx_struct.sensor_type = IU_SENSOR_BME280;
					// MaxMeasureTimeOut: 2.3*(OVERSAMP P+T+H)+1.25+0.575*2 = 9.3 ms
					bmx_struct.measure_timeout = 10 * CLOCK_16M_SYS_TIMER_CLK_1MS;
					bmx_struct.time_measure = clock_time() | 1;
					if(!bmx_struct.coef.val2_k) {
						memcpy(&bmx_struct.coef, &def_bme_cfg, sizeof(bmx_struct.coef));
					}
					return;
				}
#else
				if(!send_i2c_word(bmx_struct.i2c_addr, (BMx280_SET_CTRL_MEAS << 8) | BMx280_RA_CTRL_MEAS)
				&& !send_i2c_word(bmx_struct.i2c_addr, (BMx280_SET_CONFIG << 8) | BMx280_RA_CONFIG)
				// read calibrates values
				&& !read_i2c_byte_addr(bmx_struct.i2c_addr, BMx280_RA_DIG_T1, (u8 *)&bmx_struct.dig.T1, (3+9)*2)) {
					// bmx_struct.sensor_type = IU_SENSOR_BMP280;
					// MaxMeasureTimeOut: 2.3*(OVERSAMP P+T)+1.25+0.575*2 = 9.3 ms
					if(sensor_cfg.measure_timeout < 10 * CLOCK_16M_SYS_TIMER_CLK_1MS)
						sensor_cfg.measure_timeout = 10 * CLOCK_16M_SYS_TIMER_CLK_1MS;
					//sensor_cfg.time_measure = clock_time() | 1;
					return;
				}
#endif
#if ENABLE_TEST_REG_STATUS
				else
					break;
			}
		}
#endif
	}
	bmx_struct.i2c_addr = 0;
}

/*  return = 1 -> ok */
_attribute_ram_code_
__attribute__((optimize("-Os")))
int read_sensor_bmx280_cb(void) {
	bmx280_reg_t reg;
	if(bmx_struct.i2c_addr) {
#if ENABLE_TEST_REG_STATUS
		int i = 16;
		while(i--) {
			u8 status;
			if(read_i2c_byte_addr(bmx_struct.i2c_addr, BMx280_RA_STATUS, &status, 1)) {
				sleep_us(128); // pm_wait_ms(2)?
			} else if((status & 8) == 0) {
#endif
				if(!read_i2c_byte_addr(bmx_struct.i2c_addr, BMx280_RA_PRESSURE, (u8 *)&reg, sizeof(reg))) {
					// test cmd: 040108ECF7 -> 653c00 803900 5a35
#if USE_SENSOR_BME280
					u32 adc = (reg.temp[0] << 12) | (reg.temp[1] << 4) | (reg.temp[2] >> 4);
					measured_data.temp = (s16)(
							((BMx280_compensate_T(adc, &bmx_struct.dig) * bmx_struct.coef.val1_k) >> 13)
							+ bmx_struct.coef.val1_z);
					adc = (reg.humi[0] << 8) | reg.humi[1];
					measured_data.humi = (u16)BME280_compensate_H(adc, &bmx_struct.dig, bmx_struct.coef.val2_k, bmx_struct.coef.val2_z);
					adc = (reg.press[0] << 12) | (reg.press[1] << 4) | (reg.press[2] >> 4);
					measured_data.pressure = BMx280_compensate_P(adc, &bmx_struct.dig);
					measured_data.count++;
#else
					u32 adc = (reg.temp[0] << 12) | (reg.temp[1] << 4) | (reg.temp[2] >> 4);
					sensor_bmx280.temp = (s16)BMx280_compensate_T(adc, &bmx_struct.dig);
					adc = (reg.press[0] << 12) | (reg.press[1] << 4) | (reg.press[2] >> 4);
					measured_data.pressure = BMx280_compensate_P(adc, &bmx_struct.dig);
					//measured_data.count++;
#endif
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

/*  return: = 0 -> ok */
_attribute_ram_code_
__attribute__((optimize("-Os")))
int start_measure_sensor_bmx280_deep_sleep(void) {
	if(bmx_struct.i2c_addr) {
		if(!send_i2c_word(bmx_struct.i2c_addr, (BMx280_SET_CTRL_MEAS << 8) | BMx280_RA_CTRL_MEAS)) {
			return 0;
		}
		bmx_struct.i2c_addr = 0;
	}
	return 1;
}

#endif //  (USE_SENSOR_BME280 || USE_SENSOR_BMP280)

