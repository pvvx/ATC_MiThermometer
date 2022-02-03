/********************************************************************************************************
 * @file     ll_conn.h
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
 * @date     July. 4, 2019
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

#ifndef LL_CONN_H_
#define LL_CONN_H_





/******************************* Macro & Enumeration variables for Stack, user can not use!!!!  ********************************/

/////////////////////////////////////////////////////////////////////////////
#define		LMP_PROCEDURE_RESPONSE_TIMEOUT						40000000 //40S LL response timeout


/////////////////////////////////////////////////////////////////////////////
#define 		LL_ROLE_MASTER              					0
#define 		LL_ROLE_SLAVE               					1

#define			BLM_CONN_HANDLE									BIT(7)
#define			BLS_CONN_HANDLE									BIT(6)

#define			HANDLE_STK_FLAG									BIT(15)

/////////////////////////////////////////////////////////////////////////////
#define			MAX_OCTETS_DATA_LEN_27							27
#define			MAX_OCTETS_DATA_LEN_EXTENSION					251


#define			LL_PACKET_OCTET_TIME(n)							((n) * 8 + 112)

#define 		DATA_LENGTH_REQ_PENDING							1
#define			DATA_LENGTH_REQ_DONE							2

/////////////////////////////////////////////////////////////////////////////
#define 		BLE_INVALID_CONNECTION_HANDLE    				0xffff
#define 		IS_CONNECTION_HANDLE_VALID(handle)  			( handle != BLE_INVALID_CONNECTION_HANDLE )


/////////////////////////////////////////////////////////////////////////////
#define 		FLG_RF_CONN_DONE 		 				(FLD_RF_IRQ_CMD_DONE | FLD_RF_IRQ_FSM_TIMEOUT | FLD_RF_IRQ_FIRST_TIMEOUT | FLD_RF_IRQ_RX_TIMEOUT | FLD_RF_IRQ_RX_CRC_2)


typedef struct{
	u8	llid;
	u8  rf_len;
	u8 	opcode;
	u8	CtrData;
}rf_pkt_ll_unknown_rsp_t;





typedef struct {
	u8		connState;
	u8		conn_update;
	u8		last_rf_len;
	u8		conn_peer_sn;

	u8		conn_rx_num; 			 	//RX number (regardless of CRC correct or wrong)
	u8		conn_receive_packet;		//RX with CRC correct
	u8		conn_receive_new_packet;	//RX with CRC correct & new SN
	u8		rsvd1;

	u8		rsvd2;
	u8		conn_snnesn;
	u8		conn_chnsel;
	u8		chn_idx;

	u8		conn_chn;
	u8		conn_chn_hop;
	u8		conn_chn_map[5];
	u8		conn_chn_map_next[5];


	u16		connHandle;
	u16 	channel_id;

	u16		conn_inst;
	u16		conn_inst_next;

	u32		conn_tick;

	u32		conn_access_code;
	u32		conn_access_code_revert;
	u32		conn_crc;

	u8		chn_tbl[40];

}st_ll_conn_t ;
extern _attribute_aligned_(4) st_ll_conn_t		blttcon;
extern st_ll_conn_t	   *blt_pconn;


typedef bool (*ll_push_fifo_handler_t) (int, u8 *);
extern  ll_push_fifo_handler_t				ll_push_tx_fifo_handler;


















/********************************  Macro & Enumeration variables for User *****************************************************/









/********************************  User Interface  ****************************************************************************/
void		blc_ll_initConnection_module(void);








/*******************************  Stack Interface, user can not use!!! *******************************************************/

int 		blt_ll_connect_common(st_ll_conn_t *pc, rf_packet_connect_t * pInit);
int 		blt_ll_start_common  (st_ll_conn_t *pc);
int 		blt_ll_post_common   (st_ll_conn_t *pc);

//encryption
ble_sts_t 	blc_hci_ltkRequestNegativeReply (u16 connHandle);
ble_sts_t  	blc_hci_ltkRequestReply (u16 connHandle,  u8*ltk);

void 		blc_ll_registerLtkReqEvtCb(blt_LTK_req_callback_t evtCbFunc);

int  		blt_ll_conn_main_loop_post(void);
void		blc_procPendingEvent(void);


ble_sts_t	blt_ll_unknown_rsp(u16 connHandle, u8 op_code );

void 		blt_ll_channelTable_calc(u8* chm, u8 hop, u8 *ptbl);


#endif /* LL_CONN_H_ */
