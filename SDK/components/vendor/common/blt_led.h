/********************************************************************************************************
 * @file     blt_led.h 
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
/*
 * blt_led.h
 *
 *  Created on: 2016-1-29
 *      Author: Administrator
 */

#ifndef BLT_LED_H_
#define BLT_LED_H_

#include "tl_common.h"



#ifndef BLT_APP_LED_ENABLE
#define BLT_APP_LED_ENABLE				0
#endif


//led management
typedef struct{
	unsigned short onTime_ms;
	unsigned short offTime_ms;

	unsigned char  repeatCount;  //0xff special for long on(offTime_ms=0)/long off(onTime_ms=0)
	unsigned char  priority;     //0x00 < 0x01 < 0x02 < 0x04 < 0x08 < 0x10 < 0x20 < 0x40 < 0x80
} led_cfg_t;

typedef struct {
	unsigned char  isOn;
	unsigned char  polar;
	unsigned char  repeatCount;
	unsigned char  priority;


	unsigned short onTime_ms;
	unsigned short offTime_ms;

	unsigned int gpio_led;
	unsigned int startTick;
}device_led_t;

extern device_led_t device_led;

#define  DEVICE_LED_BUSY	(device_led.repeatCount)

extern void led_proc(void);
extern void device_led_init(u32 gpio,u8 polarity);
int device_led_setup(led_cfg_t led_cfg);

static inline void device_led_process(void)
{
#if (BLT_APP_LED_ENABLE)
	if(DEVICE_LED_BUSY){
		led_proc();
	}
#endif
}




#endif /* BLT_LED_H_ */
