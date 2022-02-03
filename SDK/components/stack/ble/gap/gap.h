/********************************************************************************************************
 * @file     gap.h 
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




#define GAP_ADTYPE_FLAGS                        0x01 //!< Discovery Mode: @ref GAP_ADTYPE_FLAGS_MODES
#define GAP_ADTYPE_16BIT_INCOMPLETE             0x02 //!< Incomplete List of 16-bit Service Class UUIDs
#define GAP_ADTYPE_16BIT_COMPLETE               0x03 //!< Complete List of 16-bit Service Class UUIDs
#define GAP_ADTYPE_32BIT_INCOMPLETE             0x04 //!< Service: More 32-bit UUIDs available
#define GAP_ADTYPE_32BIT_COMPLETE               0x05 //!< Service: Complete list of 32-bit UUIDs
#define GAP_ADTYPE_128BIT_INCOMPLETE            0x06 //!< Service: More 128-bit UUIDs available
#define GAP_ADTYPE_128BIT_COMPLETE              0x07 //!< Service: Complete list of 128-bit UUIDs
#define GAP_ADTYPE_LOCAL_NAME_SHORT             0x08 //!< Shortened local name
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE          0x09 //!< Complete local name
#define GAP_ADTYPE_TX_POWER_LEVEL               0x0A //!< TX Power Level: 0xXX: -127 to +127 dBm
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE          0x0D //!< Simple Pairing OOB Tag: Class of device (3 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC     0x0E //!< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR     0x0F //!< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
#define GAP_ADTYPE_DEVICE_ID                    0x10 //!< Device ID Profile v1.3 or later
#define GAP_ADTYPE_SM_TK                        0x10 //!< Security Manager TK Value
#define GAP_ADTYPE_SM_OOB_FLAG                  0x11 //!< Secutiry Manager OOB Flags
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE    0x12 //!< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)
#define GAP_ADTYPE_SERVICES_LIST_16BIT          0x14 //!< Service Solicitation: list of 16-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_32BIT          0x1F //!< Service Solicitation: list of 32-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_128BIT         0x15 //!< Service Solicitation: list of 128-bit Service UUIDs
#define GAP_ADTYPE_SERVICE_DATA                 0x16 //!< Service Data
#define GAP_ADTYPE_SERVICE_DATA_UUID_16BIT      0x16 //!< Service Data - 16-bit UUID
#define GAP_ADTYPE_SERVICE_DATA_UUID_32BIT      0x20 //!< Service Data - 32-bit UUID
#define GAP_ADTYPE_SERVICE_DATA_UUID_128BIT     0x21 //!< Service Data - 128-bit UUID
#define GAP_ADTYPE_TARGET_ADDR_PUBLIC           0x17 //!< Public Target Address
#define GAP_ADTYPE_TARGET_ADDR_RANDOM           0x18 //!< Random Target Address
#define GAP_ADTYPE_APPEARANCE                   0x19 //!< Appearance
#define GAP_ADTYPE_ADVERTISING_INTERVAL         0x1A //!< Advertising Interval
#define GAP_ADTYPE_LE_BLUETOOTH_DEVICE_ADDR     0x1B //!< ​LE Bluetooth Device Address
#define GAP_ADTYPE_LE_ROLE                      0x1C //!< LE Role
#define GAP_ADTYPE_SIMPLE_PAIRING_HASHC_256     0x1D //!< Simple Pairing Hash C-256
#define GAP_ADTYPE_SIMPLE_PAIRING_RAND_R256     0x1E //!< Simple Pairing Randomizer R-256
#define GAP_ADTYPE_3D_INFORMATION_DATA          0x3D //!< 3D Synchronization Profile, v1.0 or later
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC        0xFF //!< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data



#define GAP_ADTYPE_LE_LIMITED_DISCOVERABLE_MODE_BIT       0x01
#define GAP_ADTYPE_LE_GENERAL_DISCOVERABLE_MODE_BIT       0x02
#define GAP_ADTYPE_LMP_BIT37_BIT                          0x04



#define GAP_APPEARE_UNKNOWN                     0x0000 //!< Unknown






typedef enum {
	Gap_Role_Broadcaster  = 0,
	Gap_Role_Observer     = 1,
	Gap_Role_Peripheral   = 2,
	Gap_Role_Central      = 3,
}gap_role_t;









/******************************* User Interface  *****************************************/
void 		blc_gap_peripheral_init(void);

void 		blc_gap_central_init(void);









/************************* Stack Interface, user can not use!!! ***************************/



