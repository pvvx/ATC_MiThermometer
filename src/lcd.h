#ifndef _LCD_H_
#define _LCD_H_

#include "app_config.h"

#if (DEV_SERVICES & SERVICE_SCREEN)

#include "sensor.h"

typedef struct _lcd_flg_t {
	u32 chow_ext_ut; // count chow ext.vars validity time, in sec
#if  !((DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN))
	u32 min_step_time_update_lcd; // = cfg.min_step_time_update_lcd * (50 * CLOCK_16M_SYS_TIMER_CLK_1MS)
	u32 tim_last_chow; // timer show lcd >= 1.5 sec
	u8 show_stage; // count/stage update lcd code buffer
	u8 update_next_measure; 	  // flag update LCD if next_measure
#endif
	u8 update; 	  // flag update LCD
	union {
		struct {
			// reset all flags on disconnect
			u8 ext_data_buf: 	1; // LCD show external buffer
			u8 notify_on: 	1; // Send LCD dump if Notify on
			u8 res:  		5;
			u8 send_notify: 1; // flag update LCD for send notify
		}b;
		u8 all_flg;
	};
} lcd_flg_t;
extern lcd_flg_t lcd_flg;

// LCD controller I2C address
#define B14_I2C_ADDR			0x3C
#define B16_I2C_ADDR			0	 // UART
#define N16_I2C_ADDR			1	 // SPI
#define B19_I2C_ADDR			0x3E // BU9792AFUV
#define BU9792AFUV_I2C_ADDR		0x3E // BU9792AFUV
#define BL55028_I2C_ADDR		0x3E // BL55028
#define AIP31620E_I2C_ADDR		0x3E // AIP31620E

extern u8 lcd_i2c_addr; // LCD controller I2C address

#if  !((DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN))
/* CGG1 no symbol 'smiley' ! */
#define SMILE_HAPPY 5 		// "(^-^)" happy
#define SMILE_SAD   6 		// "(-^-)" sad
#define TMP_SYM_C	0xA0	// "°C"
#define TMP_SYM_F	0x60	// "°F"

//#define SET_LCD_UPDATE()
void lcd(void);
void init_lcd(void);
void send_to_lcd(void);
void update_lcd(void);
/* 0x00 = "  "
 * 0x20 = "°Г"
 * 0x40 = " -"
 * 0x60 = "°F"
 * 0x80 = " _"
 * 0xA0 = "°C"
 * 0xC0 = " ="
 * 0xE0 = "°E"
 * Warning: MHO-C401 Symbols: "%", "°Г", "(  )", "." have one control bit! */
void show_temp_symbol(u8 symbol);
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
void show_smiley(u8 state);
#endif

void show_battery_symbol(bool state);
void show_big_number_x10(s16 number); // x0.1, (-995..19995), point auto: -99 .. -9.9 .. 199.9 .. 1999
void show_ble_symbol(bool state);

#if	USE_DISPLAY_CLOCK
void show_clock(void);
#endif
////////// DEVICE_TYPE //////////
#if DEVICE_TYPE == DEVICE_MHO_C401

#define SHOW_OTA_SCREEN()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
#define SHOW_REBOOT_SCREEN()
#define LCD_BUF_SIZE	18
#define SHOW_SMILEY		1
extern u8 stage_lcd;
void show_small_number(s16 number, bool percent); // -9 .. 99
int task_lcd(void);
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];

#elif DEVICE_TYPE == DEVICE_MHO_C401N

#define SHOW_OTA_SCREEN()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
#define SHOW_REBOOT_SCREEN()
#define LCD_BUF_SIZE	16
#define SHOW_SMILEY		1
extern u8 stage_lcd;
void show_small_number(s16 number, bool percent); // -9 .. 99
void show_connected_symbol(bool state);
int task_lcd(void);
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];

#elif DEVICE_TYPE == DEVICE_CGG1

#define SHOW_OTA_SCREEN()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
#define SHOW_REBOOT_SCREEN()
#if DEVICE_CGG1_ver == 2022
#define LCD_BUF_SIZE	16
#else
#define LCD_BUF_SIZE	18
#endif
#define SHOW_SMILEY		1
extern u8 stage_lcd;
void show_small_number_x10(s16 number, bool percent); // -9 .. 99
int task_lcd(void);
void show_batt_cgg1(void);
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];

#elif DEVICE_TYPE == DEVICE_LYWSD03MMC

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
void show_reboot_screen(void);
#define SHOW_REBOOT_SCREEN() show_reboot_screen()
#define LCD_BUF_SIZE	6
#define SHOW_SMILEY		1
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];
void show_small_number(s16 number, bool percent); // -9 .. 99

