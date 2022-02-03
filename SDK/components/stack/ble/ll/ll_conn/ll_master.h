/********************************************************************************************************
 * @file     ll_master.h 
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
 * ll_master.h
 *
 *  Created on: 2017-3-8
 *      Author: Administrator
 */

#ifndef LL_MASTER_H_
#define LL_MASTER_H_



#define			BLE_DATA_CHANNEL_EN				1
#define			SYS_LINK_ADV_INTERVAL			500000
#define			BLE_MASTER_CONNECTION_REQ		1
#define			BLM_CONN_HANDLE_CANCEL			BIT(8)
#define			BLM_CONN_MASTER_TERMINATE		BIT(4)
#define			BLM_CONN_SLAVE_TERMINATE		BIT(5)
#define			BLM_CONN_TERMINATE_SEND			BIT(0)

#define			BLM_CONN_ENC_CHANGE				BIT(9)
#define			BLM_CONN_ENC_REFRESH			BIT(10)
#define			BLM_CONN_ENC_REFRESH_T			BIT(11)


#define			MASTER_LL_ENC_OFF				0
#define			MASTER_LL_ENC_REQ				1
#define			MASTER_LL_ENC_RSP_T				2
#define			MASTER_LL_ENC_START_REQ_T		3
#define			MASTER_LL_ENC_START_RSP			4
#define			MASTER_LL_ENC_START_RSP_T		5
#define			MASTER_LL_ENC_PAUSE_REQ			6
#define			MASTER_LL_ENC_PAUSE_RSP_T		7
#define			MASTER_LL_ENC_PAUSE_RSP			8
#define			MASTER_LL_REJECT_IND_T			9
#define			MASTER_LL_ENC_SMP_INFO_S		10
#define			MASTER_LL_ENC_SMP_INFO_E		11



#define  BLM_WINSIZE			4
#define  BLM_WINOFFSET			5

#define  BLM_MID_WINSIZE		2500


#define CONN_REQ_WAIT_ACK_NUM				6




#define TX_FIFO_DEFINED_IN_APP					1

//#define			BLM_TX_FIFO_NUM				8
#define 		STACK_FIFO_NUM				2  //user 6, stack 2
//#define			BLM_TX_FIFO_SIZE			40

typedef struct {
	u8		save_flg;
	u8		sn_nesn;
	u8 	    dma_tx_rptr;
	u8 		rsvd;
}bb_msts_t;

typedef struct {
#if (!TX_FIFO_DEFINED_IN_APP)
	u32		tx_fifo[BLM_TX_FIFO_NUM][BLM_TX_FIFO_SIZE>>2];
	u8		tx_wptr;
	u8		tx_rptr;
	u8		rsvd00;
	u8		rsvd01;
#endif
	u8		chn_tbl[40];

	u8 		newRx;
	u8		tx_num;
	u8		remoteFeatureReq;
	u8 		adv_filterPolicy;

	u8		conn_Req_waitAck_enable;
	u8		conn_terminate_reason;
	u8		slave_terminate_conn_flag;
	u8	 	master_terminate_conn_flag;

	u8		conn_terminate_pending;   // terminate_pending = master_terminate || slave_terminate
	u8		remote_version;
	u8		connParaUpReq_pending;
	u8		conn_sn; // softwre rcrd sn to filter retry rx data


	u8 		conn_btx_not_working;
	u8		conn_rcvd_slave_pkt;
	u8		peer_adr_type;
	u8		rsvdd;

	u16		connHandle;
	u8		peer_adr[6];
	u8		macAddress_public[6];
	u8		macAddress_random[6];   //host may set this


	bb_msts_t blm_bb;// hardware rcrd sn nesn ect.

	u32 	ll_remoteFeature; 	 //feature mask <0:31>
	//u32   ll_remoteFeature_2;  //feature mask <32:63> for future use

	u32		conn_timeout;
	u32		conn_interval;
	u32		conn_software_timeout;

	u32		conn_winsize_next;
	u32		conn_timeout_next;
	u32		conn_offset_next;
	u32		conn_interval_next;

	u16		conn_latency;
	u16		conn_latency_next;


	u32		conn_Req_noAck_timeout;

	u16		enc_ediv;

	u32		enc_ivs;
	u8		enc_random[8];
	u8 		enc_skds[8];
	ble_crypt_para_t	crypt;

#if(BQB_5P0_TEST_ENABLE)
	u32		master_teminate_time;
#endif
} st_ll_conn_master_t;





/******************************* User Interface  ************************************/
void blc_ll_initMasterRoleSingleConn_module(void);


bool blm_ll_isRfStateMachineBusy(void);






ble_sts_t  blm_hci_reset(void);

ble_sts_t blm_ll_disconnect (u16 handle, u8 reason);

ble_sts_t blm_ll_updateConnection (u16 connHandle,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );

ble_sts_t blm_ll_setHostChannel (u16 handle, u8 * map);


extern 	int		blm_create_connection;




rf_packet_l2cap_t * blm_l2cap_packet_pack (u16 conn, u8 * raw_pkt);


st_ll_conn_master_t * blm_ll_getConnection (u16 h);

bool blm_ll_deviceIsConnState (u8 addr_type, u8* mac_addr);

//------------- ATT client function -------------------------------

//------------- ATT service discovery function -------------------------------
int blm_att_discoveryService (u16 connHandle, att_db_uuid16_t *p16, int n16, att_db_uuid128_t *p128, int n128);
u16 blm_att_findHandleOfUuid16 (att_db_uuid16_t *p, u16 uuid, u16 ref);
u16 blm_att_findHandleOfUuid128 (att_db_uuid128_t *p, const u8 * uuid);
u16 blm_att_discoveryHandleOfUUID (u8 *l2cap_data, u8 *uuid128);


//------------	master function -----------------------------------



bool 	  blm_push_fifo (int connHandle, u8 *dat);

void 	blm_main_loop (void);


//------------	master security function -------------------
int  blm_ll_startEncryption (u8 connhandle ,u16 ediv, u8* random, u8* ltk);
void blm_ll_startDistributeKey (u8 connhandle );


int blm_l2cap_packet_receive (u16 conn, u8 * raw_pkt);

ble_sts_t	blm_hci_receiveHostACLData(u16 connHandle, u8 PB_Flag, u8 BC_Flag, u8 *pData );

ble_sts_t blm_ll_readRemoteFeature (u16 handle);
#endif /* LL_MASTER_H_ */
