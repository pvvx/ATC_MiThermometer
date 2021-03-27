/*
 * app.h
 *
 *  Created on: 19.12.2020
 *      Author: pvvx
 */

#ifndef MAIN_H_
#define MAIN_H_

#define EEP_ID_CFG (0x0CFC) // EEP ID config data
#define EEP_ID_TRG (0x0DFE) // EEP ID trigger data
#define EEP_ID_PCD (0xC0DE) // EEP ID pincode
#define EEP_ID_CMF (0x0FCC) // EEP ID comfort data
#define EEP_ID_DVN (0x0DB5) // EEP ID device name
#define EEP_ID_TIM (0x0ADA) // EEP ID time adjust
#define EEP_ID_KEY (0xBEAC) // EEP ID bkey

typedef struct __attribute__((packed)) _cfg_t {
	struct __attribute__((packed)) {
		uint8_t advertising_type	: 2; // 0 - atc1441, 1 - Custom, 2 - Mi, 3 - all
		uint8_t comfort_smiley		: 1;
		uint8_t blinking_time_smile	: 1; //(USE_CLOCK = 0 - smile, =1 time)
		uint8_t temp_F_or_C			: 1;
		uint8_t show_batt_enabled	: 1;
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
	 * 7 = "oOo" */
		uint8_t smiley 		: 3;	// 0..7
		uint8_t mi_beacon  	: 1; 	// advertising uses mi crypto beacon
		uint8_t reserved	: 4;
	} flg2;
	int8_t temp_offset; // Set temp offset, -12,5 - +12,5 °C (-125..125)
	int8_t humi_offset; // Set humi offset, -12,5 - +12,5 % (-125..125)
	uint8_t advertising_interval; // multiply by 62.5 for value in ms (1..160,  62.5 ms .. 10 sec)
	uint8_t measure_interval; // measure_interval = advertising_interval * x (1..10)
	uint8_t rf_tx_power; // RF_POWER_N25p18dBm .. RF_POWER_P3p01dBm (130..191)
	uint8_t connect_latency; // +1 x0.02 sec ( = connection interval), Tmin = 1*20 = 20 ms, Tmax = 256 * 20 = 5120 ms
	uint8_t min_step_time_update_lcd; // x0.05 sec, 0.5..12.75 sec (10..255)
	struct __attribute__((packed)) {
		uint8_t hwver		: 3; // 0 - LYWSD03MMC, 1 - MHO-C401, 2 - CGG1
		uint8_t clock		: 1; // clock
		uint8_t memo		: 1; // flash write measures
		uint8_t trg			: 1; // trigger out
		uint8_t mi_beacon	: 1; // advertising uses mi crypto beacon
		uint8_t reserved	: 1;
	} hw_cfg; // read only
	uint8_t averaging_measurements; // * measure_interval, 0 - off, 1..255 * measure_interval
}cfg_t;
extern cfg_t cfg;
extern const cfg_t def_cfg;
/* Warning: MHO-C401 Symbols: "%", "°Г", "(  )", "." have one control bit! */
typedef struct __attribute__((packed)) _external_data_t {
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
} external_data_t, * pexternal_data_t;
extern external_data_t ext;
extern uint32_t chow_tick_clk; // count chow validity time, in clock
extern uint32_t chow_tick_sec; // count chow validity time, in sec

#if	USE_CLOCK || USE_FLASH_MEMO
extern uint32_t utc_time_sec;	// clock in sec (= 0 1970-01-01 00:00:00)
#endif
#if	USE_CLOCK && USE_TIME_ADJUST
extern uint32_t utc_time_tick_step; // adjust time clock (in 1/16 us for 1 sec)
#endif

extern uint32_t pincode; // pincode (if = 0 - not used)

typedef struct _measured_data_t {
	uint16_t	battery_mv; // mV
	int16_t		temp; // x 0.01 C
	int16_t		humi; // x 0.01 %
	uint16_t 	count;
} measured_data_t;
extern measured_data_t measured_data;

extern uint8_t battery_level; // 0..100%
extern int16_t last_temp; // x0.1 C
extern uint16_t last_humi; // x1 %

extern volatile uint8_t tx_measures;
extern volatile uint8_t start_measure; // start measure all
extern volatile uint8_t wrk_measure;
extern volatile uint8_t end_measure;
extern uint32_t tim_measure;

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

extern scomfort_t cmf;

extern uint32_t adv_interval;
extern uint32_t connection_timeout;
extern uint32_t measurement_step_time;
void ev_adv_timeout(u8 e, u8 *p, int n);
void test_config(void);
void reset_cache(void);

#endif /* MAIN_H_ */
