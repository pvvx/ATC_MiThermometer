/********************************************************************************************************
 * @file     smp.h 
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
 * ble_smp.h
 *
 *  Created on: 2016-9-27
 *      Author: Telink
 */

#ifndef BLE_SMP_H_
#define BLE_SMP_H_

#include "stack/ble/ble_common.h"
#include "smp_const.h"


//SMP TODO list:
//1. SMP timeout process




#define SMP_INFO_STORAGE_IN_FLASH			1
#define SMP_INFO_STORAGE_IN_EEPROM			2
#define SMP_INFO_STORAGE_OTHER_MCU	 		3



//define smp database(keys/peer device informations ...)
#ifndef SMP_DATABASE_INFO_SOURCE
#define SMP_DATABASE_INFO_SOURCE					SMP_INFO_STORAGE_IN_FLASH
#endif





#define SMP_STANDARD_PAIR   	0
#define SMP_FAST_CONNECT   		1



#define  ADDR_NOT_BONDED	0xFF
#define  ADDR_NEW_BONDED	0xFE
#define  ADDR_DELETE_BOND	0xFD

#define  KEY_FLAG_IDLE		0xFF
#define  KEY_FLAG_NEW		0xFE
#define  KEY_FLAG_FAIL		0xFD


#define		LL_CONNECTION_MAX						1  //controller determine this value

#if (LL_MULTI_MASTER_SINGLE_SLAVE_ENABLE)
	#define 	SMP_BONDING_DEVICE_MAX_NUM				12
#else
	#define 	SMP_BONDING_DEVICE_MAX_NUM				4
#endif

typedef enum {
	non_debug_mode = 0,  // ECDH distribute private/public key pairs
	debug_mode = 1,      // ECDH use debug mode private/public key pairs
} ecdh_keys_mode_t;


typedef struct {

	u8 paring_status;  //if multiConnection, define as paring_status[connMaxNum]
	u8 addrIndex;
	u8 keyIndex;
	u8 cur_bondNum;

#if (SMP_DATABASE_INFO_SOURCE == SMP_INFO_STORAGE_IN_FLASH)
	u32 bond_flash_idx[SMP_BONDING_DEVICE_MAX_NUM];  //mark paired slave mac address in flash
	u8	bond_flag[SMP_BONDING_DEVICE_MAX_NUM];
	u32 index_update_method;
#else
	//may be sram address if use other mcu store smp info
#endif

} bond_device_t;

extern _attribute_aligned_(4) bond_device_t  tbl_bondDevice;





typedef struct {
	u8 security_level;
	u8 bonding_maxNum;
	u8 bonding_mode;
	u8 IO_capability;

	u32 ecdh_debug_mode;//1:debug_mode,0:ecdh public/private key pairs distribute


	u8 MITM_protetion;
	u8 oob_enable;
	u8 paring_method;  //paring_method
	u8 keyPress_en;



	u8 secure_conn; // final paring method is paring_method or not
	u8 stk_method;  // final available stk generate method
	u8 tk_status;
	u8 peerKey_mask;    //determine which peer key to store   <0>: EncKey  <1>:IdKey   <2>:Sign, NOTE: not used now in code

	u8 bonding_enable;
	u8 paring_busy;
	u8 key_distribute;
	u8 save_key_flag;

} smp_mng_t;

extern _attribute_aligned_(4) smp_mng_t  blc_smpMng;




/*
 * smp parameter about peer device.
 * */
typedef struct{
	u8		rsvd;
	u8 		peer_addr_type;  //address used in link layer connection
	u8		peer_addr[6];

	u8		peer_key_size;   // bond and key_size
	u8		peer_id_address_type;  //peer identity address information in key distribution, used to identify
	u8		peer_id_address[6];

	u8		paring_peer_rand[16];
	u8		paring_confirm[16];  // in security connection oob mode to keep peer random

	u8		peer_csrk[16];
	u8		peer_irk[16];
}smp_param_peer_t;

extern _attribute_aligned_(4) smp_param_peer_t   	smp_param_peer;


/*
 * smp parameter about own device.
 * */
typedef struct{
	u8						paring_tk[16];   // in security connection to keep own random
	u8						own_ltk[16];   //used for generate ediv and random


	smp_paring_req_rsp_t  	paring_req;
	smp_paring_req_rsp_t  	paring_rsp;
	smp_authReq_t			auth_req;
	u8						own_conn_type;  //current connection peer own type

	u16						rsvd;
	u8						own_conn_addr[6];


}smp_param_own_t;

extern _attribute_aligned_(4) smp_param_own_t		smp_param_own;






typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 data[21];
}smp2llcap_type_t;

typedef int (*smp_finish_callback_t)(void);
void blm_smp_registerSmpFinishCb (smp_finish_callback_t cb);

extern u8 smpPkShftCnt;

