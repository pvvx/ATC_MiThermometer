/*
 * app.h
 *
 *  Created on: 19.12.2020
 *      Author: pvvx
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "drivers/8258/gpio_8258.h"

enum {
	HW_VER_LYWSD03MMC_B14 = 0,
	HW_VER_MHO_C401,		//1
	HW_VER_CGG1,			//2
	HW_VER_LYWSD03MMC_B19,	//3
	HW_VER_LYWSD03MMC_B16,	//4
	HW_VER_LYWSD03MMC_B17,	//5
	HW_VER_CGDK2,			//6
	HW_VER_CGG1_2022,		//7
	HW_VER_MHO_C401_2022,	//8
	HW_VER_MJWSD05MMC,		//9
	HW_VER_LYWSD03MMC_B15,	//10
	HW_VER_MHO_C122			//11
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

#define MASK_FLG2_REBOOT	0x60
#define MASK_FLG2_SCR_OFF	0x80

typedef struct __attribute__((packed)) _cfg_t {
	struct __attribute__((packed)) {
		uint8_t advertising_type	: 2; // 0 - atc1441, 1 - Custom (pvvx), 2 - Mi, 3 - BTHome
		uint8_t comfort_smiley		: 1;
#if	(DEVICE_TYPE == DEVICE_MJWSD05MMC)
		uint8_t x100				: 1;
#else
		uint8_t show_time_smile	: 1; // if USE_CLOCK: = 0 - smile, =1 time, else: blinking on/off
#endif
		uint8_t temp_F_or_C			: 1;
#if	(DEVICE_TYPE == DEVICE_MJWSD05MMC)
		uint8_t time_am_pm			: 1;
#else
		uint8_t show_batt_enabled	: 1;
#endif
		uint8_t tx_measures			: 1; // Send all measurements in connected mode
		uint8_t lp_measures			: 1; // Sensor measurements in "Low Power" mode
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
#if	(DEVICE_TYPE == DEVICE_MJWSD05MMC)
		uint8_t screen_type	: 3;
#else
		uint8_t smiley 		: 3;	// 0..7
#endif
		uint8_t adv_crypto	: 1; 	// advertising uses crypto beacon
		uint8_t adv_flags  	: 1; 	// advertising add flags
		uint8_t bt5phy  	: 1; 	// support BT5.0 All PHY
		uint8_t longrange  	: 1;  	// advertising in LongRange mode (сбрасывается после отключения питания)
		uint8_t screen_off	: 1;	// screen off, v4.3+
	} flg2;
#if	VERSION < 0x47
	int8_t temp_offset; // Set temp offset, -12,5 - +12,5 °C (-125..125)
	int8_t humi_offset; // Set humi offset, -12,5 - +12,5 % (-125..125)
#else
	int8_t x1;
	int8_t x2;
#endif
	uint8_t advertising_interval; // multiply by 62.5 for value in ms (1..160,  62.5 ms .. 10 sec)
	uint8_t measure_interval; // measure_interval = advertising_interval * x (2..25)
	uint8_t rf_tx_power; // RF_POWER_N25p18dBm .. RF_POWER_P3p01dBm (130..191)
	uint8_t connect_latency; // +1 x0.02 sec ( = connection interval), Tmin = 1*20 = 20 ms, Tmax = 256 * 20 = 5120 ms
	uint8_t min_step_time_update_lcd; // x0.05 sec, 0.5..12.75 sec (10..255)
	uint8_t hw_ver; // read only
	uint8_t averaging_measurements; // * measure_interval, 0 - off, 1..255 * measure_interval
}cfg_t;
extern cfg_t cfg;
extern const cfg_t def_cfg;

#if (DEV_SERVICES & SERVICE_SCREEN)
/* Warning: MHO-C401 Symbols: "%", "°Г", "(  )", "." have one control bit! */
typedef struct __attribute__((packed)) _external_data_t {
#if(DEVICE_TYPE == DEVICE_MJWSD05MMC)
	int32_t		number; // -999.50..19995.50, x0.01
	uint16_t 	vtime_sec; // validity time, in sec
	struct __attribute__((packed)) {
		/* 0 = "     " off,
		 * 1 = " ^-^ "
		 * 2 = " -^- "
		 * 3 = " ooo "
		 * 4 = "(   )"
		 * 5 = "(^-^)" happy
		 * 6 = "(-^-)" sad
		 * 7 = "(ooo)" */
		uint8_t smiley			: 3;
		uint8_t battery			: 1;
		/* 0x00 = "  "
		 * 0x01 = "°г"
		 * 0x02 = " -"
		 * 0x03 = "°c"
		 * 0x04 = " |"
		 * 0x05 = "°Г"
		 * 0x06 = " г"
		 * 0x07 = "°F"
		 * 0x08 = "%" */
		uint8_t temp_symbol		: 4;
	} flg;
#else
	int16_t		big_number; // -995..19995, x0.1
	int16_t		small_number; // -9..99, x1
	uint16_t 	vtime_sec; // validity time, in sec
	struct __attribute__((packed)) {
		/* 0 = "     " off,
		 * 1 = " ^_^ "
		 * 2 = " -^- "
		 * 3 = " ooo "
		 * 4 = "(   )"
		 * 5 = "(^_^)" happy
		 * 6 = "(-^-)" sad
		 * 7 = "(ooo)" */
		uint8_t smiley			: 3;
		uint8_t percent_on		: 1;
		uint8_t battery			: 1;
		/* 0 = "  ", shr 0x00
		 * 1 = "°Г", shr 0x20
		 * 2 = " -", shr 0x40
		 * 3 = "°F", shr 0x60
		 * 4 = " _", shr 0x80
		 * 5 = "°C", shr 0xa0
		 * 6 = " =", shr 0xc0
		 * 7 = "°E", shr 0xe0 */
		uint8_t temp_symbol		: 3;
	} flg;
#endif
} external_data_t, * pexternal_data_t;
extern external_data_t ext;
#endif

extern uint32_t utc_time_sec;	// clock in sec (= 0 1970-01-01 00:00:00)
#if (DEV_SERVICES & SERVICE_TIME_ADJUST)
extern uint32_t utc_time_tick_step; // adjust time clock (in 1/16 us for 1 sec)
#endif

#if (DEV_SERVICES & SERVICE_PINCODE)
extern uint32_t pincode; // pincode (if = 0 - not used)
#endif

typedef struct _measured_data_t {
// start send part (MEASURED_MSG_SIZE)
#if USE_AVERAGE_BATTERY
	uint16_t 	average_battery_mv; // mV
#else
	uint16_t	battery_mv; // mV
#endif
	int16_t		temp; // x 0.01 C
	int16_t		humi; // x 0.01 %
	uint16_t 	count;
// end send part (MEASURED_MSG_SIZE)
#if (DEV_SERVICES & SERVICE_PRESSURE)
	uint16_t	pressure;
#endif
#if USE_AVERAGE_BATTERY
	uint16_t	battery_mv; // mV
#endif
	int16_t 	temp_x01; 		// x 0.1 C
	int16_t		humi_x01; 		// x 0.1 %
	uint8_t 	humi_x1; 		// x 1 %
	uint8_t 	battery_level;	// 0..100% (average_battery_mv)
} measured_data_t;
#define  MEASURED_MSG_SIZE  8
extern measured_data_t measured_data;


typedef union {
	uint8_t all_flgs;
	struct {
		uint8_t send_measure: 1;
		uint8_t update_lcd	: 1;
		uint8_t update_adv	: 1;
	} b;
} flg_measured_t;

typedef struct _work_flg_t {
	uint32_t tim_measure; // measurement timer
	uint8_t ble_connected; // BIT(CONNECTED_FLG_BITS): bit 0 - connected, bit 1 - conn_param_update, bit 2 - paring success, bit 7 - reset of disconnect
	uint8_t ota_is_working; // OTA_STAGES:  =1 ota enabled, =2 - ota wait, = 0xff flag ext.ota
	volatile uint8_t start_measure; // start measurements
	volatile uint8_t tx_measures; // measurement transfer counter, flag
	union {
		uint8_t all_flgs;
		struct {
			uint8_t send_measure: 1;
			uint8_t update_lcd	: 1;
			uint8_t update_adv	: 1;
		} b; // bits-flags measurements completed
	} msc; // flags measurements completed
} work_flg_t;
extern work_flg_t wrk;

typedef struct _comfort_t {
	int16_t  t[2];
	uint16_t h[2];
}scomfort_t, * pcomfort_t;
extern scomfort_t cmf;

#if (DEV_SERVICES & SERVICE_BINDKEY)
extern uint8_t bindkey[16];
void bindkey_init(void);
#endif

#if (DEV_SERVICES & SERVICE_PINCODE)
extern uint32_t pincode;
#endif

extern uint32_t adv_interval; // adv interval in 0.625 ms // = cfg.advertising_interval * 100
extern uint32_t connection_timeout; // connection timeout in 10 ms, Tdefault = connection_latency_ms * 4 = 2000 * 4 = 8000 ms
extern uint32_t measurement_step_time; // = adv_interval * measure_interval

#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)
// extension keys
typedef struct {
	int32_t rest_adv_int_tad;	// timer event restore adv.intervals (in adv count)
	uint32_t key_pressed_tik1;   // timer1 key_pressed (in sys tik)
	uint32_t key_pressed_tik2;	// timer2 key_pressed (in sys tik)
#if (DEV_SERVICES & SERVICE_KEY)
	uint8_t  key2pressed;
#endif
} ext_key_t;
extern ext_key_t ext_key; // extension keys

void set_default_cfg(void);

#if (DEV_SERVICES & SERVICE_KEY)
static inline uint8_t get_key2_pressed(void) {
	return BM_IS_SET(reg_gpio_in(GPIO_KEY2), GPIO_KEY2 & 0xff);
}
#endif // (DEV_SERVICES & SERVICE_KEY)
#endif // (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)

void ev_adv_timeout(u8 e, u8 *p, int n); // DURATION_TIMEOUT Event Callback
void test_config(void); // Test config values
void set_hw_version(void);

uint8_t * str_bin2hex(uint8_t *d, uint8_t *s, int len);

//---- blt_common.c
void blc_newMacAddress(int flash_addr, u8 *mac_pub, u8 *mac_rand);
void SwapMacAddress(u8 *mac_out, u8 *mac_in);
void flash_erase_mac_sector(u32 faddr);

#endif /* MAIN_H_ */
