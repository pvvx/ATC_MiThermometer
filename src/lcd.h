#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app_config.h"

void init_lcd();
void update_lcd();
/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E" 
 * Warning: MHO-C401 Symbols: "%", "°Г", "(  )", "." have one control bit! */
void show_temp_symbol(uint8_t symbol);
/* ==================
 * LYWSD03MMC:
 * 0 = "     " off,
 * 1 = " ^_^ "
 * 2 = " -^- "
 * 3 = " ooo "
 * 4 = "(   )"
 * 5 = "(^_^)" happy
 * 6 = "(-^-)" sad
 * 7 = "(ooo)"
 * -------------------
 * MHO-C401:
 * 0 = "   " off,
 * 1 = " o "
 * 2 = "o^o"
 * 3 = "o-o"
 * 4 = "oVo"
 * 5 = "vVv" happy
 * 6 = "^-^" sad
 * 7 = "oOo" */
void show_smiley(uint8_t state);
void show_big_number(int16_t number); // x0.1, (-995..19995), point auto: -99 .. -9.9 .. 199.9 .. 1999
void show_small_number(int16_t number, bool percent); // -9 .. 99
void show_battery_symbol(bool state);
void show_ble_symbol(bool state);
void send_to_lcd_long(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6);

#if DEVICE_TYPE == DEVICE_MHO_C401
extern uint8_t display_buff[18];
extern uint8_t stage_lcd;
int task_lcd(void);
#elif DEVICE_TYPE == DEVICE_LYWSD03MMC
extern uint8_t display_buff[6];
#else
#error "Set DEVICE_TYPE!"
#endif
