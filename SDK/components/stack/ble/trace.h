/********************************************************************************************************
 * @file     trace.h 
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

#ifndef TRACE_H_
#define TRACE_H_

#define TR_T_irq					0
#define TR_T_irq_rx					1
#define	TR_T_irq_tx					2
#define TR_T_irq_sysTimer			3

#define TR_T_irq_cmddone			4
#define TR_T_irq_rxTmt				5
#define TR_T_irq_rxFirstTmt			6
#define TR_T_irq_fsmTmt				7

#define TR_T_irq_rxCrcErr			8

#define TR_T_ll_brx_start	        9
#define TR_T_ll_brx_end	            10

#define TR_T_ll_durationInit		11
#define TR_T_ll_durationUpdate		12

#define TR_T_ll_1stRx				13
#define TR_T_ll_chn_map   	     	14
#define TR_T_ll_chn_timing      	15
#define TR_T_ll_scanRsp            	16
#define TR_T_ll_connReq	            17

#define TR_T_ll_sync            	18
#define TR_T_ll_adv	            	19

#define TR_T_ll_stopRf				20


#define TR_T_audioTask		        24
#define TR_T_audioData		        25
#define TR_T_adpcm		        	26
#define TR_T_adpcm_enc_overflow    	27



#define TR_T_master_irq				30
#define TR_T_master_rx				31
#define TR_T_master_tx				32
#define TR_T_master_sysTimer		33
#define TR_T_master_connDone		34

#define TR_T_master_adv_report		35

#define TR_T_master_createConn		40
#define TR_T_master_init			41
#define TR_T_master_connnect		42
#define TR_T_master_terminate		43
#define TR_T_master_btx				44
#define TR_T_master_scan			45

#define TR_T_master_update			50
#define TR_T_master_update_req		51




#define TR_24_ll_chn            	0
#define TR_24_ll_duration			1



#define TR_24_slot_idx				5
#define TR_24_scan_chn				6
#define TR_24_master_state			7


#endif
