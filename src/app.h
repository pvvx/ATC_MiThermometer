/*
 * app.h
 *
 *  Created on: 19.12.2020
 *      Author: pvvx
 */

#ifndef _APP_H_
#define _APP_H_

#include "drivers/8258/gpio_8258.h"

enum {
	HW_VER_LYWSD03MMC_B14 = 0,  //0 SHTV3
	HW_VER_MHO_C401,		//1 SHTV3
	HW_VER_CGG1,			//2 SHTV3
	HW_VER_LYWSD03MMC_B19,	//3 SHT4x
	HW_VER_LYWSD03MMC_B16,	//4 SHT4x
	HW_VER_LYWSD03MMC_B17,	//5 SHT4x
	HW_VER_CGDK2,			//6 SHTV3
	HW_VER_CGG1_2022,		//7 SHTV3
	HW_VER_MHO_C401_2022,	//8 SHTV3
	HW_VER_MJWSD05MMC,		//9 SHT4x
	HW_VER_LYWSD03MMC_B15,	//10 SHTV3
	HW_VER_MHO_C122, 		//11 SHTV3
	HW_VER_MJWSD05MMC_EN	//12 SHT4x
} HW_VERSION_ID;
#define HW_VER_EXTENDED  	15

// Adv. types
enum {
	ADV_TYPE_ATC = 0,
	ADV_TYPE_PVVX,
	ADV_TYPE_MI,
	ADV_TYPE_BTHOME // (default)
} ADV_TYPE_ENUM;

#define ADV_TYPE_DEFAULT	ADV_TYPE_BTHOME

// cfg.flg2
#define MASK_FLG2_REBOOT	0x60
#define MASK_FLG2_SCR_OFF	0x80

// cfg.flg3
#define MASK_FLG3_WEEKDAY	0x80


typedef struct __attribute__((packed)) _cfg_t {
	struct __attribute__((packed)) {
		u8 advertising_type	: 2; // 0 - atc1441, 1 - Custom (pvvx), 2 - Mi, 3 - BTHome
		u8 comfort_smiley		: 1;
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
		u8 x100				: 1;
#else
		u8 show_time_smile	: 1; // if USE_CLOCK: = 0 - smile, =1 time, else: blinking on/off
#endif
		u8 temp_F_or_C			: 1;
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
		u8 time_am_pm			: 1;
#else
		u8 show_batt_enabled	: 1;
#endif
		u8 tx_measures			: 1; // Send all measurements in connected mode
		u8 lp_measures			: 1; // Sensor measurements in "Low Power" mode
	} flg;
	struct __attribute__((packed)) {
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
	 * 7 = "oOo"
	 * -------------------
	 * CGG1:
	 * 0 = "   " off,
	 * &1 = "---" Line
	 * -------------------
	 * MJWSD05MMC
	 * screen_type:
	 * 0 = Time
	 * 1 = Temperature
	 * 2 = Humidity
	 * 3 = Battery %
	 * 4 = Battery V
	 * 5 = External number & symbols
	 * */
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
		u8 screen_type	: 3;
#else
		u8 smiley 		: 3;	// 0..7
#endif
		u8 adv_crypto	: 1; 	// advertising uses crypto beacon
		u8 adv_flags  	: 1; 	// advertising add flags
		u8 bt5phy  		: 1; 	// support BT5.0 All PHY
		u8 longrange  	: 1;  	// advertising in LongRange mode (сбрасывается после отключения питания)
		u8 screen_off	: 1;	// screen off, v4.3+
	} flg2;
#if	VERSION < 0x47
	s8 temp_offset; // Set temp offset, -12,5 - +12,5 °C (-125..125)
	s8 humi_offset; // Set humi offset, -12,5 - +12,5 % (-125..125)
#else

	struct __attribute__((packed)) {
		u8 adv_interval_delay	: 4; // 0..15,  in 0.625 ms, a pseudo-random value in the range from 0 to X ms is added to a fixed advInterval so that advertising events change over time.
		u8 reserved				: 3;
		u8 not_day_of_week		: 1; // do not display day of week (MJWSD05MMC)
	} flg3;
	u8 event_adv_cnt;		// min value = 5
#endif
	u8 advertising_interval; // multiply by 62.5 for value in ms (1..160,  62.5 ms .. 10 sec)
	u8 measure_interval; // measure_interval = advertising_interval * x (2..25)
	u8 rf_tx_power; // RF_POWER_N25p18dBm .. RF_POWER_P3p01dBm (130..191)
	u8 connect_latency; // +1 x0.02 sec ( = connection interval), Tmin = 1*20 = 20 ms, Tmax = 256 * 20 = 5120 ms
	u8 min_step_time_update_lcd; // x0.05 sec, 0.5..12.75 sec (10..255)
	u8 hw_ver; // read only
	u8 averaging_measurements; // * measure_interval, 0 - off, 1..255 * measure_interval
}cfg_t;
extern cfg_t cfg;
extern const cfg_t def_cfg;

#if (DEV_SERVICES & SERVICE_SCREEN)
/* Warning: MHO-C401 Symbols: "%", "°Г", "(  )", "." have one control bit! */
typedef struct __attribute__((packed)) _external_data_t {
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
	s32		number; // -999.50..19995.50, x0.01
	u16 	vtime_sec; // validity time, in sec
	struct __attribute__((packed)) {
		/* 0 = "     " off,
		 * 1 = " ^-^ "
		 * 2 = " -^- "
		 * 3 = " ooo "
		 * 4 = "(   )"
		 * 5 = "(^-^)" happy
		 * 6 = "(-^-)" sad
		 * 7 = "(ooo)" */
		u8 smiley			: 3;
		u8 battery			: 1;
		/* 0x00 = "  "
		 * 0x01 = "°г"
		 * 0x02 = " -"
		 * 0x03 = "°c"
		 * 0x04 = " |"
		 * 0x05 = "°Г"
		 * 0x06 = " г"
		 * 0x07 = "°F"
		 * 0x08 = "%" */
		u8 temp_symbol		: 4;
	} flg;
#else
	s16		big_number; // -995..19995, x0.1
	s16		small_number; // -9..99, x1
	u16 	vtime_sec; // validity time, in sec
	struct __attribute__((packed)) {
		/* 0 = "     " off,
		 * 1 = " ^_^ "
		 * 2 = " -^- "
		 * 3 = " ooo "
		 * 4 = "(   )"
		 * 5 = "(^_^)" happy
		 * 6 = "(-^-)" sad
		 * 7 = "(ooo)" */
		u8 smiley			: 3;
		u8 percent_on		: 1;
		u8 battery			: 1;
		/* 0 = "  ", shr 0x00
		 * 1 = "°Г", shr 0x20
		 * 2 = " -", shr 0x40
		 * 3 = "°F", shr 0x60
		 * 4 = " _", shr 0x80
		 * 5 = "°C", shr 0xa0
		 * 6 = " =", shr 0xc0
		 * 7 = "°E", shr 0xe0 */
		u8 temp_symbol		: 3;
	} flg;
#endif
} external_data_t, * pexternal_data_t;
extern external_data_t ext;
#endif

#if (DEV_SERVICES & SERVICE_PINCODE)
extern u32 pincode; // pincode (if = 0 - not used)
#endif

typedef struct _measured_data_t {
// start send part (MEASURED_MSG_SIZE)
#if USE_AVERAGE_BATTERY
	u16 	average_battery_mv; // mV
#else
	u16	battery_mv; // mV
#endif
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
	s16		temp; // x 0.01 C
	s16		humi; // x 0.01 %
#elif (DEV_SERVICES & SERVICE_IUS)
#if USE_SENSOR_INA3221
	s16		current[3]; // x 0.1 mA
	u16		voltage[3]; // x 1 mV
#else
	s16		current; // x 0.1 mA
	u16		voltage; // x 1 mV
#endif
#endif
	u16 	count;
	// end send part (MEASURED_MSG_SIZE)
#if (DEV_SERVICES & SERVICE_PRESSURE)
	u16	pressure;
#endif
#if USE_SENSOR_SCD41
	u16		co2; // ppm
#endif
#if (DEV_SERVICES & SERVICE_18B20)
	s16		xtemp[USE_SENSOR_MY18B20]; // x 0.01 C
#endif
#if USE_AVERAGE_BATTERY
	u16	battery_mv; // mV
#endif
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_18B20 | SERVICE_PLM))
	s16 temp_x01; 		// x 0.1 C
#endif
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
	s16	humi_x01; 		// x 0.1 %
	u8 	humi_x1; 		// x 1 %
#endif
	u8 	battery_level;	// 0..100% (average_battery_mv)
#if (DEV_SERVICES & SERVICE_IUS) && USE_SENSOR_INA226
	s32 energy;
