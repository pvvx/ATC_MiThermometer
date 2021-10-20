#pragma once
#include <stdint.h>

extern volatile uint32_t timer_measure_cb;

#if (HTSENSOR == HTSENSOR_SHTV3)
#define SENSOR_MEASURING_TIMEOUT_ms  11 // 11 ms
#else
#define SENSOR_MEASURING_TIMEOUT_ms  9 // 9 ms (SHT4x max 8.2 ms)
#endif
#define SENSOR_MEASURING_TIMEOUT  (SENSOR_MEASURING_TIMEOUT_ms * CLOCK_16M_SYS_TIMER_CLK_1MS) // 11 ms

void init_sensor();
void read_sensor_deep_sleep(void);
void read_sensor_low_power(void);
int read_sensor_cb(void);
//int read_sensor_sleep(void);

