#pragma once

#include <stdbool.h>
#include <stdint.h>

uint8_t ota_is_working;

void init_ble();
void set_adv_data(int16_t temp, uint16_t humi, uint8_t battery_level, uint16_t battery_mv);
bool ble_get_connected();
void ble_send_temp(uint16_t temp);
void ble_send_humi(uint16_t humi);
void ble_send_battery(uint8_t value);
void blt_pm_proc(void);
