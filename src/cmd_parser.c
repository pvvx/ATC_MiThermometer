#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"

#include "lcd.h"
#include "app.h"
#include "flash_eep.h"
#include "ble.h"

void cmd_parser(void * p) {
	rf_packet_att_data_t *req = (rf_packet_att_data_t*) p;
	uint8_t inData = req->dat[0];
	if (inData == 0xFF) {
		cfg.flg.temp_C_or_F = true; // Temp in F
	} else if (inData == 0xCC) {
		cfg.flg.temp_C_or_F = false; // Temp in C
	} else if (inData == 0xB1) {
		cfg.flg.show_batt_enabled = true; // Enable battery on LCD
	} else if (inData == 0xB0) {
		cfg.flg.show_batt_enabled = false; // Disable battery on LCD
	} else if (inData == 0xA0) {
		cfg.flg.blinking_smiley = false;
		cfg.flg.comfort_smiley = false;
		show_smiley(0); // Smiley off
	} else if (inData == 0xA1) {
		cfg.flg.blinking_smiley = false;
		cfg.flg.comfort_smiley = false;
		show_smiley(1); // Smiley happy
	} else if (inData == 0xA2) {
		cfg.flg.blinking_smiley = false;
		cfg.flg.comfort_smiley = false;
		show_smiley(2); // Smiley sad
	} else if (inData == 0xA3) {
		cfg.flg.blinking_smiley = false;
		cfg.flg.comfort_smiley = true; // Comfort Indicator
	} else if (inData == 0xAB) {
		cfg.flg.blinking_smiley = true; // Smiley blinking
	} else if (inData == 0xAE) {
		cfg.flg.advertising_type = false; // Advertising type Custom
	} else if (inData == 0xAF) {
		cfg.flg.advertising_type = true; // Advertising type Mi Like
	} else if (inData == 0xFE) {
		cfg.advertising_interval = req->dat[1]; // Set advertising interval with second byte, value * 62.5 ms / 0=main_delay
		if (cfg.advertising_interval == 0)
			cfg.advertising_interval = 1; // 62.5 ms
		else if (cfg.advertising_interval > 160)
			cfg.advertising_interval = 160; // 10 sec
		adv_interval = cfg.advertising_interval * 100; // t = adv_interval * 0.625 ms
		measurement_step_time = adv_interval * cfg.measure_interval * 625 * sys_tick_per_us;
		ev_adv_timeout(0,0,0);
	} else if (inData == 0xF8) {
		cfg.rf_tx_power = req->dat[1];
		user_set_rf_power(0, 0, 0);
	} else if (inData == 0xF9) {
		cfg.measure_interval = req->dat[1]; // Set advertising interval with second byte, value * 62.5 ms / 0=main_delay
		if (cfg.measure_interval == 0)
			cfg.measure_interval = 1; // x1
		else if (cfg.measure_interval > 10)
			cfg.measure_interval = 10; // x10
		measurement_step_time = adv_interval * cfg.measure_interval * 625 * sys_tick_per_us;
	} else if (inData == 0xFA) {
		cfg.temp_offset = req->dat[1]; // Set temp offset, -12,5 - +12,5 °C
	} else if (inData == 0xFB) {
		cfg.humi_offset = req->dat[1]; // Set humi offset, -50 - +50 %
		if (cfg.humi_offset < -50)
			cfg.humi_offset = -50;
		if (cfg.humi_offset > 50)
			cfg.humi_offset = 50;
	} else if (inData == 0xFC) {
		cfg.temp_alarm_point = req->dat[1]; // Set temp alarm point value divided by 10 for temp in °C
		if (cfg.temp_alarm_point == 0)
			cfg.temp_alarm_point = 1;
	} else if (inData == 0xFD) {
		cfg.humi_alarm_point = req->dat[1]; // Set humi alarm point
		if (cfg.humi_alarm_point == 0)
			cfg.humi_alarm_point = 1;
		if (cfg.humi_alarm_point > 50)
			cfg.humi_alarm_point = 50;
	}
	flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
}
