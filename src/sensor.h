#pragma once
#include <stdint.h>

extern int8_t temp_offset;
extern int8_t humi_offset;
extern uint32_t timer_measure_cb;
extern int16_t new_temp; // x 0.01 C
extern uint16_t new_humi; // x 0.01 %

void init_sensor();
void read_sensor_deep_sleep(void);
void read_sensor_sleep(void);
void read_sensor_start(void);
void read_sensor_cb(void);

