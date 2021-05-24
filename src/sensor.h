#pragma once
#include <stdint.h>

extern volatile uint32_t timer_measure_cb;

#define SENSOR_MEASURING_TIMEOUT  (11 * CLOCK_16M_SYS_TIMER_CLK_1MS) // 11 ms

void init_sensor();
void read_sensor_deep_sleep(void);
void read_sensor_low_power(void);
int read_sensor_cb(void);
//int read_sensor_sleep(void);

