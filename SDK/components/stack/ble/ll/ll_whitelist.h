/********************************************************************************************************
 * @file     ll_whitelist.h 
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
 * ll_whitelist.h
 *
 *  Created on: 2016-9-22
 *      Author: Administrator
 */

#ifndef LL_WHITELIST_H_
#define LL_WHITELIST_H_

#include <stack/ble/ble_common.h>





#define  	MAX_WHITE_LIST_SIZE    				4

#if (RAMCODE_OPTIMIZE_CONN_POWER_NEGLECT_ENABLE || BLS_BLE_RF_IRQ_TIMING_EXTREMELY_SHORT_EN)
	#define 	MAX_WHITE_IRK_LIST_SIZE          	1   //save ramcode
#else
	#define 	MAX_WHITE_IRK_LIST_SIZE          	2   //save ramcode
#endif


#define 	IRK_REVERT_TO_SAVE_AES_TMIE_ENABLE		1


#define		MAC_MATCH8(md,ms)	(md[0]==ms[0] && md[1]==ms[1] && md[2]==ms[2] && md[3]==ms[3] && md[4]==ms[4] && md[5]==ms[5])
#define		MAC_MATCH16(md,ms)	(md[0]==ms[0] && md[1]==ms[1] && md[2]==ms[2])
#define		MAC_MATCH32(md,ms)	(md[0]==ms[0] && md[1]==ms[1])


//adv filter policy
#define 	ALLOW_SCAN_WL								BIT(0)
#define 	ALLOW_CONN_WL								BIT(1)


typedef enum {
	ADV_FP_ALLOW_SCAN_ANY_ALLOW_CONN_ANY        =		0x00,  // Process scan and connection requests from all devices
	ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_ANY         =		0x01,  // Process connection requests from all devices and only scan requests from devices that are in the White List.
	ADV_FP_ALLOW_SCAN_ANY_ALLOW_CONN_WL         =		0x02,  // Process scan requests from all devices and only connection requests from devices that are in the White List..
	ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL          =		0x03,  // Process scan and connection requests only from devices in the White List.

	ADV_FP_NONE									=		ADV_FP_ALLOW_SCAN_ANY_ALLOW_CONN_ANY, //adv filter policy set to zero, not use whitelist
} adv_fp_type_t;  //adv_filterPolicy_type_t


typedef enum {
	SCAN_FP_ALLOW_ADV_ANY						=		0x00,  //except direct adv address not match
	SCAN_FP_ALLOW_ADV_WL        				=		0x01,  //except direct adv address not match
	SCAN_FP_ALLOW_UNDIRECT_ADV      			=		0x02,  //and direct adv address match initiator's resolvable private MAC
	SCAN_FP_ALLOW_ADV_WL_DIRECT_ADV_MACTH		=		0x03,  //and direct adv address match initiator's resolvable private MAC

} scan_fp_type_t;   //scan_filterPolicy_type_t



typedef enum {
	INITIATE_FP_ADV_SPECIFY        				=		0x00,  //connect ADV specified by host
	INITIATE_FP_ADV_WL         					=		0x01,  //connect ADV in whiteList

} init_fp_type_t;   //init_filterPolicy_type_t





typedef u8 irk_key_t[16];

typedef struct {
	u8 type;
	u8 address[BLE_ADDR_LEN];
	u8 reserved;
} wl_addr_t;

typedef struct {
	wl_addr_t  wl_addr_tbl[MAX_WHITE_LIST_SIZE];
	u8 	wl_addr_tbl_index;
	u8 	wl_irk_tbl_index;
} ll_whiteListTbl_t;


typedef struct {
	u8 type;
	u8 address[BLE_ADDR_LEN];
	u8 reserved;
	u8 irk[16];
} rl_addr_t;

typedef struct {
	rl_addr_t	tbl[MAX_WHITE_IRK_LIST_SIZE];
	u8 			idx;
	u8			en;
} ll_ResolvingListTbl_t;





/**************************************** User Interface  **********************************************/


/*********************************************************************
 * @fn      ll_whiteList_reset
 *
 * @brief   API to reset the white list table.
 *
 * @param   None
 *
 * @return  LL Status
 */
ble_sts_t ll_whiteList_reset(void);

/*********************************************************************
 * @fn      ll_whiteList_add
 *
 * @brief   API to add new entry to white list
 *
 * @param   None
 *
 * @return  LL Status
 */
ble_sts_t ll_whiteList_add(u8 type, u8 *addr);

/*********************************************************************
 * @fn      ll_whiteList_delete
 *
 * @brief   API to delete entry from white list
 *
 * @param   type - The specified type
 *          addr - The specified address to be delete
 *
 * @return  LL Status
 */
ble_sts_t ll_whiteList_delete(u8 type, u8 *addr);

/*********************************************************************
 * @fn      ll_whiteList_getSize
 *
 * @brief   API to get total number of white list entry size
 *
 * @param   returnSize - The returned entry size
 *
 * @return  LL Status
 */
ble_sts_t ll_whiteList_getSize(u8 *returnPublicAddrListSize) ;






ble_sts_t  ll_resolvingList_add(u8 peerIdAddrType, u8 *peerIdAddr, u8 *peer_irk, u8 *local_irk);
ble_sts_t  ll_resolvingList_delete(u8 peerIdAddrType, u8 *peerIdAddr);

ble_sts_t  ll_resolvingList_reset(void);
ble_sts_t  ll_resolvingList_getSize(u8 *Size);

ble_sts_t  ll_resolvingList_getPeerResolvableAddr (u8 peerIdAddrType, u8* peerIdAddr, u8* peerResolvableAddr); //not available now
ble_sts_t  ll_resolvingList_getLocalResolvableAddr(u8 peerIdAddrType, u8* peerIdAddr, u8* LocalResolvableAddr); //not available now

ble_sts_t  ll_resolvingList_setAddrResolutionEnable (u8 resolutionEn);

ble_sts_t  ll_resolvingList_setResolvablePrivateAddrTimer (u16 timeout_s);   //not available now






/********************************* Stack Interface, user can not use!!! ********************************/

u8 * ll_searchAddrInWhiteListTbl(u8 type, u8 *addr);

u8 * ll_searchAddrInResolvingListTbl(u8 *addr);  //addr must be RPA

u8 * ll_searchAddr_in_WhiteList_and_ResolvingList(u8 type, u8 *addr);

bool smp_quickResolvPrivateAddr(u8 *key, u8 *addr);


ll_whiteListTbl_t	ll_whiteList_tbl;
ll_ResolvingListTbl_t	ll_resolvingList_tbl;



#endif /* LL_WHITELIST_H_ */
