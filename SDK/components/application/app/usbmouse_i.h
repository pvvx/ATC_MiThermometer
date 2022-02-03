/********************************************************************************************************
 * @file     usbmouse_i.h 
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

#include <application/app/usbmouse.h>

#include "../usbstd/usb.h"
#include "../usbstd/usbhw.h"
#include "../usbstd/usbhw_i.h"
#include "../../vendor/common/default_config.h"
/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 */
/* debug note: */
/*
 0x85, 0x01, //Report ID (1) keyboard
 0x85, 0x02, //report ID 02 mouse
 是否需要配置这个 , 1,2定义是否正确
 */
static const USB_Descriptor_HIDReport_Datatype_t mouse_report_desc[] = {
#if 0
	//每行开始的第一字节为该条目的前缀，前缀的格式为：
	//D7~D4：bTag。D3~D2：bType；D1~D0：bSize。以下分别对每个条目注释。

	//这是一个全局（bType为1）条目，选择用途页为普通桌面Generic Desktop Page(0x01)
	//后面跟一字节数据（bSize为1），后面的字节数就不注释了，
	//自己根据bSize来判断。
	HID_RPT_USAGE_PAGE(8, 0x01), /* Generic Desktop */

	//这是一个局部（bType为2）条目，说明接下来的应用集合用途用于鼠标
    HID_RPT_USAGE(8, 0x02)		, /* Mouse */

	//这是一个主条目（bType为0）条目，开集合，后面跟的数据0x01表示
	//该集合是一个应用集合。它的性质在前面由用途页和用途定义为
	//普通桌面用的鼠标。
    HID_RPT_COLLECTION(8, 0x01)		, /* Application */

    HID_RPT_REPORT_ID(8, USB_HID_MOUSE)		, /*Report ID*/

    HID_RPT_USAGE_PAGE(8, 0x09)		, /* Button */

	//这是一个局部条目，说明用途的最小值为1。实际上是鼠标左键。
	// 1 is mouse left button,2 is mouse right button,3 is central buuton
    HID_RPT_USAGE_MINIMUM(8, 0x01)		,
	//这是一个局部条目，说明用途的最大值
    HID_RPT_USAGE_MAXIMUM(8, 0x05),

	//这是一个全局条目，说明返回的数据的逻辑值（就是我们返回的数据域的值啦）
	//最小为0。因为我们这里用Bit来表示一个数据域，因此最小为0，最大为1。
    HID_RPT_LOGICAL_MINIMUM(8, 0x00)		,
	//这是一个全局条目，说明逻辑值最大为1。
    HID_RPT_LOGICAL_MAXIMUM(8, 0x01),

	//这是一个全局条目，说明每个数据域的长度为1个bit。
	HID_RPT_REPORT_SIZE(8, 0x01),
	//这是一个全局条目，说明total button 数量为5个。
    HID_RPT_REPORT_COUNT(8, 0x05), /* debug note: 3->5*/

	//这是一个主条目，说明有3个长度为1bit的数据域（数量和长度
	//由前面的两个全局条目所定义）用来做为输入，
	//属性为：Data,Var,Abs。Data表示这些数据可以变动，Var表示
	//这些数据域是独立的，每个域表示一个意思。Abs表示绝对值。
	//这样定义的结果就是，第一个数据域bit0表示按键1（左键）是否按下，
	//第二个数据域bit1表示按键2（右键）是否按下，第三个数据域bit2表示
	//按键3（中键）是否按下。
    HID_RPT_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
	//这是一个全局条目，说明每个数据域的长度为
	HID_RPT_REPORT_SIZE(8, 0x03), /* debug note: 5->3*/
	//这是一个全局条目，说明数据域数量为1个
    HID_RPT_REPORT_COUNT(8, 0x01),

	//这是一个主条目，输入用，由前面两个全局条目可知，长度为3bit，
	//数量为1个。它的属性为常量（即返回的数据一直是0）。
	//这个只是为了凑齐一个字节（前面用了3个bit）而填充的一些数据
	//而已，所以它是没有实际用途的。
    HID_RPT_INPUT(8, HID_IOF_CONSTANT),

	//这是一个局部条目。说明用途为指针集合
    HID_RPT_USAGE(8, 0x01), /* Pointer */

	//这是一个主条目，开集合，后面跟的数据0x00表示该集合是一个
	//物理集合，用途由前面的局部条目定义为指针集合。
    HID_RPT_COLLECTION(8, 0x00), /* Physical */

	//这是一个全局条目，选择用途页为普通桌面Generic Desktop Page(0x01)
    HID_RPT_USAGE_PAGE(8, 0x01),  /* Generic Desktop */

	//这是一个局部条目，说明用途为X轴
    HID_RPT_USAGE(8, 0x30), /* Usage X */

	//这是一个局部条目，说明用途为Y轴
    HID_RPT_USAGE(8, 0x31), /* Usage Y */

	//下面两个为全局条目，说明返回的逻辑最小和最大值。
	//因为鼠标指针移动时，通常是用相对值来表示的，
	//相对值的意思就是，当指针移动时，只发送移动量。
	//往右移动时，X值为正；往下移动时，Y值为正。
	//对于滚轮，当滚轮往上滚时，值为正。
    HID_RPT_LOGICAL_MINIMUM(8, 0x81), //     LOGICAL_MINIMUM (-127)
    HID_RPT_LOGICAL_MAXIMUM(8, 0x7f), //     LOGICAL_MAXIMUM (127)
	//这是一个全局条目，说明数据域的长度。如果使用 16，就可以表示绝对坐标
    HID_RPT_REPORT_SIZE(8, 0x08),
    HID_RPT_REPORT_COUNT(8, 0x02),
	//这是一个主条目。它说明这三个8bit的数据域是输入用的，
	//属性为：Data,Var,Rel。Data说明数据是可以变的，Var说明
	//这些数据域是独立的，即第一个8bit表示X轴，第二个8bit表示
	//Y轴，第三个8bit表示滚轮。Rel表示这些值是相对值。
    HID_RPT_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

    HID_RPT_USAGE(8, 0x38),  /* Usage Wheel */
    HID_RPT_LOGICAL_MINIMUM(8, 0x81),	 //LOGICAL_MINIMUM (-127)
    HID_RPT_LOGICAL_MAXIMUM(8, 0x7f),	 //LOGICAL_MAXIMUM (127)
    HID_RPT_REPORT_SIZE(8, 0x08),		 //REPORT_SIZE (8)
    HID_RPT_REPORT_COUNT(8, 0x01),		//REPORT_COUNT (1)
    HID_RPT_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

    HID_RPT_END_COLLECTION(0),
    HID_RPT_END_COLLECTION(0),
        
    HID_RPT_USAGE_PAGE(8, 0x0c),             //global, usage page (follow 1 bytes) consumer page
    HID_RPT_USAGE(8, 0x01),                  //local,  usage ID 01  Consumer Control
    HID_RPT_COLLECTION(8, 0x01),             //main, collection
    HID_RPT_REPORT_ID(8, USB_HID_KB_MEDIA),  //Report ID
    HID_RPT_REPORT_SIZE(8, 0x10),            //global, report size 16 bits
    HID_RPT_REPORT_COUNT(8, 0x02),           //global, report count 2
    HID_RPT_LOGICAL_MINIMUM(8, 0x01),        //global, min  0x01
    HID_RPT_LOGICAL_MAXIMUM(16, 0x02ff),     //global, max  0x028c
    HID_RPT_USAGE_MINIMUM(8, 0x01),          //local, min   0x01
    HID_RPT_USAGE_MAXIMUM(16, 0x02ff),       //local, max    0x28c
    HID_RPT_INPUT(8, HID_IOF_ABSOLUTE),      //main,  input data varible, absolute
    HID_RPT_END_COLLECTION(0),               //main, end collection

	HID_RPT_USAGE_PAGE(8, 0x01),             //gobal,  USAGE_PAGE 1 (Generic Desktop)
	HID_RPT_USAGE(8, 0x80),                  //local, usage ID 0x80 system control
	HID_RPT_COLLECTION(8, 0x01),             //main conllection
    HID_RPT_REPORT_ID(8, USB_HID_KB_SYS),    //Report ID
	HID_RPT_REPORT_SIZE(8, 0x01),            //global, report size 2
	HID_RPT_REPORT_COUNT(8, 0x03),           //report count  1
	HID_RPT_LOGICAL_MINIMUM(8, 0x00),        //global min 01
    HID_RPT_LOGICAL_MAXIMUM(8, 0x01),        //gobal, max 3
	HID_RPT_USAGE(8, 0x81),					//local usage ID 0x81 system power down
    HID_RPT_USAGE(8, 0x82),                  //local usage ID 0x82 system sleep
    HID_RPT_USAGE(8, 0x83),                  //local usage ID 0x83 system wakeup
    HID_RPT_INPUT(8, HID_IOF_ABSOLUTE|HID_IOF_NO_PREFERRED_STATE|HID_IOF_NULLSTATE),
    HID_RPT_REPORT_SIZE(8, 0x05),            //global report size 6
	HID_RPT_REPORT_COUNT(8, 0x01),			//report count	1
    HID_RPT_INPUT(8, HID_IOF_CONSTANT|HID_IOF_VARIABLE),
    HID_RPT_END_COLLECTION(0),               //end of collection

#else

	//这是一个全局（bType为1）条目，选择用途页为普通桌面Generic Desktop Page(0x01)
	//后面跟一字节数据（bSize为1），后面的字节数就不注释了，
	//自己根据bSize来判断。
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)

	//这是一个局部（bType为2）条目，说明接下来的应用集合用途用于鼠标
	0x09, 0x02, // USAGE (Mouse)

	//这是一个主条目（bType为0）条目，开集合，后面跟的数据0x01表示
	//该集合是一个应用集合。它的性质在前面由用途页和用途定义为
	//普通桌面用的鼠标。
	0xa1, 0x01, // COLLECTION (Application)

	0x85, USB_HID_MOUSE, //report ID 01

	//这是一个局部条目。说明用途为指针集合
	0x09, 0x01, //   USAGE (Pointer)

	//这是一个主条目，开集合，后面跟的数据0x00表示该集合是一个
	//物理集合，用途由前面的局部条目定义为指针集合。
	0xa1, 0x00, //   COLLECTION (Physical)


	//这是一个全局条目，选择用途页为按键（Button Page(0x09)）
	0x05, 0x09, //     USAGE_PAGE (Button)

	//这是一个局部条目，说明用途的最小值为1。实际上是鼠标左键。
	// 1 is mouse left button,2 is mouse right button,3 is central buuton
	0x19, 0x01, //     USAGE_MINIMUM (Button 1)

	//这是一个局部条目，说明用途的最大值为3。实际上是鼠标中键。
	0x29, 0x05, //     USAGE_MAXIMUM (Button 5)

	//这是一个全局条目，说明返回的数据的逻辑值（就是我们返回的数据域的值啦）
	//最小为0。因为我们这里用Bit来表示一个数据域，因此最小为0，最大为1。
	0x15, 0x00, //     LOGICAL_MINIMUM (0)

	//这是一个全局条目，说明逻辑值最大为1。
	0x25, 0x01, //     LOGICAL_MAXIMUM (1)

	//这是一个全局条目，说明total button 数量为5个。
	0x95, 0x05, //     REPORT_COUNT (3)
	//这是一个全局条目，说明每个数据域的长度为1个bit。
	0x75, 0x01, //     REPORT_SIZE (1)

	//这是一个主条目，说明有5个长度为1bit的数据域（数量和长度
	//由前面的两个全局条目所定义）用来做为输入，
	//属性为：Data,Var,Abs。Data表示这些数据可以变动，Var表示
	//这些数据域是独立的，每个域表示一个意思。Abs表示绝对值。
	//这样定义的结果就是，第一个数据域bit0表示按键1（左键）是否按下，
	//第二个数据域bit1表示按键2（右键）是否按下，第三个数据域bit2表示
	//按键3（中键）是否按下。
	0x81, 0x02, //     INPUT (Data,Var,Abs)

	//这是一个全局条目，说明数据域数量为1个
	0x95, 0x01, //     REPORT_COUNT (1)
	//这是一个全局条目，说明每个数据域的长度为3bit。
	0x75, 0x03, //     REPORT_SIZE (3)


	//这是一个主条目，输入用，由前面两个全局条目可知，长度为3bit，
	//数量为1个。它的属性为常量（即返回的数据一直是0）。
	//这个只是为了凑齐一个字节（前面用了3个bit）而填充的一些数据
	//而已，所以它是没有实际用途的。
	0x81, 0x01, //     INPUT (Cnst,Var,Abs)


	//这是一个全局条目，选择用途页为普通桌面Generic Desktop Page(0x01)
	0x05, 0x01, //     USAGE_PAGE (Generic Desktop)

	//这是一个局部条目，说明用途为X轴
	0x09, 0x30, //     USAGE (X)

	//这是一个局部条目，说明用途为Y轴
	0x09, 0x31, //     USAGE (Y)
	//下面两个为全局条目，说明返回的逻辑最小和最大值。
	//因为鼠标指针移动时，通常是用相对值来表示的，
	//相对值的意思就是，当指针移动时，只发送移动量。
	//往右移动时，X值为正；往下移动时，Y值为正。
	//对于滚轮，当滚轮往上滚时，值为正。
	0x15, 0x81, //     LOGICAL_MINIMUM (-127)
	0x25, 0x7f, //     LOGICAL_MAXIMUM (127)

	//这是一个全局条目，说明数据域的长度为8bit。
	0x75, 0x08, //     REPORT_SIZE (16)

	//这是一个全局条目，说明数据域的个数为2个。
	0x95, 0x02, //     REPORT_COUNT (2)

	//这是一个主条目。它说明这三个8bit的数据域是输入用的，
	//属性为：Data,Var,Rel。Data说明数据是可以变的，Var说明
	//这些数据域是独立的，即第一个8bit表示X轴，第二个8bit表示
	//Y轴，第三个8bit表示滚轮。Rel表示这些值是相对值。
	0x81, 0x06, //     INPUT (Data,Var,Rel)

	//这是一个局部条目，说明用途为滚轮
	0x09, 0x38, //     USAGE (Wheel)
	0x15, 0x81, //LOGICAL_MINIMUM (-127)
	0x25, 0x7f, //LOGICAL_MAXIMUM (127)
	0x75, 0x08, //REPORT_SIZE (16)
	0x95, 0x01, //REPORT_COUNT (1)
	0x81, 0x06, //INPUT (Data,Var,Rel)


	//下面这两个主条目用来关闭前面的集合用。
	//我们开了两个集合，所以要关两次。bSize为0，所以后面没数据。
	0xc0, //   END_COLLECTION
	0xc0, // END_COLLECTION

