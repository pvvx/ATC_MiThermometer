/********************************************************************************************************
 * @file     gap_event.h 
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
 * gap_event.h
 *
 *  Created on: 2018-12-5
 *      Author: Administrator
 */

#ifndef GAP_EVENT_H_
#define GAP_EVENT_H_



#define GAP_EVT_SMP_PARING_BEAGIN		                             0
#define GAP_EVT_SMP_PARING_SUCCESS			                         1
#define GAP_EVT_SMP_PARING_FAIL			                             2
#define GAP_EVT_SMP_CONN_ENCRYPTION_DONE							 3
#define GAP_EVT_SMP_TK_DISPALY			                             4
#define GAP_EVT_SMP_TK_REQUEST_PASSKEY								 5
#define GAP_EVT_SMP_TK_REQUEST_OOB									 6
#define GAP_EVT_SMP_TK_NUMERIC_COMPARE								 7


#define GAP_EVT_ATT_EXCHANGE_MTU									 16
#define GAP_EVT_GATT_HANDLE_VLAUE_CONFIRM							 17




#define GAP_EVT_MASK_NONE                                        	 0x00000000
#define GAP_EVT_MASK_SMP_PARING_BEAGIN                          	 (1<<GAP_EVT_SMP_PARING_BEAGIN)
#define GAP_EVT_MASK_SMP_PARING_SUCCESS                           	 (1<<GAP_EVT_SMP_PARING_SUCCESS)
#define GAP_EVT_MASK_SMP_PARING_FAIL                           		 (1<<GAP_EVT_SMP_PARING_FAIL)
#define GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE                     	 (1<<GAP_EVT_SMP_CONN_ENCRYPTION_DONE)
#define GAP_EVT_MASK_SMP_TK_DISPALY                  				 (1<<GAP_EVT_SMP_TK_DISPALY)
#define GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY                  		 (1<<GAP_EVT_SMP_TK_REQUEST_PASSKEY)
#define GAP_EVT_MASK_SMP_TK_REQUEST_OOB	                     		 (1<<GAP_EVT_SMP_TK_REQUEST_OOB)
#define GAP_EVT_MASK_SMP_TK_NUMERIC_COMPARE                     	 (1<<GAP_EVT_SMP_TK_NUMERIC_COMPARE)

#define GAP_EVT_MASK_ATT_EXCHANGE_MTU                     	 		 (1<<GAP_EVT_ATT_EXCHANGE_MTU)
#define GAP_EVT_MASK_GATT_HANDLE_VLAUE_CONFIRM                     	 (1<<GAP_EVT_GATT_HANDLE_VLAUE_CONFIRM)


#define GAP_EVT_MASK_DEFAULT										( GAP_EVT_MASK_SMP_TK_DISPALY 			|| \
																	  GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY   || \
																	  GAP_EVT_MASK_SMP_TK_REQUEST_OOB		|| \
																	  GAP_EVT_MASK_ATT_EXCHANGE_MTU )





typedef struct {
	u16	connHandle;
	u8	secure_conn;
	u8	tk_method;
} gap_smp_paringBeginEvt_t;


typedef struct {
	u16	connHandle;
	u8	bonding;
	u8	bonding_result;
} gap_smp_paringSuccessEvt_t;

typedef struct {
	u16	connHandle;
	u8  reason;
} gap_smp_paringFailEvt_t;

typedef struct {
	u16	connHandle;
	u8	re_connect;   //1: re_connect, encrypt with previous distributed LTK;   0: paring , encrypt with STK
} gap_smp_connEncDoneEvt_t;





typedef struct {
	u16	connHandle;
	u32	tk_pincode;
} gap_smp_TkDisplayEvt_t;

typedef struct {
	u16	connHandle;
} gap_smp_TkReqPassKeyEvt_t;

typedef struct {
	u16	connHandle;
	u16	peer_MTU;
	u16	effective_MTU;
} gap_gatt_mtuSizeExchangeEvt_t;




extern u32		gap_eventMask;



typedef int (*gap_event_handler_t) (u32 h, u8 *para, int n);


/******************************* User Interface  ******************************************/
void 	blc_gap_setEventMask(u32 evtMask);

void 	blc_gap_registerHostEventHandler (gap_event_handler_t  handler);







/************************* Stack Interface, user can not use!!! ***************************/
int blc_gap_send_event (u32 h, u8 *para, int n);


#endif /* GAP_EVENT_H_ */
