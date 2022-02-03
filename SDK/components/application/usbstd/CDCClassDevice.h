/********************************************************************************************************
 * @file     CDCClassDevice.h 
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

/* Includes: */
#include <application/usbstd/CDCClassCommon.h>
#include "../common/types.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    extern "C" {
#endif

typedef struct
{
	struct
	{
		u8 ControlInterfaceNumber;
		u8 DataINEndpointNumber;
		u16 DataINEndpointSize;
		bool DataINEndpointDoubleBank;
		u8 DataOUTEndpointNumber;
		u16 DataOUTEndpointSize;
		bool DataOUTEndpointDoubleBank;
		u8 NotificationEndpointNumber;
		u16 NotificationEndpointSize;
		bool NotificationEndpointDoubleBank;
	} Config;

	struct
	{
		struct
		{
			u16 HostToDevice;
			u16 DeviceToHost;
		} ControlLineStates;

		CDC_LineEncoding_t LineEncoding;
	} State;
} USB_ClassInfo_CDC_Device_t;


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif
