/********************************************************************************************************
 * @file     usbhw.h 
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


/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif

enum {
	USB_EDP_PRINTER_IN = 8, // endpoint 8 is alias of enpoint 0,  becareful.  // default hw buf len = 64
	USB_EDP_MOUSE = 2,			// default hw buf len = 8
	USB_EDP_KEYBOARD_IN = 1,	// default hw buf len = 8
	USB_EDP_KEYBOARD_OUT = 3,	// default hw buf len = 16
	USB_EDP_AUDIO_IN = 4,		// default hw buf len = 64
	USB_EDP_PRINTER_OUT = 5,	// default hw buf len = 64
	USB_EDP_SPEAKER = 6,		// default hw buf len = 16
	USB_EDP_MIC = 7,			// default hw buf len = 16
#if(USB_GAMEPAD_ENABLE)
	USB_EDP_GAMEPAD_IN = 3,		// default hw buf len = 64
	USB_EDP_GAMEPAD_OUT = 5,		// default hw buf len = 64
#endif
	USB_EDP_MS_IN = USB_EDP_PRINTER_IN,		// mass storage
	USB_EDP_MS_OUT = USB_EDP_PRINTER_OUT,
	USB_EDP_SOMATIC_IN = USB_EDP_AUDIO_IN,		//  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
	USB_EDP_SOMATIC_OUT = USB_EDP_PRINTER_OUT,
    USB_EDP_CDC_IN = 4,
    USB_EDP_CDC_OUT = 5,
};

// #defined in the standard spec
enum {
	USB_HID_AUDIO       	= 2,
	USB_HID_MOUSE       	= 1,
	USB_HID_KB_MEDIA    	= 3,// media
	USB_HID_KB_SYS      	= 4,// system : power,sleep,wakeup
#if(USB_GAMEPAD_ENABLE)
	USB_HID_GAMEPAD			= 5,// somatic sensor,  may have many report ids
#endif
	USB_HID_SOMATIC			= 5,// somatic sensor,  may have many report ids
};

void usbhw_disable_manual_interrupt(int m);
void usbhw_enable_manual_interrupt(int m);
void usbhw_write_ep(u32 ep, u8 * data, int len);
void usbhw_write_ctrl_ep_u16(u16 v);
u16 usbhw_read_ctrl_ep_u16(void);

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

