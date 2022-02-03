/********************************************************************************************************
 * @file     usbcdc.h 
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

#include <application/usbstd/CDCClassCommon.h>
#include <application/usbstd/CDCClassDevice.h>
#include "../common/types.h"
#include "../common/static_assert.h"
#include "../common/bit.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    extern "C" {
#endif


typedef void ( *cdc_handlerFn_t)( u8* pData);

typedef struct {
    u8 len;
    u8 data[1];
} usbcdc_txBuf_t;


typedef enum usbcdc_sts_e {
    // success = 0
    USB_BUSY = 1,
    USB_MULTIBLOCK,
} usbcdc_sts_t;


void CDC_Device_ProcessControlRequest(u8 bRequest, u16 wValue, u16 wIndex, u16 wLength);

usbcdc_sts_t usbcdc_sendData(u8* buf, u8 len);
u8   usbcdc_sendBulkData(void);

u8   usbcdc_isAvailable(void);
u8*  usbcdc_getData(void);
void usbcdc_init(void);
void usbcdc_setCB(cdc_handlerFn_t rxFunc, cdc_handlerFn_t txCb);
void usbcdc_setRxBuf(u8 *buf);




/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    }
#endif
