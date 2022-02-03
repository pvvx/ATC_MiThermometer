/********************************************************************************************************
 * @file     usbdesc.c 
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

#include <application/usbstd/usbdesc.h>
#include "tl_common.h"
#include "drivers.h"

#if (USB_MOUSE_ENABLE)
#include <application/app/usbmouse_i.h>
#endif

#if (USB_KEYBOARD_ENABLE)
#include <application/app/usbkb_i.h>
#endif

#if (USB_SOMATIC_ENABLE)
#include "../app/usbsomatic_i.h"
#endif

#if (USB_SPEAKER_ENABLE || USB_MIC_ENABLE)
#include <application/app/usbaud_i.h>
#endif

#if (USB_CDC_ENABLE)
#include <application/app/usbcdc_i.h>
#endif
//#include "usb.h"

// request parameters
/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t language_desc = { {
		sizeof(USB_Descriptor_Hdr_t) + 2, DTYPE_String },
		{ LANGUAGE_ID_ENG } };

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t vendor_desc = { { sizeof(USB_Descriptor_Hdr_t)
		+ sizeof(STRING_VENDOR) - 2, DTYPE_String }, // Header
		STRING_VENDOR };

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t product_desc = { {
		sizeof(USB_Descriptor_Hdr_t) + sizeof(STRING_PRODUCT) - 2,
		DTYPE_String }, // Header
		STRING_PRODUCT };

/** Serial number string. This is a Unicode string containing the device's unique serial number, expressed as a
 *  series of uppercase hexadecimal digits.
 */
const USB_Descriptor_String_t serial_desc = { { sizeof(USB_Descriptor_Hdr_t)
		+ sizeof(STRING_SERIAL) - 2, DTYPE_String }, // Header
		STRING_SERIAL };


#if (MS_OS_DESCRIPTOR_ENABLE)
const USB_Descriptor_String_t microsoft_OS_desc = { {
		sizeof(USB_Descriptor_Hdr_t) + sizeof(STRING_MSFT) - 2,
		DTYPE_String }, // Header
		STRING_MSFT };


const unsigned char OSFeatureDescriptor[] ={
        0x26,0x01, 0x00, 0x00,
        0x00,0x01,
		0x05,0x00,
		0x05,0x00,

	// DeviceIdleEnabled
        0x36,0x00, 0x00, 0x00,
        0x04,0x00, 0x00, 0x00,
		0x24, 0x00,

		0x44,0x00,0x65,0x00,0x76,0x00,0x69,0x00,0x63,0x00,0x65,0x00,0x49,0x00,0x64,0x00,
		0x6C,0x00,0x65,0x00,0x45,0x00,0x6E,0x00,0x61,0x00,0x62,0x00,0x6C,0x00,0x65,0x00,
		0x64,0x00,0x00,0x00,

		0x04,0x00,0x00,0x00,
		0x01,0x00,0x00,0x00,

    //DefaultIdleState
		0x34,0x00,0x00,0x00,
        0x04,0x00, 0x00, 0x00,
		0x22,0x00,

		0x44,0x00,	0x65,0x00,	0x66,0x00,	0x61,0x00,	0x75,0x00,
		0x6C,0x00,	0x74,0x00,	0x49,0x00,	0x64,0x00,	0x6C,0x00,
		0x65,0x00,	0x53,0x00,	0x74,0x00,	0x61,0x00,	0x74,0x00,
		0x65,0x00,	0x00,0x00,

		0x04,0x00,0x00,0x00,
		0x01,0x00,0x00,0x00,


		//default timeout value for selective suspend.
		0x38,0x00,0x00,0x00,
        0x04,0x00, 0x00, 0x00,
		0x26,0x00,

		0x44,0x00,0x65,0x00,0x66,0x00,0x61,0x00,
		0x75,0x00,0x6C,0x00,0x74,0x00,0x49,0x00,
		0x64,0x00,0x6C,0x00,0x65,0x00,0x54,0x00,
		0x69,0x00,0x6D,0x00,0x65,0x00,0x6F,0x00,
		0x75,0x00,0x74,0x00,
		0x00,0x00,

		0x04,0x00,0x00,0x00,
		0x88,0x13,0x00,0x00,

// user-enabled selective suspend.
		0x44,0x00,0x00,0x00,
        0x04,0x00, 0x00, 0x00,
		0x32,0x00,

		0x55,0x00,0x73,0x00,
		0x65,0x00,0x72,0x00,
		0x53,0x00,0x65,0x00,
		0x74,0x00,0x44,0x00,
		0x65,0x00,0x76,0x00,
		0x69,0x00,0x63,0x00,
		0x65,0x00,0x49,0x00,
		0x64,0x00,0x6C,0x00,
		0x65,0x00,0x45,0x00,
		0x6E,0x00,0x61,0x00,
		0x62,0x00,0x6C,0x00,
		0x65,0x00,0x64,0x00,
		0x00,0x00,

		0x04,0x00,0x00,0x00,
		0x01,0x00,0x00,0x00,

//user-enabled remote wake setting.

		0x36,0x00,0x00,0x00,
		0x04,0x00,0x00,0x00,
		0x24,0x00,
		0x53,0x00,0x79,0x00,0x73,0x00,0x74,0x00,0x65,0x00,0x6D,0x00,0x57,0x00,0x61,0x00,
		0x6B,0x00,0x65,0x00,0x45,0x00,0x6E,0x00,0x61,0x00,0x62,0x00,0x6C,0x00,0x65,0x00,
		0x64,0x00,0x00,0x00,

		0x04,0x00,0x00,0x00,
		0x01,0x00,0x00,0x00

};



