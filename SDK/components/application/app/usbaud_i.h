/********************************************************************************************************
 * @file     usbaud_i.h 
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


#include "drivers.h"
#include "usbaud.h"
#include "../usbstd/AudioClassCommon.h"
#include "../usbstd/usbhw.h"
#include "../usbstd/usbhw_i.h"
#include "../../common/config/user_config.h"


#if 0
static const USB_Descriptor_HIDReport_Datatype_t usbaud_report_desc[] = {
	HID_RI_USAGE_PAGE(8, 0x0c), /* Consumer Page */
	HID_RI_USAGE(8, 0x01)		, /* Consumer Controls */
    HID_RI_COLLECTION(8, 0x01)		, /* Application */

    HID_RI_REPORT_ID(8, 0x01)		, /*Report ID*/

    HID_RI_LOGICAL_MINIMUM(8, 0x00)		,
    HID_RI_LOGICAL_MAXIMUM(8, 0x01),

    HID_RI_USAGE(8, 0xcd),  /* Play/Pause (toggle) */
	HID_RI_USAGE(8, 0xb5)		, /* Next Track */
	HID_RI_USAGE(8, 0xb6)		, /* Previous Track */
	HID_RI_USAGE(8, 0xb7)		, /* Stop */

    HID_RI_REPORT_SIZE(8, 0x01)		,
    HID_RI_REPORT_COUNT(8, 0x04),
    HID_RI_INPUT(8, HID_IOF_VARIABLE),
    HID_RI_LOGICAL_MINIMUM(8, 0x00),
    HID_RI_LOGICAL_MAXIMUM(8, 0x01),
    
    HID_RI_USAGE(8, 0xb0),  /* Play */
    HID_RI_USAGE(8, 0xb1)		, /* Pause */
    HID_RI_USAGE(8, 0xb3)		, /* Fast Forward */
    HID_RI_USAGE(8, 0xb4)		, /* Rewind */

    HID_RI_REPORT_SIZE(8, 0x01)		,
    HID_RI_REPORT_COUNT(8, 0x04),
    HID_RI_INPUT(8, HID_IOF_NO_PREFERRED_STATE | HID_IOF_VARIABLE),
    HID_RI_LOGICAL_MINIMUM(8, 0x00),
    HID_RI_LOGICAL_MAXIMUM(8, 0x01),
    HID_RI_USAGE(8, 0xe9),
    HID_RI_USAGE(8, 0xea),
    HID_RI_USAGE(8, 0xe2),
    HID_RI_REPORT_SIZE(8, 0x01),
    HID_RI_REPORT_COUNT(8, 0x03),
    HID_RI_INPUT(8, HID_IOF_NO_PREFERRED_STATE | HID_IOF_VARIABLE),
    HID_RI_REPORT_SIZE(8, 0x05),
    HID_RI_REPORT_COUNT(8, 0x01),
    HID_RI_INPUT(8, HID_IOF_CONSTANT),
    HID_RI_END_COLLECTION(0),
	HID_RI_USAGE_PAGE(16, 0xffa0),
    HID_RI_USAGE(8, 0x01),
    HID_RI_COLLECTION(8, 0x01),  /* Application */

    HID_RI_REPORT_ID(8, 0x02)		, /*Report ID*/

    HID_RI_USAGE(8, 0x01)		,
    HID_RI_LOGICAL_MINIMUM(8, 0x00),
	HID_RI_LOGICAL_MAXIMUM(16, 0x00ff),
    HID_RI_REPORT_SIZE(8, 0x08),
    HID_RI_REPORT_COUNT(8, 0x02),
    HID_RI_OUTPUT(8, HID_IOF_DATA),
    HID_RI_USAGE(8, 0x02),  /* mouse? */

    HID_RI_REPORT_SIZE(8, 0x08)		,
    HID_RI_REPORT_COUNT(8, 0x02),
    HID_RI_INPUT(8, HID_IOF_DATA),
    HID_RI_END_COLLECTION(0),  };

#endif

//USB HID报告描述符的定义
static const USB_Descriptor_HIDReport_Datatype_t usbaud_report_desc[] = {
		0x05, 0x0c, 0x09, 0x01, 0xa1, 0x01,
		0x85, 0x01, 0x15, 0x00, 0x25, 0x01, 0x09, 0xcd, 0x09, 0xb5, 0x09, 0xb6,
		0x09, 0xb7, 0x75, 0x01, 0x95, 0x04, 0x81, 0x02, 0x15, 0x00, 0x25, 0x01,
		0x09, 0xb0, 0x09, 0xb1, 0x09, 0xb3, 0x09, 0xb4, 0x75, 0x01, 0x95, 0x04,
		0x81, 0x22, 0x15, 0x00, 0x25, 0x01, 0x09, 0xe9, 0x09, 0xea, 0x09, 0xe2,
		0x75, 0x01, 0x95, 0x03, 0x81, 0x22, 0x75, 0x05, 0x95, 0x01, 0x81, 0x01,
		0xc0, 0x06, 0xa0, 0xff, 0x09, 0x01, 0xa1, 0x01, 0x85, 0x02, 0x09, 0x01,
		0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x02, 0x91, 0x00, 0x09,
		0x02, 0x75, 0x08, 0x95, 0x02, 0x81, 0x00, 0xc0, };

static inline u8* usbaud_get_report_desc(void) {
	return (u8*) (usbaud_report_desc);
}

static inline u16 usbaud_get_report_desc_size(void) {
	return sizeof(usbaud_report_desc);
}


