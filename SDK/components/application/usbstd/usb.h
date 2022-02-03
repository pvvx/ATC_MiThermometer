/********************************************************************************************************
 * @file     usb.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 30, 2010
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

#pragma once
#include "tl_common.h"
#include "usbdesc.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

enum {
    //  3000 ms
	USB_TIME_BEFORE_ALLOW_SUSPEND = (3000*1000),
};

enum {
	USB_IRQ_SETUP_REQ = 0,
	USB_IRQ_DATA_REQ,
};


// telink usb report ctrl command. used mixed with USB_AUD_PLAY_PAUSE...
enum{
	USB_REPORT_NO_EVENT		= 0xf0,
	USB_REPORT_RELEASE 		= 0xff,
};

#if (USB_MIC_ENABLE)
extern u8 usb_alt_intf[USB_INTF_MAX];
static inline int usb_mic_is_enable(){
	return usb_alt_intf[USB_INTF_MIC];
}
#endif

extern u8 usb_just_wakeup_from_suspend;
extern u8 usb_has_suspend_irq;
extern u8 edp_toggle[8];

void usb_init();

#ifndef		USB_SOFTWARE_CRC_CHECK
#define		USB_SOFTWARE_CRC_CHECK		0
#endif

#define MS_VENDORCODE            'T'    //This must match the char after the "MSFT100"
#define STRING_MSFT              L"MSFT100T"

#define MS_OS_DESCRIPTOR_ENABLE        0

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

