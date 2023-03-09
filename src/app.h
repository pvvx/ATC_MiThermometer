/*
 * app.h
 *
 *  Created on: 19.12.2020
 *      Author: pvvx
 */

#ifndef MAIN_H_
#define MAIN_H_

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
	HW_VER_UNKNOWN = 15
} HW_VERSION_ID;

// EEPROM IDs
#define EEP_ID_CFG (0x0CFC) // EEP ID config data
#define EEP_ID_TRG (0x0DFE) // EEP ID trigger data
#define EEP_ID_RPC (0x0DF5) // EEP ID reed switch pulse counter
#define EEP_ID_PCD (0xC0DE) // EEP ID pincode
#define EEP_ID_CMF (0x0FCC) // EEP ID comfort data
#define EEP_ID_DVN (0x0DB5) // EEP ID device name
#define EEP_ID_TIM (0x0ADA) // EEP ID time adjust
#define EEP_ID_KEY (0xBEAC) // EEP ID bkey
#define EEP_ID_HWV (0x1234) // EEP ID Mi HW version

// Adv. types
enum {
	ADV_TYPE_ATC = 0,
	ADV_TYPE_PVVX, // (default)
	ADV_TYPE_MI,
	ADV_TYPE_HA_BLE
} ADV_TYPE_ENUM;
#define ADV_TYPE_DEFAULT	ADV_TYPE_PVVX

#if	(DEVICE_TYPE != DEVICE_MJWSD05MMC)
#define MASK_FLG2_REBOOT	0xe0
#define MASK_FLG2_EXT_ADV	0xc0
#else
#define MASK_FLG2_REBOOT	0x60
#endif
typedef struct __attribute__((packed)) _cfg_t {
	struct __attribute__((packed)) {
		uint8_t advertising_type	: 2; // 0 - atc1441, 1 - Custom (pvvx), 2 - Mi, 3 - HA_BLE
		uint8_t comfort_smiley		: 1;
#if	(DEVICE_TYPE == DEVICE_MJWSD05MMC)
		uint8_t x100				: 1;
#else
		uint8_t blinking_time_smile	: 1; //(USE_CLOCK = 0 - smile, =1 time)
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
//		uint8_t reserved1	: 1;
#else
		uint8_t smiley 		: 3;	// 0..7
#endif
		uint8_t adv_crypto	: 1; 	// advertising uses crypto beacon
#if	(DEVICE_TYPE == DEVICE_MJWSD05MMC)
		uint8_t reserved1	: 1;
#else
		uint8_t adv_flags  	: 1; 	// advertising add flags
#endif
		uint8_t bt5phy  	: 1; 	// support BT5.0 All PHY
		uint8_t longrange  	: 1;  	// advertising in LongRange mode (сбрасывается после отключения питания)
		uint8_t reserved	: 1;
	} flg2;
	int8_t temp_offset; // Set temp offset, -12,5 - +12,5 °C (-125..125)
	int8_t humi_offset; // Set humi offset, -12,5 - +12,5 % (-125..125)
	uint8_t advertising_interval; // multiply by 62.5 for value in ms (1..160,  62.5 ms .. 10 sec)
	uint8_t measure_interval; // measure_interval = advertising_interval * x (2..25)
	uint8_t rf_tx_power; // RF_POWER_N25p18dBm .. RF_POWER_P3p01dBm (130..191)
	uint8_t connect_latency; // +1 x0.02 sec ( = connection interval), Tmin = 1*20 = 20 ms, Tmax = 256 * 20 = 5120 ms
#if	(DEVICE_TYPE == DEVICE_MJWSD05MMC)
	uint8_t rezerved;
#else
	uint8_t min_step_time_update_lcd; // x0.05 sec, 0.5..12.75 sec (10..255)
#endif
	struct __attribute__((packed)) {
		uint8_t hwver		: 4; // 0 - LYWSD03MMC B1.4, 1 - MHO-C401, 2 - CGG1-M, 3 - LYWSD03MMC B1.9, 4 - LYWSD03MMC B1.6, 5 - LYWSD03MMC B1.7, 6 - CGDK2, 7 - CGG1-M-2022, 8 - MHO-C401-2022
		uint8_t reserved	: 3; // reserved
		uint8_t shtc3		: 1; // =1 - sensor SHTC3, = 0 - sensor SHT4x
	} hw_cfg; // read only
	uint8_t averaging_measurements; // * measure_interval, 0 - off, 1..255 * measure_interval
}cfg_t;
extern cfg_t cfg;
extern const cfg_t def_cfg;
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
extern uint32_t chow_tick_clk; // count chow validity time, in clock
extern uint32_t chow_tick_sec; // count chow validity time, in sec

