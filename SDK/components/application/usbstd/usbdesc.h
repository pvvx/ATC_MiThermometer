/********************************************************************************************************
 * @file     usbdesc.h 
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

#include <application/usbstd/AudioClassCommon.h>
#include <application/usbstd/CDCClassCommon.h>
#include <application/usbstd/HIDClassCommon.h>
#include <application/usbstd/PrinterClassCommon.h>
#include <application/usbstd/USBController.h>
#include "../../common/config/user_config.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

// interface id
typedef enum {

#if USB_CDC_ENABLE
	USB_INTF_CDC_CCI,
	USB_INTF_CDC_DCI,
#endif

#if (USB_PRINTER_ENABLE)
	USB_INTF_PRINTER,
#endif
#if (USB_SPEAKER_ENABLE || USB_MIC_ENABLE)
	USB_INTF_AUDIO_CONTROL,
#endif
#if (USB_SPEAKER_ENABLE)
	USB_INTF_SPEAKER,
#endif
#if (USB_MIC_ENABLE)
	USB_INTF_MIC,
#endif
#if(0)
	USB_INTF_AUDIO_HID,		//  use for volumn control, mute, next, prev track,  move to mouse hid
#endif
#if USB_KEYBOARD_ENABLE
	USB_INTF_KEYBOARD,
#endif
#if USB_MOUSE_ENABLE
	USB_INTF_MOUSE,
#endif
#if USB_SOMATIC_ENABLE
	USB_INTF_SOMATIC,
#endif
	USB_INTF_MAX,
} USB_INTF_ID_E;

enum {
	USB_SPEAKER_FEATURE_UNIT = USB_SPEAKER_ENABLE,
	USB_MIC_FEATURE_UNIT = USB_MIC_ENABLE,
};

enum {
	USB_SPEAKER_INPUT_TERMINAL_ID = 1,
	USB_SPEAKER_FEATURE_UNIT_ID,
	USB_SPEAKER_OUTPUT_TERMINAL_ID,
	USB_MIC_INPUT_TERMINAL_ID,
	USB_MIC_FEATURE_UNIT_ID,
	USB_MIC_OUTPUT_TERMINAL_ID,
};

enum {
	USB_SPEAKER_FEATURE_UNIT_SOURCE_ID = 1,
	USB_SPEAKER_OUTPUT_TERMINAL_SOURCE_ID,
	USB_MIC_FEATURE_UNIT_SOURCE_ID = 4,
	USB_MIC_OUTPUT_TERMINAL_SOURCE_ID,
};

#if(USB_MIC_ENABLE)
#define USB_MIC_CHANNELS_LEN   (MIC_CHANNLE_COUNT*(MIC_SAMPLE_RATE*MIC_RESOLUTION_BIT/1000/8))
#endif

enum {
	USB_AUDIO_FORMAT_UNKNOWN = 0,
	USB_AUDIO_FORMAT_PCM,
	USB_AUDIO_FORMAT_ADPCM,
	USB_AUDIO_FORMAT_IEEE_FLOAT,
	USB_AUDIO_FORMAT_IBM_CVSD,
	USB_AUDIO_FORMAT_ALAW,
	USB_AUDIO_FORMAT_MULAW,
	USB_AUDIO_FORMAT_WMAVOICE9,
	USB_AUDIO_FORMAT_OKI_ADPCM,
	USB_AUDIO_FORMAT_DVI_ADPCM,
	USB_AUDIO_FORMAT_IMA_ADPCM,
	USB_AUDIO_FORMAT_MEDIASPACE_ADPCM,
	USB_AUDIO_FORMAT_SIERRA_ADPCM,
	USB_AUDIO_FORMAT_G723_ADPCM,
	USB_AUDIO_FORMAT_DIGISTD,
	USB_AUDIO_FORMAT_DIGIFIX,
	USB_AUDIO_FORMAT_DIALOGIC_OKI_ADPCM,
	USB_AUDIO_FORMAT_MEDIAVISION_ADPCM,
	USB_AUDIO_FORMAT_YAMAHA_ADPCM,
	USB_AUDIO_FORMAT_SONARC,
	USB_AUDIO_FORMAT_DSPGROUP_TRUESPEECH,
	USB_AUDIO_FORMAT_ECHOSC1,
	USB_AUDIO_FORMAT_AUDIOFILE_AF36,
	USB_AUDIO_FORMAT_APTX,
	USB_AUDIO_FORMAT_AUDIOFILE_AF10,
	USB_AUDIO_FORMAT_DOLBY_AC2,
	USB_AUDIO_FORMAT_GSM610,
	USB_AUDIO_FORMAT_MSNAUDIO,
	USB_AUDIO_FORMAT_ANTEX_ADPCME,
	USB_AUDIO_FORMAT_CONTROL_RES_VQLPC,
	USB_AUDIO_FORMAT_DIGIREAL,
	USB_AUDIO_FORMAT_DIGIADPCM,
	USB_AUDIO_FORMAT_CONTROL_RES_CR10,
	USB_AUDIO_FORMAT_NMS_VBXADPCM,
	USB_AUDIO_FORMAT_CS_IMAADPCM,
	USB_AUDIO_FORMAT_ECHOSC3,
	USB_AUDIO_FORMAT_ROCKWELL_ADPCM,
	USB_AUDIO_FORMAT_ROCKWELL_DIGITALK,
	USB_AUDIO_FORMAT_XEBEC,
	USB_AUDIO_FORMAT_G721_ADPCM,
	USB_AUDIO_FORMAT_G728_CELP,
	USB_AUDIO_FORMAT_MPEG,
	USB_AUDIO_FORMAT_MPEGLAYER3,
	USB_AUDIO_FORMAT_CIRRUS,
	USB_AUDIO_FORMAT_ESPCM,
	USB_AUDIO_FORMAT_VOXWARE,
	USB_AUDIO_FORMAT_WAVEFORMAT_CANOPUS_ATRAC,
	USB_AUDIO_FORMAT_G726_ADPCM,
	USB_AUDIO_FORMAT_G722_ADPCM,
	USB_AUDIO_FORMAT_DSAT,
	USB_AUDIO_FORMAT_DSAT_DISPLAY,
	USB_AUDIO_FORMAT_SOFTSOUND,
	USB_AUDIO_FORMAT_RHETOREX_ADPCM,
	USB_AUDIO_FORMAT_MSAUDIO1,
	USB_AUDIO_FORMAT_WMAUDIO2,
	USB_AUDIO_FORMAT_WMAUDIO3,
	USB_AUDIO_FORMAT_WMAUDIO_LOSSLESS,
	USB_AUDIO_FORMAT_CREATIVE_ADPCM,
	USB_AUDIO_FORMAT_CREATIVE_FASTSPEECH8,
	USB_AUDIO_FORMAT_CREATIVE_FASTSPEECH10,
	USB_AUDIO_FORMAT_QUARTERDECK,
	USB_AUDIO_FORMAT_FM_TOWNS_SND,
	USB_AUDIO_FORMAT_BTV_DIGITAL,
	USB_AUDIO_FORMAT_OLIGSM,
	USB_AUDIO_FORMAT_OLIADPCM,
	USB_AUDIO_FORMAT_OLICELP,
	USB_AUDIO_FORMAT_OLISBC,
	USB_AUDIO_FORMAT_OLIOPR,
	USB_AUDIO_FORMAT_LH_CODEC,
	USB_AUDIO_FORMAT_NORRIS,
};

enum {
	USB_HID_COUNTRY_NONE = 0,
	USB_HID_COUNTRY_ARABIC,
	USB_HID_COUNTRY_BELGIAN,
	USB_HID_COUNTRY_CANADA_BI,
	USB_HID_COUNTRY_CANADA_FR,
	USB_HID_COUNTRY_CZECH_REPUBLIC,
	USB_HID_COUNTRY_DANISH,
	USB_HID_COUNTRY_FINNISH,
	USB_HID_COUNTRY_FRENCH,
	USB_HID_COUNTRY_GERMAN,
	USB_HID_COUNTRY_GREEK,
	USB_HID_COUNTRY_HEBREW,
	USB_HID_COUNTRY_HUNGARY,
	USB_HID_COUNTRY_INTERNATIONAL_ISO,
	USB_HID_COUNTRY_ITALIAN,
	USB_HID_COUNTRY_JAPAN_KATAKANA,
	USB_HID_COUNTRY_KOREAN,
	USB_HID_COUNTRY_LATIN_AMERICAN,
	USB_HID_COUNTRY_NETHERLANDS,
	USB_HID_COUNTRY_NORWEGIAN,
	USB_HID_COUNTRY_PERSIAN,
	USB_HID_COUNTRY_POLAND,
	USB_HID_COUNTRY_PORTUGUESE,
	USB_HID_COUNTRY_RUSSIA,
	USB_HID_COUNTRY_SLOVAKIA,
	USB_HID_COUNTRY_SPANISH,
	USB_HID_COUNTRY_SWEDISH,
	USB_HID_COUNTRY_SWISS_FRENCH,
	USB_HID_COUNTRY_SWISS_GERMAN,
	USB_HID_COUNTRY_SWITZERLAND,
	USB_HID_COUNTRY_TAIWAN,
	USB_HID_COUNTRY_TURKISH_Q,
	USB_HID_COUNTRY_UK,
	USB_HID_COUNTRY_US,
	USB_HID_COUNTRY_YUGOSLAVIA,
	USB_HID_COUNTRY_TURKISH_F,
};

enum {
	USB_STRING_LANGUAGE = 0,
	USB_STRING_VENDOR,
	USB_STRING_PRODUCT,
	USB_STRING_SERIAL,

	USB_STRING_MS_OS =0xee,
};

#if (USB_CDC_ENABLE)
/** Endpoint number of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPNUM         2

/** Endpoint number of the CDC device-to-host data IN endpoint. */
#ifndef		CDC_TX_EPNUM
#define CDC_TX_EPNUM                   4 ///3
#endif

