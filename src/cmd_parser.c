#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"

#include "lcd.h"
#include "app.h"
#include "flash_eep.h"
#include "ble.h"

RAM uint8_t mi_key_stage;
RAM uint8_t mi_key_chk_cnt;
RAM uint8_t keybuf[0x20];

uint8_t find_mi_keys(uint16_t chk_id, uint8_t cnt, uint8_t *pbuf) {
	uint8_t * p = (uint8_t *)(0x78000);
	uint16_t id;
	uint8_t len;
	do {
		id = p[0] | (p[1] << 8);
		len = p[2];
		p += 3;
		if(len <= 0x20 && len >= 0x04
			&& id == chk_id
			&& --cnt == 0) {
				pbuf[1] = len;
				memcpy(&pbuf[2], p, len);
				return len;
		}
		p += len;
	} while(id != 0xffff || len != 0xff || p < (uint8_t *)(0x79000));
	return 0;
}

uint8_t send_mi_key(void) {
	if (blc_ll_getTxFifoNumber() < 9) {
		while(keybuf[1] > 16) {
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, keybuf, 16 +2);
			keybuf[1] -= 16;
			if(keybuf[1])
				memcpy(&keybuf[2], &keybuf[16+2], keybuf[1]);
		};
		if(keybuf[1])
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, keybuf, keybuf[1] +2);
		return 1;
	};
	return 0;
}
void send_mi_no_key(void) {
	keybuf[1] = 0;
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, keybuf, 2);
}
uint8_t get_mi_keys(uint8_t chk_stage) {
	if(keybuf[1]) {
		if(!send_mi_key())
			return chk_stage;
	};
	switch(chk_stage) {
	case 1:
		if(find_mi_keys(0x10, 1, keybuf)) {
			send_mi_key();
			chk_stage = 2;
		} else {
			send_mi_no_key();
			chk_stage = 0;
		}
		break;
	case 2:
		if(find_mi_keys(0, ++mi_key_chk_cnt, keybuf)) {
			send_mi_key();
		} else {
			send_mi_no_key();
			chk_stage = 0;
		}
		break;
	default:
		keybuf[0] = 0x11;
		mi_key_chk_cnt = 0;
		if(find_mi_keys(1, 1, keybuf)) {
			send_mi_key();
			chk_stage = 1;
		} else {
			send_mi_no_key();
			chk_stage = 0;
		}
		break;
	};
	return chk_stage;
}

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
			else {
				end_measure = 1;
				tx_measures = 0xff;
			}
		} if (cmd == 0x22) { // test
			blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, 128); // 234
		} if (cmd == 0x11) { // test
			mi_key_stage = get_mi_keys(0);
		}
#else // Atc1441 variant
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
