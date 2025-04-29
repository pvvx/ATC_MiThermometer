#include "tl_common.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "ble.h"
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
#include "rtc.h"
#endif
#include "i2c.h"
#include "lcd.h"
#include "sensor.h"
#if (DEV_SERVICES & SERVICE_18B20)
#include "my18b20.h"
#endif
#include "app.h"
#include "flash_eep.h"
#if (DEV_SERVICES & SERVICE_TH_TRG)
#include "trigger.h"
#include "rds_count.h"
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
#include "logger.h"
#endif
#if USE_MIHOME_BEACON
#include "mi_beacon.h"
#endif
#include "cmd_parser.h"
#if (DEV_SERVICES & SERVICE_OTA_EXT)
#include "ext_ota.h"
#endif
#include "rh.h"
#if (USE_SENSOR_HX71X)
#include "hx71x.h"
#endif
#if (DEV_SERVICES & SERVICE_SCANTIM)
#include "scanning.h"
#endif
#if USE_SDM_OUT
#include "sdm_out.h"
#endif


#define _flash_read(faddr,len,pbuf) flash_read_page(FLASH_BASE_ADDR + (u32)faddr, len, (u8 *)pbuf)

#if (DEV_SERVICES & SERVICE_TIME_ADJUST)
RAM u32 utc_set_time_sec; // clock setting time for delta calculation
#endif

#if (DEV_SERVICES & SERVICE_MI_KEYS)

//#define SEND_BUFFER_SIZE	 (ATT_MTU_SIZE-3) // = 20
#define FLASH_MIMAC_ADDR CFG_ADR_MAC // 0x76000
#define FLASH_MIKEYS_ADDR 0x78000
//#define FLASH_SECTOR_SIZE 0x1000 // in "flash_eep.h"

RAM u8 mi_key_stage;
RAM u8 mi_key_chk_cnt;

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

#if ((DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_MHO_C401N))
u32 find_mi_keys(u16 chk_id, u8 cnt) {
	u32 faddr = FLASH_MIKEYS_ADDR;
	u32 faend = faddr + FLASH_SECTOR_SIZE;
	pblk_mi_keys_t pk = &keybuf;
	u16 id;
	u8 len;
	u8 fbuf[4];
	do {
		_flash_read(faddr, sizeof(fbuf), &fbuf);
		len = fbuf[1];
		id = fbuf[2] | (fbuf[3] << 8);
		if (fbuf[0] == 0xA5) {
			faddr += 8;
			if (len <= sizeof(keybuf.data) && len > 0 && id == chk_id && --cnt
					== 0) {
				pk->klen = len;
				_flash_read(faddr, len, &pk->data);
				return faddr;
			}
		}
		faddr += len + 0x0f;
		faddr &= 0xfffffff0;
	} while (id != 0xffff || len != 0xff || faddr < faend);
	return 0;
}
#else // DEVICE_LYWSD03MMC & DEVICE_CGG1 & DEVICE_CGDK2 & DEVICE_MJWSD05MMC
/* if return != 0 -> keybuf = keys */
u32 find_mi_keys(u16 chk_id, u8 cnt) {
	u32 faddr = FLASH_MIKEYS_ADDR;
	u32 faend = faddr + FLASH_SECTOR_SIZE;
	pblk_mi_keys_t pk = &keybuf;
	u16 id;
	u8 len;
	u8 fbuf[3];
	do {
		_flash_read(faddr, sizeof(fbuf), &fbuf);
		id = fbuf[0] | (fbuf[1] << 8);
		len = fbuf[2];
		faddr += 3;
		if (len <= sizeof(keybuf.data) && len > 0 && id == chk_id && --cnt == 0) {
			pk->klen = len;
			_flash_read(faddr, len, &pk->data);
			return faddr;
		}
		faddr += len;
	} while (id != 0xffff || len != 0xff || faddr < faend);
	return 0;
}
#endif

u8 send_mi_key(void) {
	if (blc_ll_getTxFifoNumber() < 9) {
		while (keybuf.klen > SEND_BUFFER_SIZE - 2) {
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (u8 *) &keybuf, SEND_BUFFER_SIZE);
			keybuf.klen -= SEND_BUFFER_SIZE - 2;
			if (keybuf.klen)
				memcpy(keybuf.data, &keybuf.data[SEND_BUFFER_SIZE - 2], keybuf.klen);
		};
		if (keybuf.klen)
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (u8 *) &keybuf,
					keybuf.klen + 2);
		keybuf.klen = 0;
		return 1;
	};
	return 0;
}