const USB_MS_OS_compatID_t  OSFeatureDescriptor_compatID = {
		{
				sizeof(USB_MS_OS_compatID_Header_t)
				+ USB_KEYBOARD_ENABLE * sizeof(USB_MS_OS_compatID_Function_t)
				+ USB_MOUSE_ENABLE * sizeof(USB_MS_OS_compatID_Function_t),

				0x0100,
				0x0004,
				USB_KEYBOARD_ENABLE + USB_MOUSE_ENABLE,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		},
		{
#if USB_KEYBOARD_ENABLE
		{
				USB_INTF_KEYBOARD,
				 0x01,
				 'W', 'I', 'N', 'U' ,'S','B',  0x00,0x00,
				 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				 0x00,0x00,0x00,0x00,0x00,0x00,
		},
#endif

#if USB_MOUSE_ENABLE
		{
			 USB_INTF_MOUSE,
			 0x01,
			 'W', 'I', 'N', 'U' ,'S','B',  0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,
		},
#endif
		}
};
#if 0
const unsigned char OSFeatureDescriptor_compatID[] = {
		 0x28,0x00,0x00,0x00,
		 0x00,0x01,
		 0x04,0x00,
		 0x01,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 USB_INTF_MOUSE,
		 0x01,
		 'W', 'I', 'N', 'U' ,'S','B',  0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,
 };
#endif
#endif


#ifndef ID_PRODUCT
#define ID_PRODUCT	(ID_PRODUCT_BASE | (USB_PRINTER_ENABLE?(1<<0):0) | (USB_SPEAKER_ENABLE?(1<<1):0) | (USB_MIC_ENABLE?(1<<2):0)	\
	 | (USB_MOUSE_ENABLE?(1<<3):0) | (USB_KEYBOARD_ENABLE?(1<<4):0) | (USB_SOMATIC_ENABLE?(1<<5):0))
#endif

const USB_Descriptor_Device_t device_desc = { {
		sizeof(USB_Descriptor_Device_t), DTYPE_Device }, // Header
#if (MS_OS_DESCRIPTOR_ENABLE)
		0x0200, // USBSpecification, USB 2.0
#else
		0x0110, // USBSpecification, USB 1.1
#endif
#if (USB_CDC_ENABLE)
        CDC_CSCP_CDCClass, // Class
        USB_CSCP_NoDeviceSubclass, // SubClass
        USB_CSCP_NoDeviceProtocol, // Protocol
#else
        USB_CSCP_NoDeviceClass,
        USB_CSCP_NoDeviceSubclass, // SubClass
        USB_CSCP_NoDeviceProtocol, // Protocol
#endif
		8, // Endpoint0Size, Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64
		ID_VENDOR, // VendorID
#if USB_CDC_ENABLE
        0x8846,
#else
        ID_PRODUCT, // ProductID
#endif
		0x0100, // .ReleaseNumber
		USB_STRING_VENDOR, 	// .ManufacturerStrIndex
		USB_STRING_PRODUCT, // .ProductStrIndex
		3, 	// .SerialNumStrIndex, iSerialNumber
		1 };

