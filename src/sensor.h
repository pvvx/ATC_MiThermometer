#pragma once
#include <stdint.h>

//extern volatile uint32_t timer_measure_cb; // time start measure

#define SENSOR_MEASURING_TIMEOUT_ms  11 // SHTV3 11 ms, SHT4x max 8.2 ms
#define SENSOR_MEASURING_TIMEOUT  (SENSOR_MEASURING_TIMEOUT_ms * CLOCK_16M_SYS_TIMER_CLK_1MS) // clk tick

#define SHTC3_I2C_ADDR		0x70
#define SHT4x_I2C_ADDR		0x44
#define SHT4xB_I2C_ADDR		0x45

typedef struct _thsensor_coef_t {
	uint32_t temp_k;
	uint32_t humi_k;
	int16_t temp_z;
	int16_t humi_z;
} thsensor_coef_t;

typedef struct _thsensor_cfg_t {
	thsensor_coef_t coef;
	uint32_t id;
	uint8_t i2c_addr;
	volatile uint32_t time_measure;
//	psernsor_rd_t read_sensor;
//	psernsor_sm_t start_measure;
} thsensor_cfg_t;

extern thsensor_cfg_t thsensor_cfg;
#define thsensor_cfg_send_size 17

extern const thsensor_coef_t def_thcoef_shtc3;
extern const thsensor_coef_t def_thcoef_sht4x;
//extern uint8_t sensor_i2c_addr;
//extern uint32_t sensor_id;

void init_sensor(void);
void start_measure_sensor_deep_sleep(void);
void start_measure_sensor_low_power(void);
int read_sensor_cb(void);
void sensor_go_sleep(void);