#elif DEVICE_TYPE == DEVICE_CGDK2

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()
#define LCD_BUF_SIZE	18
#define SHOW_SMILEY		0
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
void show_reboot_screen(void);
#define SHOW_REBOOT_SCREEN() show_reboot_screen()
void show_batt_cgdk2(void);
void show_small_number_x10(s16 number, bool percent); // -9 .. 99
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];

#elif DEVICE_TYPE == DEVICE_MHO_C122

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
void show_reboot_screen(void);
#define SHOW_REBOOT_SCREEN() show_reboot_screen()
#define LCD_BUF_SIZE	6
#define SHOW_SMILEY		1
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];
void show_small_number(s16 number, bool percent); // -9 .. 99

#elif DEVICE_TYPE == DEVICE_ZTH03

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
void show_reboot_screen(void);
#define SHOW_REBOOT_SCREEN() show_reboot_screen()
#define LCD_BUF_SIZE	6
#define SHOW_SMILEY		1
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE+1];
void show_small_number(s16 number, bool percent); // -9 .. 99

#elif DEVICE_TYPE == DEVICE_LKTMZL02

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
void show_reboot_screen(void);
#define SHOW_REBOOT_SCREEN() show_reboot_screen()
#define LCD_BUF_SIZE	7
#define SHOW_SMILEY		0
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];
void show_small_number(s16 number, bool percent); // -9 .. 99

#elif DEVICE_TYPE == DEVICE_ZTH05Z

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
void show_reboot_screen(void);
#define SHOW_REBOOT_SCREEN() show_reboot_screen()
#define LCD_BUF_SIZE	6
#define SHOW_SMILEY		1
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];
void show_small_number(s16 number, bool percent); // -9 .. 99

#elif (DEVICE_TYPE == DEVICE_ZYZTH01)

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
void show_reboot_screen(void);
#define SHOW_REBOOT_SCREEN() show_reboot_screen()
#define LCD_BUF_SIZE	6
#define SHOW_SMILEY		0
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE+1];
void show_small_number(s16 number, bool percent); // -9 .. 99

#elif DEVICE_TYPE == DEVICE_MJWSD06MMC

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()
#define SET_LCD_UPDATE() { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define SHOW_CONNECTED_SYMBOL(a) { lcd_flg.update = 1; lcd_flg.update_next_measure = 0; }
#define POWERUP_SCREEN	0
void show_reboot_screen(void);
#define SHOW_REBOOT_SCREEN() show_reboot_screen()
#define LCD_BUF_SIZE	6
#define SHOW_SMILEY		1
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE+1];
void show_small_number(s16 number, bool percent); // -9 .. 99

#else
#error "Set DEVICE_TYPE!"
#endif

#else // (DEVICE_TYPE == DEVICE_MJWSD05MMC)


enum {
	SCR_TYPE_TIME = 0,	//0 Time (default)
	SCR_TYPE_TEMP,		//1 Temperature
	SCR_TYPE_HUMI,		//2 Humidity
	SCR_TYPE_BAT_P,		//3 Battery %
	SCR_TYPE_BAT_V,		//4 Battery V
	SCR_TYPE_THERMOSTAT_SMALL, //5 Small thermostat
	SCR_TYPE_THERMOSTAT_BIG,   //6 Big thermostat
	SCR_TYPE_EXT		//7 External number & symbols
} SCR_TYPE_ENUM;


//#define  min_step_time_update_lcd (500 * CLOCK_16M_SYS_TIMER_CLK_1MS)

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
void show_smiley(u8 state);

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
void show_symbol_s1(u8 symbol);

void show_reboot_screen(void);
#define POWERUP_SCREEN	1
#define SHOW_REBOOT_SCREEN() show_reboot_screen()

extern volatile u8 lcd_update;
#define SET_LCD_UPDATE() lcd_flg.update = 1
#define SHOW_CONNECTED_SYMBOL(a) lcd_flg.update = 1

void show_ota_screen(void);
#define SHOW_OTA_SCREEN() show_ota_screen()

void init_lcd(void);
void lcd(void);
void send_to_lcd(void);
void update_lcd(void);
void show_battery_symbol(bool state);
void show_ble_symbol(bool state);
void show_low_bat(void);

#define LCD_BUF_SIZE	18
extern u8 display_buff[LCD_BUF_SIZE], display_cmp_buff[LCD_BUF_SIZE];

#endif // (DEVICE_TYPE == DEVICE_MJWSD05MMC)

#else

#define POWERUP_SCREEN	0
#define SET_LCD_UPDATE()
#define SHOW_OTA_SCREEN()
#define SHOW_REBOOT_SCREEN()
#define SHOW_CONNECTED_SYMBOL(a)
#define init_lcd()
#define lcd()
#define update_lcd()

#endif // #if (DEV_SERVICES & SERVICE_SCREEN)

#endif //_LCD_H_
