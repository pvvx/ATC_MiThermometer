#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app_config.h"

void init_lcd();
void update_lcd();
void show_temp_symbol(uint8_t symbol);
void show_battery_symbol(bool state);
void show_big_number(int16_t number); // x0.1, (-995..19995), point auto: -99 .. -9.9 .. 199.9 .. 1999
void show_small_number(int16_t number, bool percent); // -9 .. 99
void show_smiley(uint8_t state);
//void show_atc_mac();
void show_ble_symbol(bool state);
void send_to_lcd_long(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6);

#if DEVICE_TYPE == DEVICE_MHO_C401
extern uint8_t display_buff[18];
extern uint8_t stage_lcd;
int task_lcd(void);
#elif DEVICE_TYPE == DEVICE_LYWSD03MMC
extern uint8_t display_buff[6];
#endif
