/*
 * ext_ota.c
 *
 *  Created on: 04.03.2023
 *      Author: pvvx
 */
#include "tl_common.h"

#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "stack/ble/service/ble_ll_ota.h"
#include "ble.h"
#include "lcd.h"
#include "logger.h"
#include "flash_eep.h"
#include "cmd_parser.h"
#include "ext_ota.h"

#define OTA1_FADDR 0x00000
#define OTA1_FADDR_ID (OTA1_FADDR + 8)
#define OTA2_FADDR 0x20000
#define SIZE_LOW_OTA OTA2_FADDR
#define OTA2_FADDR_ID (OTA2_FADDR + 8)
#define BIG_OTA2_FADDR 0x40000 // Big OTA2
#define BIG_OTA2_FADDR_ID (BIG_OTA2_FADDR + 8)
#define MI_HW_SAVE_FADDR (CFG_ADR_MAC+0xfe0) // check flash_erase_mac_sector()

#if !ZIGBEE_TUYA_OTA

/* Reformat Big OTA to Low OTA */
void big_to_low_ota(void) {
	// find the real FW flash address
	u32 id = ID_BOOTABLE;
	u32 size;
	u32 faddrr = OTA1_FADDR;
	u32 faddrw = OTA1_FADDR;
	u32 buf_blk[64];
	do {
		flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
		if(buf_blk[2] == id)
			return;
		faddrr += SIZE_LOW_OTA;
	} while(faddrr < BIG_OTA2_FADDR);
	// faddrr = BIG_OTA2_FADDR
	flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
	if(buf_blk[2] == id && buf_blk[6] > FLASH_SECTOR_SIZE && buf_blk[6] < SIZE_LOW_OTA) {
		buf_blk[2] &= 0xffffffff; // clear id "bootable"
		size = buf_blk[6];
		size += FLASH_SECTOR_SIZE - 1;
		size &= ~(FLASH_SECTOR_SIZE - 1);
		flash_erase_sector(faddrw); // 45 ms, 4 mA
		flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
		faddrr += sizeof(buf_blk);
		// size += faddrw;
		faddrw += sizeof(buf_blk);
		while(faddrw < size) {
			if((faddrw & (FLASH_SECTOR_SIZE - 1)) == 0)
				flash_erase_sector(faddrw); // 45 ms, 4 mA
				// rd-wr 4kB - 20 ms, 4 mA
				flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
			faddrr += sizeof(buf_blk);
			flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
			faddrw += sizeof(buf_blk);
		}
		// set id "bootable" to new segment
		flash_write_page(OTA1_FADDR+8, sizeof(id), (unsigned char *) &id);
		// clear the "bootable" identifier on the current OTA segment?
		while(1)
			start_reboot();
	}
}

#else

#define ZIGBEE_BOOT_OTA_FADDR	0x8000
#define ZIGBEE_MAC_FADDR	0xff000

