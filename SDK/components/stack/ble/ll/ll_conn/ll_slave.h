/********************************************************************************************************
 * @file     ll_slave.h 
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
 * ll_slave.h
 *
 *  Created on: 2017-3-9
 *      Author: Administrator
 */

#ifndef LL_SLAVE_H_
#define LL_SLAVE_H_




#define					SLAVE_LL_ENC_OFF						0
#define					SLAVE_LL_ENC_REQ						1
#define					SLAVE_LL_ENC_RSP_T						2
#define					SLAVE_LL_ENC_START_REQ_T				3
#define					SLAVE_LL_ENC_START_RSP					4
#define					SLAVE_LL_ENC_START_RSP_T				5
#define					SLAVE_LL_ENC_PAUSE_REQ					6
#define					SLAVE_LL_ENC_PAUSE_RSP_T				7
#define					SLAVE_LL_ENC_PAUSE_RSP					8
#define					SLAVE_LL_REJECT_IND_T					9


#define 				TRY_FIX_ERR_BY_ADD_BRX_WAIT				1



typedef struct {
	u8		time_update_st;
	u8 		last_rf_len;
	u8		remoteFeatureReq;
	u8 		long_suspend;

	u8		interval_level;
	u8		ll_recentAvgRSSI;
	u8		conn_sn_master;
	u8		master_not_ack_slaveAckUpReq;

	u8		conn_rcvd_ack_pkt;
	u8		conn_new_param;
	u8		conn_winsize_next;
	u8		rsvd1;

	u8 		conn_master_terminate;
	u8		conn_terminate_reason;
	u8 		conn_slave_terminate;
	u8		conn_terminate_pending;   // terminate_pending = master_terminate || slave_terminate


	u16		conn_update_inst_diff;
	u16		connHandle;
	u16     conn_offset_next;
	u16		conn_interval_next; //standard value,  not * 1.25ms
	u16		conn_latency_next;
	u16		conn_timeout_next;  //standard value,  not *10ms


	u32		conn_access_code_revert;
	u32		connExpectTime;
	int		conn_interval_adjust;
	u32		conn_timeout;
	u32		conn_interval;
	u32		conn_latency;
	u32		conn_duration;

	u32 	ll_remoteFeature; 	 //feature mask <0:31>,  not only one for BLE master, use connHandle to identify
	//u32   ll_remoteFeature_2;  //feature mask <32:63> for future use

	u32		conn_start_tick;

	int		conn_tolerance_time;

	u32		tick_1st_rx;
	u32		conn_brx_tick;



	u32 	conn_slaveTerminate_time;


	u32		conn_pkt_rcvd;
	u32		conn_pkt_rcvd_no;
	u8 *	conn_pkt_dec_pending;
	int		conn_enc_dec_busy;
	int		conn_stop_brx;

	u32		conn_fsm_timeout;   // unit:  uS

#if	(BQB_5P0_TEST_ENABLE)
	u32		conn_establish_pending_timeout;;
	u32		conn_establish_pending_flag;
#endif


} st_ll_conn_slave_t;





typedef struct {
	u8		save_flg;
	u8		sn_nesn;
	u8		rsvd1;
	u8 		rsvd2;

#if (DEEP_RET_ENTRY_CONDITIONS_OPTIMIZATION_EN)
	u8		dma_tx_rcvry_dat_depth;
	u8 		dma_tx_data_depth;

	u16		dma_tx_ptr[8];//dma tx buffer ptr rcd
#endif

}bb_sts_t;

extern _attribute_aligned_(4) bb_sts_t	blt_bb;
extern _attribute_aligned_(4) st_ll_conn_slave_t		bltc;
#define 	IS_LL_CONNECTION_VALID(handle)  ( bltc.connHandle == (handle) && bltc.connHandle != BLE_INVALID_CONNECTION_HANDLE )






/************************************ User Interface  ******************************************************/
void 		blc_ll_initSlaveRole_module(void);

ble_sts_t  	bls_ll_terminateConnection (u8 reason);

bool		bls_ll_isConnectState (void);

u16			bls_ll_getConnectionInterval(void);  // if return 0, means not in connection state
u16			bls_ll_getConnectionLatency(void);	 // if return 0, means not in connection state
u16			bls_ll_getConnectionTimeout(void);	 // if return 0, means not in connection state


int			bls_ll_requestConnBrxEventDisable(void);
void		bls_ll_disableConnBrxEvent(void);
void		bls_ll_restoreConnBrxEvent(void);





//ble module event
ble_sts_t 	bls_hci_mod_setEventMask_cmd(u32 evtMask);  //eventMask: module special




/*********************************** Stack Interface, user can not use!!! ********************************/
bool		bls_ll_pushTxFifo (int hanlde, u8 *p);
ble_sts_t  	bls_hci_reset(void);

ble_sts_t	bls_hci_receiveHostACLData(u16 connHandle, u8 PB_Flag, u8 BC_Flag, u8 *pData );
ble_sts_t 	bls_hci_le_getRemoteSupportedFeatures(u16 connHandle);

void		blt_push_fifo_hold (u8 *p);




extern ble_crypt_para_t 	blc_cyrpt_para;
static inline int  bls_ll_isConnectionEncrypted(u16 connHandle)
{
	return blc_cyrpt_para.enable;
}

#if (TRY_FIX_ERR_BY_ADD_BRX_WAIT)
	extern int CONN_SHORT_TOLERANCE_TIME;
	static inline void blc_pm_modefy_brx_early_set(int us)
	{
		CONN_SHORT_TOLERANCE_TIME = us * sys_tick_per_us;
	}

	static inline int blc_pm_get_brx_early_time(void)
	{
		return CONN_SHORT_TOLERANCE_TIME;
	}
#endif

/////////////////////// SN/NESN /////////////////////////////////////////////////////
static inline void	blt_save_snnesn()
{
								// SN									NESN
	blt_bb.sn_nesn = ((REG_ADDR8(0xf22) & BIT(0)) << 4) | ((REG_ADDR8(0xf23) & BIT(4)) << 1);
}

static inline void	blt_restore_snnesn()
{
	reg_rf_ll_ctrl_1 &= ~(FLD_RF_BRX_SN_INIT | FLD_RF_BRX_NESN_INIT);
	reg_rf_ll_ctrl_1 |= blt_bb.sn_nesn;
}

/////////////////////// DMA Tx fifo rptr /////////////////////////////////////////////
extern 	u8 blt_dma_tx_rptr;//not need store in deep retention section.

static inline void	blt_save_dma_tx_rptr()
{
	//TX Fifo: 0xc2a[0:3] means rptr
	blt_dma_tx_rptr = reg_dma_tx_rptr & 0x0f;
}

static inline void	blt_restore_dma_tx_rptr()
{
	//0xc2a[6] rptr set
	reg_dma_tx_rptr = (BIT(6) | blt_dma_tx_rptr);//restore tx_rptr
}



#endif /* LL_SLAVE_H_ */
