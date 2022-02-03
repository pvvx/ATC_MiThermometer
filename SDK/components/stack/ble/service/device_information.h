/********************************************************************************************************
 * @file     device_information.h 
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

#pragma once

#include "tl_common.h"
//#include <stack/ble/att.h>
#include "../ble_common.h"
//#include <stack/ble/gap.h>

/** @addtogroup  TELINK_BLE_STACK TELINK BLE Stack
 *  @{
 */

/** @addtogroup  SERVICE_MODULE Service
 *  @{
 */


/** @addtogroup  DEVICE_INFO_Module Device Information
 *  @{
 */
 
/** @addtogroup  DEVICE_INFO_Constant Device Information Constants
 *  @{
 */

/**
 *  @brief  Definition for Characteristics UUID
 */

/** @addtogroup device_char_uuid Device Information Characteristc UUID
 * @{
 */

#define CHARACTERISTIC_UUID_MANU_NAME_STRING                    0x2A29
#define CHARACTERISTIC_UUID_MODEL_NUM_STRING                    0x2A24
#define CHARACTERISTIC_UUID_SERIAL_NUM_STRING                   0x2A25
#define CHARACTERISTIC_UUID_HW_REVISION_STRING                  0x2A27
#define CHARACTERISTIC_UUID_FW_REVISION_STRING                  0x2A26
#define CHARACTERISTIC_UUID_SW_REVISION_STRING                  0x2A28
#define CHARACTERISTIC_UUID_SYSTEM_ID                           0x2A23
#define CHARACTERISTIC_UUID_IEEE_11073_CERT_LIST                0x2A2A
#define CHARACTERISTIC_UUID_PNP_ID                              0x2A50

/** @} end of group device_char_uuid */


/** @addtogroup  IEEE_AUTHORITATIVE_BODY_VALUES IEEE Authoritative Body Values
 *  @{
 */

/**
 *  @brief  IEEE 11073 authoritative body values
 */
#define IEEE_11073_BODY_EMPTY                                   0
#define IEEE_11073_BODY_IEEE                                    1
#define IEEE_11073_BODY_CONTINUA                                2
#define IEEE_11073_BODY_EXP                                     254

/** @} end of group IEEE_AUTHORITATIVE_BODY_VALUES */




/** @addtogroup  SYSTEM_ID_LEN System Id Len
 *  @{
 */

/**
 *  @brief  Definition for System ID length
 */
#define SYSTEM_ID_LEN                                           8

/** @} end of group SYSTEM_ID_LEN */



/** @addtogroup  PNP_ID_LEN PnP ID Len
 *  @{
 */

/**
 *  @brief  Definition for PnP ID length
 */
#define DEVINFO_PNP_ID_LEN                                      7

/** @} end of group PNP_ID_LEN */

/** @} end of group DEVICE_INFO_Constant */



 /** @addtogroup  DEVICE_INFORMATION_Variables Device Information Variables
 *  @{
 */

/**
 *  @brief  External variable for device information Attribute tables
 */

extern  attribute_t devInfo_attrTbl[];

/**
 *  @brief  External variable for device information attribute size
 */
extern u16         devInfo_attrSize;

/** @} end of group DEVICE_INFORMATION_Variables */




 /** @addtogroup  PUBLIC_FUNCTION Device Information APIs
 *  @{
 */



/** @} end of group PUBLIC_FUNCTION */

/** @} end of group DEVICE_INFO_Module */

/** @} end of group SERVICE_MODULE */

/** @} end of group TELINK_BLE_STACK */
