/********************************************************************************************************
 * @file     ll_ext_adv.h
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

#ifndef LL_ADV_EXT_H_
#define LL_ADV_EXT_H_


#include <stack/ble/ble_common.h>
#include <stack/ble/blt_config.h>
#include <stack/ble/hci/hci_const.h>
#include <stack/ble/hci/hci_event.h>
#include <stack/ble/hci/hci_cmd.h>
#include <stack/ble/ll/ll_ext.h>
#include <stack/ble/ll/ll_adv.h>
#include <stack/ble/ll/ll_whitelist.h>
#include <stack/ble/phy/phy.h>

#include "tl_common.h"




#define	INVALID_ADVHD_FLAG										0xFF


#define AUX_ADV_FEA			BIT(0)
#define PER_ADV_FEA			BIT(1)




/* Advertising_Handle */
#define ADV_HANDLE0												0x00
#define ADV_HANDLE1												0x01
#define ADV_HANDLE2												0x02
#define ADV_HANDLE3												0x03




/* Advertising_Handle */
#define ADV_SID_0												0x00
#define ADV_SID_1												0x01
#define ADV_SID_2												0x02
#define ADV_SID_3												0x03



/* Advertising Event Properties
 * See the Core_v5.0(Vol 2/Part E/7.8.53 & Vol 6/Part B/4.4.2/Table 4.1) for more information
*/
typedef enum{
  ADV_EVT_PROP_LEGACY_CONNECTABLE_SCANNABLE_UNDIRECTED 				       	= 0x0013,		//  0001 0011'b 	ADV_IND
  ADV_EVT_PROP_LEGACY_CONNECTABLE_DIRECTED_LOW_DUTY 				       	= 0x0015,		//  0001 0101'b		ADV_DIRECT_IND(low duty cycle)
  ADV_EVT_PROP_LEGACY_CONNECTABLE_DIRECTED_HIGH_DUTY 	 				    = 0x001D,		//  0001 1101'b		ADV_DIRECT_IND(high duty cycle)
  ADV_EVT_PROP_LEGACY_SCANNABLE_UNDIRECTED 							       	= 0x0012,		//  0001 0010'b		ADV_SCAN_IND
  ADV_EVT_PROP_LEGACY_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED				= 0x0010,		//  0001 0000'b		ADV_NONCONN_IND


  ADV_EVT_PROP_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED    	    = 0x0000,		//  0000 0000'b		ADV_EXT_IND + AUX_ADV_IND/AUX_CHAIN_IND
  ADV_EVT_PROP_EXTENDED_CONNECTABLE_UNDIRECTED       				 	  	= 0x0001,		//  0000 0001'b		ADV_EXT_IND + AUX_ADV_IND/AUX_CHAIN_IND
  ADV_EVT_PROP_EXTENDED_SCANNABLE_UNDIRECTED						        = 0x0002,		//  0000 0010'b		ADV_EXT_IND + AUX_ADV_IND/AUX_CHAIN_IND
  ADV_EVT_PROP_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED				= 0x0004,		//  0000 0100'b		ADV_EXT_IND + AUX_ADV_IND/AUX_CHAIN_IND
  ADV_EVT_PROP_EXTENDED_CONNECTABLE_DIRECTED			       				= 0x0005,		//  0000 0101'b		ADV_EXT_IND + AUX_ADV_IND/AUX_CHAIN_IND
  ADV_EVT_PROP_EXTENDED_SCANNABLE_DIRECTED								    = 0x0006,		//  0000 0110'b		ADV_EXT_IND + AUX_ADV_IND/AUX_CHAIN_IND


  ADV_EVT_PROP_EXTENDED_MASK_ANONYMOUS_ADV 		= 0x0020,  //if this mask on(only extended ADV event can mask it), anonymous advertising
  ADV_EVT_PROP_EXTENDED_MASK_TX_POWER_INCLUDE	= 0x0040,  //if this mask on(only extended ADV event can mask it), TX power include

}adv_event_prop_t;










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
}rf_pkt_pri_adv_t;
#define MAX_LENGTH_PRIMARY_ADV_PKT				44   //sizeof(rf_pkt_pri_adv_t) = 43






typedef struct{
	u8 chn_ind		:6;
	u8 ca			:1;
	u8 off_unit		:1;
	u16 aux_off		:13;
	u16 aux_phy		:3;
} aux_ptr_t;







//NOTE: this data structure must 4 bytes aligned
typedef struct
{
    u8		adv_handle;
    u8 		extAdv_en;
    u8 		adv_chn_mask;
    u8		adv_chn_num;

	u8 		own_addr_type;
	u8 		peer_addr_type;
    u8 		pri_phy;
    u8 		sec_phy;


    u8 		max_ext_adv_evt;
    u8 		run_ext_adv_evt;
    u8		unfinish_advData;
    u8		unfinish_scanRsp;


	u8		adv_filterPolicy;
    u8 		scan_req_noti_en;
    u8 		coding_ind;					//s2 or s8
    u8		param_update_flag;


	u8		with_aux_adv_ind;   //ADV_EXT_IND  with AUX_ADV_IND
	u8		with_aux_chain_ind;
	u8 		rand_adr_flg;
    u8 		adv_sid;


    // u8 		s_adv_max_skip;


	u16     adv_did; 	// BIT<11:0>
	u16 	evt_props;
	u16		advInt_use;
	u16		send_dataLen;
    u16 	maxLen_advData;			//for each ADV sets, this value can be different to save SRAM
    u16 	curLen_advData;
    u16 	maxLen_scanRsp;			//for each ADV sets, this value can be different to save SRAM
    u16 	curLen_scanRsp;

    u16		send_dataLenBackup;
    u16		rsvd_16_1;


	u32 	adv_duration_tick;
	u32 	adv_begin_tick;				//24
    u32		adv_event_tick;

	u8*		dat_extAdv;
	u8*		dat_scanRsp;                //Scan response data.
	rf_pkt_pri_adv_t*		primary_adv;
	rf_pkt_ext_adv_t*		secondary_adv;

	u8 		rand_adr[6];
	u8 		peer_addr[6];
}ll_ext_adv_t;


