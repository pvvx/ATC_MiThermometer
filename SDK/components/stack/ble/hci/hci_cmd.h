/********************************************************************************************************
 * @file     hci_cmd.h
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

#ifndef HCI_CMD_H_
#define HCI_CMD_H_


typedef enum {
	 TX_POWER_0dBm  = 0,
	 TX_POWER_1dBm  = 1,
	 TX_POWER_2dBm 	= 2,
	 TX_POWER_3dBm 	= 3,
	 TX_POWER_4dBm 	= 4,
	 TX_POWER_5dBm 	= 5,
	 TX_POWER_6dBm 	= 6,
	 TX_POWER_7dBm 	= 7,
	 TX_POWER_8dBm 	= 8,
	 TX_POWER_9dBm  = 9,
	 TX_POWER_10dBm = 10,
} tx_power_t;


/**
 *  @brief   "Operation" in "LE Set Extended Advertising Data Command" and "LE Set Extended Scan Response Data Command"
 */
typedef enum {
	DATA_OPER_INTER      	=	0x00,
	DATA_OPER_FIRST      	=	0x01,
	DATA_OPER_LAST       	=	0x02,
	DATA_OPER_COMPLETE   	=	0x03,
	DATA_OPER_UNCHANGEED	=  	0x04,
} data_oper_t;




/**
 *  @brief   "Fragment_Preference" in "LE Set Extended Advertising Data Command" and "LE Set Extended Scan Response Data Command"
 */
typedef enum {
	DATA_FRAGM_ALLOWED			      	=	0x00,
	DATA_FRAGM_NONE_OR_MINIMIZE      	=	0x01,
} data_fragm_t;















/**
 *  @brief  Return Parameters for "HCI LE Read PHY Command"
 */
typedef struct {
	u8         status;
	u8         handle[2];
	u8         tx_phy;
	u8         rx_phy;
} hci_le_readPhyCmd_retParam_t;




/**
 *  @brief  Command Parameters for "HCI LE Read Set Command"
 */

typedef struct {
	u16 connHandle;
	u8 	all_phys;
	u8 	tx_phys;
	u8 	rx_phys;
	u16 phy_options;
} hci_le_setPhyCmd_param_t;







/**
 *  @brief  Command Parameters Definition for "HCI LE Set Extended Advertising Parameters Command"
 */
typedef struct {
	u8		adv_handle;
    u16		adv_evt_props;
    u8	 	pri_advIntMin[3];
	u8		pri_advIntMax[3];
    u8 		pri_advChnMap;
    u8		ownAddrType;
    u8 		peerAddrType;
    u8  	peerAddr[6];
    u8		advFilterPolicy;
    u8 		adv_tx_pow;
    u8		pri_adv_phy;
    u8		sec_adv_max_skip;
    u8		sec_adv_phy;
    u8		adv_sid;
    u8		scan_req_noti_en;
}hci_le_setExtAdvParam_cmdParam_t;







ble_sts_t blc_hci_le_setPhy(hci_le_setPhyCmd_param_t* para);


#endif /* HCI_CMD_H_ */
