/********************************************************************************************************
 * @file     smp_peripheral.h 
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
 * smp_peripheral.h
 *
 *  Created on: 2018-12-4
 *      Author: Administrator
 */

#ifndef SMP_PERIPHERAL_H_
#define SMP_PERIPHERAL_H_






// "SecReq" refer to "security request"
typedef enum {
	SecReq_NOT_SEND = 0,   // do not send "security request" after link layer connection established
	SecReq_IMM_SEND = BIT(0),   //"IMM" refer to immediate, send "security request" immediately after link layer connection established
	SecReq_PEND_SEND = BIT(1),  //"PEND" refer to pending,  pending "security request" for some time after link layer connection established, when pending time arrived. send it
}secReq_cfg;



typedef struct{
	u8  secReq_conn;    //BIT<7:4> reConn;  BIT<3:0> newConn;
	u8  secReq_pending;
	u16 pending_ms;
}secReq_ctl_t;

extern _attribute_aligned_(4) secReq_ctl_t	blc_SecReq_ctrl;




/******************************* User Interface  *****************************************/
int 		blc_smp_peripheral_init (void);

void 		blc_smp_configSecurityRequestSending( secReq_cfg newConn_cfg,  secReq_cfg re_conn_cfg, u16 pending_ms);
int 		blc_smp_sendSecurityRequest (void);




/************************* Stack Interface, user can not use!!! ***************************/

int 		blc_smp_peripheral_init (void);



u8 *		bls_smp_pushPkt (int type);
int 		bls_smp_encryption_done(u16 connHandle);
void 		bls_smp_peripheral_paring_loop(void);
paring_sts_t	bls_smp_get_paring_statas(u16 connHandle);

void 		blc_smp_procParingEnd(u8 err_reason);

u8 * 		l2cap_smp_handler(u16 connHandle, u8 * p);




#endif /* SMP_PERIPHERAL_H_ */
