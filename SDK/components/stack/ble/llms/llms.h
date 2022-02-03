/********************************************************************************************************
 * @file     ll_master_slave.h
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
 * @date     May. 22, 2019
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

#ifndef LL_MASTER_SLAVE_H_
#define LL_MASTER_SLAVE_H_



/********************* Macro & Enumeration & Structure Definition for Stack, user can not use!!!!  **********************/
#define			BLMS_STATE_NONE									0
#define			BLMS_STATE_ADV									BIT(1)
#define			BLMS_STATE_SCAN									BIT(2)
#define			BLMS_STATE_SCAN_POST							BIT(3)
#define			BLMS_STATE_BTX_S								BIT(4)
#define			BLMS_STATE_BTX_E								BIT(5)
#define			BLMS_STATE_BRX_S								BIT(6)
#define			BLMS_STATE_BRX_E								BIT(7)




#define 		BLMS_FLG_RF_CONN_DONE 		 					(FLD_RF_IRQ_CMD_DONE  | FLD_RF_IRQ_FIRST_TIMEOUT | FLD_RF_IRQ_RX_TIMEOUT | FLD_RF_IRQ_RX_CRC_2)

//TODO: delete all debug codes after functions are tested OK
#define 		BLMS_ERR_DEBUG(x)								do{irq_disable();write_reg32(0x40000, (x));while(1);}while(0)



#define 		BLMS_CONNECT_RX_MAX_TRY_NUM						4


#define			SYS_IRQ_TRIG_SLOT_TASK							0         //Scan/ ADV /BRX start /BTX start
#define			SYS_IRQ_TRIG_BTX_POST							BIT(0)
#define			SYS_IRQ_TRIG_BRX_POST							BIT(1)
#define			SYS_IRQ_TRIG_SCAN_POST							BIT(2)

#define			SYS_IRQ_TRIG_BRX_START							BIT(3)   //only slave high duty mode use




/***************************************** LL ENC *****************************************/
#define			MS_LL_ENC_OFF									0
#define			MS_LL_ENC_REQ									1
#define			MS_LL_ENC_RSP_T									2
#define			MS_LL_ENC_START_REQ_T							3
#define			MS_LL_ENC_START_RSP								4
#define			MS_LL_ENC_START_RSP_T							5
#define			MS_LL_ENC_PAUSE_REQ								6
#define			MS_LL_ENC_PAUSE_RSP_T							7
#define			MS_LL_ENC_PAUSE_RSP								8
#define			MS_LL_REJECT_IND_T								9
#define         MS_LL_ENC_SMP_INFO_S                			10
#define         MS_LL_ENC_SMP_INFO_E                			11

#define			MS_CONN_ENC_CHANGE								BIT(16) //offset 2byte
#define			MS_CONN_ENC_REFRESH								BIT(17)
#define			MS_CONN_ENC_REFRESH_T							BIT(18)




#define			BLMS_CONN_HANDLE								(BLM_CONN_HANDLE | BLS_CONN_HANDLE)


#define			BLMS_MAX_CONN_NUM								4

#define			BLMS_MAX_CONN_MASTER_NUM						3
#define			CONN_IDX_SLAVE									0
#define			CONN_IDX_MASTER1								1
#define			CONN_IDX_MASTER2								2
#define			CONN_IDX_MASTER3								3



#define		 	CONN_STATUS_DISCONNECT							0
#define			CONN_STATUS_COMPLETE							1
#define			CONN_STATUS_ESTABLISH							2





#define			BLMS_TX_FIFO_NUM								8
#define			BLMS_TX_FIFO_SIZE								40


typedef struct {
	u8		save_flg;
	u8		sn_nesn;
	u8 	    dma_tx_rptr;
	u8		reg_core_f03;
}bb_ms_sts_t;