extern u32 smp_timeout_start_tick;
extern u8 smpDistirbuteKeyOrder;
extern smp_keyDistribution_t smp_DistributeKeyInit ;
extern smp_keyDistribution_t smp_DistributeKeyResp ;

extern smp_finish_callback_t  blc_smpFinishCb;
extern smp2llcap_type_t smpResSignalPkt;
extern u8 smpOwnRand_const[16];
extern u8 smpParingOwnIRK[16];
extern u8 smpParingOwnCSRK[16];
extern u8 smpOwnParingConfirm[16];

typedef void (*smp_check_handler_t)(u32);
typedef int (*smp_init_handler_t)(u16 conn, u8 *p);
typedef u8 * (*smp_info_handler_t)(void);
typedef void (*smp_bond_clean_handler_t)(void);
typedef void (*smp_trigger_cb_t)(u16 conn_handle);


#if (SECURE_CONNECTION_ENABLE)
typedef u8 * (*smp_sc_cmd_handler_t)(u16 conn, u8*p);
typedef void (*smp_sc_pushPkt_handler_t)(int type);
extern const u8 PublicKey[64];
extern const u8 PrivateKey[32];
extern _attribute_aligned_(4) smp_sc_key_t 			smp_sc_key;
extern smp_sc_cmd_handler_t		func_smp_sc_proc;
extern smp_sc_pushPkt_handler_t func_smp_sc_pushPkt_proc;
#endif


extern smp_check_handler_t		func_smp_check; //HID on android 7.0
extern smp_init_handler_t		func_smp_init;
extern smp_info_handler_t		func_smp_info;







/******************************* User Interface  *****************************************/
void 		blc_smp_param_setBondingDeviceMaxNumber ( int device_num);

void 		blc_smp_setSecurityLevel(le_security_mode_level_t  mode_level);





void 		blc_smp_setParingMethods (paring_methods_t  method);    //select paring methods

void 		blc_smp_setSecurityParamters (  bonding_mode_t 		mode,
											int 				MITM_en,
											int 				OOB_en,
											int				    keyPress_en,
											io_capability_t 	ioCapablility);

void 		blc_smp_setBondingMode(bonding_mode_t mode);			   //set bonding_mode
void 		blc_smp_enableAuthMITM (int MITM_en); 					   //enable authentication MITM protection
void 		blc_smp_enableOobAuthentication (int OOB_en);    		   //enable OOB authentication
void 		blc_smp_setIoCapability (io_capability_t  ioCapablility);  //set IO capability for passKey Entry/Numeric Comparison
void 		blc_smp_enableKeypress (int keyPress_en);
void 		blc_smp_setEcdhDebugMode(ecdh_keys_mode_t mode);

void 		blc_smp_setTK_by_OOB (u8 *oobData);  //used for set oob data
void 		blc_smp_setTK_by_PasskeyEntry (u32 pinCodeInput);
void		blc_smp_setNumericComparisonResult(bool YES_or_NO);   //numeric compare confirm, 1: YES  0: NO







/************************* Stack Interface, user can not use!!! ***************************/
u8* 	blc_smp_pushParingFailed(u8 failReason);

void 	blc_smp_generate_initial_keys(void);

int 	blc_smp_getGenMethod (int SC_en);    //Return STK generate method.

int 	blc_smp_param_init (void);



static inline int  blc_smp_isParingBusy(void)
{
	return blc_smpMng.paring_busy;
}

static inline  void  blc_smp_setParingBusy(u8 busy)
{
	blc_smpMng.paring_busy = busy;
}

static inline void blc_smp_setCertTimeoutTick (u32 t)
{
	smp_timeout_start_tick = t;
}

smp_keyDistribution_t blc_smp_setInitiatorKey (u8 LTK_distributeEn, u8 IRK_distributeEn, u8 CSRK_DistributeEn);
smp_keyDistribution_t blc_smp_setResponderKey (u8 LTK_distributeEn, u8 IRK_distributeEn, u8 CSRK_DistributeEn);


int 		blc_stack_smp_setSecurityParamters (  bonding_mode_t 		mode,
											paring_methods_t 	method,
											int 				MITM_en,
											int 				OOB_en,
											io_capability_t 	ioCapablility,
											int				    keyPress_en);

void 		blc_stack_smp_setBondingMode(bonding_mode_t mode);
void 		blc_stack_smp_setParingMethods (paring_methods_t  method);
void 		blc_stack_smp_enableAuthMITM (int MITM_en);
void 		blc_stack_smp_enableOobAuthentication (int OOB_en);
void 		blc_stack_smp_setIoCapability (io_capability_t  ioCapablility);
void 		blc_stack_smp_enableKeypress (int keyPress_en);

u8* 		blc_smp_sc_handler(u16 conn, u8*p);
void 		blc_smp_sc_pushPkt_handler(int type);






















#endif /* BLE_SMP_H_ */
