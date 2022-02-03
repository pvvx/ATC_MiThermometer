/********************************************************************************************************
 * @file     usbhw.c 
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
#include "tl_common.h"
#include "drivers.h"


#include "usbhw.h"
#include "usbhw_i.h"

// Endpont8 is the alias of endpoint0
void usbhw_disable_manual_interrupt(int m) {
	SET_FLD(reg_ctrl_ep_irq_mode, m);
}

void usbhw_enable_manual_interrupt(int m) {
	CLR_FLD(reg_ctrl_ep_irq_mode, m);
}

void usbhw_write_ep(u32 ep, u8 * data, int len) {
	assert(ep < 8);
	reg_usb_ep_ptr(ep) = 0;

	foreach(i,len){
		reg_usb_ep_dat(ep) = data[i];
	}
	reg_usb_ep_ctrl(ep) = FLD_EP_DAT_ACK;		// ACK
}

// handy help function
void usbhw_write_ctrl_ep_u16(u16 v){
	usbhw_write_ctrl_ep_data(v & 0xff);
	usbhw_write_ctrl_ep_data(v >> 8);
}

u16 usbhw_read_ctrl_ep_u16(void){
	u16 v = usbhw_read_ctrl_ep_data();
	return (usbhw_read_ctrl_ep_data() << 8) | v;
} 