extern uint32_t utc_time_sec;	// clock in sec (= 0 1970-01-01 00:00:00)
#if	USE_TIME_ADJUST
extern uint32_t utc_time_tick_step; // adjust time clock (in 1/16 us for 1 sec)
#endif
extern int32_t rest_adv_int_tad;	// timer event restore adv.intervals

#if BLE_SECURITY_ENABLE
extern uint32_t pincode; // pincode (if = 0 - not used)
#endif

typedef struct _measured_data_t {
	uint16_t	battery_mv; // mV
	int16_t		temp; // x 0.01 C
	int16_t		humi; // x 0.01 %
	uint16_t 	count;

	int16_t 	temp_x01; 		// x 0.1 C
	int16_t		humi_x01; 		// x 0.1 %
	uint8_t 	humi_x1; 		// x 1 %
	uint8_t 	battery_level;	// 0..100%
} measured_data_t;
#define  MEASURED_MSG_SIZE  8

extern measured_data_t measured_data;

//extern uint8_t battery_level; // 0..100%
//extern int16_t last_temp; // x0.1 C
//extern uint16_t last_humi; // x1 %

extern volatile uint8_t tx_measures; // connect notify send measure flag
extern volatile uint8_t start_measure; // start measure all
extern volatile uint8_t wrk_measure;
extern volatile uint8_t end_measure;
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC)
extern volatile uint8_t lcd_update;
#endif

#if (DEVICE_TYPE == DEVICE_MJWSD05MMC)
#define MI_HW_VER_FADDR 0x7D000 // Mi HW version
#else
#define MI_HW_VER_FADDR 0x55000 // Mi HW version
#endif


extern uint32_t tim_measure; // timer measurements >= 10 sec

typedef union _lcd_flg_t {
	struct  {
		uint8_t ext_data: 	1; // LCD show external data
		uint8_t notify_on: 	1; // Send LCD dump if Notify on
		uint8_t res:  		5;
		uint8_t new_update: 1; // flag update LCD for send notify
	}b;
	uint8_t uc;
} lcd_flg_t;
extern lcd_flg_t lcd_flg;

typedef struct _comfort_t {
	int16_t  t[2];
	uint16_t h[2];
}scomfort_t, * pcomfort_t;

#if USE_SECURITY_BEACON
extern uint8_t bindkey[16];
void bindkey_init(void);
#endif

extern scomfort_t cmf;
#if BLE_SECURITY_ENABLE
extern uint32_t pincode;
#endif
extern uint32_t adv_interval;
extern uint32_t connection_timeout;
extern uint32_t measurement_step_time;
extern uint32_t tim_last_chow; // timer show lcd >= 1.5 sec

#if DEVICE_TYPE != DEVICE_MJWSD05MMC
extern uint32_t min_step_time_update_lcd; // = cfg.min_step_time_update_lcd * (50 * CLOCK_16M_SYS_TIMER_CLK_1MS)
#endif

void ev_adv_timeout(u8 e, u8 *p, int n);
void test_config(void);
void set_hw_version(void);
void reset_cache(void);

void blc_newMacAddress(int flash_addr, u8 *mac_pub, u8 *mac_rand);
void SwapMacAddress(u8 *mac_out, u8 *mac_in);
void flash_erase_mac_sector(u32 faddr);

#endif /* MAIN_H_ */
