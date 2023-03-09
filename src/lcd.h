#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app_config.h"
#if (DEVICE_TYPE != DEVICE_MJWSD05MMC)
/* CGG1 no symbol 'smiley' ! */
#define SMILE_HAPPY 5 		// "(^-^)" happy
#define SMILE_SAD   6 		// "(-^-)" sad
#define TMP_SYM_C	0xA0	// "°C"
#define TMP_SYM_F	0x60	// "°F"

#define B14_I2C_ADDR		0x3C
#define B16_I2C_ADDR		0	// UART
#define B19_I2C_ADDR		0x3E
#define CGDK2_I2C_ADDR		0x3E

extern uint8_t lcd_i2c_addr;


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
#if	(DEVICE_TYPE != DEVICE_CGDK2)
void show_smiley(uint8_t state);
#endif
void show_battery_symbol(bool state);
void show_big_number_x10(int16_t number); // x0.1, (-995..19995), point auto: -99 .. -9.9 .. 199.9 .. 1999
void show_ble_symbol(bool state);
#if	USE_CLOCK
void show_clock(void);
#endif

#if DEVICE_TYPE == DEVICE_MHO_C401
extern uint8_t display_buff[18];
extern uint8_t stage_lcd;
void show_small_number(int16_t number, bool percent); // -9 .. 99
int task_lcd(void);
#elif DEVICE_TYPE == DEVICE_MHO_C401N
extern uint8_t display_buff[16];
extern uint8_t stage_lcd;
void show_small_number(int16_t number, bool percent); // -9 .. 99
#define LCD_CONN_SYMBOL		1
void show_connected_symbol(bool state);
int task_lcd(void);
#elif DEVICE_TYPE == DEVICE_CGG1
#if DEVICE_CGG1_ver == 2022
extern uint8_t display_buff[16];
#else
extern uint8_t display_buff[18];
#endif
extern uint8_t stage_lcd;
void show_small_number_x10(int16_t number, bool percent); // -9 .. 99
int task_lcd(void);
void show_batt_cgg1(void);
#elif DEVICE_TYPE == DEVICE_LYWSD03MMC
extern uint8_t display_buff[6];
void show_small_number(int16_t number, bool percent); // -9 .. 99
#elif DEVICE_TYPE == DEVICE_CGDK2
extern uint8_t display_buff[18];
void show_batt_cgdk2(void);
void show_small_number_x10(int16_t number, bool percent); // -9 .. 99
#else
#error "Set DEVICE_TYPE!"
#endif

#else // (DEVICE_TYPE == DEVICE_MJWSD05MMC)

#define SHOW_OTA_SCREEN	1
#define SHOW_REBOOT_SCREEN	1

enum {
	SCR_TYPE_TIME = 0,	//0 Time (default)
	SCR_TYPE_TEMP,		//1 Temperature
	SCR_TYPE_HUMI,		//2 Humidity
	SCR_TYPE_BAT_P,		//3 Battery %
	SCR_TYPE_BAT_V,		//4 Battery V
	SCR_TYPE_EXT		//5 External number & symbols
} SCR_TYPE_ENUM;

#define MJWSD05MMC_LCD_I2C_ADDR	0x3E
#define  min_step_time_update_lcd (500 * CLOCK_16M_SYS_TIMER_CLK_1MS)

#define LCD_SYM_SMILEY_NONE  0 	// "(^-^)" happy
#define LCD_SYM_SMILEY_HAPPY 5 	// "(^-^)" happy
#define LCD_SYM_SMILEY_SAD   6 	// "(-^-)" sad
/* 0 = "     " off,
 * 1 = " ^-^ "
 * 2 = " -^- "
 * 3 = " ooo "
 * 4 = "(   )"
 * 5 = "(^-^)" happy
 * 6 = "(-^-)" sad
 * 7 = "(ooo)" */
void show_smiley(uint8_t state);

#define LCD_SYM_N	0x00	// " "
#define LCD_SYM_C	0x03	// "°C"
#define LCD_SYM_F	0x07	// "°F"
#define LCD_SYM_P	0x08	// "%"
#define LCD_SYM_T	0x10	// ":"
/* 0x00 = "  "
 * 0x01 = "°г"
 * 0x02 = " -"
 * 0x03 = "°c"
 * 0x04 = " |"
 * 0x05 = "°Г"
 * 0x06 = " г"
 * 0x07 = "°F"
 * 0x08 = "%"
 * 0x10 = ":" */
void show_symbol_s1(uint8_t symbol);

extern uint8_t lcd_i2c_addr;

#if (defined(SHOW_OTA_SCREEN) && SHOW_OTA_SCREEN)
void show_ota_screen(void);
#endif
#if (defined(SHOW_REBOOT_SCREEN) && SHOW_REBOOT_SCREEN)
void show_reboot_screen(void);
#endif

void init_lcd(void);
void lcd(void);
void update_lcd(void);
void show_battery_symbol(bool state);
void show_ble_symbol(bool state);

void show_s1_number_x100(int32_t number, uint8_t atr);
void show_s3_number_x10(int32_t number, uint8_t atr);
void show_s4_number_x10(int32_t number, uint8_t atr);
void show_clock_s1(void);
void show_clock_s3(void);
void show_battery_s1(uint8_t level);
void show_data_s2(void);
void show_low_bat(void);

#define LCD_BUF_SIZE	18
//extern uint32_t min_step_time_update_lcd; // = cfg.min_step_time_update_lcd * 0.05 sec
//extern uint32_t tim_last_chow; // timer show lcd >= 1.5 sec
extern uint8_t display_buff[LCD_BUF_SIZE];

#endif // (DEVICE_TYPE == DEVICE_MJWSD05MMC)
