/********************************************************************************************************
 * @file     PrinterClassCommon.h 
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
	

#define PRNT_PORTSTATUS_NOTERROR	BIT(3)
#define PRNT_PORTSTATUS_SELECT		BIT(4)
#define PRNT_PORTSTATUS_PAPEREMPTY	BIT(5)

enum PRNT_Descriptor_ClassSubclassProtocol_t
{
	PRNT_CSCP_PrinterClass = 0x07,
	PRNT_CSCP_PrinterSubclass = 0x01,
	PRNT_CSCP_BidirectionalProtocol = 0x02,
};

enum PRNT_ClassRequests_t
{
	PRNT_REQ_GetDeviceID,
	PRNT_REQ_GetPortStatus,
	PRNT_REQ_SoftReset,
};

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif

