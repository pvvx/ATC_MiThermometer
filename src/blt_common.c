/********************************************************************************************************
 * @file     blt_common.c
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#include "tl_common.h"
#include "drivers.h"
// CGDK2 076000:  5A 58 2D 34 12 D7 F8 58 2D 34 12 D7 F8 FF FF FF = 58:2D:34:12:D7:F8
/*
 * VVWWXX38C1A4YYZZ
 * public_mac: 		  VVWWXX 38C1A4
 * random_static_mac: VVWWXXYYZZ C0
 */
_attribute_ram_code_
__attribute__((optimize("-Os")))
void SwapMacAddress(u8 *mac_out, u8 *mac_in) {
	mac_out[0] = mac_in[5];
	mac_out[1] = mac_in[4];
	mac_out[2] = mac_in[3];
	mac_out[3] = mac_in[2];
	mac_out[4] = mac_in[1];
	mac_out[5] = mac_in[0];
}

__attribute__((optimize("-Os")))
void blc_newMacAddress(int flash_addr, u8 *mac_pub, u8 *mac_rand) {
#if ((DEVICE_TYPE == DEVICE_CGG1) && (DEVICE_CGG1_ver == 2022))
	u8 mac_flash[16];
	flash_erase_sector(flash_addr);
	mac_flash[0] = 0x5A;
	SwapMacAddress(&mac_flash[1], mac_pub);
	SwapMacAddress(&mac_flash[7], mac_rand);
	mac_flash[13] = 0xFF;
	mac_flash[14] = 0xFF;
	mac_flash[15] = 0x01;
	flash_write_page(flash_addr, sizeof(mac_flash), mac_flash);
#else
#if DEVICE_TYPE == DEVICE_CGDK2
	u8 mac_flash[13];
	flash_erase_sector(flash_addr);
	SwapMacAddress(&mac_flash[1], mac_pub);
	SwapMacAddress(&mac_flash[7], mac_pub);
#else // DEVICE_TYPE != DEVICE_CGDK2
	u8 mac_flash[8];
	flash_erase_sector(flash_addr);
#if DEVICE_TYPE == DEVICE_CGG1
	SwapMacAddress(mac_flash, mac_pub);
#else // DEVICE_TYPE != DEVICE_CGG1
	memcpy(mac_flash, mac_pub, 6);
#endif // DEVICE_TYPE == DEVICE_CGG1
	mac_flash[6] = mac_rand[3];
	mac_flash[7] = mac_rand[4];
#endif	// DEVICE_TYPE == DEVICE_CGDK2
	flash_write_page(flash_addr, sizeof(mac_flash), mac_flash);
#endif
}

__attribute__((optimize("-Os")))
void blc_initMacAddress(int flash_addr, u8 *mac_pub, u8 *mac_rand) {
#if ((DEVICE_TYPE == DEVICE_CGG1) && (DEVICE_CGG1_ver == 2022))
	u8 mac_read[16];
	flash_read_page(flash_addr, sizeof(mac_read), mac_read);
	if (mac_read[0] != 0x5A) {
		mac_read[0] = 0x5A;
		mac_read[1] = 0x58; // company id: 0x582D34
		mac_read[2] = 0x2D;
		mac_read[3] = 0x34;
		generateRandomNum(3, &mac_read[4]);
		memcpy(&mac_read[7], &mac_read[1], 6);
		mac_read[13] = 0xFF;
		mac_read[14] = 0xFF;
		mac_read[15] = 0x01;
		flash_write_page(flash_addr, sizeof(mac_read), mac_read);
	}
	// copy public address
	SwapMacAddress(mac_pub, &mac_read[1]);
	// copy random address
	memcpy(mac_rand, mac_pub, 6);
#else
#if DEVICE_TYPE == DEVICE_CGDK2
	u8 mac_read[14];
#else
	u8 mac_read[8];
#endif
	u32 * p = (u32 *) &mac_read;
	flash_read_page(flash_addr, sizeof(mac_read), mac_read);

	if (p[0] == 0xffffffff && p[1] == 0xffffffff) {
		// no public address on flash
#if DEVICE_TYPE == DEVICE_CGG1
		mac_read[0] = 0x58; // company id: 0x582D34
		mac_read[1] = 0x2D;
		mac_read[2] = 0x34;
		generateRandomNum(5, &mac_read[3]);
		flash_write_page(flash_addr, sizeof(mac_read), mac_read);
	}
	// copy public address
	SwapMacAddress(mac_pub, mac_read);
	// copy random address
	mac_rand[0] = mac_pub[0];
	mac_rand[1] = mac_pub[1];
	mac_rand[2] = mac_pub[2];
	mac_rand[3] = mac_read[6];
	mac_rand[4] = mac_read[7];
	mac_rand[5] = 0xC0; 			//for random static
#elif DEVICE_TYPE == DEVICE_CGDK2
		mac_read[0] = 0x5a; // ID
		mac_read[1] = 0x58; // company id: 0x582D34
		mac_read[2] = 0x2D;
		mac_read[3] = 0x34;
		generateRandomNum(3, &mac_read[4]);
		memcpy(&mac_read[7],&mac_read[1], 6);
		flash_write_page(flash_addr, sizeof(mac_read), mac_read);
	}
	// copy public address
	SwapMacAddress(mac_pub, &mac_read[1]);
	// copy random address
	mac_rand[0] = mac_pub[3];
	mac_rand[1] = mac_pub[2];
	mac_rand[2] = mac_pub[1];
	mac_rand[5] = 0xC0; 			//for random static
	generateRandomNum(2, &mac_rand[3]);
#else
		generateRandomNum(3, mac_read);
		mac_read[3] = 0x38;             //company id: 0xA4C138
		mac_read[4] = 0xC1;
		mac_read[5] = 0xA4;
		generateRandomNum(2, &mac_read[6]);
		flash_write_page(flash_addr, sizeof(mac_read), mac_read);
	}
	// copy public address
	memcpy(mac_pub, mac_read, 6);
	// copy random address
	mac_rand[0] = mac_pub[0];
	mac_rand[1] = mac_pub[1];
	mac_rand[2] = mac_pub[2];
	mac_rand[3] = mac_read[6];
	mac_rand[4] = mac_read[7];
	mac_rand[5] = 0xC0; 			//for random static
#endif
#endif // ((DEVICE_TYPE == DEVICE_CGG1) && (DEVICE_CGG1_ver == 2022))
}