_attribute_ram_code_
void tuya_zigbee_ota(void) {
	// find the real FW flash address
	u32 id = ID_BOOTABLE;
	u32 size;
	u32 faddrr = OTA1_FADDR;
	u32 faddrw = OTA2_FADDR;
	u32 faddrs = OTA2_FADDR;
	u32 buf_blk[64];
	flash_unlock();
	flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
	if(buf_blk[2] == id) {
		faddrr = ZIGBEE_BOOT_OTA_FADDR;
		flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
		if(buf_blk[2] != id)
			return;
	} else {
		faddrr = BIG_OTA2_FADDR;
		flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
		if(buf_blk[2] != id)
			return;
		faddrw = OTA1_FADDR;
	}
	flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
	if(buf_blk[2] == id && buf_blk[6] > FLASH_SECTOR_SIZE && buf_blk[6] < SIZE_LOW_OTA) {
		faddrs = faddrw;
		buf_blk[2] &= 0xffffffff; // clear id "bootable"
		size = buf_blk[6];
		size += FLASH_SECTOR_SIZE - 1;
		size &= ~(FLASH_SECTOR_SIZE - 1);
		size += faddrw;
		flash_erase_sector(faddrw); // 45 ms, 4 mA
		flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
		faddrr += sizeof(buf_blk);
		// size += faddrw;
		faddrw += sizeof(buf_blk);
		while(faddrw < size) {
			if((faddrw & (FLASH_SECTOR_SIZE - 1)) == 0)
				flash_erase_sector(faddrw); // 45 ms, 4 mA
				// rd-wr 4kB - 20 ms, 4 mA
				flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
			faddrr += sizeof(buf_blk);
			flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
			faddrw += sizeof(buf_blk);
		}
		// set id "bootable" to new segment
		flash_write_page(faddrs+8, sizeof(id), (unsigned char *) &id);
		if(faddrs != OTA1_FADDR) { // clear the "bootable" identifier on the current OTA segment?
			faddrw = CFG_ADR_MAC;
			do {
				flash_erase_sector(faddrw);
				faddrw += FLASH_SECTOR_SIZE;
			} while(faddrw < FMEMORY_SCFG_BASE_ADDR);
			flash_read_page(ZIGBEE_MAC_FADDR, 8, (unsigned char *) &buf_blk);
			u16 *p = (u16 *)buf_blk;
			if(p[2] == 0xa4c1)
				flash_write_page(CFG_ADR_MAC, 8, (unsigned char *) &buf_blk);
			flash_erase_sector(OTA1_FADDR);
			flash_erase_sector(ZIGBEE_BOOT_OTA_FADDR);
		}
		while(1)
			reg_pwdn_ctrl = BIT(5);
	}
}

#endif

// #if (DEVICE_TYPE == DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MHO_C401)
#if defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)

/*
 * Read HW version
 * Flash LYWSD03MMC B1.4:
 * 00055000:  42 31 2E 34 46 31 2E 30 2D 43 46 4D 4B 2D 4C 42  B1.4F1.0-CFMK-LB
 * 00055010:  2D 5A 43 58 54 4A 2D 2D FF FF FF FF FF FF FF FF  -ZCXTJ--
 * Flash LYWSD03MMC B1.5:
 * 00055000:  42 31 2E 35 46 32 2E 30 2D 43 46 4D 4B 2D 4C 42  B1.5F2.0-CFMK-LB
 * 00055010:  2D 4A 48 42 44 2D 2D 2D FF FF FF FF FF FF FF FF  -JHBD---
 * Flash LYWSD03MMC B1.6 & B1.7 & B1.8 -- ?
 * Flash LYWSD03MMC B1.9:
 * 00055000:  42 31 2E 39 46 31 2E 30 2D 43 46 4D 4B 2D 4C 42  B1.9F1.0-CFMK-LB
 * 00055010:  2D 46 4C 44 2D 2D 2D 2D FF FF FF FF FF FF FF FF  -FLD----
 * Flash LYWSD03MMC B1.6(2025):
 * 00055000:  42 31 2E 36│46 32 2E 30│2D 33 2D 46│59 2D 4E 46  B1.6F2.0-3-FY-NF
 * 00055010:  2D 53 2D 47│2D 4A 48 2D│FF FF FF FF│FF FF FF FF  -S-G-JH-
 * Flash LYWSD03MMC B2.0:
 * 00055000:  42 32 2E 30 46 31 2E 30 2D 43 46 4D 4B 2D 4C 42  B2.0F1.0-CFMK-LB
 * 00055010:  2D 4D 4A 44 5A 2D 2D 2D FF FF FF FF FF FF FF FF  -MJDZ---
 * Flash MHO-C401:
 * 00055000:  56 5F 31 34 47 2D 31 39 2D 30 30 30 30 30 30 30  V_14G-19-0000000
 * 00055010:  30 30 30 30 30 30 30 30 FF FF FF FF FF FF FF FF  00000000
 * Flash MJWSD05MMC:
 * 0007D000:  56 32 2E 33 46 32 2E 30 2D 43 46 4D 4B 2D 4C 42  V2.3F2.0-CFMK-LB
 * 0007D010:  2D 54 4D 44 5A 2D 2D 2D FF FF FF FF FF FF FF FF  -TMDZ---  
 */

