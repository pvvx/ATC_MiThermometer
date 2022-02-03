/********************************************************************************************************
 * @file     ll_ext.h
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
#ifndef LL_EXT_H_
#define LL_EXT_H_

#include "tl_common.h"

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
#include "stack/ble/ll/ll_conn/ll_slave.h"
#include "stack/ble/ll/ll_conn/ll_master.h"

#include <stack/ble/ll/ll_whitelist.h>






#define BLE_EXT_ADV_PROP_CONNECTABLE							(0x0001)
#define BLE_EXT_ADV_PROP_SCANNABLE     							(0x0002)
#define BLE_EXT_ADV_PROP_DIRECTED  							    (0x0004)
#define BLE_EXT_ADV_PROP_HD_DIRECTED     						(0x0008)
#define BLE_EXT_ADV_PROP_LEGACY          						(0x0010)
#define BLE_EXT_ADV_PROP_ANON_ADV       		 				(0x0020)
#define BLE_EXT_ADV_PROP_INC_TX_PWR      						(0x0040)

#define BLE_EXT_ADV_PROP_CONNECTABLE_SCANNABLE					(0x0003)  // BLE_EXT_ADV_PROP_CONNECTABLE | BLE_EXT_ADV_PROP_SCANNABLE
#define BLE_EXT_ADV_PROP_LEGACY_SCANNABLE						(0x0012)  // BLE_EXT_ADV_PROP_LEGACY | BLE_EXT_ADV_PROP_SCANNABLE
#define	BLE_EXT_ADV_PROP_LEGACY_DIRECTED						(0x0014)  // BLE_EXT_ADV_PROP_LEGACY | BLE_EXT_ADV_PROP_DIRECTED
#define	BLE_EXT_ADV_PROP_LEGACY_HD_DIRECTED						(0x0018)  //BLE_EXT_ADV_PROP_LEGACY | BLE_EXT_ADV_PROP_HD_DIRECTED
#define BLE_EXT_ADV_PROP_LEGACY_CONNECTABLE_SCANNABLE			(0x0013)  // BLE_EXT_ADV_PROP_LEGACY | BLE_EXT_ADV_PROP_CONNECTABLE | BLE_EXT_ADV_PROP_SCANNABLE










typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	advA[6];			//address
	u8	data[31];			//0-31 byte
}rf_pkt_pri_scanrsp_t;




typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	data[253];   //Extended Header + AdvData
}rf_pkt_ext_adv_t;
#define MAX_LENGTH_SECOND_ADV_PKT				264   //sizeof(rf_pkt_ext_adv_t) = 261




/******************************************************  ADV_EXT_IND ************************************************************
												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData			Structure
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected without AUX		  	 M		 X		X	   X	 X		X	   X	 X			rf_pkt_adv_ext_ind_1

Non-Connectable
Non-Scannable   Undirected with    AUX			 X 	 	 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Non-Connectable
Non-Scannable	Directed   without AUX			 M		 M		X	   X	 X		X	   X	 X			rf_pkt_adv_ext_ind_1

Non-Connectable
Non-Scannable	Directed   with	   AUX			 X	 	 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2


Connectable 	Undirected					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Connectable 	Directed					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Scannable 		Undirected				     	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2

Scannable 		Directed					 	 X		 X		M	   M	 X		X	   X	 X			rf_pkt_adv_ext_ind_2
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	targetA[6];
}rf_pkt_adv_ext_ind_1;


typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	//ADI
	u16	did	:12;
	u16	sid : 4;

	//AuxPrt
	u8  chn_index    :6;
	u8  ca 		 	:1;
	u8  offset_unit	:1;
	u16 aux_offset  :13;
	u16	aux_phy		:3;

}rf_pkt_adv_ext_ind_2;




/******************************************************  AUX_ADV_IND ************************************************************
												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power
Non-Connectable
Non-Scannable   Undirected with    AUX			 M 	 	 X		M	   O	 X		X	   X	 O			rf_pkt_aux_adv_ind_1

Non-Connectable
Non-Scannable	Directed   with	   AUX			 M	 	 M		M	   O	 X		X	   X	 O			rf_pkt_aux_adv_ind_2

Connectable 	Undirected					 	 M		 X		M	   X	 X		X	   X	 O			rf_pkt_aux_adv_ind_1

Connectable 	Directed					 	 M		 M		M	   X	 X		X	   X	 O			rf_pkt_aux_adv_ind_2

Scannable 		Undirected				     	 M		 X		M	   X	 X		X	   X	 X			rf_pkt_aux_adv_ind_1

Scannable 		Directed					 	 M		 M		M	   X	 X		X	   X	 X			rf_pkt_aux_adv_ind_2
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];

	//ADI
	u16	did	:12;
	u16	sid : 4;

	u8	dat[1];   // AuxPtr/AdvData
}rf_pkt_aux_adv_ind_1;


typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	targetA[6];

	//ADI
	u16	did	:12;
	u16	sid : 4;

	u8	dat[1]; // AuxPtr/AdvData
}rf_pkt_aux_adv_ind_2;





typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];

	//ADI
	u16	did	:12;
	u16	sid : 4;

	//AuxPrt
	u8  chn_index    :6;
	u8  ca 		 	:1;
	u8  offset_unit	:1;
	u16 aux_offset  :13;
	u16	aux_phy		:3;

	u8	dat[1];   // AdvData
}rf_pkt_aux_adv_ind_3;




typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	targetA[6];

	//ADI
	u16	did	:12;
	u16	sid : 4;

	//AuxPrt
	u8  chn_index    :6;
	u8  ca 		 	:1;
	u8  offset_unit	:1;
	u16 aux_offset  :13;
	u16	aux_phy		:3;

	u8	dat[1]; // AdvData
}rf_pkt_aux_adv_ind_4;



/******************************************************  AUX_SCAN_RSP ***********************************************************

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

								  	  	  	  	  M		 X		X	   O	 X		X	   X	 M
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	dat[1];   // AuxPtr/AdvData
}rf_pkt_aux_scan_rsp_t;

#define MAX_ADVDATA_NUM_AUX_SCANRSP									247   // 255 - 8( Extended Header Length(1) + Extended Header Flag(1) + AdvA(6) ) = 247




/******************************************************  AUX_CHAIN_IND ***********************************************************

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

		Chained data:							  X		 X		C3	   O	 X		O	   X	 O

														    			||
																		||
														   	   	   	   \||/
																		\/

Chained data after AUX_ADV_IND  :				 X		 X		M	   O	 X		X	   X	 O			rf_pkt_aux_chain_ind_1

Chained data after AUX_SCAN_RSP :				 X		 X		X	   O	 X		X	   X	 O			rf_pkt_aux_chain_ind_2
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;


	//ADI
	u16	did	:12;
	u16	sid : 4;

	u8	dat[1]; // AuxPrt/AdvData
}rf_pkt_aux_chain_ind_1;

#define MAX_ADVDATA_NUM_AUX_CHAIN_IND_1									251   // 255 - 4( Extended Header Length(1) + Extended Header Flag(1) + ADI(2) ) = 251








/******************************************************  AUX_CONNECT_RSP *********************************************************

												AdvA  TargetA  ADI	  Aux	Sync	Tx   ACAD	AdvData
																  	  Ptr  	Info   Power

								  	  	  	  	  M		 M		X	   X	 X		X	   X	 X
*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	ext_hdr_len		:6;
	u8	adv_mode		:2;

	u8	ext_hdr_flg;

	u8	advA[6];
	u8	targetA[6];
}rf_pkt_aux_conn_rsp_t;




/****************************************************  SCAN_REQ / AUX_SCAN_REQ **************************************************

*********************************************************************************************************************************/
typedef struct{
	u32 dma_len;

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;

	u8	scanA[6];
	u8	advA[6];
}rf_pkt_ext_scan_req_t;


