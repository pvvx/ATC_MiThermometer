#ifndef _SENSORS_H_
#define _SENSORS_H_

#ifndef USE_SENSOR_BMP280
#define USE_SENSOR_BMP280	0
#endif
#ifndef USE_SENSOR_BME280
#define USE_SENSOR_BME280	0
#endif

#if USE_SENSOR_BME280 || USE_SENSOR_BMP280
#include "bme280.h"
#endif

#define AHT2x_I2C_ADDR			0x38
#define CHT8305_I2C_ADDR		0x40
#define CHT8305_I2C_ADDR_MAX	0x43
#define SHT30_I2C_ADDR			0x44 // 0x44, 0x45
#define SHT30_I2C_ADDR_MAX		0x45
#define SHT4x_I2C_ADDR			0x44 // 0x44, 0x45, 0x46
#define SHT4x_I2C_ADDR_MAX		0x46
#define SHTC3_I2C_ADDR			0x70

#ifndef USE_SENSOR_CHT8305
#define USE_SENSOR_CHT8305 	0
#endif
#ifndef USE_SENSOR_AHT20_30
#define USE_SENSOR_AHT20_30	0
#endif
#ifndef USE_SENSOR_SHT4X
#define USE_SENSOR_SHT4X 	0
#endif
#ifndef USE_SENSOR_SHTC3
#define USE_SENSOR_SHTC3 	0
#endif
#ifndef USE_SENSOR_SHT30
#define USE_SENSOR_SHT30 	0
#endif

enum {
	ID_SENSOR_NONE = 0,
	ID_SENSOR_SHTC3,   // 1
	ID_SENSOR_SHT4x,   // 2
	ID_SENSOR_SHT30,	// 3
	ID_SENSOR_CHT8305,	// 4
	ID_SENSOR_AHT2x,	// 5
	ID_SENSOR_CHT8215,	// 6
	ID_SENSOR_INA226,	// 7
	ID_SENSOR_MY18B20,	// 8
	ID_SENSOR_MY18B20x2,	// 9
	ID_SENSOR_HX71X,	// 10
	ID_SENSOR_PWMRH,	// 11
	ID_SENSOR_NTC,		// 12
	ID_SENSOR_INA3221,	// 13
	ID_SENSOR_SCD41,	// 14
	ID_SENSOR_BME280,	// 15
	ID_SENSOR_BMP280,	// 16
	ID_SENSOR_PCF85163,	// 17
	ID_SENSOR_MCU,		// 18
	ID_SENSOR_TYPE_MAX // 18
} ID_SENSOR_TYPES;

#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS)) && !USE_SENSOR_BME280

typedef struct _thsensor_coef_t {
	u32 val1_k;	// temp_k / current_k
	u32 val2_k;	// humi_k / voltage_k
	s16 val1_z;		// temp_z / current_z
	s16 val2_z;		// humi_z / voltage_z
} sensor_coef_t; // [12]

typedef struct _sensor_def_cfg_t {
#if USE_SENSOR_INA3221
	sensor_coef_t coef[3];
#else
	sensor_coef_t coef;
#endif
#if SENSOR_SLEEP_MEASURE
	u32 measure_timeout;
#endif
	u8 sensor_type; // SENSOR_TYPES
} sensor_def_cfg_t;

typedef struct _sensor_cfg_t {
#if USE_SENSOR_INA3221
	sensor_coef_t coef[3];
#else
	sensor_coef_t coef;
#endif
	u32 id;
	u8 i2c_addr;
	u8 sensor_type; // SENSOR_TYPES
// not saved
#if USE_SENSOR_SCD41
	u32 wait_tik;
#endif
#if SENSOR_SLEEP_MEASURE
	volatile u32 time_measure;
	u32 measure_timeout;
#endif
} sensor_cfg_t;

extern sensor_cfg_t sensor_cfg;
#define sensor_cfg_send_size 18 //max 19

void init_sensor(void);
int start_measure_sensor_deep_sleep(void);
int read_sensor_cb(void);

#if (DEV_SERVICES & SERVICE_ILLUMI)
void read_illumi_sensor(void);
#endif

#endif

#endif //_SENSORS_H_
