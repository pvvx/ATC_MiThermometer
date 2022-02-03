/********************************************************************************************************
 * @file     StdRequestType.h 
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
#include "tl_common.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif


#define CONTROL_REQTYPE_TYPE       		0x60
#define CONTROL_REQTYPE_DIRECTION  		0x80
#define CONTROL_REQTYPE_RECIPIENT  		0x1F
#define REQDIR_HOSTTODEVICE        		(0<<7)
#define REQDIR_DEVICETOHOST        		(1<<7)
#define REQTYPE_STANDARD           		(0<<5)
#define REQTYPE_CLASS              		(1<<5)
#define REQTYPE_VENDOR             		(2<<5)
#define REQREC_DEVICE              		(0<<0)
#define REQREC_INTERFACE           		(1<<0)
#define REQREC_ENDPOINT            		(2<<0)
#define REQREC_OTHER               		(3<<0)
#define FEATURE_SELFPOWERED_ENABLED     (1<<0)
#define FEATURE_REMOTE_WAKEUP_ENABLED   (1<<1)

typedef struct
{
	u8  RequestType;
	u8  Request;
	u16 Value;
	u16 Index;
	u16 Length;
}USB_Request_Hdr_t;

enum USB_Control_Request_t
{
	REQ_GetStatus,
	REQ_ClearFeature,
	REQ_SetFeature = 3,
	REQ_SetAddress = 5,
	REQ_GetDescriptor,
	REQ_SetDescriptor,
	REQ_GetConfiguration,
	REQ_SetConfiguration,
	REQ_GetInterface,
	REQ_SetInterface,
	REQ_SynchFrame,
};


enum USB_Feature_Selectors_t
{
	FEATURE_SEL_EndpointHalt,
	FEATURE_SEL_DeviceRemoteWakeup,
	FEATURE_SEL_TestMode,
};

#if defined(__cplusplus)
	}
#endif