/** Endpoint number of the CDC host-to-device data OUT endpoint. */
#define CDC_RX_EPNUM                   5///4

/** Size in bytes of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPSIZE        8

/** Size in bytes of the CDC data IN and OUT endpoints. */
#define CDC_TXRX_EPSIZE                64
#endif  /* USB_CDC_ENABLE */

typedef struct {
	USB_HID_Descriptor_HID_t audio_hid;
	USB_Descriptor_Endpoint_t audio_in_endpoint;
} USB_HID_Descriptor_HID_Audio_t;

typedef struct {
	USB_HID_Descriptor_HID_t mouse_hid;
	USB_Descriptor_Endpoint_t mouse_in_endpoint;
} USB_HID_Descriptor_HID_Mouse_t;

typedef struct {
	USB_HID_Descriptor_HID_t keyboard_hid;
	USB_Descriptor_Endpoint_t keyboard_in_endpoint;
} USB_HID_Descriptor_HID_Keyboard_t;

typedef struct {
	USB_HID_Descriptor_HID_t somatic_hid;
	USB_Descriptor_Endpoint_t somatic_in_endpoint;
	USB_Descriptor_Endpoint_t somatic_out_endpoint;
} USB_HID_Descriptor_HID_Somatic_t;

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

#if (USB_CDC_ENABLE)
#if 0
    // IAD0
    USB_Descriptor_Interface_Association_t cdc_iad;
