/********************************************************************************************************
 * @file     putchar.c 
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

#ifndef WIN32

#include "tl_common.h"
#include "application/print/putchar.h"
#include "application/usbstd/usbhw.h"
#include "drivers.h"


#ifndef UART_PRINT_DEBUG_ENABLE
#define UART_PRINT_DEBUG_ENABLE                    			0
#endif

#ifndef USB_PRINT_DEBUG_ENABLE
#define USB_PRINT_DEBUG_ENABLE                    			0
#endif


#if (USB_PRINT_DEBUG_ENABLE)
#define USB_PRINT_TIMEOUT	 10		//  about 10us at 30MHz
#define USB_SWIRE_BUFF_SIZE  248	// 256 - 8

#define USB_EP_IN  		(USB_EDP_PRINTER_IN  & 0X07)	//  from the point of HOST 's view,  IN is the printf out
#define USB_EP_OUT  	(USB_EDP_PRINTER_OUT & 0X07)

int usb_putc(int c) {
	int i = 0;
	while(i ++ < USB_PRINT_TIMEOUT){
		if(!(reg_usb_ep8_fifo_mode & FLD_USB_ENP8_FULL_FLAG)){
			reg_usb_ep_dat(USB_EP_IN) = (unsigned char)c;
			return c;
		}
	}
	return -1;
}

static inline void swire_set_clock(unsigned char div){
	reg_swire_clk_div = div;
}

static int swire_is_init = 0;
void swire_init(){
#if(USB_SOMATIC_ENABLE)
    //  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
#else
//	r_usb.ep_adr[USB_EP_IN] = r_usb.ep_adr[USB_EP_OUT] = 0;
	reg_usb_ep_ptr(USB_EP_IN) = reg_usb_ep_ptr(USB_EP_OUT) = 0;
	reg_usb_ep8_send_max = 64;				// 32 * 8 == 256

	//swire_set_clock(2);

#endif
}

int swire_putc(int c) {
#if(USB_SOMATIC_ENABLE)
    //  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
#else
	if(!swire_is_init){
		swire_init();
		swire_is_init = 1;
	}
	int i = 0;
	while(i ++ < USB_PRINT_TIMEOUT){
		if(reg_usb_ep_ptr(USB_EP_IN) - reg_usb_ep_ptr(USB_EP_OUT) <= USB_SWIRE_BUFF_SIZE){	//  not full
			reg_usb_ep_dat(USB_EP_IN) = (unsigned char)c;
			return c;
		}
	}
#endif
	return -1;
}
#endif

#if (UART_PRINT_DEBUG_ENABLE)

#ifndef		BIT_INTERVAL
#define		BIT_INTERVAL		(16000000/PRINT_BAUD_RATE)
#endif

_attribute_ram_code_
int uart_putc(char byte) //GPIO simulate uart print func
{
	unsigned char  j = 0;
	unsigned int t1 = 0,t2 = 0;

	//REG_ADDR8(0x582+((DEBUG_INFO_TX_PIN>>8)<<3)) &= ~(DEBUG_INFO_TX_PIN & 0xff) ;//Enable output

	unsigned int  pcTxReg = (0x583+((DEBUG_INFO_TX_PIN>>8)<<3));//register GPIO output
	unsigned char tmp_bit0 = read_reg8(pcTxReg) & (~(DEBUG_INFO_TX_PIN & 0xff));
	unsigned char tmp_bit1 = read_reg8(pcTxReg) | (DEBUG_INFO_TX_PIN & 0xff);
	unsigned char bit[10] = {0};

	bit[0] = tmp_bit0;
	bit[1] = (byte & 0x01)? tmp_bit1 : tmp_bit0;
	bit[2] = ((byte>>1) & 0x01)? tmp_bit1 : tmp_bit0;
	bit[3] = ((byte>>2) & 0x01)? tmp_bit1 : tmp_bit0;
	bit[4] = ((byte>>3) & 0x01)? tmp_bit1 : tmp_bit0;
	bit[5] = ((byte>>4) & 0x01)? tmp_bit1 : tmp_bit0;
	bit[6] = ((byte>>5) & 0x01)? tmp_bit1 : tmp_bit0;
	bit[7] = ((byte>>6) & 0x01)? tmp_bit1 : tmp_bit0;
	bit[8] = ((byte>>7) & 0x01)? tmp_bit1 : tmp_bit0;
	bit[9] = tmp_bit1;

	//unsigned char r = irq_disable();
	t1 = read_reg32(0x740);
	for(j = 0;j<10;j++)
	{
		t2 = t1;
		while(t1 - t2 < BIT_INTERVAL){
			t1  = read_reg32(0x740);
		}
		write_reg8(pcTxReg,bit[j]);        //send bit0
	}
	//irq_restore(r);

	return byte;
}
#endif

int putchar(int c){
#if (UART_PRINT_DEBUG_ENABLE)
	return uart_putc((char)c);
#elif (USB_PRINT_DEBUG_ENABLE)
	if(reg_usb_host_conn){
		swire_is_init = 0;		// should re-init swire if connect swire again
		return usb_putc((char)c);
	}else{
		return swire_putc((char)c);
	}
#endif
return 0;
}

#endif

