/********************************************************************************************************
 * @file     hci.h 
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
#pragma  once


#include <stack/ble/ble_common.h>

typedef int (*blc_hci_rx_handler_t) (void);
typedef int (*blc_hci_tx_handler_t) (void);
typedef int (*blc_hci_handler_t) (unsigned char *p, int n);
typedef int (*blc_hci_app_handler_t) (unsigned char *p);

extern blc_hci_handler_t			blc_master_handler;


#define			HCI_FLAG_EVENT_PHYTEST_2_WIRE_UART			(1<<23)
#define			HCI_FLAG_EVENT_TLK_MODULE					(1<<24)
#define			HCI_FLAG_EVENT_BT_STD						(1<<25)
#define			HCI_FLAG_EVENT_STACK						(1<<26)
#define			HCI_FLAG_ACL_BT_STD							(1<<27)

#define			TLK_MODULE_EVENT_STATE_CHANGE				0x0730
#define			TLK_MODULE_EVENT_DATA_RECEIVED				0x0731
#define			TLK_MODULE_EVENT_DATA_SEND					0x0732
#define			TLK_MODULE_EVENT_BUFF_AVAILABLE				0x0733




#define			HCI_MAX_ACL_DATA_LEN              			27

#define 		HCI_MAX_DATA_BUFFERS_SALVE              	8
#define 		HCI_MAX_DATA_BUFFERS_MASTER              	8


#define 		HCI_FIRST_NAF_PACKET             			0x00
#define 		HCI_CONTINUING_PACKET             			0x01
#define 		HCI_FIRST_AF_PACKET               			0x02


/*********************************************************************
 * ENUMS
 */


/**
 *  @brief  Definition for HCI request type
 */
typedef enum hci_type_e {
	HCI_TYPE_CMD = 0x01,
	HCI_TYPE_ACL_DATA,
	HCI_TYPE_SCO_DATA,
	HCI_TYPE_EVENT,
} hci_type_t;



// hci event
extern u32		hci_eventMask;
extern u32		hci_le_eventMask;
ble_sts_t 		blc_hci_setEventMask_cmd(u32 evtMask);      //eventMask: BT/EDR
ble_sts_t 		blc_hci_le_setEventMask_cmd(u32 evtMask);   //eventMask: LE


// Controller event handler
typedef int (*hci_event_handler_t) (u32 h, u8 *para, int n);
extern hci_event_handler_t		blc_hci_event_handler;
void 	blc_hci_registerControllerEventHandler (hci_event_handler_t  handler);


int 		blc_hci_sendACLData2Host (u16 handle, u8 *p);



int blc_hci_send_data (u32 h, u8 *para, int n);
void blc_enable_hci_master_handler ();




int blc_acl_from_btusb ();

void blc_register_hci_handler (void *prx, void *ptx);
int blc_hci_rx_from_usb (void);
int blc_hci_tx_to_usb (void);
int blc_hci_tx_to_btusb (void);

int blc_hci_handler (u8 *p, int n);
int blm_hci_handler (u8 *p, int n);
int blc_hci_send_event (u32 h, u8 *para, int n);

int blc_hci_proc (void);
