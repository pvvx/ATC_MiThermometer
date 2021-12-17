#pragma once

#include <stdint.h>

#define MAX_VBAT_MV		3100 // 3100 mV - > battery = 100%
#define MIN_VBAT_MV		2200 // 2200 mV - > battery = 0%

uint16_t get_adc_mv(ADC_InputPchTypeDef p_ain);
#define get_battery_mv() get_adc_mv(6)	// Channel B5P
uint8_t get_battery_level(uint16_t battery_mv);