#if (ONEKEY_WEB==0)
	// begin of media key
	0x05,0x0c,     //global, usage page (follow 1 bytes) consumer page
	0x09,0x01,     //local,  usage ID 01  Consumer Control
	0xA1,0x01,     //main, collection
	0x85,USB_HID_KB_MEDIA,     //global, report ID 0x03
	0x75,0x10,     //global, report size 16 bits
	0x95,0x02,     //global, report count 2
	0x15,0x01,     //global, min  0x01
	0x26,0x9c,0x02,  //global, max  0x29c
#if CHIP_EOP_ERROR
	0x19,0x01,     //local, min   0x01
	0x2a,0xff,0x02,  //local, max    0x2ff
#else
	0x19,0x01,     //local, min   0x01
	0x2a,0x8c,0x02,  //local, max    0x28c
#endif
	0x81,0x00,     //main,  input data varible, absolute
	0xc0,        //main, end collection

	0x05,0x01,     //gobal,  USAGE_PAGE 1 (Generic Desktop)
	0x09,0x80,     //local, usage ID 0x80 system control
	0xa1,0x01,     //main conllection
	0x85,USB_HID_KB_SYS,     //global report ID 0x4
	0x75,0x02,     //global, report size 2
	0x95,0x01,     //report count  1
	0x15,0x01,     //global min 01
	0x25,0x03,     //gobal, max 3
	0x09,0x82,     //local usage ID 0x82 system sleep
	0x09,0x81,     //local usage ID 0x81 system power down
	0x09,0x83,     //local usage ID 0x83 system wakeup
	0x81,0x60,     //main, input data, var, abs, No Prefer, NULL state
	0x75,0x06,     //global report size 6
	0x81,0x03,     //main input, constant, array
	0xc0,        //end of collection
	// end of media key
#endif
#endif
    //need Jensen's help: report ID 5
    HID_RPT_USAGE_PAGE(8, 0x01),     //global,  USAGE_PAGE 1 (Generic Desktop)
	0x09,0x00,     //usage undefined
	0xa1,0x01,     //main collection
	0x85,0x05,     //global report ID 0x5
	0x06,0x00,0xff,  //global usage page
	0x09,0x01,     //local,  usage ID 01  Consumer Control
	0x15,0x81,     //global min 81
	0x25,0x7f,     //global, max 7f
	0x75,0x08,     //global, report size 8
	0x95,0x07,     //report count  7
	0xb1,0x02,     //feature (data, var, abs)
	HID_RPT_END_COLLECTION(0),         //main, end collection
};

static inline u8* usbmouse_get_report_desc(void) {
	return (u8*) (mouse_report_desc);
}

static inline u16 usbmouse_get_report_desc_size(void) {
	return sizeof(mouse_report_desc);
}