#if (DEVICE_TYPE == DEVICE_MJWSD05MMC)
static const u8 _mi_hw_vers[] = "F2.0-CFMK-LB-TMDZ---";
#elif (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
static const u8 _mi_hw_vers[] = "F2.0-JY-LB-TMDZ-HW--";
#elif (DEVICE_TYPE == DEVICE_LYWSD03MMC)
static const u8 _mi_hw_vers[] = "F1.0-CFMK-LB-ZCXTJ--";
static const u8 _mi_hw_vers2[] = "F2.0-3-FY-NF-S-G-JH-";
#elif (DEVICE_TYPE == DEVICE_MHO_C401)
static const u8 _mi_hw_vers[] = "G-19-000000000000000";
#elif (DEVICE_TYPE == DEVICE_MJWSD06MMC)
//static const u8 _mi_hw_vers[] = "F3.0-4-FY-LB-S-G-TM-";
#else
#error "Define MI_HW_VER_FADDR & _mi_hw_vers!"
#endif

u32 get_mi_hw_version(void) {
	u32 hw[6];
	flash_read_page(MI_HW_SAVE_FADDR, sizeof(hw[0]), (unsigned char *) &hw);
	if(hw[0] == 0xffffffff) {
		flash_read_page(MI_HW_VER_FADDR, sizeof(hw), (unsigned char *) &hw);
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN) || (DEVICE_TYPE == DEVICE_MJWSD06MMC)
		if ((hw[0] & 0xf0fff0ff) != 0x302E3056)
#elif (DEVICE_TYPE == DEVICE_LYWSD03MMC)
		if ((hw[0] & 0xf0fff0ff) != 0x302E3042)
#elif (DEVICE_TYPE == DEVICE_MHO_C401)
		if ((hw[0] & 0xf0f0ffff) != 0x30305F56)
#else
#error "DEVICE_TYPE = ?"
#endif
			return 0;
		flash_write(MI_HW_SAVE_FADDR, sizeof(hw), (unsigned char *) &hw);
		memcpy(my_HardStr, &hw[0],  sizeof(my_HardStr));
		memcpy(my_SerialStr, &hw[1], sizeof(my_SerialStr));
	}
	return hw[0];
}

void set_SerialStr(void) {
	u32 hw[6];
	flash_read_page(MI_HW_SAVE_FADDR, sizeof(hw), (unsigned char *) &hw);
	if(hw[0] == 0xffffffff) {
#if (DEVICE_TYPE == DEVICE_LYWSD03MMC)
		if(lcd_i2c_addr == N16_I2C_ADDR) {
			memcpy(my_SerialStr, _mi_hw_vers2, sizeof(my_SerialStr));
		} else
#endif
		{
			memcpy(my_SerialStr, _mi_hw_vers, sizeof(my_SerialStr));
		}
#if (DEVICE_TYPE == DEVICE_MHO_C401)
		hw[0] = 0x34315F56; // "V_14" -  56 5F 31 34
#elif (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
		hw[0] = 0x332E3256; // "V2.3" -  56 32 2E 33
#endif
	} else {
		memcpy(my_SerialStr, &hw[1], sizeof(my_SerialStr));
	}
#if (DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
	memcpy(my_HardStr, &hw[0], sizeof(my_HardStr));
#endif
}
#endif // defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)


#if (DEV_SERVICES & SERVICE_OTA_EXT)  // Compatible BigOTA

RAM ext_ota_t ext_ota;

u32 check_sector_clear(u32 addr) {
	u32 faddr = addr, efaddr, fbuf;
	faddr &= ~(FLASH_SECTOR_SIZE-1);
	efaddr = faddr + FLASH_SECTOR_SIZE;
	while(faddr < efaddr) {
		flash_read_page(faddr, sizeof(fbuf), (unsigned char *) &fbuf);
		if(fbuf != 0xffffffff) {
			flash_erase_sector(faddr & (~(FLASH_SECTOR_SIZE-1)));
			break;
		}
		faddr += FLASH_SECTOR_SIZE/4;
	}
	return efaddr;
}