#endif
    // CDC Interface
    USB_Descriptor_Interface_t  cdc_interface;
    USB_CDC_Descriptor_t  cdc_descriptor;
#endif

#if(USB_PRINTER_ENABLE)
	// printer HID Interface
	USB_Descriptor_Interface_t printer_interface;
	USB_Descriptor_Endpoint_t printer_in_endpoint;
#if(!USB_SOMATIC_ENABLE)		
	USB_Descriptor_Endpoint_t printer_out_endpoint;
#endif
#endif
#if (USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
	USB_Descriptor_Interface_t audio_control_interface;
#if (USB_MIC_ENABLE && USB_SPEAKER_ENABLE)
	USB_Audio_Descriptor_Interface_AC_TL_t audio_control_interface_ac;
#else
	USB_Audio_Descriptor_Interface_AC_t audio_control_interface_ac;
#endif
#endif	
#if (USB_SPEAKER_ENABLE)
	USB_Audio_Descriptor_InputTerminal_t speaker_input_terminal;
	USB_Audio_StdDescriptor_FeatureUnit_t speaker_feature_unit;
	USB_Audio_Descriptor_OutputTerminal_t speaker_output_terminal;
#endif
#if (USB_MIC_ENABLE)
	USB_Audio_Descriptor_InputTerminal_t mic_input_terminal;
	USB_Audio_Descriptor_FeatureUnit_Mic_t mic_feature_unit;
	USB_Audio_Descriptor_OutputTerminal_t mic_output_terminal;
