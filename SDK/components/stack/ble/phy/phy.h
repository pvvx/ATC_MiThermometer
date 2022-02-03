/********************************************************************************************************
 * @file     phy.h
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

#ifndef PHY_H_
#define PHY_H_



#include "stack/ble/hci/hci_cmd.h"






/********************* Macro & Enumeration variables for Stack, user can not use!!!!  **********************/
#define PHY_USED_AUXPTR_LE_1M									0
#define PHY_USED_AUXPTR_LE_2M									1
#define PHY_USED_AUXPTR_LE_CODED								2



typedef struct{
	u8	llid;
	u8  rf_len;
	u8	opcode;
	u8	tx_phys;
	u8	rx_phys;
}rf_pkt_ll_phy_req_rsp_t;   //phy_req, phy_rsp


typedef struct{
	u8	llid;
	u8  rf_len;
	u8	opcode;
	u8	m_to_s_phy;
	u8	s_to_m_phy;
	u8 	instant0;
	u8 	instant1;
}rf_pkt_ll_phy_update_ind_t;   //phy_req, phy_rsp



typedef enum{
	LE_CODED_S2 = 2,
	LE_CODED_S8 = 8,
}le_coding_ind_t;



typedef struct {
	u8	dft_tx_prefer_phys;
	u8 	dft_rx_prefer_phys;
	u8	dft_prefer_phy;
	u8	dft_CI;

	//for Extended ADV
	u8	cur_llPhy;
	u8	cur_CI;  //cur_coding_ind
	u16 rsvd;
}ll_phy_t;
extern _attribute_aligned_(4) ll_phy_t		bltPHYs;



//do not support Asymmetric PHYs, conn_phys = tx_phys & rx_phys
typedef struct {
	u8	conn_prefer_phys;  // conn_prefer_phys = tx_prefer_phys & rx_prefer_phys
	u8	conn_cur_phy;	   //
	u8	conn_next_phy;	   //
	u8	conn_cur_CI;	   // CI: coding_ind

	u8	conn_next_CI;
	u8	phy_req_trigger;  // 1: means current device triggers phy_req, due to API "blc_ll_setPhy" called by Host or Application
	u8	phy_req_pending;
	u8	phy_update_pending;

	u32	conn_update_phy;

	#if 0
		u8	tx_prefer_phys;		//phy set
		u8 	rx_prefer_phys;
		u8  tx_next_phys;
		u8 	rx_next_phys;

		u8	cur_tx_phy;		//phy using
		u8	cur_rx_phy;
		u16 rsvd;
	#endif

}ll_conn_phy_t;




typedef ll_conn_phy_t* (*ll_get_conn_phy_ptr_callback_t)(u16);
extern ll_get_conn_phy_ptr_callback_t  ll_get_conn_phy_ptr_cb;


extern u8   tx_settle_adv[4];
extern u8	tx_settle_slave[4];
extern u8	tx_settle_master[4];


/******************************* Macro & Enumeration variables for User ************************************/

typedef enum {
	BLE_PHY_1M 			= 0x01,
	BLE_PHY_2M 			= 0x02,
	BLE_PHY_CODED	 	= 0x03,
} le_phy_type_t;

typedef enum {
	PHY_PREFER_1M 		= BIT(0),
	PHY_PREFER_2M		= BIT(1),
	PHY_PREFER_CODED 	= BIT(2),
} le_phy_prefer_type_t;



typedef enum {
    PHY_TRX_PREFER		= 0,					//has 	 preference among TX & RX PHYs
	PHY_TX_NO_PREFER 	= BIT(0),   			//has no preference among TX PHYs
	PHY_RX_NO_PREFER 	= BIT(1),				//has no preference among RX PHYs
	PHY_TRX_NO_PREFER 	= (BIT(0) | BIT(1)),	//has no preference among TX & RX PHYs
} le_phy_prefer_mask_t;






typedef enum {
	CODED_PHY_PREFER_NONE 	= 0,
	CODED_PHY_PREFER_S2		= 1,
	CODED_PHY_PREFER_S8 	= 2,
} le_ci_prefer_t;   //le coding indication prefer



/************************************ User Interface  ******************************************************/
void		blc_ll_init2MPhyCodedPhy_feature(void);

ble_sts_t 	blc_ll_setDefaultPhy(le_phy_prefer_mask_t all_phys, le_phy_prefer_type_t tx_phys, le_phy_prefer_type_t rx_phys);

ble_sts_t	blc_ll_readPhy(u16 connHandle, hci_le_readPhyCmd_retParam_t *);

ble_sts_t  	blc_ll_setPhy(	u16 connHandle,					le_phy_prefer_mask_t all_phys,
							le_phy_prefer_type_t tx_phys, 	le_phy_prefer_type_t rx_phys,
							le_ci_prefer_t phy_options);



ble_sts_t	blc_ll_setDefaultConnCodingIndication(le_ci_prefer_t prefer_CI);



/*********************************** Stack Interface, user can not use!!! **********************************/
int 	blt_reset_conn_phy_param(ll_conn_phy_t* pconn_phy);
void 	rf_ble_switch_phy(le_phy_type_t phy, le_coding_ind_t coding_ind);

typedef void (*ll_phy_switch_callback_t)(le_phy_type_t, le_coding_ind_t);
extern ll_phy_switch_callback_t		ll_phy_switch_cb;




#endif /* PHY_H_ */
