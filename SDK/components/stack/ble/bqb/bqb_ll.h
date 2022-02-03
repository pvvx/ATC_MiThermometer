/********************************************************************************************************
 * @file     bqb_ll.h
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
 * @date     Sep. 18, 2018
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
#ifndef BQB_LL_H_
#define BQB_LL_H_


typedef enum {
	TP_STATUS_CONN_PKT_MAX = 1,
	TP_STATUS_CONN_UNKNOWN_RSP,

	TP_STATUS_CONNECTION_DATA_SEND,
//adv
	TP_STATUS_ADV_PKT_MAX		,
	TP_STATUS_CONN_ESTABLISH	,
	TP_STATUS_CONN_TERMINATE				,

	TP_STATUS_CONN_PARA_REQ					,
	TP_STATUS_CONN_PARA_UPDATE				,

	TP_STATUS_CONNECTION_TIMEOUT			,
	TP_STATUS_CONNE_RCVD_L2CAP_DATA			,

	TP_STATUS_CONN_MAP_REQ					,
	TP_STATUS_CONN_MAP_UPDATE				,
	TP_STATUS_CONN_DATA_LENGTH_EXCHANGE		,

	TP_STATUS_CONN_BRX_MISS_END				,

	TP_STATUS_L2CAP_DATA_IN_IRQ_RX			,




	TP_STATUS_TERMINATE_IN_IRQ_RX			,
	TP_STATUS_BRX_START						,
	TP_STATUS_RECV_CERT_DATA				,
	TP_STATUS_PUSH_DATA_CRC_ERR				,
	TP_STATUS_PUSH_DATA_SN_ERR				,
	TP_STATUS_CONN_IN_IRQ_RX				,
	TP_STATUS_BRX_POST						,
	TP_STATUS_BRX_TX						,




	TP_STATUS_SCAN_HIT_MAX					,
	TP_STATUS_SCAN_RCVD_RSP					,
	TP_STATUS_SCAN_REQ_SENT					,
	TP_STATUS_CONN_HIT_MAX					,

	TP_STATUS_CONN_RCVD_DAT					,
	TP_STATUS_CONN_REQ_SENT					,
	TP_STATUS_TIMEOUT						,
	TP_STATUS_CONNECTION_LOST				,
	TP_STATUS_CONN_PKT_MISSING				,
	TP_STATUS_CONNECTION_RETX_MAX			,

	 TP_STATUS_RCVD_NO_EMPTY_PKT			,
	 TP_STATUS_CHN_MAP_FULL_USED			,


	TP_STATUS_CONN_PHY_UPDATE       		,

	TP_STATUS_CONNECTION_FEATURE_REQ		,

	TP_STATUS_CONN_BTX_MIDDLE				,



	TP_STATUS_INIT = 0x8000,
} tp_status_t;




















typedef int (*callback_testcase_t)(int);
extern	callback_testcase_t		p_callback_testcase;





typedef struct {
	u8		En_AdvData_inc;
	s8		AdvData_inc_step;
	u8		En_adv_crcERR;
	u8		En_scanRsp_crcERR;


	u32 	AdvPkt_max;
	u32 	AdvPkt_no;
	u32		ConnPkt_max;
	u32		ConnPkt_no;

	u8 		feature_req_skip;
	u8		version_ind_skip;
	u8		ver_ind_once;
	u8 		phy_req_col;
	u8		phy_req_skip;

	u8 		phy_req_nochange;
	u8		Disable_moreData;
	u8		phy_no_common;
//	u8		testEn_printRcvdData_onConnect;
	u8		En_rcvd_l2cap_data_callback;
//    u8 		lenReq_no_support;

	int		testData_pkts;
	int		testData_no;


	u16 	brx_miss_en;
	u16 	brx_miss_delay_intervals;
	u32 	brx_miss_startTick;
	u32 	brx_miss_duration;  //us

	u8      conn_pkt_crcERR;
} st_bqb_ll_t;
extern st_bqb_ll_t		bqbS;







extern	u8	pkt_test_data[];

extern u8 testData_pkts;
extern u32 testConnPkt_no;
extern u32 testConnPkt_max;
extern u8 testPhyUpdate_Same;
extern u8 testPhyUpdate_PastIns;
extern u8 testPhyUpdate_TO;
extern u8 testDiffProc_ChnMap;
extern u8 testDiffProc_ConUp;
extern u8 test_ConRecv_no;
extern u8 testPhyInd_TO;
extern u8 testNoPhyReqChan;
extern u8 testNoPhyRspChan;



void 	tp_set_adv_pkt_max (u16 m);
void 	tp_enable_advData_inrease(u8 en, s8 step);

void 	tp_AdvAddress_companyId_exchange(void);
void 	tp_AdvAddress_companyAssignId_exchange(void);
void 	tp_AdvAddress_companyId_companyAssignId_mismatch(void);
//void 	tp_ScanRspAddress_companyId_exchange(void);
//void 		tp_ScanRspAddress_companyAssignId_exchange(void);
//void tp_ScanRspAddress_companyId_companyAssignId_mismatch(void);


void 	tp_enable_adv_scanRsp_Bad_CRC (u8 advBadCrc, u8 scanRspBadCrc );

void 	tp_set_conn_pkt_max (u32 m);

void	tp_flip_access_address (int n);
void	tp_set_test_data (u8 type, u8 len, u8 d, int pkts);
void	tp_set_manual_timeout (u32 timeout);
void 	tp_disable_more_data(u8 disable);
void 	tp_enbale_print_rcvd_data_connect(u8 en);
void 	tp_enbale_rcvd_l2cap_data_callback(u8 en);
void 	tp_disable_data_len_exchange(u8 en);
void 	tp_set_brx_missing_time(u32 time_ms);
void 	tp_set_brx_missing_delay_intervals(int interval_num);
void 	tp_enable_conn_pkt_Bad_CRC (u8 connBadCrc );



void 	tp_set_adv_pkt_max (u16 m);
void 	tp_disable_more_data(u8 disable);
void 	tp_set_conn_pkt_max(u32 m);
void 	tp_phy_req_col(u8 en);
void 	tp_phy_req_skip(u8 en);
void 	tp_phy_req_nochange(u8 en);
void 	tp_phy_no_common(u8 en);
void 	tp_set_test_data (u8 type, u8 len, u8 d, int pkts);
void 	tp_enbale_rcvd_l2cap_data_callback(u8 en);




void 	tp_set_test_pkt(u8 num);
void 	tp_SetPhyUpdate_Same(u8 en);
void 	tp_set_PhyUpdate_InsInPast(u8 en);
void 	tp_set_PhyReq_TO(u8 en);
void 	tp_set_DiffProc_ChnMap(u8 en);
void 	tp_set_DiffProc_ConUp(u8 en);
void 	tp_setRecv_no(u8 num);
void 	tp_set_PhyInd_TO(u8 en);
void 	tp_set_NoPhyReqChan(u8 en);
void 	tp_set_NoPhyRspChan(u8 en);





#endif /* LL_BQB_H_ */
