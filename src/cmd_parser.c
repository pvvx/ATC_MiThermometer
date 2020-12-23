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
	uint32_t len = req->l2cap - 3;
	if(len) {
		uint8_t cmd = req->dat[0];
#if 1
		if (cmd == 0x55) {
			if(--len > sizeof(cfg)) len = sizeof(cfg);
			if(len)
				memcpy(&cfg, &req->dat[1], len);
			test_config();
			ev_adv_timeout(0, 0, 0);
			flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
			ble_send_cfg();
		} if (cmd == 0x33) {
			if(len >= 2)
				tx_measures = req->dat[1];
			else
				tx_measures = 1;
		} if (cmd == 0x11) {
			bls_l2cap_requestConnParamUpdate(8, 8, 99, 800);
		}
#else
		if (cmd == 0xFF) {
			cfg.flg.temp_C_or_F = true; // Temp in F
		} else if (cmd == 0xCC) {
			cfg.flg.temp_C_or_F = false; // Temp in C
		} else if (cmd == 0xB1) {
			cfg.flg.show_batt_enabled = true; // Enable battery on LCD
		} else if (cmd == 0xB0) {
			cfg.flg.show_batt_enabled = false; // Disable battery on LCD
		} else if (cmd == 0xA0) {
			cfg.flg.blinking_smiley = false;
			cfg.flg.comfort_smiley = false;
		} else if (cmd == 0xA1) {
			cfg.flg.blinking_smiley = false;
			cfg.flg.comfort_smiley = false;
		} else if (cmd == 0xA2) {
			cfg.flg.blinking_smiley = false;
			cfg.flg.comfort_smiley = false;
		} else if (cmd == 0xA3) {
			cfg.flg.blinking_smiley = false;
			cfg.flg.comfort_smiley = true; // Comfort Indicator
		} else if (cmd == 0xAB) {
			cfg.flg.blinking_smiley = true; // Smiley blinking
		} else if (cmd == 0xAE) {
			cfg.flg.advertising_type = false; // Advertising type Custom
		} else if (cmd == 0xAF) {
			cfg.flg.advertising_type = true; // Advertising type Mi Like
		} else if (cmd == 0xF8) {
			cfg.rf_tx_power = req->dat[1];
		} else if (cmd == 0xF9) {
			cfg.measure_interval = req->dat[1]; // Set advertising interval with second byte, value * 62.5 ms / 0=main_delay
		} else if (cmd == 0xFA) {
			cfg.temp_offset = req->dat[1]; // Set temp offset, -12,5 - +12,5 °C
		} else if (cmd == 0xFB) {
			cfg.humi_offset = req->dat[1]; // Set humi offset, -50 - +50 %
		} else if (cmd == 0xFC) {
		} else if (cmd == 0xFD) {
		} else if (cmd == 0xFE) {
			cfg.advertising_interval = req->dat[1]; // Set advertising interval with second byte, value * 62.5 ms / 0=main_delay
		}
		test_config();
//		user_set_rf_power(0, 0, 0);
		ev_adv_timeout(0, 0, 0);
		flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
#endif
	}
}
