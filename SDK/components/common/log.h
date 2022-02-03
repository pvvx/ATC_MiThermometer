/********************************************************************************************************
 * @file     log.h 
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

#include "log_id.h"
#include "application/print/u_printf.h"


#ifndef LOG_IN_RAM
#define LOG_IN_RAM   0
#endif

#define		LOG_MASK_BEGIN		0x40
#define		LOG_MASK_END		0x00
#define		LOG_MASK_TGL		0xC0
#define		LOG_MASK_DAT		0x80

static inline void swire2usb_init (void) {
	U8_SET(0x800128, 0x00);
	U8_SET(0x80012d, 0);
	U8_SET(0x80013b, 32);
}

static inline void usb_log_init (void) {
	U8_SET(0x80013c, 0x40);
	U8_SET(0x80013d, 0x09);
}

#if (__LOG_RT_ENABLE__)
void log_write_data(int id, int type, u32 dat);
void log_task_begin(int id);
void log_task_end(int id);
void log_event(int id);
void log_data(int id, u32 dat);

#define	LOG_TICK(id,e)	do{log_task_begin(id); e; log_task_end(id);}while(0)

#define LOG(x, s,...) printf("(%s:%d)"  x "\r\n" , __FUNCTION__, __LINE__, ## s)

#else
#define log_write_data(id, type, dat)
#define log_task_begin(id)
#define log_task_end(id)
#define log_event(id)
#define log_data(id, dat)
#define	LOG_TICK(id,e)			do{e;}while (0)

#define LOG(x, s,...)

#endif

