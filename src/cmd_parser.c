#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "ble.h"

#include "lcd.h"
#include "app.h"
#include "flash_eep.h"
#if	USE_TRIGGER_OUT
#include "trigger.h"
#endif
#if USE_FLASH_MEMO
#include "logger.h"
#endif
#if USE_MIHOME_BEACON
#include "mi_beacon.h"
#endif
#include "cmd_parser.h"

#define TX_MAX_SIZE	 (ATT_MTU_SIZE-3) // = 20
#define FLASH_MIMAC_ADDR CFG_ADR_MAC // 0x76000
#define FLASH_MIKEYS_ADDR 0x78000
//#define FLASH_SECTOR_SIZE 0x1000 // in "flash_eep.h"

RAM uint8_t mi_key_stage;
RAM uint8_t mi_key_chk_cnt;

enum {
	MI_KEY_STAGE_END = 0,
	MI_KEY_STAGE_DNAME,
	MI_KEY_STAGE_TBIND,
	MI_KEY_STAGE_CFG,
	MI_KEY_STAGE_KDEL,
	MI_KEY_STAGE_RESTORE,
	MI_KEY_STAGE_WAIT_SEND,
	MI_KEY_STAGE_GET_ALL = 0xff,
	MI_KEY_STAGE_MAC = 0xfe
} MI_KEY_STAGES;

RAM blk_mi_keys_t keybuf;

#if DEVICE_TYPE == DEVICE_MHO_C401
uint8_t * find_mi_keys(uint16_t chk_id, uint8_t cnt) {
	uint8_t * p = (uint8_t *)(FLASH_MIKEYS_ADDR);
	uint8_t * pend = p + FLASH_SECTOR_SIZE;
	pblk_mi_keys_t pk = &keybuf;
	uint16_t id;
	uint8_t len;
	do {
		len = p[1];
		id = p[2] | (p[3] << 8);
		if(p[0] == 0xA5) {
			p += 8;
			if(len <= sizeof(keybuf.data)
				&& len > 0
				&& id == chk_id
				&& --cnt == 0) {
					pk->klen = len;
					memcpy(&pk->data, p, len);
					return p;
			}
		}
		p += len + 0x0f;
		p = (uint8_t *)((uint32_t)(p) & 0xfffffff0);
	} while(id != 0xffff || len != 0xff  || p < pend);
	return NULL;
}
#else // DEVICE_LYWSD03MMC
uint8_t * find_mi_keys(uint16_t chk_id, uint8_t cnt) {
	uint8_t * p = (uint8_t *)(FLASH_MIKEYS_ADDR);
	uint8_t * pend = p + FLASH_SECTOR_SIZE;
	pblk_mi_keys_t pk = &keybuf;
	uint16_t id;
	uint8_t len;
	do {
		id = p[0] | (p[1] << 8);
		len = p[2];
		p += 3;
		if(len <= sizeof(keybuf.data)
			&& len > 0
			&& id == chk_id
			&& --cnt == 0) {
				pk->klen = len;
				memcpy(&pk->data, p, len);
				return p;
		}
		p += len;
	} while(id != 0xffff || len != 0xff  || p < pend);
	return NULL;
}
#endif

uint8_t send_mi_key(void) {
	if (blc_ll_getTxFifoNumber() < 9) {
		while(keybuf.klen > TX_MAX_SIZE - 2) {
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (u8 *) &keybuf, TX_MAX_SIZE);
			keybuf.klen -= TX_MAX_SIZE - 2;
			if(keybuf.klen)
				memcpy(&keybuf.data, &keybuf.data[TX_MAX_SIZE - 2], keybuf.klen);
		};
		if(keybuf.klen)
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (u8 *) &keybuf, keybuf.klen +2);
		keybuf.klen = 0;
		return 1;
	};
	return 0;
}
void send_mi_no_key(void) {
	keybuf.klen = 0;
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (u8 *) &keybuf, 2);
}