typedef struct {

	u8		tx_wptr;
	u8		tx_rptr;
	u8		tx_num;
	u8		chn_idx;

	u8		role;
	u8		remote_version;
	u16     ll_enc_busy;

	u8		connState;			 // 0/Conn_Complete/Conn_Establish
	u8		last_rf_len;
	u8		conn_peer_sn;
	u8		conn_snnesn;

	u16		conn_nesn;//local nesn
	u16     conn_sn;  //local sn

	u8      conn_dma_tx_rptr;
	u8      smp_empty_pkt_entry; //slave-role empty pkt entry: ble main_loop entry enable flg
	u8      blt_tx_pkt_hold;
	u8      rsvd;



	u8		conn_chn;
	u8		conn_update;
	u8		conn_rx_num;  //rx number in a new interval
	u8		conn_receive_packet;

	u8		conn_receive_new_packet;
	u8 		conn_peer_terminate;
	u8 		conn_local_terminate;
	u8		conn_terminate_reason;

	u32     conn_established_tick;
	u32     conn_established; //mark

	u8		conn_Req_waitAck_enable;
	u8		peer_adr_type;
	u8		peer_adr[6];

	u16		updateReq_conn_min;
	u16		updateReq_conn_max;
	u16		updateReq_conn_latency;
	u16		updateReq_conn_timeout;

	u16		connHandle;
	u16		conn_interval;
	u16		conn_latency;
	u16		conn_inst;

	u16     conn_winsize_next;
	u16     conn_offset_next;
	u16		conn_inst_next;
	u16		conn_interval_next;
	u16		conn_latency_next;
	u16		enc_ediv;       //EDIV

	u8		enc_random[8];  //RANDOM

	u32		conn_terminate_tick;

	u32 	conn_interval_tick;
	u32		conn_timeout;
	u32		conn_timeout_next;
	u32		conn_missing;
	u32		conn_tick;
	u32		conn_access_code;
	u32		conn_crc;
	u32     conn_crc_revert; //revert crc24 init value

	u32 	ll_remoteFeature;
	u32		conn_inst_u32;


	u8		conn_sca;
	u8		conn_chn_hop;
	u8		conn_chn_map[5];
	u8		conn_chn_map_next[5];

	u32		enc_ivm;       //master IVm
	u8 		enc_skdm[8];   //master SKDm
	u32		enc_ivs;       //slave  IVs
	u8 		enc_skds[8];   //slave  SKDs

	bb_ms_sts_t blms_bb;// BaseBand info: SN/NESN/TX_FIFO_RPTR

	u8		chn_tbl[40];
	u32		tx_fifo[BLMS_TX_FIFO_NUM][BLMS_TX_FIFO_SIZE>>2];

	ble_crypt_para_t	crypt;

} st_llms_conn_t;

extern st_llms_conn_t	blms[];
extern st_llms_conn_t  *blms_pconn;
extern int				blms_conn_sel;

extern u32 				Crc24Lookup[16];


typedef struct {
	u8		adv_en;
	u8		scan_en;
	u8		scan_interval;



	u8		ll_recentAvgRSSI;
	u8		scan_hci_cmd;
	u8		tx_irq_proc_en;
	u8      md_max_nums;  //supported max md nums, default value: 0
	u8		drop_rx_data;

#if (LE_AUTHENTICATED_PAYLOAD_TIMEOUT_SUPPORT_EN)
	u32		tick_LE_Authenticated_Payload;    //timer start tick
	u32		to_us_LE_Authenticated_Payload;    //timer threshold
	u8		enable_LE_Authenticated_Payload;
#endif
} st_llms_para_t;

extern st_llms_para_t  blmsParam;


extern	volatile	u8	blms_state;
extern				u8	blms_tx_empty_packet[];

extern	volatile	u32 rx_tick_now;

/******************************* Macro & Enumeration variables for User ************************************/










/************************************ User Interface  ******************************************************/
void 	blc_llms_initBasicMCU (void);
void 	blc_llms_initStandby_module (u8 *public_adr);
void 	blc_llms_initMultiMasterSingleSlave_module(void);

u8 * blms_l2cap_packet_pack(u16 connHandle, u8 * raw_pkt);
int 	blms_ll_pushTxfifo (u16 connHandle, u8 *p);
ble_sts_t  blms_att_pushNotifyData (u16 connHandle, u16 attHandle, u8 *p, int len);
void 	blms_sdk_main_loop (void);

u8 blms_getCurrConnRole(u16 connHandle);
u8 blms_fifo_num (u16 connHandle);
u8  blms_ll_getTxFifoNumber (u16 connHandle);
bool blms_ll_isTxFifoAvailableForApp(u16 connHandle);

int blms_is_handle_valid (u16 handle);
ble_sts_t blms_ll_disconnect (u16 connHandle, u8 reason);
void  blms_ll_setEncryptionBusy(u16 connHandle, u8 enc_busy);
int  blms_ll_isEncryptionBusy(u16 connHandle);
u8 blms_ll_pushTxfifo_hold (u16 connHandle, u8 *p);

/*********************************** Stack Interface, user can not use!!! **********************************/
void 	irq_ble_master_slave_handler(void);
void 	blt_llms_channelTable_calc(u8* chm, u8 hop, u8 *ptbl);
int 	blt_llms_procPendingEvent(void);

int 	blms_connect_common(st_llms_conn_t *pc, rf_packet_connect_t * pInit);
int 	blms_start_common  (st_llms_conn_t *pc);
int 	blms_post_common   (st_llms_conn_t *pc);

void	smemset4(int * dest, int val, int len);

#endif /* LL_MASTER_SLAVE_H_ */