#endif
} measured_data_t;  // save max 18 bytes
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS))
#if USE_SENSOR_INA3221
#define  MEASURED_MSG_SIZE  16
#else
#define  MEASURED_MSG_SIZE  8
#endif
#else // !(DEV_SERVICES & (SERVICE_THS | SERVICE_IUS))
#define  MEASURED_MSG_SIZE  4
#endif
extern measured_data_t measured_data;

typedef union {
	u8 all_flgs;
	struct {
		u8 send_measure: 1;
		u8 update_lcd	: 1;
		u8 update_adv	: 1;
	} b;
} flg_measured_t;

// bits: wrk.ble_connected
enum {
	CONNECTED_FLG_ENABLE = 0,
	CONNECTED_FLG_PAR_UPDATE = 1,
	CONNECTED_FLG_BONDING = 2,
	CONNECTED_FLG_RESET_OF_DISCONNECT = 7
} CONNECTED_FLG_BITS_e;

// wrk.ota_is_working
enum {
	OTA_NONE = 0,
	OTA_WORK,
	OTA_WAIT,
	OTA_EXTENDED
} OTA_STAGES_e;

typedef struct _work_flg_t {
	u32 utc_time_sec;	// clock in sec (= 0 1970-01-01 00:00:00)
	u32 utc_time_sec_tick; // clock counter in 1/16 us
	u32 utc_time_tick_step; //  adjust time clock (in 1/16 us for 1 sec)
	u32 adv_interval; // adv interval in 0.625 ms // = cfg.advertising_interval * 100
	u32 connection_timeout; // connection timeout in 10 ms, Tdefault = connection_latency_ms * 4 = 2000 * 4 = 8000 ms
	u32 measurement_step_time; // = adv_interval * measure_interval
	u32 tim_measure; // measurement timer
	u8 ble_connected; // BIT(CONNECTED_FLG_BITS_e): bit 0 - connected, bit 1 - conn_param_update, bit 2 - paring success, bit 7 - reset of disconnect
	u8 ota_is_working; // OTA_STAGES_e:  =1 ota enabled, =2 - ota wait, =3 0xff flag ext.ota
	volatile u8 start_measure; // start measurements
	u8 tx_measures; // measurement transfer counter, flag
	union {
		u8 all_flgs;
		struct {
			u8 send_measure	: 1;
			u8 update_lcd		: 1;
			u8 update_adv		: 1;
			u8 th_sensor_read	: 1;
		} b; // bits-flags measurements completed
	} msc; // flags measurements completed
	u8 adv_interval_delay; // adv interval + rand delay in 0.625 ms // = 10 or 0
} work_flg_t;
extern work_flg_t wrk;

typedef struct _comfort_t {
	s16  t[2];
	u16 h[2];
}scomfort_t, * pcomfort_t;
extern scomfort_t cmf;

#if (DEV_SERVICES & SERVICE_BINDKEY)
extern u8 bindkey[16];
void bindkey_init(void);
#endif

#if (DEV_SERVICES & SERVICE_PINCODE)
extern u32 pincode;
#endif

#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)
// extension keys
typedef struct {
	s32 rest_adv_int_tad;	// timer event restore adv.intervals (in adv count)
	u32 key_pressed_tik1;   // timer1 key_pressed (in sys tik)
	u32 key_pressed_tik2;	// timer2 key_pressed (in sys tik)
#if (DEV_SERVICES & SERVICE_KEY)
	u8  key2pressed;
#endif
} ext_key_t;
extern ext_key_t ext_key; // extension keys

void set_default_cfg(void);

#if (DEV_SERVICES & SERVICE_KEY)
static inline u8 get_key2_pressed(void) {
	return BM_IS_SET(reg_gpio_in(GPIO_KEY2), GPIO_KEY2 & 0xff);
}
#endif // (DEV_SERVICES & SERVICE_KEY)
#endif // (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)

void ev_adv_timeout(u8 e, u8 *p, int n); // DURATION_TIMEOUT Event Callback
void test_config(void); // Test config values
void set_hw_version(void);

u8 * str_bin2hex(u8 *d, u8 *s, int len);

//---- blt_common.c
void blc_newMacAddress(int flash_addr, u8 *mac_pub, u8 *mac_rand);
void SwapMacAddress(u8 *mac_out, u8 *mac_in);
void flash_erase_mac_sector(u32 faddr);

#endif /* _APP_H_ */
