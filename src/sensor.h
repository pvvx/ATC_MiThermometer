#pragma once
#include <stdint.h>

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
	TH_SENSOR_NONE = 0,
	TH_SENSOR_SHTC3,   // 1
	TH_SENSOR_SHT4x,   // 2
	TH_SENSOR_SHT30,	// 3
	TH_SENSOR_CHT8305,	// 4
	TH_SENSOR_AHT2x,	// 5
	TH_SENSOR_CHT8215,	// 6
	IU_SENSOR_INA226,	// 7
	IU_SENSOR_MY18B20,	// 8
	IU_SENSOR_MY18B20x2,	// 9
	IU_SENSOR_HX71X,	// 10
	IU_SENSOR_PWMRH,	// 11
	IU_SENSOR_NTC,		// 12
	TH_SENSOR_TYPE_MAX // 13
} TH_SENSOR_TYPES;

#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS))

typedef struct _thsensor_coef_t {
	uint32_t val1_k;	// temp_k / current_k
	uint32_t val2_k;	// humi_k / voltage_k
	int16_t val1_z;		// temp_z / current_z
	int16_t val2_z;		// humi_z / voltage_z
} sensor_coef_t; // [12]

typedef struct _sensor_def_cfg_t {
	sensor_coef_t coef;
	uint32_t measure_timeout;
	uint8_t sensor_type; // SENSOR_TYPES
} sensor_def_cfg_t;

typedef struct _sensor_cfg_t {
	sensor_coef_t coef;
	uint32_t id;
	uint8_t i2c_addr;
	uint8_t sensor_type; // SENSOR_TYPES
	// not saved
#if SENSOR_SLEEP_MEASURE
	volatile uint32_t time_measure;
	uint32_t measure_timeout;
#endif
} sensor_cfg_t;

extern sensor_cfg_t sensor_cfg;
#define sensor_cfg_send_size 18 //max 19


void init_sensor(void);
void start_measure_sensor_deep_sleep(void);
int read_sensor_cb(void);

#endif

