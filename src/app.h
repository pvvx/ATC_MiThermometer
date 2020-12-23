/*
 * app.h
 *
 *  Created on: 19.12.2020
 *      Author: pvvx
 */

#ifndef MAIN_H_
#define MAIN_H_

#define EEP_ID_CFG 0xCFCD // EEP ID config data

typedef struct _cfg_t {
	struct {
		uint8_t smiley				: 2;
		uint8_t comfort_smiley		: 1;
		uint8_t blinking_smiley		: 1;
		uint8_t temp_C_or_F			: 1;
		uint8_t show_batt_enabled	: 1;
		uint8_t advertising_type	: 1; // Custom or Mi Advertising (true)
		uint8_t tx_measures			: 1;
	} flg;
	int8_t temp_offset; // Set temp offset, -12,5 - +12,5 °C (-125..125)
	int8_t humi_offset; // Set humi offset, -50 - +50 %
	uint8_t advertising_interval; // multiply by 62.5 for value in ms (1..160,  62.5 ms .. 10 sec)
	uint8_t measure_interval; // measure_interval = advertising_interval * x (1..10)
	uint8_t rf_tx_power;
	uint8_t connect_latency; // x 0.02 sec ( = connection interval)
}cfg_t;
extern cfg_t cfg;

typedef struct _measured_data_t {
	uint16_t	battery_mv; // mV
	uint16_t	temp; // x 0.01 C
	int16_t		humi; // x 0.01 %
	uint16_t 	count;
} measured_data_t;
extern measured_data_t measured_data;
extern volatile uint8_t tx_measures;

extern uint32_t adv_interval;
extern uint32_t measurement_step_time;
void ev_adv_timeout(u8 e, u8 *p, int n);
void test_config(void);

#endif /* MAIN_H_ */
