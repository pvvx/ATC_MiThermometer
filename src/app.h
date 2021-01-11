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

typedef struct __attribute__((packed)) _cfg_t {
	struct __attribute__((packed)) {
		uint8_t advertising_type	: 2; // 0 - atc1441, 1 - Custom, 2 - Mi, 3 - all
		uint8_t comfort_smiley		: 1;
		uint8_t blinking_smiley		: 1;
		uint8_t temp_F_or_C			: 1;
		uint8_t show_batt_enabled	: 1;
		uint8_t tx_measures			: 1; // Send all measurements in connected mode
		uint8_t lp_measures			: 1; // Sensor measurements in "Low Power" mode
	} flg;
	/* 0 = "     " off,
	 * 1 = " ^_^ "
	 * 2 = " -^- "
	 * 3 = " ooo "
	 * 4 = "(   )"
	 * 5 = "(^_^)" happy
	 * 6 = "(-^-)" sad
	 * 7 = "(ooo)" */
	uint8_t smiley;		// 0..7
	int8_t temp_offset; // Set temp offset, -12,5 - +12,5 °C (-125..125)
	int8_t humi_offset; // Set humi offset, -12,5 - +12,5 % (-125..125)
	uint8_t advertising_interval; // multiply by 62.5 for value in ms (1..160,  62.5 ms .. 10 sec)
	uint8_t measure_interval; // measure_interval = advertising_interval * x (1..10)
	uint8_t rf_tx_power; // RF_POWER_N25p18dBm .. RF_POWER_P3p01dBm (130..191)
	uint8_t connect_latency; // +1 x0.02 sec ( = connection interval), Tmin = 1*20 = 20 ms, Tmax = 256 * 20 = 5120 ms
	uint8_t min_step_time_update_lcd; // x0.05 sec, 0.5..12.75 sec (10..255)
}cfg_t;
extern cfg_t cfg;
extern const cfg_t def_cfg;
//
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
extern uint32_t vtime_count_us;
extern uint32_t vtime_count_sec;

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

extern uint32_t adv_interval;
extern uint32_t connection_timeout;
extern uint32_t measurement_step_time;
void ev_adv_timeout(u8 e, u8 *p, int n);
void test_config(void);

#endif /* MAIN_H_ */
