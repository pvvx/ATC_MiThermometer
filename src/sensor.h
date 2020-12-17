#pragma once

#include <stdint.h>


typedef void (*wakeup_callback_t)(void);
#define GPIO_WAKEUP_SENS GPIO_PC3 // GPIO_PC2 or GPIO_PC3 ?

extern int8_t temp_offset;
extern int8_t humi_offset;
extern uint32_t timer_measure_cb;
extern int32_t temp;
extern uint32_t humi;

void init_sensor();
void read_sensor_deep_sleep(void);
void read_sensor_sleep(void);
void read_sensor_cb(void);