uint8_t store_mi_keys(uint8_t klen, uint16_t key_id, uint8_t * pkey) {
	uint8_t key_chk_cnt = 0;
	uint8_t * pfoldkey = NULL;
	uint8_t * pfnewkey;
	uint8_t * p;
	if(pkey == NULL) {
		while((p = find_mi_keys(MI_KEYDELETE_ID, ++key_chk_cnt)) != NULL) {
		if(p && keybuf.klen == klen)
			pfoldkey = p;
		}
	};
	if(pfoldkey || pkey) {
		pfnewkey = find_mi_keys(key_id, 1);
		if(pfnewkey && keybuf.klen == klen) {
			uint8_t backupsector[FLASH_SECTOR_SIZE];
			memcpy(&backupsector,(uint8_t *)(FLASH_MIKEYS_ADDR), sizeof(backupsector));
			if(pkey) {
				if(memcmp(pfnewkey, pkey, keybuf.klen)) {
					memcpy(&backupsector[(uint32_t)pfnewkey - FLASH_MIKEYS_ADDR], pkey, keybuf.klen);
					flash_erase_sector(FLASH_MIKEYS_ADDR);
					flash_write_all_size(FLASH_MIKEYS_ADDR, sizeof(backupsector), backupsector);
					return 1;
				}
			} else {
				if(memcmp(pfnewkey, pfoldkey, keybuf.klen)) {
					memcpy(&backupsector[(uint32_t)pfoldkey - FLASH_MIKEYS_ADDR], pfnewkey, keybuf.klen);
					memcpy(&keybuf.data, pfoldkey, keybuf.klen);
					memcpy(&backupsector[(uint32_t)pfnewkey - FLASH_MIKEYS_ADDR], pfoldkey, keybuf.klen);
					flash_erase_sector(FLASH_MIKEYS_ADDR);
					flash_write_all_size(FLASH_MIKEYS_ADDR, sizeof(backupsector), backupsector);
					return 1;
				}
			}
		}
	}
	return 0;
}

