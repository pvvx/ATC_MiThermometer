/********************************************************************************************************
 * @file     ble_ll_ota.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2015
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
/*
 * ble_ll_ota.h
 *
 *  Created on: 2015-7-20
 *      Author: Administrator
 */

#ifndef BLE_LL_OTA_H_
#define BLE_LL_OTA_H_


#ifndef BLE_OTA_FW_CHECK_EN
#define BLE_OTA_FW_CHECK_EN					1
#endif

#define FW_MAX_SIZE							0x40000			//256K

#define CMD_OTA_FW_VERSION					0xff00
#define CMD_OTA_START						0xff01
#define CMD_OTA_END							0xff02


#define FLAG_FW_CHECK						0x5D
#define FW_CHECK_AGTHM2						0x02



typedef struct{
	u8  ota_start_flag;
#if (BLE_OTA_FW_CHECK_EN)
	u8 	fw_check_en;
	u8  fw_check_match;
	u8  rsvd;

	u32 fw_crc_init;

	u16 fw_crc_last_index;
#endif
}ota_service_t;

extern ota_service_t blcOta;


extern int blt_ota_start_flag;
extern u32 blt_ota_start_tick;
extern u32 blt_ota_timeout_us;

extern u32	ota_program_offset;
extern int 	ota_firmware_size_k;


typedef void (*ota_startCb_t)(void);
typedef void (*ota_versionCb_t)(void);

typedef void (*ota_resIndicateCb_t)(int result);

extern ota_resIndicateCb_t otaResIndicateCb;


enum{
	OTA_SUCCESS = 0,     //success
	OTA_PACKET_LOSS,     //lost one or more OTA PDU
	OTA_DATA_CRC_ERR,     //data CRC err
	OTA_WRITE_FLASH_ERR,  //write OTA data to flash ERR
 	OTA_DATA_UNCOMPLETE,  //lost last one or more OTA PDU
 	OTA_TIMEOUT, 		  //
 	OTA_FW_CHECK_ERR,
};

void bls_ota_procTimeout(void);

//user interface
void bls_ota_registerStartCmdCb(ota_startCb_t cb);
void bls_ota_registerVersionReqCb(ota_versionCb_t cb);
void bls_ota_registerResultIndicateCb(ota_resIndicateCb_t cb);

void bls_ota_setTimeout(u32 timeout_us);

extern int otaWrite(void * p);
extern int otaRead(void * p);


//firmware_size_k  must be 4k aligned
void bls_ota_set_fwSize_and_fwBootAddr(int firmware_size_k, int boot_addr);



void bls_ota_clearNewFwDataArea(void);

unsigned short crc16 (unsigned char *pD, int len);

#endif /* BLE_LL_OTA_H_ */
