/********************************************************************************************************
 * @file     stdDescriptors.h 
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
#include "drivers.h"

//#include "../mcu/compiler.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif

#define NO_DESCRIPTOR                     (0)
#define USB_CONFIG_POWER_MA(mA)           ((mA) >> 1)
#define USB_STRING_LEN(UnicodeChars)      (sizeof(USB_Descriptor_Hdr_t) + ((UnicodeChars) << 1))
#define LANGUAGE_ID_ENG                   (0x0409)
#define USB_CONFIG_ATTR_REMOTEWAKEUP      (0x20)
#define USB_CONFIG_ATTR_SELFPOWERED       (0x40)
#define USB_CONFIG_ATTR_RESERVED          (0x80)
#define ENDPOINT_ATTR_NO_SYNC             (0<<2)
#define ENDPOINT_ATTR_ASYNC               (1<<2)
#define ENDPOINT_ATTR_ADAPTIVE            (2<<2)
#define ENDPOINT_ATTR_SYNC                (3<<2)
#define ENDPOINT_USAGE_DATA               (0<<4)
#define ENDPOINT_USAGE_FEEDBACK           (1<<4)
#define ENDPOINT_USAGE_IMPLICIT_FEEDBACK  (2<<4)

enum USB_DescriptorTypes_t
{
	DTYPE_Device = 0x01,
	DTYPE_Configuration,
	DTYPE_String,
	DTYPE_Interface,
	DTYPE_Endpoint,
	DTYPE_DeviceQualifier,
	DTYPE_Other,
	DTYPE_InterfacePower,
	DTYPE_InterfaceAssociation = 0x0B,
	DTYPE_CSInterface = 0x24,
	DTYPE_CSEndpoint = 0x25,
};

enum USB_Descriptor_ClassSubclassProtocol_t
{
	USB_CSCP_NoDeviceClass = 0x00,
	USB_CSCP_NoDeviceSubclass = 0x00,
	USB_CSCP_NoDeviceProtocol = 0x00,
	USB_CSCP_IADDeviceProtocol = 0x01,
	USB_CSCP_IADDeviceSubclass = 0x02,
	USB_CSCP_IADDeviceClass = 0xEF,
	USB_CSCP_VendorSpecificClass = 0xFF,
	USB_CSCP_VendorSpecificSubclass = 0xFF,
	USB_CSCP_VendorSpecificProtocol = 0xFF,
};

typedef struct
{
    u8 Size;
    u8 Type;
}USB_Descriptor_Hdr_t;

typedef struct
{
    u8 Length;
    u8 DescriptorType;
}USB_StdDescriptor_Hdr_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u16 USBSpecification;
	u8 Class;
	u8 SubClass;
	u8 Protocol;
	u8 Endpoint0Size;
	u16 VendorID;
	u16 ProductID;
	u16 ReleaseNumber;
	u8 ManufacturerStrIndex;
	u8 ProductStrIndex;
	u8 SerialNumStrIndex;
	u8 NumberOfConfigurations;
}USB_Descriptor_Device_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u16 BcdUSB;
	u8 DeviceClass;
	u8 DeviceSubClass;
	u8 DeviceProtocol;
	u8 MaxPacketSize0;
	u16 IdVendor;
	u16 IdProduct;
	u16 BcdDevice;
	u8 Manufacturer;
	u8 Product;
	u8 SerialNumber;
	u8 NumConfigurations;
}USB_StdDescriptor_Device_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u16 USBSpecification;
	u8 Class;
	u8 SubClass;
	u8 Protocol;
	u8 Endpoint0Size;
	u8 NumberOfConfigurations;
	u8 Reserved;
}USB_Descriptor_DeviceQualifier_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u16 BcdUSB;
	u8 DeviceClass;
	u8 DeviceSubClass;
	u8 DeviceProtocol;
	u8 MaxPacketSize0;
	u8 NumConfigurations;
	u8 Reserved;
}USB_StdDescriptor_DeviceQualifier_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u16 TotalConfigurationSize;
	u8 TotalInterfaces;
	u8 ConfigurationNumber;
	u8 ConfigurationStrIndex;
	u8 ConfigAttributes;
	u8 MaxPowerConsumption;
}USB_Descriptor_Configuration_Hdr_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u16 TotalLength;
	u8 NumInterfaces;
	u8 ConfigurationValue;
	u8 Configuration;
	u8 MAttributes;
	u8 MaxPower;
}USB_StdDescriptor_Configuration_Hdr_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 InterfaceNumber;
	u8 AlternateSetting;
	u8 TotalEndpoints;
	u8 Class;
	u8 SubClass;
	u8 Protocol;
	u8 InterfaceStrIndex;
}USB_Descriptor_Interface_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 InterfaceNum;
	u8 AlternateSetting;
	u8 NumberEndpoints;
	u8 InterfaceClass;
	u8 InterfaceSubclass;
	u8 InterfaceProtocol;
	u8 Interface;
}USB_StdDescriptor_Interface_t;


typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 FirstInterfaceIndex;
	u8 TotalInterfaces;
	u8 Class;
	u8 SubClass;
	u8 Protocol;
	u8 IADStrIndex;
}USB_Descriptor_Interface_Association_t;


typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u8 FirstInterface;
	u8 InterfaceCount;
	u8 FunctionClass;
	u8 FunctionSubClass;
	u8 FunctionProtocol;
	u8 Function;
}USB_StdDescriptor_Interface_Association_t;


typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 EndpointAddress;
	u8 Attributes;
	u16 EndpointSize;
	u8 PollingIntervalMS;
}USB_Descriptor_Endpoint_t;

typedef struct
{
	u8  Length;
	u8  DescriptorType;
	u8  EndpointAddress;
	u8  MAttributes;
	u16 MaxPacketSize;
	u8  Interval;
}USB_StdDescriptor_Endpoint_t;


#ifndef __GNUC__
#pragma warning(push)
#pragma warning(disable:4200)	// none standard zero length array
#endif

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	wchar_t UnicodeString[];
}USB_Descriptor_String_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	wchar_t bString[];
}USB_StdDescriptor_String_t;

#ifndef __GNUC__
#pragma warning(pop)
#endif


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif

