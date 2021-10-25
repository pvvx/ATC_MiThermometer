#pragma once
#include <stdint.h>

extern volatile uint32_t timer_measure_cb; // time start measure

#define SENSOR_MEASURING_TIMEOUT_ms  11 // SHTV3 11 ms, SHT4x 8.2 ms
#define SENSOR_MEASURING_TIMEOUT  (SENSOR_MEASURING_TIMEOUT_ms * CLOCK_16M_SYS_TIMER_CLK_1MS) // clk tick

#define SHTC3_I2C_ADDR		0x70
#define SHT4x_I2C_ADDR		0x44

extern uint8_t sensor_i2c_addr;

void init_sensor();
void read_sensor_deep_sleep(void);
void read_sensor_low_power(void);
int read_sensor_cb(void);
//int read_sensor_sleep(void);

