#pragma once
#include <stdint.h>

//extern volatile uint32_t timer_measure_cb; // time start measure

//#define SENSOR_MEASURING_TIMEOUT_ms  11 // SHTV3 11 ms, SHT4x max 8.2 ms
//#define SENSOR_MEASURING_TIMEOUT  thsensor_cfg.measure_timeout // (SENSOR_MEASURING_TIMEOUT_ms * CLOCK_16M_SYS_TIMER_CLK_1MS) // clk tick

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
#define USE_SENSOR_AHT20_30 	0
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
	TH_SENSOR_TYPE_MAX // 7
} TH_SENSOR_TYPES;


typedef struct _thsensor_coef_t {
	uint32_t temp_k;
	uint32_t humi_k;
	int16_t temp_z;
	int16_t humi_z;
} thsensor_coef_t; // [12]

typedef struct _thsensor_def_cfg_t {
	thsensor_coef_t coef;
	uint32_t measure_timeout;
	uint8_t sensor_type; // TH_SENSOR_TYPES
} thsensor_def_cfg_t;

typedef struct _thsensor_cfg_t {
	thsensor_coef_t coef;
	uint32_t id;
	uint8_t i2c_addr;
	uint8_t sensor_type; // TH_SENSOR_TYPES
	// not saved
	volatile uint32_t time_measure;
	uint32_t measure_timeout;
} thsensor_cfg_t;

extern thsensor_cfg_t thsensor_cfg;
#define thsensor_cfg_send_size 18 //max 19


void init_sensor(void);
void start_measure_sensor_deep_sleep(void);
int read_sensor_cb(void);


