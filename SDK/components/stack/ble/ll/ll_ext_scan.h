/********************************************************************************************************
 * @file     ll_ext_scan.h
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Feb. 1, 2018
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

#ifndef LL_SCAN_EXT_H_
#define LL_SCAN_EXT_H_

#include <stack/ble/ble_common.h>
#include <stack/ble/blt_config.h>
#include <stack/ble/hci/hci_const.h>
#include <stack/ble/hci/hci_event.h>
#include <stack/ble/hci/hci_cmd.h>
#include <stack/ble/ll/ll_adv.h>
#include <stack/ble/ll/ll_encrypt.h>
#include <stack/ble/ll/ll_init.h>
#include <stack/ble/ll/ll_pm.h>
#include <stack/ble/ll/ll_scan.h>
#include <stack/ble/ll/ll_whitelist.h>
#include "stack/ble/ll/ll_conn/ll_slave.h"
#include "stack/ble/ll/ll_conn/ll_master.h"


#include "tl_common.h"
#include "drivers.h"


/******************************************** User Interface  ********************************************************************/
ble_sts_t blc_ll_setExtScanParam_1_phy (own_addr_type_t  ownAddrType, 	scan_fp_type_t scanFilterPolicy,	u8	scan_phys,
										scan_type_t 	 scanType,    	u16 		   scan_interval, 		u16 scan_window);

ble_sts_t blc_hci_le_setExtScanEnable (scan_en_t  extScan_en, 	dupFilter_en_t filter_duplicate,	u16 duration,	u16 period);



/****************************************** Stack Interface, user can not use!!! *************************************************/

ble_sts_t blc_hci_le_setExtScanParam (own_addr_type_t  ownAddrType, 	scan_fp_type_t scanFilterPolicy,	u8 scan_phys,	u8 *pData);


#endif /* LL_SCAN_EXT_H_ */
