/********************************************************************************************************
 * @file     USBController.h 
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


/**If Isochronous endpoint,
Bits 3..2 = Synchronisation Type (Iso Mode)
00 = No Synchonisation
01 = Asynchronous
10 = Adaptive
11 = Synchronous
Bits 5..4 = Usage Type (Iso Mode)
00 = Data Endpoint
01 = Feedback Endpoint
10 = Explicit Feedback Data Endpoint
11 = Reserved
*/
#define EP_SYNC_TYPE_NO_SYNC                0
#define EP_SYNC_TYPE_ASYN                   1
#define EP_SYNC_TYPE_ADAPTIVE               2
#define EP_SYNC_TYPE_SYNC                   3

#define EP_USAGE_TYPE_DATA                  0
#define EP_USAGE_TYPE_FEEDBACK              1
#define EP_USAGE_TYPE_FEEDBACK_DATA         2
#define EP_USAGE_TYPE_RSV                   3


#define ENDPOINT_DIR_MASK				    BIT(7)
#define ENDPOINT_DIR_OUT				    0
#define ENDPOINT_DIR_IN 				    BIT(7)
#define EP_TYPE_MASK					    3
#define EP_TYPE_CONTROL 				    0
#define EP_TYPE_ISOCHRONOUS 			    1
#define EP_TYPE_BULK					    2
#define EP_TYPE_INTERRUPT				    3

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