uint8_t get_mi_keys(uint8_t chk_stage) {
	if(keybuf.klen) {
		if(!send_mi_key())
			return chk_stage;
	};
	switch(chk_stage) {
	case MI_KEY_STAGE_DNAME:
		chk_stage = MI_KEY_STAGE_TBIND;
		keybuf.id = CMD_MI_ID_DNAME;
		if(find_mi_keys(MI_KEYDNAME_ID, 1)) {
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case MI_KEY_STAGE_TBIND:
		chk_stage = MI_KEY_STAGE_CFG;
		keybuf.id = CMD_MI_ID_TBIND;
		if(find_mi_keys(MI_KEYTBIND_ID, 1)) {
			mi_key_chk_cnt = 0;
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case MI_KEY_STAGE_CFG:
		chk_stage = MI_KEY_STAGE_KDEL;
		keybuf.id = CMD_MI_ID_CFG;
		if(find_mi_keys(MI_KEYSEQNUM_ID, 1)) {
			mi_key_chk_cnt = 0;
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case MI_KEY_STAGE_KDEL:
		keybuf.id = CMD_MI_ID_KDEL;
		if(find_mi_keys(MI_KEYDELETE_ID, ++mi_key_chk_cnt)) {
			send_mi_key();
		} else {
			chk_stage = MI_KEY_STAGE_END;
			send_mi_no_key();
		}
		break;
	case MI_KEY_STAGE_RESTORE: // restore prev mi token & bindkeys
		keybuf.id = CMD_MI_ID_TBIND;
		if(store_mi_keys(MI_KEYTBIND_SIZE, MI_KEYTBIND_ID, NULL)) {
			chk_stage = MI_KEY_STAGE_WAIT_SEND;
			send_mi_key();
		} else {
			chk_stage = MI_KEY_STAGE_END;
			send_mi_no_key();
		}
		break;
	case MI_KEY_STAGE_WAIT_SEND:
		chk_stage = MI_KEY_STAGE_END;
		break;
	default: // Start get all mi keys // MI_KEY_STAGE_MAC
		memcpy(&keybuf.data,(uint8_t *)(FLASH_MIMAC_ADDR), 8); // MAC[6] + mac_random[2]
		keybuf.klen = 8;
		keybuf.id = CMD_MI_ID_MAC;
		chk_stage = MI_KEY_STAGE_DNAME;
		send_mi_key();
		break;
	};
	return chk_stage;
}

static void erase_mikeys(void) {
	int32_t tmp;
	flash_read_page(FLASH_MIKEYS_ADDR, 4, (unsigned char *)&tmp);
	if(tmp != -1)
		flash_erase_sector(FLASH_MIKEYS_ADDR);
}

__attribute__((optimize("-Os"))) void cmd_parser(void * p) {
	rf_packet_att_data_t *req = (rf_packet_att_data_t*) p;
	uint32_t len = req->l2cap - 3;
	if(len) {
		uint8_t cmd = req->dat[0];
		send_buf[0] = cmd;
		send_buf[1] = 0; // no err?
		uint32_t olen = 0;
		if (cmd == CMD_ID_MEASURE) { // Start/stop notify measures in connection mode
			if(len >= 2)
				tx_measures = req->dat[1];
			else {
				end_measure = 1;
				tx_measures = 1;
			}
			olen = 2;
		} else if (cmd == CMD_ID_EXTDATA) { // Show ext. small and big number
			if(--len > sizeof(ext)) len = sizeof(ext);
			if(len) {
				memcpy(&ext, &req->dat[1], len);
				chow_tick_sec = ext.vtime_sec;
				chow_tick_clk = clock_time();
			}
			ble_send_ext();
		} else if (cmd == CMD_ID_CFG || cmd == CMD_ID_CFG_NS) { // Get/set config
			if(--len > sizeof(cfg)) len = sizeof(cfg);
			if(len) {
				memcpy(&cfg, &req->dat[1], len);
#if USE_MIHOME_BEACON
				if(!pbindkey)
					cfg.flg2.mi_beacon = 0;
#endif
			}
			test_config();
			ev_adv_timeout(0, 0, 0);
			if(cmd != CMD_ID_CFG_NS) // Get/set config (not save to Flash)
				flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
			ble_send_cfg();
		} else if (cmd == CMD_ID_CFG_DEF) { // Set default config
			memcpy(&cfg, &def_cfg, sizeof(cfg));
			test_config();
			ev_adv_timeout(0, 0, 0);
			flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
			ble_send_cfg();
#if USE_TRIGGER_OUT
		} else if (cmd == CMD_ID_TRG || cmd == CMD_ID_TRG_NS) { // Get/set trg data
			if(--len > sizeof(trg))	len = sizeof(trg);
			if(len)
				memcpy(&trg, &req->dat[1], len);
			test_trg_on();
			if(cmd != CMD_ID_TRG_NS) // Get/set trg data (not save to Flash)
				flash_write_cfg(&trg, EEP_ID_TRG, FEEP_SAVE_SIZE_TRG);
			ble_send_trg();
		} else if (cmd == CMD_ID_TRG_OUT) { // Set trg out
			if(len > 1)
				trg.flg.trg_output = req->dat[1] != 0;
			test_trg_on();
			ble_send_trg_flg();
#endif // USE_TRIGGER_OUT
		} else if (cmd == CMD_MI_ID_MAC) { // Get/Set mac
			if(len == 2 && req->dat[1] == 0) { // default MAC
				flash_erase_sector(FLASH_MIMAC_ADDR);
				blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
				ble_connected |= 0x80; // reset device on disconnect
			} else if(len == sizeof(mac_public)+2 && req->dat[1] == sizeof(mac_public)) {
				if(memcmp(&mac_public, &req->dat[2], sizeof(mac_public))) {
					memcpy(&mac_public, &req->dat[2], sizeof(mac_public));
					mac_random_static[0] = mac_public[0];
					mac_random_static[1] = mac_public[1];
					mac_random_static[2] = mac_public[2];
					generateRandomNum(2, &mac_random_static[3]);
					mac_random_static[5] = 0xC0; 			//for random static
					flash_erase_sector(FLASH_MIMAC_ADDR);
					flash_write_page(FLASH_MIMAC_ADDR, sizeof(mac_public), mac_public);
					flash_write_page(FLASH_MIMAC_ADDR + sizeof(mac_public), 2, &mac_random_static[3]);
					ble_connected |= 0x80; // reset device on disconnect
				}
			} else	if(len == sizeof(mac_public)+2+2 && req->dat[1] == sizeof(mac_public)+2) {
				if(memcmp(&mac_public, &req->dat[2], sizeof(mac_public))
						|| mac_random_static[3] != req->dat[2+6]
						|| mac_random_static[4] != req->dat[2+7] ) {
					memcpy(&mac_public, &req->dat[2], sizeof(mac_public));
					mac_random_static[0] = mac_public[0];
					mac_random_static[1] = mac_public[1];
					mac_random_static[2] = mac_public[2];
					mac_random_static[3] = req->dat[2+6];
					mac_random_static[4] = req->dat[2+7];
					mac_random_static[5] = 0xC0; 			//for random static
					flash_erase_sector(FLASH_MIMAC_ADDR);
					flash_write_page(FLASH_MIMAC_ADDR, sizeof(mac_public)+2, &req->dat[2]);
					ble_connected |= 0x80; // reset device on disconnect
				}
			}
			get_mi_keys(MI_KEY_STAGE_MAC);
			mi_key_stage = MI_KEY_STAGE_WAIT_SEND;
		} else if (cmd == CMD_MI_ID_KALL) { // Get all mi keys
			mi_key_stage = get_mi_keys(MI_KEY_STAGE_GET_ALL);
		} else if (cmd == CMD_MI_ID_REST) { // Restore prev mi token & bindkeys
			mi_key_stage = get_mi_keys(MI_KEY_STAGE_RESTORE);
		} else if (cmd == CMD_MI_ID_CLR) {
			cfg.flg2.mi_beacon = 0;
			erase_mikeys();
			olen = 2;
		} else if (cmd == CMD_ID_LCD_DUMP) { // Get/set lcd buf
			if(--len > sizeof(display_buff)) len = sizeof(display_buff);
			if(len) {
				memcpy(display_buff, &req->dat[1], len);
				//update_lcd();
				lcd_flg.b.ext_data = 1;
			} else lcd_flg.b.ext_data = 0;
			ble_send_lcd();
		} else if (cmd == CMD_ID_LCD_FLG) { // Start/stop notify lcd dump and ...
			 if (len > 1)
				 lcd_flg.uc = req->dat[1];
			 send_buf[1] = lcd_flg.uc;
 			 olen = 2;
#if BLE_SECURITY_ENABLE
		} else if (cmd == CMD_ID_PINCODE && len > 4) { // Set new pinCode 0..999999
			uint32_t old_pincode = pincode;
			uint32_t new_pincode = req->dat[1] | (req->dat[2]<<8) | (req->dat[3]<<16) | (req->dat[4]<<24);
			if(pincode != new_pincode) {
				pincode = new_pincode;
				if (flash_write_cfg(&pincode, EEP_ID_PCD, sizeof(pincode))) {
					if((pincode != 0) ^ (old_pincode != 0)) {
						bls_smp_eraseAllParingInformation();
//						cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time() + 5*CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 5 sec
						ble_connected |= 0x80; // reset device on disconnect
					}
					send_buf[1] = 1;
				} else	send_buf[1] = 3;
			} //else send_buf[1] = 0;
			olen = 2;
#endif
		} else if (cmd == CMD_ID_COMFORT) { // Get/set comfort parameters
			if(--len > sizeof(cfg)) len = sizeof(cmf);
			if(len)
				memcpy(&cmf, &req->dat[1], len);
			flash_write_cfg(&cmf, EEP_ID_CMF, sizeof(cmf));
			ble_send_cmf();
		} else if (cmd == CMD_ID_DNAME) { // Get/Set device name
			if(--len > sizeof(ble_name) - 2) len = sizeof(ble_name) - 2;
			if(len) {
				flash_write_cfg(&req->dat[1], EEP_ID_DVN, (req->dat[1] != 0)? len : 0);
				ble_get_name();
				ble_connected |= 0x80; // reset device on disconnect
			}
//			send_buf[0] = CMD_ID_DNAME;
			memcpy(&send_buf[1], &ble_name[2], ble_name[0] - 1);
			olen = ble_name[0];
		} else if (cmd == CMD_MI_ID_DNAME) { // Mi key: DevNameId
			if(len == MI_KEYDNAME_SIZE + 1)
				store_mi_keys(MI_KEYDNAME_SIZE, MI_KEYDNAME_ID, &req->dat[1]);
			get_mi_keys(MI_KEY_STAGE_DNAME);
			mi_key_stage = MI_KEY_STAGE_WAIT_SEND;
		} else if (cmd == CMD_MI_ID_TBIND) { // Mi keys: Token & Bind
			if(len == MI_KEYTBIND_SIZE + 1)
				store_mi_keys(MI_KEYTBIND_SIZE, MI_KEYTBIND_ID, &req->dat[1]);
			get_mi_keys(MI_KEY_STAGE_TBIND);
			mi_key_stage = MI_KEY_STAGE_WAIT_SEND;
#if USE_CLOCK || USE_FLASH_MEMO
		} else if (cmd == CMD_ID_UTC_TIME) { // Get/set utc time
			if(--len > sizeof(utc_time_sec)) len = sizeof(utc_time_sec);
			if(len)
				memcpy(&utc_time_sec, &req->dat[1], len);
			memcpy(&send_buf[1], &utc_time_sec, sizeof(utc_time_sec));
			olen = sizeof(utc_time_sec) + 1;
#if USE_TIME_ADJUST
		} else if (cmd == CMD_ID_TADJUST) { // Get/set adjust time clock delta (in 1/16 us for 1 sec)
			if(len > 2) {
				int16_t delta = req->dat[1] | (req->dat[2] << 8);
				utc_time_tick_step = CLOCK_16M_SYS_TIMER_CLK_1S + delta;
				flash_write_cfg(&utc_time_tick_step, EEP_ID_TIM, sizeof(utc_time_tick_step));
			}
			memcpy(&send_buf[1], &utc_time_tick_step, sizeof(utc_time_tick_step));
			olen = sizeof(utc_time_tick_step) + 1;
#endif
#endif
#if USE_FLASH_MEMO
		} else if (cmd == CMD_ID_LOGGER && len > 2) { // Read memory measures
			rd_memo.cnt = req->dat[1] | (req->dat[2] << 8);
			if(rd_memo.cnt) {
				rd_memo.saved = memo;
				if(len > 4)
					rd_memo.cur = req->dat[3] | (req->dat[4] << 8);
				else
					rd_memo.cur = 0;
				bls_pm_setManualLatency(0);
			} else
				bls_pm_setManualLatency(cfg.connect_latency);
		} else if (cmd == CMD_ID_CLRLOG && len > 2) { // Clear memory measures
			if(req->dat[1] == 0x12 && req->dat[2] == 0x34) {
				clear_memo();
				olen = 2;
			}
#endif
		} else if (cmd == CMD_ID_MTU && len > 1) { // Request Mtu Size Exchange
			if(req->dat[1] > ATT_MTU_SIZE)
				send_buf[1] = blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, req->dat[1]);
			else
				send_buf[1] = 0xff;
			olen = 2;

			// Debug commands (unsupported in different versions!):

		} else if (cmd == CMD_ID_REBOOT) { // Set Reboot on disconnect
			ble_connected |= 0x80; // reset device on disconnect
			olen = 2;
		} else if (cmd == CMD_ID_DEBUG && len > 3) { // test/debug
			flash_read_page((req->dat[1] | (req->dat[2]<<8) | (req->dat[3]<<16)), 18, &send_buf[4]);
			memcpy(send_buf, req->dat, 4);
			olen = 18+4;
		}
		if(olen)
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, olen);
	}
}