#endif
#if (USB_SPEAKER_ENABLE)
	USB_Descriptor_Interface_t speaker_setting0;
	USB_Descriptor_Interface_t speaker_setting1;
	USB_Audio_Descriptor_Interface_AS_t speaker_audio_stream;
	USB_Audio_Descriptor_Format_t speaker_audio_format;
	USB_Audio_SampleFreq_t speaker_sample_rate;
	USB_Audio_Descriptor_StreamEndpoint_Std_t speaker_stream_endpoint;
	USB_Audio_Descriptor_StreamEndpoint_Spc_t speaker_stream_endpoint_spc;
#if(USB_AUDIO_441K_ENABLE)
	NOTE("Add 441k descriptor if USB_AUDIO_441K_ENABLE defined")
#endif
#endif
#if (USB_MIC_ENABLE)
	USB_Descriptor_Interface_t mic_setting0;
	USB_Descriptor_Interface_t mic_setting1;
	USB_Audio_Descriptor_Interface_AS_t mic_audio_stream;
	USB_Audio_Descriptor_Format_t mic_audio_format;
	USB_Audio_SampleFreq_t mic_sample_rate;
	USB_Audio_Descriptor_StreamEndpoint_Std_t mic_stream_endpoint;
	USB_Audio_Descriptor_StreamEndpoint_Spc_t mic_stream_endpoint_spc;
#endif
#if (0)
	// audio HID Interface
	USB_Descriptor_Interface_t audio_interface;
	USB_HID_Descriptor_HID_Audio_t audio_descriptor;
#endif	
#if (USB_KEYBOARD_ENABLE)
	// Keyboard HID Interface
	USB_Descriptor_Interface_t keyboard_interface;
	USB_HID_Descriptor_HID_Keyboard_t keyboard_descriptor;
#endif
#if (USB_MOUSE_ENABLE)
	// Mouse HID Interface
	USB_Descriptor_Interface_t mouse_interface;
	USB_HID_Descriptor_HID_Mouse_t mouse_descriptor;
#endif
#if (USB_SOMATIC_ENABLE)
	// SOMATIC HID Interface
	USB_Descriptor_Interface_t somatic_interface;
	USB_HID_Descriptor_HID_Somatic_t somatic_descriptor;
#endif
} USB_Descriptor_Configuration_t;

typedef struct {
	u32 dwLength;    // length, in bytes, of the complete extended compat ID descriptor
	u16 bcdVersion;	 // BCD	The descriptor¡¯s version number, in binary coded decimal (BCD) format
	u16 wIndex;      // An index that identifies the particular OS feature descriptor
	u8 bCount;	     //The number of custom property sections
	u8 RESERVED[7];  //Reserved

}USB_MS_OS_compatID_Header_t;

typedef struct {

	u8 bFirstInterfaceNumber;  //The interface or function number
	u8 RESERVED1;               //Reserved
	u8 compatibleID[8];        //The function¡¯s compatible ID
	u8 subCompatibleID[8];     //The function¡¯s subcompatible ID
	u8 RESERVED2[6];           //Reserved

}USB_MS_OS_compatID_Function_t;


typedef struct {
	USB_MS_OS_compatID_Header_t    compatID_Header;
	USB_MS_OS_compatID_Function_t  compatID_Function[];
}USB_MS_OS_compatID_t;


u8* usbdesc_get_language(void);
u8* usbdesc_get_vendor(void);
u8* usbdesc_get_product(void);
u8* usbdesc_get_serial(void);
u8* usbdesc_get_device(void);
u8* usbdesc_get_configuration(void);

#if(USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
u8* usbdesc_get_audio(void);
#endif

#if (USB_MOUSE_ENABLE)
u8* usbdesc_get_mouse(void);
#endif

#if (USB_KEYBOARD_ENABLE)
u8* usbdesc_get_keyboard(void);
#endif

#if (USB_SOMATIC_ENABLE)
u8* usbdesc_get_somatic(void);
#endif

#if (USB_CRC_ENABLE)
u8 *usbdesc_get_cdc(void);
#endif

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

