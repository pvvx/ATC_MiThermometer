/********************************************************************************************************
 * @file     phy_test.h
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


#ifndef PHY_TEST_H_
#define PHY_TEST_H_

#include "tl_common.h"

#ifndef			PHYTEST_MODE_DISABLE
#define			PHYTEST_MODE_DISABLE					0
#endif

#ifndef			PHYTEST_MODE_THROUGH_2_WIRE_UART
#define 		PHYTEST_MODE_THROUGH_2_WIRE_UART		1   //Direct Test Mode through a 2-wire UART interface
#endif

#ifndef			PHYTEST_MODE_OVER_HCI_WITH_USB
#define 		PHYTEST_MODE_OVER_HCI_WITH_USB			2   //Direct Test Mode over HCI(UART hardware interface)
#endif

#ifndef			PHYTEST_MODE_OVER_HCI_WITH_UART
#define 		PHYTEST_MODE_OVER_HCI_WITH_UART			3   //Direct Test Mode over HCI(USB  hardware interface)
#endif


#define 		BLC_PHYTEST_DISABLE						0
#define 		BLC_PHYTEST_ENABLE						1



#define			PHY_CMD_SETUP							0
#define			PHY_CMD_RX								1
#define			PHY_CMD_TX								2
#define			PHY_CMD_END								3


#define 		PKT_TYPE_PRBS9 							0
#define 		PKT_TYPE_0X0F 							1
#define 		PKT_TYPE_0X55 							2
#define 		PKT_TYPE_0XFF 							3

enum{
	PHY_EVENT_STATUS	 = 0,
	PHY_EVENT_PKT_REPORT = 0x8000,
};

enum{
	PHY_STATUS_SUCCESS 	 = 0,
	PHY_STATUS_FAIL 	 = 0x0001,
};





typedef struct {
	u8 cmd;
	u8 tx_start;
	u16 pkts;

	u32 tick_tx;
}phy_data_t;








/******************************* User Interface  ************************************/
void 	  blc_phy_initPhyTest_module(void);

ble_sts_t blc_phy_setPhyTestEnable (u8 en);
bool 	  blc_phy_isPhyTestEnable(void);


//user for phy test 2 wire uart mode
int 	 phy_test_2_wire_rx_from_uart (void);
int 	 phy_test_2_wire_tx_to_uart (void);



/************************* Stack Interface, user can not use!!! ***************************/

int 	  blc_phy_test_main_loop(void);

int 	  blc_phytest_cmd_handler (u8 *p, int n);

ble_sts_t blc_phy_setReceiverTest (u8 rx_chn);
ble_sts_t blc_phy_setTransmitterTest (u8 tx_chn, u8 length, u8 pkt_type);
ble_sts_t blc_phy_setPhyTestEnd(u8 *pkt_num);

ble_sts_t blc_phy_reset(void);
void blc_phy_preamble_length_set(unsigned char len);






#endif /* PHY_TEST_H_ */
