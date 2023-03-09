/*
 * ext_ota.c
 *
 *  Created on: 04.03.2023
 *      Author: pvvx
 */
#include <stdint.h>
#include "tl_common.h"

#if USE_EXT_OTA  // Compatible BigOTA

typedef struct _ext_ota_t {
	uint32_t start_addr;
	uint32_t ota_size;
	uint32_t check_addr;
} ext_ota_t;

RAM ext_ota_t ext_ota;

// check_ext_ota(ota_addr, ota_size);
uint8_t check_ext_ota(uint32_t ota_addr, uint32_t ota_size) {
	uint32_t faddr, efaddr;
	uint32_t fbuf;
	if(ota_size >= 208) // 208 kbytes
		return -2;
	if(ota_addr & (FLASH_SECTOR_SIZE-1) || ota_size < FLASH_SECTOR_SIZE)
		return -3;
	if(ota_addr == ota_program_offset && ota_size < ota_firmware_size_k)
		return 0;
	if(ota_addr < 0x40000)
		return -4;
	ext_ota.start_addr = ota_addr;
	ext_ota.ota_size = ota_size;
	efaddr = ota_addr + (ota_size<<10);
	while(faddr < efaddr) {
		flash_read_page(faddr, sizeof(fbuf), (unsigned char *) &fbuf);
		if(fbuf != -1) {
			ext_ota.check_addr = ota_addr;
			return 1;
		}
		faddr += 1024;
	}
	ota_program_offset = ext_ota.start_addr;
	ota_firmware_size_k = ext_ota.ota_size >> 10; // in kbytes

	ota_is_working = 0xff;
	ble_connected &= ~2;
	bls_pm_setManualLatency(0);
	return 0;
}

#endif // USE_EXT_OTA