typedef struct {



} st_ll_ext_t;

extern st_ll_ext_t  bltExtPara;




/********************* Macro & Enumeration variables for Stack, user can not use!!!!  **********************/
#define BLE_LL_EXT_ADV_ADVA_BIT         						BIT(0)
#define BLE_LL_EXT_ADV_TARGETA_BIT      						BIT(1)
#define BLE_LL_EXT_ADV_RFU_BIT          						BIT(2)
#define BLE_LL_EXT_ADV_DATA_INFO_BIT    						BIT(3)
#define BLE_LL_EXT_ADV_AUX_PTR_BIT      						BIT(4)
#define BLE_LL_EXT_ADV_SYNC_INFO_BIT    						BIT(5)
#define BLE_LL_EXT_ADV_TX_POWER_BIT     						BIT(6)



#define BLE_LL_EXT_ADV_MODE_NON_CONN_NON_SCAN    				(0x00)
#define BLE_LL_EXT_ADV_MODE_CONN        						(0x01)		//connectable, none_scannable
#define BLE_LL_EXT_ADV_MODE_SCAN        						(0x02)      //scannable,   none_connectable




#define	EXT_ADV_PDU_AUXPTR_OFFSET_UNITS_30_US					0
#define	EXT_ADV_PDU_AUXPTR_OFFSET_UNITS_300_US					1

#define	EXT_ADV_PDU_AUXPTR_CA_51_500_PPM						0
#define	EXT_ADV_PDU_AUXPTR_CA_0_50_PPM							1




#define TLK_T_MAFS													302  // T_MAFS >= 300 uS, add some margin
#define TLK_T_MAFS_30US_NUM											10

#define	TX_TX_DELAY_US												120 // TX settle(74 uS) + 5  preamble(40uS) + some other baseband cost,   and be 30*N





















/******************************* Macro & Enumeration variables for User ************************************/
















/************************************ User Interface  ******************************************************/






/*********************************** Stack Interface, user can not use!!! **********************************/










#endif /* LL_EXT_H_ */
