/********************************************************************************************************
 * @file     gatt.h
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

#ifndef GATT_H_
#define GATT_H_




/************************************ User Interface  *****************************************/

ble_sts_t	blc_gatt_pushHandleValueNotify  (u16 connHandle, u16 attHandle, u8 *p, int len);

ble_sts_t	blc_gatt_pushHandleValueIndicate(u16 connHandle, u16 attHandle, u8 *p, int len);


ble_sts_t 	blc_gatt_pushWriteComand (u16 connHandle, u16 attHandle, u8 *p, int len);

ble_sts_t 	blc_gatt_pushWriteRequest (u16 connHandle, u16 attHandle, u8 *p, int len);




#if 0  //not available now

ble_sts_t 	blc_gatt_pushFindInformationRequest(u8 *dat, u16 start_attHandle, u16 end_attHandle);

ble_sts_t 	blc_gatt_pushFindByTypeValueRequest (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, u8* attr_value, int len);

ble_sts_t 	blc_gatt_pushReadByTypeRequest (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, int uuid_len);

ble_sts_t 	blc_gatt_pushReadRequest (u8 *dat, u16 attHandle);

ble_sts_t 	blc_gatt_pushReadBlobRequest (u8 *dat, u16 attHandle, u16 offset);

ble_sts_t 	blc_gatt_pushReadByGroupTypeRequest (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, int uuid_len);


#endif






/****************************** Stack Interface, user can not use!!! ***************************/
u8 blc_gatt_requestServiceAccess(u16 connHandle, int gatt_perm);






#endif /* GATT_H_ */
