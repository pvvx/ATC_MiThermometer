#pragma once

#include <stdbool.h>
#include <stdint.h>

void init_lcd();
void update_lcd();
void show_temp_symbol(uint8_t symbol);
void show_battery_symbol(bool state);
void show_big_number(int16_t number, bool point);
void show_small_number(uint16_t number, bool percent);
void show_smiley(uint8_t state);
void show_atc_mac();
void show_ble_symbol(bool state);
void send_to_lcd_long(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6);
