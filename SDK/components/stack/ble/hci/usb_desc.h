/********************************************************************************************************
 * @file     usb_desc.h 
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
#pragma once

#include <application/usbstd/CDCClassCommon.h>

enum {
	BTUSB_USB_STRING_LANGUAGE = 0,
	BTUSB_USB_STRING_VENDOR,
	BTUSB_USB_STRING_PRODUCT,
	BTUSB_USB_STRING_SERIAL,
};

typedef struct {
	USB_Descriptor_Configuration_Hdr_t Config;
	USB_Descriptor_Interface_Association_t intfa;

	USB_Descriptor_Interface_t intf0;
	USB_Descriptor_Endpoint_t irq_in;
	USB_Descriptor_Endpoint_t bulk_in;
	USB_Descriptor_Endpoint_t bulk_out;

	USB_Descriptor_Interface_t intf1_0;
	USB_Descriptor_Endpoint_t iso_in;
	USB_Descriptor_Endpoint_t iso_out;

	USB_Descriptor_Interface_t intf1_1;
	USB_Descriptor_Endpoint_t iso_in1;
	USB_Descriptor_Endpoint_t iso_out1;

	USB_Descriptor_Interface_t intf_prn;
	USB_Descriptor_Endpoint_t endp0;
	USB_Descriptor_Endpoint_t endp1;
} BTUSB_Descriptor_Configuration_with_Printer_t;

typedef struct {
	USB_Descriptor_Configuration_Hdr_t Config;
	USB_Descriptor_Interface_Association_t intfa;

	USB_Descriptor_Interface_t intf0;
	USB_Descriptor_Endpoint_t irq_in;
	USB_Descriptor_Endpoint_t bulk_in;
	USB_Descriptor_Endpoint_t bulk_out;

	USB_Descriptor_Interface_t intf1_0;
	USB_Descriptor_Endpoint_t iso_in;
	USB_Descriptor_Endpoint_t iso_out;

	USB_Descriptor_Interface_t intf1_1;
	USB_Descriptor_Endpoint_t iso_in1;
	USB_Descriptor_Endpoint_t iso_out1;
} BTUSB_Descriptor_Configuration_t;

u8* btusb_usbdesc_get_language(void);
u8* btusb_usbdesc_get_vendor(void);
u8* btusb_usbdesc_get_product(void);
u8* btusb_usbdesc_get_serial(void);
u8* btusb_usbdesc_get_device(void);
u8* btusb_usbdesc_get_configuration(void);
int btusb_usbdesc_get_configuration_size(void);

void btusb_select_cdc_device (int en);

///////////////////////////////////////////////////////////////////////////////
//typedef void (*usb_bulk_out_callback_t)(u8 *p, int n, int offset);

void myusb_bulk_drv_init (void *p);	//initialization
int myusb_bulk_in (u8 *p, int n);		//send status
int myusb_bulk_proc ();				//called in main loop
void myusb_bulk_out_command (u8 *p, int n, int offset);

#define CMD_GET_VERSION                 0 //0x00
#define CMD_SELECT_DPIMPL               16//0x10
#define CMD_SET_TCK_FREQUENCY           17//0x11
#define CMD_GET_TCK_FREQUENCY           18//0x12
#define CMD_MEASURE_MAX_TCK_FREQ        21//0x15
#define CMD_MEASURE_RTCK_RESPONSE       22//0x16
#define CMD_TAP_SHIFT                   23//0x17
#define CMD_SET_TAPHW_STATE             32//0x20
#define CMD_GET_TAPHW_STATE             33//0x21
#define CMD_TGPWR_SETUP                 34//0x22



//---------------  CDC ----------------------------------

/** Endpoint number of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPNUM         2
/** Endpoint number of the CDC device-to-host data IN endpoint. */
#define CDC_TX_EPNUM                   4
/** Endpoint number of the CDC host-to-device data OUT endpoint. */
#define CDC_RX_EPNUM                   5
/** Size in bytes of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPSIZE        8
/** Size in bytes of the CDC data IN and OUT endpoints. */
#define CDC_TXRX_EPSIZE                64

enum {
    USB_STRING_LANGUAGE = 0,
    USB_STRING_VENDOR,
    USB_STRING_PRODUCT,
    USB_STRING_SERIAL,
};

// interface id
typedef enum {
    USB_INTF_CDC_CCI,
    USB_INTF_CDC_DCI,
    USB_INTF_MAX,
} USB_INTF_ID_E;

typedef struct {
    // CDC Control Interface
    USB_CDC_Descriptor_FunctionalHeader_t    CDC_Functional_Header;
    USB_CDC_Descriptor_FunctionalACM_t       CDC_Functional_ACM;
    USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_Union;
    USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_CallManagement;
    USB_Descriptor_Endpoint_t                CDC_NotificationEndpoint;

    // CDC Data Interface
    USB_Descriptor_Interface_t               CDC_DCI_Interface;
    USB_Descriptor_Endpoint_t                CDC_DataOutEndpoint;
    USB_Descriptor_Endpoint_t                CDC_DataInEndpoint;
} USB_CDC_Descriptor_t;

typedef struct {
    USB_Descriptor_Configuration_Hdr_t Config;
    USB_Descriptor_Interface_Association_t cdc_iad;
    USB_Descriptor_Interface_t cdc_interface;
    USB_CDC_Descriptor_t cdc_descriptor;
} USB_Descriptor_Configuration_t;
