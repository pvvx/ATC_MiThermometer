/********************************************************************************************************
 * @file     log.c 
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

#include "../tl_common.h"
#include "log.h"
#include "drivers.h"


#ifndef __LOG_RT_ENABLE__
#define __LOG_RT_ENABLE__		0
#endif


#if (__LOG_RT_ENABLE__)

#ifndef WIN32	
// the printf will mess the realtime log
// the realtime log use usb printer endpoint
//STATIC_ASSERT((!__DEBUG_PRINT__) && (USB_PRINTER_ENABLE));
#endif

#ifdef WIN32	
#include <stdio.h>
#include <io.h>
FILE *hFile = 0;
#endif

/*
	ID == -1 is invalid
	if you want to shut down logging a specified id,  assigne -1 to it
*/
#if LOG_IN_RAM
_attribute_ram_code_
#endif
static void log_write(int id, int type, u32 dat){
	if(-1 == id) return;
	u8 r = irq_disable();
	reg_usb_ep8_dat = (dat & 0xff);
	reg_usb_ep8_dat = ((dat >> 8) & 0xff);
	reg_usb_ep8_dat = ((dat >> 16)& 0xff);
	reg_usb_ep8_dat = (type)|(id);
	irq_restore(r);

#ifdef WIN32		// write to file directly
	if(!hFile){		// file not open yet
		foreach(i, 10){
			char fn[20];
			sprintf(fn, "vcd_%d.log", i);
			if(access(fn, 0)){		// file not exist
				hFile = fopen(fn, "w");
				assert(hFile);
				if(!hFile){
					while(1);
				}
				break;
			}
		}
		if(!hFile){
			hFile = fopen("vcd_0.log", "w");
		}
	}
	if(hFile){
		char temp[4];
		temp[0] = (dat & 0xff);
		temp[1] = ((dat >> 8) & 0xff);
		temp[2] = ((dat >> 16)& 0xff);
		temp[3] = (type)|(id);
		fwrite(&temp[0], 1, 4, hFile);
	}
#endif	
}

#if LOG_IN_RAM
_attribute_ram_code_
#endif
void log_task_begin(int id){
	log_write(id,LOG_MASK_BEGIN,clock_time());
}


#if LOG_IN_RAM
_attribute_ram_code_
#endif
void log_task_end(int id){
	log_write(id,LOG_MASK_END,clock_time());
}


#if LOG_IN_RAM
_attribute_ram_code_
#endif
void log_event(int id){
	log_write(id,LOG_MASK_TGL,clock_time());
}


#if LOG_IN_RAM
_attribute_ram_code_
#endif
void log_data(int id, u32 dat){
	log_write(id,LOG_MASK_DAT,dat);
}

#endif