const USB_Descriptor_Configuration_t
		configuration_desc = { { {
				sizeof(USB_Descriptor_Configuration_Hdr_t),
				DTYPE_Configuration }, // Length, type
				sizeof(USB_Descriptor_Configuration_t), // TotalLength: variable
				USB_INTF_MAX, // NumInterfaces
				1, // Configuration index
				NO_DESCRIPTOR, // Configuration String
				USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTEWAKEUP, // Attributes
				USB_CONFIG_POWER_MA(50) // MaxPower = 100mA
                },

#if (USB_CDC_ENABLE)
#if 0
                {
					// iad0
					{sizeof(USB_Descriptor_Interface_Association_t), DTYPE_InterfaceAssociation}, // Header
					0, // FirstInterfaceIndex
					2, // TotalInterface
					CDC_CSCP_CDCClass, // Class
					CDC_CSCP_ACMSubclass, // Subclass
					CDC_CSCP_ATCommandProtocol, // protocol
					NO_DESCRIPTOR  // IADStrIndex
				},
#endif


				{
					// cdc_interface
					
					{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
					USB_INTF_CDC_CCI, // InterfaceNumber
					0, // AlternateSetting
					1, // TotalEndpoints
					CDC_CSCP_CDCClass, // Class
					CDC_CSCP_ACMSubclass, // SubClass
					CDC_CSCP_ATCommandProtocol, // Protocol
					NO_DESCRIPTOR //InterfaceStrIndex
				},

				{
					

					// cdc_descriptor
					//CDC_Functional_Header
					{
						{sizeof(USB_CDC_Descriptor_FunctionalHeader_t), DTYPE_CSInterface}, // Header
						CDC_DSUBTYPE_CSInterface_Header, // Subtype
						0x0110 // CDCSpecification
					},

					// CDC_Functional_ACM =
					{
						{sizeof(USB_CDC_Descriptor_FunctionalACM_t), DTYPE_CSInterface}, // Header
						CDC_DSUBTYPE_CSInterface_ACM, // Subtype
						0x02 // Capabilities
					},

					// CDC_Functional_Union =
					{
						{sizeof(USB_CDC_Descriptor_FunctionalUnion_t), DTYPE_CSInterface}, // Header
						CDC_DSUBTYPE_CSInterface_Union, // Subtype
						0, // MasterInterfaceNumber
						1, // SlaveInterfaceNumber
					},

						
					// CDC_CallManagement =
					{
						{sizeof(USB_CDC_Descriptor_FunctionalUnion_t), DTYPE_CSInterface}, // Header
						CDC_DSUBTYPE_CSInterface_CallManagement, // Subtype
						0, // MasterInterfaceNumber
						1, // SlaveInterfaceNumber
					},

						

					// CDC_NotificationEndpoint =
					{
						{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
						(ENDPOINT_DIR_IN | CDC_NOTIFICATION_EPNUM), // EndpointAddress
						(EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes
						CDC_NOTIFICATION_EPSIZE, // EndpointSize
						0x40 // PollingIntervalMS
					},


					// CDC_DCI_Interface =
					{
						{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
						USB_INTF_CDC_DCI, // InterfaceNumber
						0, // AlternateSetting
						2, // TotalEndpoints
						CDC_CSCP_CDCDataClass, // Class
						CDC_CSCP_NoDataSubclass, // SubClass
						CDC_CSCP_NoDataProtocol, // Protocol
						NO_DESCRIPTOR // InterfaceStrIndex
					},


					// CDC_DataOutEndpoint =
					{
						{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
						(ENDPOINT_DIR_OUT | CDC_RX_EPNUM), // EndpointAddress
						(EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes
						CDC_TXRX_EPSIZE, // EndpointSize
						0x00 // PollingIntervalMS
					},

					// CDC_DataInEndpoint =
					{
						{sizeof(USB_Descriptor_Endpoint_t),  DTYPE_Endpoint}, // Header
						(ENDPOINT_DIR_IN | CDC_TX_EPNUM), // EndpointAddress
						(EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes
						CDC_TXRX_EPSIZE, // EndpointSize
						0x00 // PollingIntervalMS
					},
				},
#endif



#if(USB_PRINTER_ENABLE)
				// printer_interface
				{ { sizeof(USB_Descriptor_Interface_t), DTYPE_Interface },
						USB_INTF_PRINTER, 0, // AlternateSetting
				#if(USB_SOMATIC_ENABLE)
						1, // bNumEndpoints
				#else
						2, // bNumEndpoints
				#endif
						PRNT_CSCP_PrinterClass, // bInterfaceclass ->Printer
						PRNT_CSCP_PrinterSubclass, // bInterfaceSubClass -> Control
						PRNT_CSCP_BidirectionalProtocol,// bInterfaceProtocol
						NO_DESCRIPTOR // iInterface,  same as iProduct in USB_Descriptor_Device_t, or else not working
				},
				// printer_in_endpoint
				{ { sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint }, // length, bDescriptorType
						ENDPOINT_DIR_IN | USB_EDP_PRINTER_IN, // endpoint id
						EP_TYPE_BULK, // endpoint type
						0x0040, // wMaxPacketSize
						0 // bInterval
				}, 
#if(!USB_SOMATIC_ENABLE)		
				// printer_out_endpoint
				{ { sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint }, // length, bDescriptorType
						USB_EDP_PRINTER_OUT, // endpoint id
						EP_TYPE_BULK, // endpoint type
						0x0040, // wMaxPacketSize
						0 // polling bInterval. valid for iso or interrupt type
				},
#endif				
#endif						
#if (USB_MIC_ENABLE || USB_SPEAKER_ENABLE)
				// audio_control_interface
				{	{	sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_AUDIO_CONTROL, 0, // AlternateSetting
					0, // bNumEndpoints
					AUDIO_CSCP_AudioClass, // bInterfaceclass ->Printer
					AUDIO_CSCP_ControlSubclass, // bInterfaceSubClass -> Control
					AUDIO_CSCP_ControlProtocol, // bInterfaceProtocol
					NO_DESCRIPTOR // iInterface
				},
				// audio_control_interface_ac;
				{
#if (USB_MIC_ENABLE && USB_SPEAKER_ENABLE)
				{	sizeof(USB_Audio_Descriptor_Interface_AC_TL_t),DTYPE_CSInterface}, AUDIO_DSUBTYPE_CSInterface_Header, // Subtype
					{0x00, 0x01}, // ACSpecification, version == 1.0
				// debug note: TotalLength must less than  256
					{(sizeof(USB_Audio_Descriptor_Interface_AC_TL_t) + /*10*/
							sizeof(USB_Audio_Descriptor_InputTerminal_t) + /*12*/
							sizeof(USB_Audio_Descriptor_OutputTerminal_t) + /*9*/
							sizeof(USB_Audio_Descriptor_FeatureUnit_Mic_t) + /*9*/
							sizeof(USB_Audio_Descriptor_InputTerminal_t) + /*12*/
							sizeof(USB_Audio_Descriptor_OutputTerminal_t) + /*9*/
							sizeof(USB_Audio_StdDescriptor_FeatureUnit_t)/*10*/), 0}, 2, // InCollection
					USB_INTF_SPEAKER, USB_INTF_MIC
#else
					{sizeof(USB_Audio_Descriptor_Interface_AC_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_Header, // Subtype
					{0x00, 0x01}, // ACSpecification, version == 1.0
#if (USB_MIC_ENABLE)
				{(sizeof(USB_Audio_Descriptor_Interface_AC_t) + /*9*/
						sizeof(USB_Audio_Descriptor_InputTerminal_t) + /*12*/
						sizeof(USB_Audio_Descriptor_OutputTerminal_t) + /*9*/
						sizeof(USB_Audio_Descriptor_FeatureUnit_Mic_t) /*9*/),0},
				1,
				USB_INTF_MIC
#else
				{(sizeof(USB_Audio_Descriptor_Interface_AC_t) + /*9*/
						sizeof(USB_Audio_Descriptor_InputTerminal_t) + /*12*/
						sizeof(USB_Audio_Descriptor_OutputTerminal_t) + /*9*/
						sizeof(USB_Audio_StdDescriptor_FeatureUnit_t) /*10*/),0},

				1,
				USB_INTF_SPEAKER
#endif
#endif 
			},
#endif	
#if (USB_SPEAKER_ENABLE)
				// speaker_input_terminal
				{	{	sizeof(USB_Audio_Descriptor_InputTerminal_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_InputTerminal,
					USB_SPEAKER_INPUT_TERMINAL_ID, AUDIO_TERMINAL_STREAMING, 0, // AssociatedOutputTerminal
					2, // TotalChannels
					0x0003, // ChannelConfig
					0, // ChannelStrIndex
					NO_DESCRIPTOR},
				// speaker_feature_unit
				{	sizeof(USB_Audio_StdDescriptor_FeatureUnit_t), DTYPE_CSInterface,
					AUDIO_DSUBTYPE_CSInterface_Feature,
					USB_SPEAKER_FEATURE_UNIT_ID,
					USB_SPEAKER_FEATURE_UNIT_SOURCE_ID, 1, // bControlSize
					{	0x03, 0x00, 0x00}, // bmaControls
					NO_DESCRIPTOR},
				// speaker_output_terminal
				{	{	sizeof(USB_Audio_Descriptor_OutputTerminal_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_OutputTerminal,
					USB_SPEAKER_OUTPUT_TERMINAL_ID, AUDIO_TERMINAL_OUT_SPEAKER, 0, // AssociatedOutputTerminal
					USB_SPEAKER_OUTPUT_TERMINAL_SOURCE_ID, NO_DESCRIPTOR},
#endif
#if (USB_MIC_ENABLE)
				// mic_input_terminal
				{	{	sizeof(USB_Audio_Descriptor_InputTerminal_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_InputTerminal,
					USB_MIC_INPUT_TERMINAL_ID, AUDIO_TERMINAL_IN_MIC, 0, // AssociatedOutputTerminal
					1, // TotalChannels
					0x0001, // ChannelConfig
					0, // ChannelStrIndex
					NO_DESCRIPTOR},
				// mic_feature_unit
				{
					{	sizeof(USB_Audio_Descriptor_FeatureUnit_Mic_t),
						DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_Feature, USB_MIC_FEATURE_UNIT_ID,
					USB_MIC_FEATURE_UNIT_SOURCE_ID, 1, // bControlSize
					{	0x03, 0x00}, // bmaControls
					NO_DESCRIPTOR},
				// mic_output_terminal
				{	{	sizeof(USB_Audio_Descriptor_OutputTerminal_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_OutputTerminal,
					USB_MIC_OUTPUT_TERMINAL_ID, AUDIO_TERMINAL_STREAMING, 0, // AssociatedOutputTerminal
					USB_MIC_OUTPUT_TERMINAL_SOURCE_ID, NO_DESCRIPTOR},
#endif
#if (USB_SPEAKER_ENABLE)
				// speaker_setting0
				{	{	sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_SPEAKER,
					0, // AlternateSetting
					0, // bNumEndpoints
					AUDIO_CSCP_AudioClass, AUDIO_CSCP_AudioStreamingSubclass,
					AUDIO_CSCP_StreamingProtocol, NO_DESCRIPTOR},
				// speaker_setting1
				{	{	sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_SPEAKER,
					1, // AlternateSetting
					1, // bNumEndpoints
					AUDIO_CSCP_AudioClass, AUDIO_CSCP_AudioStreamingSubclass,
					AUDIO_CSCP_StreamingProtocol, NO_DESCRIPTOR},
				// speaker_audio_stream
				{	{	sizeof(USB_Audio_Descriptor_Interface_AS_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_General, 1, // TerminalLink #1 USB Streaming IT
					1, // FrameDelay
					{	USB_AUDIO_FORMAT_PCM & 0xff, (USB_AUDIO_FORMAT_PCM >> 8)
						& 0xff}},
				// speaker_audio_format
				{	{	sizeof(USB_Audio_Descriptor_Format_t)
						+ sizeof(USB_Audio_SampleFreq_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_FormatType, USB_AUDIO_FORMAT_PCM, 2, // Channels
					2, // SubFrameSize
					0x10, // BitsResolution
					1 // TotalDiscreteSampleRates
				},
				// speaker_sample_rate AUDIO_SAMPLE_FREQ
				{	0x80, 0xbb, 0x00},
				// speaker_stream_endpoint
				{	{
						{	sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t), DTYPE_Endpoint},
						USB_EDP_SPEAKER,
						EP_TYPE_ISOCHRONOUS | (EP_SYNC_TYPE_ADAPTIVE << 2) | (EP_USAGE_TYPE_DATA << 4), // Attributes ENDPOINT_ATTR_ASYNC
						0x00c0, // EndpointSize USB_MIC_CHANNELS_LEN
						1 // PollingIntervalMS
					}, 0, // Refresh
					0 // SyncEndpointNumber
				},
				// speaker_stream_endpoint_spc
				{
					{	sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t),
						DTYPE_CSEndpoint}, AUDIO_DSUBTYPE_CSInterface_General,
					AUDIO_EP_FULL_PACKETS_ONLY | AUDIO_EP_SAMPLE_FREQ_CONTROL, 0, // LockDelayUnits
					{	0, 0} // LockDelay
				},
#if(USB_AUDIO_441K_ENABLE)
				NOTE("Add 441k descriptor if USB_AUDIO_441K_ENABLE defined")
#endif
#endif
#if (USB_MIC_ENABLE)
				// mic_setting0
				{	{	sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_MIC,
					0, // AlternateSetting
					0, // bNumEndpoints
					AUDIO_CSCP_AudioClass, AUDIO_CSCP_AudioStreamingSubclass,
					AUDIO_CSCP_StreamingProtocol, NO_DESCRIPTOR
				},
				// mic_setting1
				{	{	sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_MIC,
					1, // AlternateSetting
					1, // bNumEndpoints
					AUDIO_CSCP_AudioClass, AUDIO_CSCP_AudioStreamingSubclass,
					AUDIO_CSCP_StreamingProtocol, NO_DESCRIPTOR
				},
				// mic_audio_stream
				{	{	sizeof(USB_Audio_Descriptor_Interface_AS_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_General, 6, // TerminalLink #6USB USB Streaming OT
					1, // FrameDelay
					{	USB_AUDIO_FORMAT_PCM & 0xff, (USB_AUDIO_FORMAT_PCM >> 8)& 0xff}
				},
				// mic_audio_format
				{	{	sizeof(USB_Audio_Descriptor_Format_t)
						+ sizeof(USB_Audio_SampleFreq_t), DTYPE_CSInterface},
					AUDIO_DSUBTYPE_CSInterface_FormatType, USB_AUDIO_FORMAT_PCM, // FormatType
					MIC_CHANNLE_COUNT, // Channels
					2, // SubFrameSize
					MIC_RESOLUTION_BIT, // BitsResolution
					1 // TotalDiscreteSampleRates
				},
				// mic_sample_rate
				{(MIC_SAMPLE_RATE & 0xff), (MIC_SAMPLE_RATE >> 8), 0x00},
				// mic_stream_endpoint
				{	{
						{	sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t), DTYPE_Endpoint}
						, ENDPOINT_DIR_MASK | USB_EDP_MIC,
						EP_TYPE_ISOCHRONOUS | (EP_SYNC_TYPE_SYNC << 2) | (EP_USAGE_TYPE_DATA << 4), // Attributes
						USB_MIC_CHANNELS_LEN, 1 // PollingIntervalMS
					},
					0, // Refresh
					0 // SyncEndpointNumber
				},
				// mic_stream_endpoint_spc
				{
					{	sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t),
						DTYPE_CSEndpoint}, AUDIO_DSUBTYPE_CSInterface_General,
						AUDIO_EP_SAMPLE_FREQ_CONTROL, 0, // LockDelayUnits
					{	0, 0} // LockDelay
				},
#endif
#if(0)
				// audio_interface
				{	
					{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_AUDIO_HID,
					0, // AlternateSetting
					1, // bNumEndpoints
					HID_CSCP_HIDClass, HID_CSCP_NonBootSubclass,
					HID_CSCP_NonBootProtocol, NO_DESCRIPTOR
				}, 
				{
					// audio_hid
					{	
						{sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID}, 0x0111, // HIDSpec
						USB_HID_COUNTRY_NONE, 1, // TotalReportDescriptors
						HID_DTYPE_Report, {sizeof(usbaud_report_desc), 0x00}, // HIDReportLength sizeof(Report)
					},
					// audio_in_endpoint
					{	
						{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},
						ENDPOINT_DIR_IN | USB_EDP_AUDIO,
						EP_TYPE_INTERRUPT, 0x0010, // EndpointSize
						1 // PollingIntervalMS
					}
				},
#endif	
#if (USB_KEYBOARD_ENABLE)
				// keyboardInterface
				{	{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_KEYBOARD,
					0, // AlternateSetting
					1, // bNumEndpoints
					HID_CSCP_HIDClass, HID_CSCP_BootSubclass,
					HID_CSCP_KeyboardBootProtocol, NO_DESCRIPTOR
				}, 
				{
					// keyboard_hid
					{	
						{sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID}, 0x0111, // HIDSpec
						USB_HID_COUNTRY_US, 1, // TotalReportDescriptors
						HID_DTYPE_Report, {sizeof(keyboard_report_desc), 0x00}, // HIDReportLength
					},
					// keyboard_in_endpoint
					{	
						{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},
						ENDPOINT_DIR_IN | USB_EDP_KEYBOARD_IN,
						EP_TYPE_INTERRUPT, 0x0008, // EndpointSize
						USB_KEYBOARD_POLL_INTERVAL // PollingIntervalMS
					},
				},
#endif
#if (USB_MOUSE_ENABLE)
				// mouse_interface
				{	{	sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_MOUSE,
					0, // AlternateSetting
					1, // bNumEndpoints
					HID_CSCP_HIDClass, HID_CSCP_BootSubclass,
					HID_CSCP_MouseBootProtocol, NO_DESCRIPTOR}, {
					// mouse_hid
					{	{	sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID}, 0x0111, // HIDSpec
						USB_HID_COUNTRY_US, 1, // TotalReportDescriptors
						HID_DTYPE_Report, {sizeof(mouse_report_desc), 0x00}, // HIDReportLength
					},
					// mouse_in_endpoint
					{	{	sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},
						ENDPOINT_DIR_IN | USB_EDP_MOUSE,
						EP_TYPE_INTERRUPT, 0x0008, // EndpointSize
						USB_MOUSE_POLL_INTERVAL // PollingIntervalMS
					}},
#endif
#if (USB_SOMATIC_ENABLE)
				// SOMATICInterface
				{	{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
					USB_INTF_SOMATIC,
					0, // AlternateSetting
					1, // bNumEndpoints
					HID_CSCP_HIDClass, HID_CSCP_BootSubclass,
					HID_CSCP_NonBootProtocol, NO_DESCRIPTOR
				}, 
				{
					// SOMATIC_hid
					{	
						{sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID}, 0x0111, // HIDSpec
						USB_HID_COUNTRY_US, 1, // TotalReportDescriptors
						HID_DTYPE_Report, {sizeof(somatic_report_desc), 0x00}, // HIDReportLength
					},
					// SOMATIC_in_endpoint
					{	
						{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},
						ENDPOINT_DIR_IN | USB_EDP_SOMATIC_OUT,
						EP_TYPE_INTERRUPT, 0x0010, // EndpointSize
						USB_SOMATIC_POLL_INTERVAL // PollingIntervalMS
					},
				},
#endif
};

u8* usbdesc_get_language(void) {
	return (u8*) (&language_desc);
}

u8* usbdesc_get_vendor(void) {
	return (u8*) (&vendor_desc);
}

u8* usbdesc_get_product(void) {
	return (u8*) (&product_desc);
}

#if (MS_OS_DESCRIPTOR_ENABLE)

u8* usbdesc_get_OS_descriptor(void) {
	return (u8*) (&microsoft_OS_desc);
}

u8* usbdesc_get_OSFeature(int *length) {
	*length = sizeof(OSFeatureDescriptor);
	return (u8*) (&OSFeatureDescriptor);
}

u8* usbdesc_get_compatID(int *length) {
	*length = OSFeatureDescriptor_compatID.compatID_Header.dwLength;

	return (u8*) (&OSFeatureDescriptor_compatID);
}

#endif

u8* usbdesc_get_serial(void) {
	return (u8*) (&serial_desc);
}

u8* usbdesc_get_device(void) {
	return (u8*) (&device_desc);
}

u8* usbdesc_get_configuration(void) {
	return (u8*) (&configuration_desc);
}

#if(0)
u8* usbdesc_get_audio(void) {
	return (u8*) (&configuration_desc.audio_descriptor);
}
#endif

#if (USB_MOUSE_ENABLE)
u8* usbdesc_get_mouse(void) {
	return (u8*) (&configuration_desc.mouse_descriptor);
}
#endif

#if (USB_KEYBOARD_ENABLE)
u8* usbdesc_get_keyboard(void) {
	return (u8*) (&configuration_desc.keyboard_descriptor);
}
#endif

#if (USB_SOMATIC_ENABLE)
u8* usbdesc_get_somatic(void) {
	return (u8*) (&configuration_desc.somatic_descriptor);
}
#endif

#if (USB_CDC_ENABLE)
u8* usbdesc_get_cdc(void) {
	return (u8*) (&configuration_desc.cdc_descriptor);
}

u8* usbdesc_get_cdc_inf(void) {
	return (u8*) (&configuration_desc.cdc_interface);
}
#endif