#define ADV_SET_PARAM_LENGTH				(sizeof(ll_ext_adv_t))   //sizeof(ll_ext_adv_t) =  ,  must 4 byte aligned




#if 0
//	u16 	advInt_min;
//	u16 	advInt_max;
	u8 		periodicAdv_en;
	u16 	maxLen_periodicAdv;
	u16 	curLen_periodicAdv;
	u8*		dat_periodicAdv;
    u8 adv_secondary_chan;
    s8 adv_tx_pow;
#endif






typedef struct
{
    u8 		maxNum_advSets;
    u8		useNum_advSets;
    u8		last_advSet;
    u8		last_advHand;


    u8		T_SCAN_RSP_INTVL;
    u8 		custom_aux_chn;
    u8 		T_AUX_RSP_INTVL; //settle aux_scan_rsp/aux_conn_rsp's IFS 150s
    u8 		rsvd3;

    u32		rand_delay;

}ll_adv_mng_t;












/******************************************** User Interface  ********************************************************************/
//initialization
void 		blc_ll_initExtendedAdvertising_module(	u8 *pAdvCtrl, u8 *pPriAdv,int num_sets);

void 		blc_ll_initExtSecondaryAdvPacketBuffer(u8 *pSecAdv, int sec_adv_buf_len);

void 		blc_ll_initExtAdvDataBuffer(u8 *pExtAdvData, int max_len_advData);								//set AdvData buffer for all adv_set
void 		blc_ll_initExtAdvDataBuffer_by_advHandle(u8 *pExtAdvData, u8 advHandle, int max_len_advData);  //set AdvData buffer for specific adv_set

void 		blc_ll_initExtScanRspDataBuffer(u8 *pScanRspData, int max_len_scanRspData);
void 		blc_ll_initExtScanRspDataBuffer_by_advHandle(u8 *pScanRspData,  u8 advHandle, int max_len_scanRspData);



//Set Extended ADV parameters
ble_sts_t	blc_ll_setAdvRandomAddr(u8 advHandle, u8* rand_addr);


ble_sts_t 	blc_ll_setExtAdvParam(  u8 advHandle, 		 			adv_event_prop_t adv_evt_prop, u32 pri_advIntervalMin, 		u32 pri_advIntervalMax,
									u8 pri_advChnMap,	 			own_addr_type_t ownAddrType, 	u8 peerAddrType, 			u8  *peerAddr,
									adv_fp_type_t advFilterPolicy,  tx_power_t adv_tx_pow,			le_phy_type_t pri_adv_phy, 	u8 sec_adv_max_skip,
									le_phy_type_t sec_adv_phy, 	 	u8 adv_sid, 					u8 scan_req_noti_en);
ble_sts_t	blc_ll_setExtAdvData	(u8 advHandle, data_oper_t operation, data_fragm_t fragment_prefer, u8 adv_dataLen, 	u8 *advdata);
ble_sts_t 	blc_ll_setExtScanRspData(u8 advHandle, data_oper_t operation, data_fragm_t fragment_prefer, u8 scanRsp_dataLen, u8 *scanRspData);
ble_sts_t 	blc_ll_setExtAdvEnable_1(u32 extAdv_en, u8 sets_num, u8 advHandle, 	 u16 duration, 	  u8 max_extAdvEvt);
ble_sts_t	blc_ll_setExtAdvEnable_n(u32 extAdv_en, u8 sets_num, u8 *pData);


ble_sts_t	blc_ll_removeAdvSet(u8 advHandle);
ble_sts_t	blc_ll_clearAdvSets(void);


// if Coded PHY is used, this API set default S2/S8 mode for Extended ADV
void		blc_ll_setDefaultExtAdvCodingIndication(u8 advHandle, le_ci_prefer_t prefer_CI);


void        blc_ll_setAuxAdvChnIdxByCustomers(u8 aux_chn);
void		blc_ll_setMaxAdvDelay_for_AdvEvent(u8 max_delay_ms);    //unit: mS, only 8/4/2/1/0  available


/****************************************** Stack Interface, user can not use!!! *************************************************/
ble_sts_t 	blc_hci_le_setExtAdvParam( hci_le_setExtAdvParam_cmdParam_t *para, u8 *pTxPower);
ble_sts_t 	blc_hci_le_setExtAdvEnable(u8 extAdv_en, u8 sets_num, u8 *pData);

u16 	  	blc_ll_readMaxAdvDataLength(void);
u8		  	blc_ll_readNumberOfSupportedAdvSets(void);


int  		blt_ext_adv_proc(void);
int  		blt_send_adv2(void);
int 		blt_send_legacy_adv(void);
int 		blt_send_extend_adv(void);
void 		blt_send_extend_no_aux_adv(void);
int 		blt_send_aux_adv(void);
ble_sts_t	blt_ll_clearAdvSets(void);
void 		blt_clearAdvSetsParam(ll_ext_adv_t		*pEadv);
u8			blt_ll_searchExistingAdvSet(u8 advHandle);
u8 			blt_ll_searchAvailableAdvSet(u8 advHandle);
void		blt_ll_updateAdvState(void);
ble_sts_t   blt_ll_enableExtAdv(int adv_en);
void 		blt_ll_procAuxConnectReq(u8 * prx);
int  		blt_ll_updateAdvPacket(void);
void		blt_ll_reset_ext_adv(void);

#endif /* LL_ADV_EXT_H_ */