void send_mi_no_key(void) {
	keybuf.klen = 0;
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (u8 *) &keybuf, 2);
}
/* if pkey == NULL -> write new key, else: change deleted keys and current keys*/
u8 store_mi_keys(u8 klen, u16 key_id, u8 * pkey) {
	u8 key_chk_cnt = 0;
	u32 faoldkey = 0;
	u32 fanewkey;
	u32 faddr;
	if (pkey == NULL) {
		while ((faddr = find_mi_keys(MI_KEYDELETE_ID, ++key_chk_cnt)) != 0) {
			if (faddr && keybuf.klen == klen)
				faoldkey = faddr;
		}
	};
	if (faoldkey || pkey) {
		fanewkey = find_mi_keys(key_id, 1);
		if (fanewkey && keybuf.klen == klen) {
			u8 backupsector[FLASH_SECTOR_SIZE];
			_flash_read(FLASH_MIKEYS_ADDR, sizeof(backupsector), &backupsector);
			if (pkey) {
				if (memcmp(&backupsector[fanewkey - FLASH_MIKEYS_ADDR], pkey, keybuf.klen)) {
					memcpy(&backupsector[fanewkey - FLASH_MIKEYS_ADDR], pkey, keybuf.klen);
					flash_erase_sector(FLASH_MIKEYS_ADDR);
					flash_write(FLASH_MIKEYS_ADDR, sizeof(backupsector), backupsector);
					return 1;
				}
			} else if (faoldkey) {
				if (memcmp(&backupsector[fanewkey - FLASH_MIKEYS_ADDR], &backupsector[faoldkey - FLASH_MIKEYS_ADDR], keybuf.klen)) {
					// memcpy(&keybuf.data, &backupsector[faoldkey - FLASH_MIKEYS_ADDR], keybuf.klen);
					memcpy(&backupsector[faoldkey - FLASH_MIKEYS_ADDR], &backupsector[fanewkey - FLASH_MIKEYS_ADDR], keybuf.klen);
					memcpy(&backupsector[fanewkey - FLASH_MIKEYS_ADDR], keybuf.data, keybuf.klen);
					flash_erase_sector(FLASH_MIKEYS_ADDR);
					flash_write(FLASH_MIKEYS_ADDR, sizeof(backupsector), backupsector);
					return 1;
				}
			}
		}
	}
	return 0;
}