void ota_result_cb(int result) {
	if(result == OTA_SUCCESS
		&& wrk.ota_is_working == OTA_WAIT
		&& ext_ota.start_addr == BIG_OTA2_FADDR) {
		// clear the "bootable" identifier on the current work segment
#if 0 // big size flash
		flash_read_page(OTA1_FADDR_ID, 1, (unsigned char *) &boot_id);
		if(boot_id != 0xff) {
			flash_write_page(OTA1_FADDR_ID, 1, (unsigned char *) &z_byte);
		}
		flash_read_page(OTA2_FADDR_ID, 1, (unsigned char *) &boot_id);
		if(boot_id != 0xff) {
			flash_write_page(OTA2_FADDR_ID, 1, (unsigned char *) &z_byte);
		}
#else // optimize size flash
		check_sector_clear(OTA1_FADDR_ID);
		check_sector_clear(OTA2_FADDR_ID);
#endif
	}
}

/*
 * Input:
 *  ota_addr >= 0x400000
 *  ota_size > 4 and < 208 (in kB)
 */
u8 check_ext_ota(u32 ota_addr, u32 ota_size) {
	if(wrk.ota_is_working == OTA_EXTENDED)
		return EXT_OTA_BUSY;
	if(wrk.ota_is_working)
		return EXT_OTA_WORKS;
	if(ota_addr <= BIG_OTA2_FADDR)
		return EXT_OTA_OK;
	if(ota_size >= 208 || ota_size < 4
		|| (ota_addr & (FLASH_SECTOR_SIZE-1)))
		return EXT_OTA_ERR_PARM;
	wrk.ble_connected |= BIT(CONNECTED_FLG_RESET_OF_DISCONNECT);
	wrk.ota_is_working = OTA_EXTENDED; // flag ext.ota
	ext_ota.start_addr = ota_addr;
	ext_ota.check_addr = ota_addr;
	ext_ota.ota_size = (ota_size + 3) & 0xfffffc;
	bls_pm_setManualLatency(3);
	SHOW_OTA_SCREEN();
	return EXT_OTA_BUSY;
}

// call if(wrk.ota_is_working == OTA_EXTENDED)
void clear_ota_area(void) {
	struct __attribute__((packed)) {
		u16 id_ok;
		u32 start_addr;
		u32 ota_size;
	} msg;
	if(bls_ll_requestConnBrxEventDisable() < 256)
		return;
	if (ext_ota.check_addr >= ext_ota.start_addr + (ext_ota.ota_size << 10)) {
#if 0 // big size flash
			bls_ota_set_fwSize_and_fwBootAddr(ext_ota.ota_size, ext_ota.start_addr);
#else // optimize size flash
			ota_firmware_size_k = ext_ota.ota_size;
			ota_program_offset = ext_ota.start_addr;
#endif
			// send notify
			msg.id_ok = (EXT_OTA_READY << 8) + CMD_ID_SET_OTA;
			msg.start_addr = ext_ota.start_addr;
			msg.ota_size = ext_ota.ota_size;
			bls_ota_registerResultIndicateCb(ota_result_cb);
			if(bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (u8 *)&msg, sizeof(msg)) == BLE_SUCCESS) {
				ext_ota.check_addr = 0;
				wrk.ota_is_working = OTA_WAIT; // flag ext.ota wait
			}
	}
	else  {
			bls_ll_disableConnBrxEvent();
			ext_ota.check_addr = check_sector_clear(ext_ota.check_addr);
			bls_ll_restoreConnBrxEvent();
			// send notify
			msg.id_ok = (EXT_OTA_EVENT << 8) + CMD_ID_SET_OTA;
			msg.start_addr = ext_ota.check_addr;
			msg.ota_size = 0;
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (u8 *)&msg, sizeof(msg));
	}
#ifdef	SET_NO_SLEEP_MODE
	bls_pm_setSuspendMask(SET_NO_SLEEP_MODE);
#else
	bls_pm_setSuspendMask(
			SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
#endif
}

#endif // (DEV_SERVICES & SERVICE_OTA_EXT)

