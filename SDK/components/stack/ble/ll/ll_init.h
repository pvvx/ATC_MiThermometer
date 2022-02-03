/********************************************************************************************************
 * @file     ll_init.h 
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
 * ll_init.h
 *
 *  Created on: 2017-3-9
 *      Author: Administrator
 */

#ifndef LL_INIT_H_
#define LL_INIT_H_



typedef struct {
	u8		init_en;
	u8		conn_policy;
	u8		conn_advType;
	u8		conn_established;

	u32		create_conn_startTick;

	u16     rsvd;
	u8		conn_mac[6];

}st_ll_init_t;

//st_ll_init_t  blti;

extern u32 blm_timeout_connectDevice;



extern rf_packet_ll_init_t	pkt_init;




typedef int  (*ll_module_init_callback_t)(u8 *, u32);





static inline u32 rf_get_ble_access_code_16to32 (u16 code)
{
	u32 r = 0;
	int i;
	for (i=0; i<4; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}

	for (i=4; i<8; i++) {  //no more 24 transitions
		r = r << 4;
		r |= code & BIT(i) ? 0x0c : 0x03;
	}

	for (i=12; i<16; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}

	return r;
}




/******************************* User Interface  ************************************/
void	 	blc_ll_initInitiating_module(void);


ble_sts_t 	blc_ll_createConnection (u16 scan_interval, u16 scan_window, init_fp_type_t initiator_filter_policy,
							  u8 adr_type, u8 *mac, u8 own_adr_type,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );

ble_sts_t 	blc_ll_createConnectionCancel (void);


ble_sts_t   blc_ll_setCreateConnectionTimeout (u32 timeout_ms);


/************************* Stack Interface, user can not use!!! ***************************/
ble_sts_t 	blc_ll_setInitEnable (u8 en);


#endif /* LL_INIT_H_ */