u8 get_mi_keys(u8 chk_stage) {
	if (keybuf.klen) {
		if (!send_mi_key())
			return chk_stage;
	};
	switch(chk_stage) {
	case MI_KEY_STAGE_DNAME:
		chk_stage = MI_KEY_STAGE_TBIND;
		keybuf.id = CMD_ID_MI_DNAME;
		if (find_mi_keys(MI_KEYDNAME_ID, 1)) {
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case MI_KEY_STAGE_TBIND:
		chk_stage = MI_KEY_STAGE_CFG;
		keybuf.id = CMD_ID_MI_TBIND;
		if (find_mi_keys(MI_KEYTBIND_ID, 1)) {
			mi_key_chk_cnt = 0;
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case MI_KEY_STAGE_CFG:
		chk_stage = MI_KEY_STAGE_KDEL;
		keybuf.id = CMD_ID_MI_CFG;
		if (find_mi_keys(MI_KEYSEQNUM_ID, 1)) {
			mi_key_chk_cnt = 0;
			send_mi_key();
		} else
			send_mi_no_key();
		break;
	case MI_KEY_STAGE_KDEL:
		keybuf.id = CMD_ID_MI_KDEL;
		if (find_mi_keys(MI_KEYDELETE_ID, ++mi_key_chk_cnt)) {
			send_mi_key();
		} else {
			chk_stage = MI_KEY_STAGE_END;
			send_mi_no_key();
		}
		break;
	case MI_KEY_STAGE_RESTORE: // restore prev mi token & bindkeys
		keybuf.id = CMD_ID_MI_TBIND;
		if (store_mi_keys(MI_KEYTBIND_SIZE, MI_KEYTBIND_ID, NULL)) {
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
#if (DEVICE_TYPE == DEVICE_CGG1)
#if (DEVICE_CGG1_ver == 2022)
		_flash_read(FLASH_MIMAC_ADDR + 1, 6, &keybuf.data[8]); // MAC[6] + mac_random[2]
#else
		_flash_read(FLASH_MIMAC_ADDR, 8, &keybuf.data[8]); // MAC[6] + mac_random[2]
#endif // (DEVICE_CGG1_ver == 2022)
		SwapMacAddress(keybuf.data, &keybuf.data[8]);
		keybuf.data[6] = keybuf.data[8+6];
		keybuf.data[7] = keybuf.data[8+7];
#elif (DEVICE_TYPE == DEVICE_CGDK2)
		_flash_read(FLASH_MIMAC_ADDR + 1, 6, &keybuf.data[8]); // MAC[6] + mac_random[2]
		SwapMacAddress(keybuf.data, &keybuf.data[8]);
		keybuf.data[6] = keybuf.data[8+6];
		keybuf.data[7] = keybuf.data[8+7];
#else
		_flash_read(FLASH_MIMAC_ADDR, 8, keybuf.data); // MAC[6] + mac_random[2]
#endif // DEVICE_TYPE
		keybuf.klen = 8;
		keybuf.id = CMD_ID_DEV_MAC;
		chk_stage = MI_KEY_STAGE_DNAME;
		send_mi_key();
		break;
	};
	return chk_stage;
}

static s32 erase_mikeys(void) {
	s32 tmp;
	_flash_read(FLASH_MIKEYS_ADDR, 4, &tmp);
	if (++tmp) {
		flash_erase_sector(FLASH_MIKEYS_ADDR);
	}
	return tmp;
}

#endif // (DEV_SERVICES & SERVICE_MI_KEYS)

__attribute__((optimize("-Os")))
void cmd_parser(void * p) {
	u8 send_buf[32];
	rf_packet_att_data_t *req = (rf_packet_att_data_t*) p;
	u32 len = req->l2cap - 3;
	if (len) {
		len--;
		u8 cmd = req->dat[0];
		send_buf[0] = cmd;
		send_buf[1] = 0; // no err
		u32 olen = 0;
		if (cmd == CMD_ID_DEV_ID) { // Get DEV_ID
			pdev_id_t p = (pdev_id_t) send_buf;
			// p->pid = CMD_ID_DEV_ID;
			// p->revision = 0;
#if (DEVICE_TYPE == DEVICE_LYWSD03MMC)
			p->hw_version = cfg.hw_ver;
#else
			p->hw_version = DEVICE_TYPE;
#endif
			p->sw_version = VERSION;
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_PLM))
			p->dev_spec_data = sensor_cfg.sensor_type;

#else
			p->dev_spec_data = TH_SENSOR_NONE;
#endif
#if USE_SENSOR_HX71X
			p->dev_spec_data |= IU_SENSOR_HX71X << 8;
#elif (DEV_SERVICES & SERVICE_18B20)
#if USE_SENSOR_MY18B20 == 2
			p->dev_spec_data |= IU_SENSOR_MY18B20x2 << 8;
#else
			p->dev_spec_data |= IU_SENSOR_MY18B20 << 8;
#endif
#elif (DEV_SERVICES & SERVICE_PLM)
			p->dev_spec_data = IU_SENSOR_NTC;
#endif
			p->services = DEV_SERVICES;
			olen = sizeof(dev_id_t);
		} else if (cmd == CMD_ID_MEASURE) { // Start/stop notify measures in connection mode
			if(len >= 1)
				wrk.tx_measures = req->dat[1];
			else
				wrk.tx_measures = 1;
			wrk.msc.b.send_measure = 1;
			send_buf[1] = wrk.tx_measures;
			olen = 2;
#if (DEV_SERVICES & SERVICE_SCREEN)
		} else if (cmd == CMD_ID_EXTDATA) { // Show ext. small and big number
			if (len) {
				if (len > sizeof(ext)) len = sizeof(ext);
				memcpy(&ext, &req->dat[1], len);
				if(ext.vtime_sec == 0xffff)
					lcd_flg.chow_ext_ut = 0xffffffff;
				else
					lcd_flg.chow_ext_ut = wrk.utc_time_sec + ext.vtime_sec;
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
				SET_LCD_UPDATE();
#else
				lcd_flg.update_next_measure = 0;
#endif
			}
			ble_send_ext();
#endif // DEV_SERVICES & SERVICE_SCREEN
		} else if (cmd == CMD_ID_CFG) { // Get/set config
			u8 tmp = ((volatile u8 *)&cfg.flg2)[0];
#if	USE_SENSOR_SCD41
			u8 tst2 = ((volatile u8 *)&cfg.flg)[0];
#endif
			if (len) {
				if (len > sizeof(cfg)) len = sizeof(cfg);
				memcpy(&cfg, &req->dat[1], len);
#if (DEV_SERVICES & SERVICE_SCREEN)
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
				SET_LCD_UPDATE();
#else
				lcd_flg.update_next_measure = 0;
#endif
#endif // DEV_SERVICES & SERVICE_SCREEN
			}
			test_config();
			tmp ^= ((volatile u8 *)&cfg.flg2)[0];
#if (DEV_SERVICES & SERVICE_SCREEN)
			if(tmp & MASK_FLG2_SCR_OFF)
				init_lcd();
#endif // DEV_SERVICES & SERVICE_SCREEN
#if	USE_SENSOR_SCD41
			tst2 ^= ((volatile u8 *)&cfg.flg)[0];
			if(tst2 & MASK_FLG_LP_MSR) // lp_measures on/off
				init_sensor();
#endif
			ev_adv_timeout(0, 0, 0);
			if(tmp & MASK_FLG2_REBOOT) { // (cfg.flg2.bt5phy || cfg.flg2.ext_adv)
				wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			}
			flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
			ble_send_cfg();
		} else if (cmd == CMD_ID_CFG_DEF) { // Set default config
			u8 tmp = ((volatile u8 *)&cfg.flg2)[0];
			memcpy(&cfg, &def_cfg, sizeof(cfg));
			test_config();
			tmp ^= ((volatile u8 *)&cfg.flg2)[0];
			if(tmp & MASK_FLG2_REBOOT) { // (cfg.flg2.bt5phy || cfg.flg2.ext_adv)
				wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			}
			if(tmp & MASK_FLG2_SCR_OFF)
				init_lcd();
			ev_adv_timeout(0, 0, 0);
			flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
			ble_send_cfg();
#if (DEV_SERVICES & SERVICE_TH_TRG) || (DEV_SERVICES & SERVICE_RDS)
		} else if (cmd == CMD_ID_TRG) { // Get/set trg data
			if (len) {
				if (len > sizeof(trg))	len = sizeof(trg);
				memcpy(&trg, &req->dat[1], len);
			}
#if (DEV_SERVICES & SERVICE_RDS)
			//rds.type = trg.rds.type;
			rds_init();
#endif
			flash_write_cfg(&trg, EEP_ID_TRG, FEEP_SAVE_SIZE_TRG);
			test_trg_on();
			ble_send_trg();
		} else if (cmd == CMD_ID_TRG_OUT) { // Set trg out
			if (len)
				trg.flg.trg_output = req->dat[1] != 0;
			ble_send_trg_flg();
#endif // #if (DEV_SERVICES & SERVICE_TH_TRG) || (DEV_SERVICES & SERVICE_RDS)
#if (DEV_SERVICES & SERVICE_MI_KEYS)
		} else if (cmd == CMD_ID_DEV_MAC) { // Get/Set mac
			if (len == 1 && req->dat[1] == 0) { // default MAC
				flash_erase_mac_sector(FLASH_MIMAC_ADDR);
				blc_initMacAddress(FLASH_MIMAC_ADDR, mac_public, mac_random_static);
				wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			} else if (len == sizeof(mac_public)+1 && req->dat[1] == sizeof(mac_public)) {
				if (memcmp(mac_public, &req->dat[2], sizeof(mac_public))) {
					memcpy(mac_public, &req->dat[2], sizeof(mac_public));
					mac_random_static[0] = mac_public[0];
					mac_random_static[1] = mac_public[1];
					mac_random_static[2] = mac_public[2];
					generateRandomNum(2, &mac_random_static[3]);
					mac_random_static[5] = 0xC0; 			//for random static
					blc_newMacAddress(FLASH_MIMAC_ADDR, mac_public, mac_random_static);
					wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
				}
			} else	if (len == sizeof(mac_public)+2+1 && req->dat[1] == sizeof(mac_public)+2) {
				if (memcmp(mac_public, &req->dat[2], sizeof(mac_public))
						|| mac_random_static[3] != req->dat[2+6]
						|| mac_random_static[4] != req->dat[2+7] ) {
					memcpy(mac_public, &req->dat[2], sizeof(mac_public));
					mac_random_static[0] = mac_public[0];
					mac_random_static[1] = mac_public[1];
					mac_random_static[2] = mac_public[2];
					mac_random_static[3] = req->dat[2+6];
					mac_random_static[4] = req->dat[2+7];
					mac_random_static[5] = 0xC0; 			//for random static
					blc_newMacAddress(FLASH_MIMAC_ADDR, mac_public, mac_random_static);
					wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
				}
			}
			get_mi_keys(MI_KEY_STAGE_MAC);
			mi_key_stage = MI_KEY_STAGE_WAIT_SEND;
#else
		} else if (cmd == CMD_ID_DEV_MAC) { // Get/Set mac
			if (len == 1 && req->dat[1] == 0) { // default MAC
				flash_erase_mac_sector(CFG_ADR_MAC);
				blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
				wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			} else if (len == sizeof(mac_public)+1 && req->dat[1] == sizeof(mac_public)) {
				if (memcmp(mac_public, &req->dat[2], sizeof(mac_public))) {
					memcpy(mac_public, &req->dat[2], sizeof(mac_public));
					mac_random_static[0] = mac_public[0];
					mac_random_static[1] = mac_public[1];
					mac_random_static[2] = mac_public[2];
					generateRandomNum(2, &mac_random_static[3]);
					mac_random_static[5] = 0xC0; 			//for random static
					blc_newMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
					wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
				}
			} else	if (len == sizeof(mac_public)+2+1 && req->dat[1] == sizeof(mac_public)+2) {
				if (memcmp(mac_public, &req->dat[2], sizeof(mac_public))
						|| mac_random_static[3] != req->dat[2+6]
						|| mac_random_static[4] != req->dat[2+7] ) {
					memcpy(mac_public, &req->dat[2], sizeof(mac_public));
					mac_random_static[0] = mac_public[0];
					mac_random_static[1] = mac_public[1];
					mac_random_static[2] = mac_public[2];
					mac_random_static[3] = req->dat[2+6];
					mac_random_static[4] = req->dat[2+7];
					mac_random_static[5] = 0xC0; 			//for random static
					blc_newMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
					wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
				}
			}
			send_buf[1] = 8;
			_flash_read(CFG_ADR_MAC, 8, &send_buf[2]); // MAC[6] + mac_random[2]
			olen = 8 + 2;
#endif // (DEV_SERVICES & SERVICE_MI_KEYS)

#if (DEV_SERVICES & SERVICE_BINDKEY)
		} else if (cmd == CMD_ID_BKEY) { // Get/set beacon bindkey
			if (len == sizeof(bindkey)) {
				memcpy(bindkey, &req->dat[1], sizeof(bindkey));
				flash_write_cfg(&bindkey, EEP_ID_KEY, sizeof(bindkey));
				bindkey_init();
			}
			if (flash_read_cfg(&bindkey, EEP_ID_KEY, sizeof(bindkey)) == sizeof(bindkey)) {
				memcpy(&send_buf[1], bindkey, sizeof(bindkey));
				olen = sizeof(bindkey) + 1;
			} else { // No bindkey in EEP!
				send_buf[1] = 0xff;
				olen = 2;
			}
#endif
#if (DEV_SERVICES & SERVICE_MI_KEYS)
		} else if (cmd == CMD_ID_MI_KALL) { // Get all mi keys
			mi_key_stage = get_mi_keys(MI_KEY_STAGE_GET_ALL);
		} else if (cmd == CMD_ID_MI_REST) { // Restore prev mi token & bindkeys
			mi_key_stage = get_mi_keys(MI_KEY_STAGE_RESTORE);
//			wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
		} else if (cmd == CMD_ID_MI_CLR) { // Delete all mi keys
			erase_mikeys();
			olen = 2;
#endif // (DEV_SERVICES & SERVICE_MI_KEYS)
#if (DEV_SERVICES & SERVICE_SCREEN)
		} else if (cmd == CMD_ID_LCD_DUMP) { // Get/set lcd buf
			if (len) {
				if (len > sizeof(display_buff))
					len = sizeof(display_buff);
				memcpy(display_buff, &req->dat[1], len);
				lcd_flg.b.ext_data_buf = 1; // update_lcd();
				lcd_flg.update = 1;	// SET_LCD_UPDATE();
			} else if(lcd_flg.b.ext_data_buf) {
				lcd_flg.b.ext_data_buf = 0;
				lcd_flg.update = 1;	// SET_LCD_UPDATE();
			}
			ble_send_lcd();
		} else if (cmd == CMD_ID_LCD_FLG) { // Start/stop notify lcd dump and ...
			 if (len)
				 lcd_flg.all_flg = req->dat[1];
			 send_buf[1] = lcd_flg.all_flg;
 			 olen = 2;
#endif // DEV_SERVICES & SERVICE_SCREEN
#if (DEV_SERVICES & SERVICE_PINCODE)
		} else if (cmd == CMD_ID_PINCODE && len > 3) { // Set new pinCode 0..999999
			u32 old_pincode = pincode;
			u32 new_pincode = req->dat[1] | (req->dat[2]<<8) | (req->dat[3]<<16) | (req->dat[4]<<24);
			if (pincode != new_pincode) {
				pincode = new_pincode;
				if (flash_write_cfg(&pincode, EEP_ID_PCD, sizeof(pincode))) {
					if ((pincode != 0) ^ (old_pincode != 0)) {
						bls_smp_eraseAllParingInformation();
						wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
					}
					send_buf[1] = 1;
				} else	send_buf[1] = 3;
			} //else send_buf[1] = 0;
			olen = 2;
#endif
#if (DEV_SERVICES & SERVICE_SCREEN)
		} else if (cmd == CMD_ID_COMFORT) { // Get/set comfort parameters
			if (len) {
				if (len > sizeof(cmf)) len = sizeof(cmf);
				memcpy(&cmf, &req->dat[1], len);
			}
			flash_write_cfg(&cmf, EEP_ID_CMF, sizeof(cmf));
			ble_send_cmf();
#endif
		} else if (cmd == CMD_ID_DNAME) { // Get/Set device name
			if (len) {
				if (len > MAX_DEV_NAME_LEN) len = MAX_DEV_NAME_LEN;
				flash_write_cfg(&req->dat[1], EEP_ID_DVN, (req->dat[1] != 0)? len : 0);
				ble_set_name();
				wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			}
			memcpy(&send_buf[1], &ble_name[2], ble_name[0] - 1);
			olen = ble_name[0];
#if (DEV_SERVICES & SERVICE_MI_KEYS)
		} else if (cmd == CMD_ID_MI_DNAME) { // Mi key: DevNameId
			if (len == MI_KEYDNAME_SIZE)
				store_mi_keys(MI_KEYDNAME_SIZE, MI_KEYDNAME_ID, &req->dat[1]);
			get_mi_keys(MI_KEY_STAGE_DNAME);
			mi_key_stage = MI_KEY_STAGE_WAIT_SEND;
		} else if (cmd == CMD_ID_MI_TBIND) { // Mi keys: Token & Bind
			if (len == MI_KEYTBIND_SIZE)
				store_mi_keys(MI_KEYTBIND_SIZE, MI_KEYTBIND_ID, &req->dat[1]);
			get_mi_keys(MI_KEY_STAGE_TBIND);
			mi_key_stage = MI_KEY_STAGE_WAIT_SEND;
#endif // (DEV_SERVICES & SERVICE_MI_KEYS)
		} else if (cmd == CMD_ID_UTC_TIME) { // Get/set utc time
			if (len) {
				if (len > sizeof(wrk.utc_time_sec)) len = sizeof(wrk.utc_time_sec);
				memcpy(&wrk.utc_time_sec, &req->dat[1], len);
#if (DEV_SERVICES & SERVICE_TIME_ADJUST)
				utc_set_time_sec = wrk.utc_time_sec;
#endif
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
				rtc_set_utime(wrk.utc_time_sec);
#endif
				SET_LCD_UPDATE();
			}
			memcpy(&send_buf[1], &wrk.utc_time_sec, sizeof(wrk.utc_time_sec));
#if (DEV_SERVICES & SERVICE_TIME_ADJUST)
			memcpy(&send_buf[sizeof(wrk.utc_time_sec) + 1], &utc_set_time_sec, sizeof(utc_set_time_sec));
			olen = sizeof(wrk.utc_time_sec) + sizeof(utc_set_time_sec) + 1;
#else
			olen = sizeof(wrk.utc_time_sec) + 1;
#endif
#if (DEV_SERVICES & SERVICE_TIME_ADJUST)
		} else if (cmd == CMD_ID_TADJUST) { // Get/set adjust time clock delta (in 1/16 us for 1 sec)
			if (len > 1) {
				s16 delta = req->dat[1] | (req->dat[2] << 8);
				wrk.utc_time_tick_step = CLOCK_16M_SYS_TIMER_CLK_1S + delta;
				flash_write_cfg(&wrk.utc_time_tick_step, EEP_ID_TIM, sizeof(wrk.utc_time_tick_step));
			}
			memcpy(&send_buf[1], &wrk.utc_time_tick_step, sizeof(wrk.utc_time_tick_step));
			olen = sizeof(wrk.utc_time_tick_step) + 1;
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
		} else if (cmd == CMD_ID_LOGGER && len > 1) { // Read memory measures
			rd_memo.cnt = req->dat[1] | (req->dat[2] << 8);
			if (rd_memo.cnt) {
				rd_memo.saved = memo;
				if (len > 3)
					rd_memo.cur = req->dat[3] | (req->dat[4] << 8);
				else
					rd_memo.cur = 0;
				bls_pm_setManualLatency(0);
			} else
				bls_pm_setManualLatency(cfg.connect_latency);
		} else if (cmd == CMD_ID_CLRLOG && len > 2) { // Clear memory measures
			if (req->dat[1] == 0x12 && req->dat[2] == 0x34) {
				clear_memo();
				olen = 2;
			}
#endif
		} else if (cmd == CMD_ID_MTU && len) { // Request Mtu Size Exchange
			if (req->dat[1] >= ATT_MTU_SIZE)
				send_buf[1] = blc_att_requestMtuSizeExchange(BLS_CONN_HANDLE, req->dat[1]);
			else
				send_buf[1] = 0xff;
			olen = 2;
		} else if (cmd == CMD_ID_REBOOT) { // Set Reboot on disconnect
			wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
			olen = 2;
		} else if (cmd == CMD_ID_SET_OTA) { // Get/Set OTA address and size
#if (DEV_SERVICES & SERVICE_OTA_EXT)  // Compatible BigOTA
			u32 ota_addr, ota_size;
			if (len > 7) {
				memcpy(&ota_addr, &req->dat[1], 4);
				memcpy(&ota_size, &req->dat[5], 4);
				send_buf[1] = check_ext_ota(ota_addr, ota_size);
			}
#endif
			memcpy(&send_buf[2], &ota_program_offset, 4);
			memcpy(&send_buf[2+4], &ota_firmware_size_k, 4);
			olen = 2 + 8;
		} else if (cmd == CMD_ID_GDEVS) {   // Get address devises
#if (DEV_SERVICES & SERVICE_THS) || (DEV_SERVICES & SERVICE_IUS)
			send_buf[1] = sensor_cfg.i2c_addr;
#else
			send_buf[1] = 0;
#endif
#if (DEV_SERVICES & SERVICE_SCREEN)
#if ((DEVICE_TYPE == DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_CGDK2) || (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MHO_C122) || (DEVICE_TYPE == DEVICE_LKTMZL02) || (DEVICE_TYPE == DEVICE_ZTH05Z) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN) || (DEVICE_TYPE == DEVICE_ZYZTH01) || (DEVICE_TYPE == DEVICE_MJWSD06MMC))
			send_buf[2] = lcd_i2c_addr;
#else
			send_buf[2] = 1;	// SPI
#endif
#else
			send_buf[2] = 0;	// none
#endif
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
			send_buf[3] = rtc_i2c_addr;
			olen = 3 + 1;
#else
			olen = 2 + 1;
#endif
#if defined(I2C_GROUP) || defined(I2C_SDA)
		} else if (cmd == CMD_ID_I2C_SCAN) {   // Universal I2C/SMBUS read-write
			len = 0;
			olen = 1;
			while(len < 0x100 && olen < SEND_BUFFER_SIZE) {
				send_buf[olen] = (u8)scan_i2c_addr(len);
				if(send_buf[olen])
					olen++;
				len += 2;
			}
		} else if (cmd == CMD_ID_I2C_UTR) {   // Universal I2C/SMBUS read-write
			i2c_utr_t * pbufi = (i2c_utr_t *)&req->dat[1];
			olen = pbufi->rdlen & 0x7f;
			if(len > sizeof(i2c_utr_t)
				&& olen <= SEND_BUFFER_SIZE - 3 // = 17
				&& I2CBusUtr(&send_buf[3],
						pbufi,
						len - sizeof(i2c_utr_t) - 1) == 0 // wrlen: - addr
						)  {
				send_buf[1] = len - 1 - sizeof(i2c_utr_t); // write data len
				send_buf[2] = pbufi->wrdata[0]; // i2c addr
				olen += 3;
			} else {
				send_buf[1] = 0xff; // Error cmd
				olen = 2;
			}
#endif
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_PLM))
#if USE_SENSOR_INA3221
		} else if (cmd == CMD_ID_CFS || cmd == CMD_ID_KZ2 || cmd == CMD_ID_KZ3) {	// Get/Set sensor config
			olen = 0;
			if(cmd == CMD_ID_KZ2)
				olen = 1;
			else if(cmd == CMD_ID_KZ3)
				olen = 2;
			if (len) {
				if (len > sizeof(sensor_cfg.coef[0]))
					len = sizeof(sensor_cfg.coef[0]);
				memcpy(&sensor_cfg.coef[olen], &req->dat[1], len);
				flash_write_cfg(&sensor_cfg.coef, EEP_ID_CFS, sizeof(sensor_cfg.coef));
			}
			memcpy(&send_buf[1], &sensor_cfg.coef[olen], sizeof(sensor_cfg.coef[0]));
			memcpy(&send_buf[1 + sizeof(sensor_cfg.coef[0])], &sensor_cfg.id, 6);
#else
		} else if (cmd == CMD_ID_CFS) {	// Get/Set sensor config
			if (len) {
				if (len > sizeof(sensor_cfg.coef))
					len = sizeof(sensor_cfg.coef);
				memcpy(&sensor_cfg.coef, &req->dat[1], len);
				flash_write_cfg(&sensor_cfg.coef, EEP_ID_CFS, sizeof(sensor_cfg.coef));
			}
			memcpy(&send_buf[1], &sensor_cfg, sensor_cfg_send_size);
#endif
			olen = sensor_cfg_send_size + 1;
		} else if (cmd == CMD_ID_CFS_DEF) {	// Get/Set default sensor config
			memset(&sensor_cfg, 0, sensor_cfg_send_size);
			init_sensor();
			memcpy(&send_buf[1], &sensor_cfg, sensor_cfg_send_size);
			olen = sensor_cfg_send_size + 1;
		} else if (cmd == CMD_ID_SEN_ID) { // Get sensor ID
			memcpy(&send_buf[1], &sensor_cfg.id, sizeof(sensor_cfg.id));
			olen = sizeof(sensor_cfg.id) + 1;
#endif
#if (DEV_SERVICES & SERVICE_18B20)
		} else if (cmd == CMD_ID_CFB20) {	// Get/Set sensor MY18B20 config
			if (len) {
				if (len > sizeof(my18b20.coef))
					len = sizeof(my18b20.coef);
				memcpy(&my18b20.coef, &req->dat[1], len);
				flash_write_cfg(&my18b20.coef, EEP_ID_CMY, sizeof(my18b20.coef));
			}
			memcpy(&send_buf[1], &my18b20, my18b20_send_size);
			send_buf[my18b20_send_size + 1] =
			olen = my18b20_send_size + 2;
		} else if (cmd == CMD_ID_CFB20_DEF) {	// Get/Set default sensor MY18B20 config
			memset(&my18b20.coef, 0, my18b20_send_size);
			init_my18b20();
			memcpy(&send_buf[1], &my18b20.coef, my18b20_send_size);
			olen = my18b20_send_size + 1;
#endif
#if USE_SENSOR_HX71X
		} else if (cmd == CMD_ID_HXC) { // Get/set HX71X config
			if (len) {
				if (len > sizeof(hx71x.cfg))
					len = sizeof(hx71x.cfg);
				memcpy(&hx71x.cfg, &req->dat[1], len);
				flash_write_cfg(&hx71x.cfg, EEP_ID_HXC, sizeof(hx71x.cfg));
			}
			memcpy(&send_buf[1], &hx71x, sizeof(hx71x.cfg) + 4);
			olen = sizeof(hx71x.cfg) + 4 + 1;
#endif
#if (DEV_SERVICES & SERVICE_PLM)
		} else if (cmd == CMD_ID_RH) { // Get/Set sensor RH config
			if (len) {
				if(req->dat[1] == 100)
					send_buf[1] = calibrate_rh_100();
				else if(req->dat[1] == 0)
					send_buf[1] = calibrate_rh_0();
				else
					send_buf[1] = 0xff; // Error cmd
			} else send_buf[1] = 2;
			memcpy(&send_buf[2], &sensor_cfg.adc_rh, 4);
			olen = 5 + 1;
		} else if (cmd == CMD_ID_RH_CAL) { // Calibrate sensor RH
			memcpy(&send_buf[1], &sensor_cfg.adc_rh, 4);
			olen = 4 + 1;
#endif
#if (DEV_SERVICES & SERVICE_SCANTIM)
		} else if (cmd == CMD_ID_SCAN_CFG) { // Get/Set Scan Config parameters
			if (len) {
				scan_stop(); // stop scan
				if (len > sizeof(scan.cfg))
					len = sizeof(scan.cfg);
				memcpy(&scan.cfg, &req->dat[1], len);
				flash_write_cfg(&scan.cfg, EEP_ID_SCN, sizeof(scan.cfg));
			}
			memcpy(&send_buf[1], &scan.cfg, sizeof(scan.cfg));
			olen = sizeof(scan.cfg) + 1;
#endif
#if USE_SDM_OUT
		} else if (cmd == CMD_ID_DAC_CFG) { // Set SDMDAC config
			if (len) {
				if (len > sizeof(sdmdac.cfg))
					len = sizeof(sdmdac.cfg);
				memcpy(&sdmdac.cfg, &req->dat[1], len);
				flash_write_cfg(&sdmdac.cfg, EEP_ID_DAC, sizeof(sdmdac.cfg));
				init_dac();
			}
			memcpy(&send_buf[1], &sdmdac, sizeof(sdmdac));
			olen = sizeof(sdmdac) + 1;
#endif
		} else if (cmd == CMD_ID_FLASH_ID) { // Get Flash JEDEC ID
			flash_read_id(&send_buf[1]); // Read flash UID
			olen = 1 + 3;

		// Debug commands (unsupported in different versions!):

		} else if (cmd == CMD_ID_EEP_RW && len > 1) {
			send_buf[1] = req->dat[1];
			send_buf[2] = req->dat[2];
			olen = req->dat[1] | (req->dat[2] << 8);
			if(len > 2) {
				flash_write_cfg(&req->dat[3], olen, len - 2);
			}
			s16 i = flash_read_cfg(&send_buf[3], olen, SEND_BUFFER_SIZE - 3);
			if(i < 0) {
				send_buf[1] = (u8)(i & 0xff); // Error
				olen = 2;
			} else
				olen = i + 3;
		} else if (cmd == CMD_ID_DEBUG && len > 2) { // test/debug
			_flash_read((req->dat[1] | (req->dat[2]<<8) | (req->dat[3]<<16)), 18, &send_buf[4]);
			memcpy(send_buf, &req->dat, 4);
			olen = 18+4;
		} else if (cmd == CMD_ID_LR_RESET) { // Reset Long Range
			cfg.flg2.longrange = 0;
			cfg.flg2.bt5phy = 0;
			flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
			ble_send_cfg();
			wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT); // reset device on disconnect
#if USE_RH_SENSOR
		} else if (cmd == 0xDF) {
			if(len) {
				get_adc_rh_mv();
			} else {
				calibrate_rh();
			}
			memcpy(&send_buf[1], &rh, sizeof(rh));
			olen = sizeof(rh) + 1;
#endif
		} else {
			send_buf[1] = 0xff; // Error cmd
			olen = 2;
		}
		if (olen)
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, olen);
	}
}
