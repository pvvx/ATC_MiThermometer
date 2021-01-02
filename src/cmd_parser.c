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
#include "cmd_parser.h"

#define TX_MAX_SIZE	 (ATT_MTU_SIZE-3) // = 20
#define FLASH_MIMAC_ADDR CFG_ADR_MAC // 0x76000
#define FLASH_MIKEYS_ADDR 0x78000
//#define FLASH_SECTOR_SIZE 0x1000 // in "flash_eep.h"

RAM uint8_t mi_key_stage;
RAM uint8_t mi_key_chk_cnt;

#define MI_KEYTBIND_ID  0x10 // id token + bindkey
#define MI_KEYCFG_ID    0x04 // id config
#define MI_KEYDNAME_ID  0x01 // id device name
#define MI_KEYDELETE_ID 0x00
#define MI_KEYTBIND_SIZE (12+16) // buf token + bindkey size
#define MI_KEYDNAME_SIZE (20) // device name buf size

typedef struct __attribute__((packed)) _blk_mi_keys_t {
	uint8_t id;
	uint8_t klen;	// max length, end length, current length, ...
	uint8_t data[MI_KEYTBIND_SIZE]; // token + bindkey
} blk_mi_keys_t, * pblk_mi_keys_t;
RAM blk_mi_keys_t keybuf;

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

uint8_t backupsector[FLASH_SECTOR_SIZE];
uint8_t restore_prev_mi_keys(void) {
	uint8_t key_chk_cnt = 0;
	uint8_t * pfoldkey = NULL;
	uint8_t * pfnewkey;
	uint8_t * p;
	while((p = find_mi_keys(MI_KEYDELETE_ID, ++key_chk_cnt)) != NULL) {
		if(p && keybuf.klen == MI_KEYTBIND_SIZE)
			pfoldkey = p;
	};
	if(pfoldkey) {
		pfnewkey = find_mi_keys(MI_KEYTBIND_ID, 1);
		if(pfnewkey && keybuf.klen == MI_KEYTBIND_SIZE) {
			if(memcmp(pfnewkey, pfoldkey, keybuf.klen)) {
				memcpy(&backupsector,(uint8_t *)(FLASH_MIKEYS_ADDR), sizeof(backupsector));
				memcpy(&backupsector[(uint32_t)pfoldkey - FLASH_MIKEYS_ADDR], pfnewkey, keybuf.klen);
				memcpy(&keybuf.data, pfoldkey, keybuf.klen);
				memcpy(&backupsector[(uint32_t)pfnewkey - FLASH_MIKEYS_ADDR], pfoldkey, keybuf.klen);
				flash_erase_sector(FLASH_MIKEYS_ADDR);
				flash_write_(FLASH_MIKEYS_ADDR, sizeof(backupsector), backupsector);
				return 1;
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
	case 1:
		chk_stage = 2;
		keybuf.id = CMD_MI_ID_DNAME;
		if(find_mi_keys(MI_KEYDNAME_ID, 1)) {
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case 2:
		chk_stage = 3;
		keybuf.id = CMD_MI_ID_TBIND;
		if(find_mi_keys(MI_KEYTBIND_ID, 1)) {
			mi_key_chk_cnt = 0;
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case 3:
		chk_stage = 4;
		keybuf.id = CMD_MI_ID_CFG;
		if(find_mi_keys(MI_KEYCFG_ID, 1)) {
			mi_key_chk_cnt = 0;
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case 4:
		keybuf.id = CMD_MI_ID_KDEL;
		if(find_mi_keys(MI_KEYDELETE_ID, ++mi_key_chk_cnt)) {
			send_mi_key();
		} else {
			chk_stage = 0;
			send_mi_no_key();
		}
		break;
	case 5: // restore prev mi token & bindkeys
		keybuf.id = CMD_MI_ID_TBIND;
		if(restore_prev_mi_keys()) {
			chk_stage = 6;
			send_mi_key();
		} else {
			chk_stage = 0;
			send_mi_no_key();
		}
		break;
	case 6:
		chk_stage = 0;
		break;
	default: // get all mi keys
		memcpy(&keybuf.data,(uint8_t *)(FLASH_MIMAC_ADDR), 8); // MAC[6] + mac_random[2]
		keybuf.klen = 8;
		keybuf.id = CMD_MI_ID_MAC;
		chk_stage = 1;
		send_mi_key();
		break;
	};
	return chk_stage;
}

void cmd_parser(void * p) {
	rf_packet_att_data_t *req = (rf_packet_att_data_t*) p;
	uint32_t len = req->l2cap - 3;
	if(len) {
		uint8_t cmd = req->dat[0];
		if (cmd == CMD_ID_CFG || cmd == CMD_ID_CFG_NS) {
			if(--len > sizeof(cfg)) len = sizeof(cfg);
			else if(len)
				memcpy(&cfg, &req->dat[1], len);
			test_config();
			ev_adv_timeout(0, 0, 0);
			if(cmd != CMD_ID_CFG_NS)
				flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
			ble_send_cfg();
#if USE_TRIGGER_OUT
		} else if (cmd == CMD_ID_TRG || cmd == CMD_ID_TRG_NS) {
			if(--len > sizeof(trg))
				len = sizeof(trg);
			else if(len)
				memcpy(&trg, &req->dat[1], len);
			test_trg_on();
			if(cmd != CMD_ID_TRG_NS)
				flash_write_cfg(&trg, EEP_ID_TRG, FEEP_SAVE_SIZE_TRG);
			ble_send_trg();
		} else if (cmd == CMD_ID_TRG_OUT) {
			trg.flg.trg_output = req->dat[1] != 0;
			test_trg_on();
#endif // USE_TRIGGER_OUT
		} else if (cmd == CMD_MI_ID_MAC) { // mac
		} else if (cmd == CMD_MI_ID_KALL) { // get all mi keys
			mi_key_stage = get_mi_keys(0xff);
		} else if (cmd == CMD_MI_ID_REST) { // restore prev mi token & bindkeys
			mi_key_stage = get_mi_keys(5);
		} else if (cmd == CMD_ID_EXTDATA) { // Show ext. small and big number
			if(--len > sizeof(ext)) len = sizeof(ext);
			else if(len) {
				memcpy(&ext, &req->dat[1], len);
				vtime_count_sec = ext.vtime_sec;
				vtime_count_us = clock_time();
			}
			ble_send_ext();
		} else if (cmd == CMD_ID_MEASURE) {
			if(len >= 2)
				tx_measures = req->dat[1];
			else {
				end_measure = 1;
				tx_measures = 1;
			}
//		} else if (cmd == 0x44) { // test
//			blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, 128); // 234
		}
	}
}
