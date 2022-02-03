/********************************************************************************************************
 * @file     HIDClassCommon.h 
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
#include <application/usbstd/StdDescriptors.h>
#include "tl_common.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define HID_KEYBOARD_MODIFIER_LEFTCTRL                    BIT(0)
#define HID_KEYBOARD_MODIFIER_LEFTSHIFT                   BIT(1)
#define HID_KEYBOARD_MODIFIER_LEFTALT                     BIT(2)
#define HID_KEYBOARD_MODIFIER_LEFTGUI                     BIT(3)
#define HID_KEYBOARD_MODIFIER_RIGHTCTRL                   BIT(4)
#define HID_KEYBOARD_MODIFIER_RIGHTSHIFT                  BIT(5)
#define HID_KEYBOARD_MODIFIER_RIGHTALT                    BIT(6)
#define HID_KEYBOARD_MODIFIER_RIGHTGUI                    BIT(7)
#define HID_KEYBOARD_LED_NUMLOCK                          BIT(0)
#define HID_KEYBOARD_LED_CAPSLOCK                         BIT(1)
#define HID_KEYBOARD_LED_SCROLLLOCK                       BIT(2)
#define HID_KEYBOARD_LED_KATANA                           BIT(3)

#define HID_KEYBOARD_SC_ERROR_ROLLOVER                    1
#define HID_KEYBOARD_SC_POST_FAIL                         2
#define HID_KEYBOARD_SC_ERROR_UNDEFINED                   3
#define HID_KEYBOARD_SC_A                                 4
#define HID_KEYBOARD_SC_B                                 5
#define HID_KEYBOARD_SC_C                                 6
#define HID_KEYBOARD_SC_D                                 7
#define HID_KEYBOARD_SC_E                                 8
#define HID_KEYBOARD_SC_F                                 9
#define HID_KEYBOARD_SC_G                                 10
#define HID_KEYBOARD_SC_H                                 11
#define HID_KEYBOARD_SC_I                                 12
#define HID_KEYBOARD_SC_J                                 13
#define HID_KEYBOARD_SC_K                                 14
#define HID_KEYBOARD_SC_L                                 15
#define HID_KEYBOARD_SC_M                                 16
#define HID_KEYBOARD_SC_N                                 17
#define HID_KEYBOARD_SC_O                                 18
#define HID_KEYBOARD_SC_P                                 19
#define HID_KEYBOARD_SC_Q                                 20
#define HID_KEYBOARD_SC_R                                 21
#define HID_KEYBOARD_SC_S                                 22
#define HID_KEYBOARD_SC_T                                 23
#define HID_KEYBOARD_SC_U                                 24
#define HID_KEYBOARD_SC_V                                 25
#define HID_KEYBOARD_SC_W                                 26
#define HID_KEYBOARD_SC_X                                 27
#define HID_KEYBOARD_SC_Y                                 28
#define HID_KEYBOARD_SC_Z                                 29
#define HID_KEYBOARD_SC_1_AND_EXCLAMATION                 30
#define HID_KEYBOARD_SC_2_AND_AT                          31
#define HID_KEYBOARD_SC_3_AND_HASHMARK                    32
#define HID_KEYBOARD_SC_4_AND_DOLLAR                      33
#define HID_KEYBOARD_SC_5_AND_PERCENTAGE                  34
#define HID_KEYBOARD_SC_6_AND_CARET                       35
#define HID_KEYBOARD_SC_7_AND_AND_AMPERSAND               36
#define HID_KEYBOARD_SC_8_AND_ASTERISK                    37
#define HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS         38
#define HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS         39
#define HID_KEYBOARD_SC_ENTER                             40
#define HID_KEYBOARD_SC_ESCAPE                            41
#define HID_KEYBOARD_SC_BACKSPACE                         42
#define HID_KEYBOARD_SC_TAB                               43
#define HID_KEYBOARD_SC_SPACE                             44
#define HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE              45
#define HID_KEYBOARD_SC_EQUAL_AND_PLUS                    46
#define HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE 47
#define HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE 48
#define HID_KEYBOARD_SC_BACKSLASH_AND_PIPE                49
#define HID_KEYBOARD_SC_NON_US_HASHMARK_AND_TILDE         50
#define HID_KEYBOARD_SC_SEMICOLON_AND_COLON               51
#define HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE              52
#define HID_KEYBOARD_SC_GRAVE_ACCENT_AND_TILDE            53
#define HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN          54
#define HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN         55
#define HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK           56
#define HID_KEYBOARD_SC_CAPS_LOCK                         57
#define HID_KEYBOARD_SC_F1                                58
#define HID_KEYBOARD_SC_F2                                59
#define HID_KEYBOARD_SC_F3                                60
#define HID_KEYBOARD_SC_F4                                61
#define HID_KEYBOARD_SC_F5                                62
#define HID_KEYBOARD_SC_F6                                63
#define HID_KEYBOARD_SC_F7                                64
#define HID_KEYBOARD_SC_F8                                65
#define HID_KEYBOARD_SC_F9                                66
#define HID_KEYBOARD_SC_F10                               67
#define HID_KEYBOARD_SC_F11                               68
#define HID_KEYBOARD_SC_F12                               69
#define HID_KEYBOARD_SC_PRINT_SCREEN                      70
#define HID_KEYBOARD_SC_SCROLL_LOCK                       71
#define HID_KEYBOARD_SC_PAUSE                             72
#define HID_KEYBOARD_SC_INSERT                            73
#define HID_KEYBOARD_SC_HOME                              74
#define HID_KEYBOARD_SC_PAGE_UP                           75
#define HID_KEYBOARD_SC_DELETE                            76
#define HID_KEYBOARD_SC_END                               77
#define HID_KEYBOARD_SC_PAGE_DOWN                         78
#define HID_KEYBOARD_SC_RIGHT_ARROW                       79
#define HID_KEYBOARD_SC_LEFT_ARROW                        80
#define HID_KEYBOARD_SC_DOWN_ARROW                        81
#define HID_KEYBOARD_SC_UP_ARROW                          82
#define HID_KEYBOARD_SC_NUM_LOCK                          83
#define HID_KEYBOARD_SC_KEYPAD_SLASH                      84
#define HID_KEYBOARD_SC_KEYPAD_ASTERISK                   85
#define HID_KEYBOARD_SC_KEYPAD_MINUS                      86
#define HID_KEYBOARD_SC_KEYPAD_PLUS                       87
#define HID_KEYBOARD_SC_KEYPAD_ENTER                      88
#define HID_KEYBOARD_SC_KEYPAD_1_AND_END                  89
#define HID_KEYBOARD_SC_KEYPAD_2_AND_DOWN_ARROW           90
#define HID_KEYBOARD_SC_KEYPAD_3_AND_PAGE_DOWN            91
#define HID_KEYBOARD_SC_KEYPAD_4_AND_LEFT_ARROW           92
#define HID_KEYBOARD_SC_KEYPAD_5                          93
#define HID_KEYBOARD_SC_KEYPAD_6_AND_RIGHT_ARROW          94
#define HID_KEYBOARD_SC_KEYPAD_7_AND_HOME                 95
#define HID_KEYBOARD_SC_KEYPAD_8_AND_UP_ARROW             96
#define HID_KEYBOARD_SC_KEYPAD_9_AND_PAGE_UP              97
#define HID_KEYBOARD_SC_KEYPAD_0_AND_INSERT               98
#define HID_KEYBOARD_SC_KEYPAD_DOT_AND_DELETE             99
#define HID_KEYBOARD_SC_NON_US_BACKSLASH_AND_PIPE         100
#define HID_KEYBOARD_SC_POWER                             102
#define HID_KEYBOARD_SC_EQUAL_SIGN                        103
#define HID_KEYBOARD_SC_F13                               104
#define HID_KEYBOARD_SC_F14                               105
#define HID_KEYBOARD_SC_F15                               106
#define HID_KEYBOARD_SC_F16                               107
#define HID_KEYBOARD_SC_F17                               108
#define HID_KEYBOARD_SC_F18                               109
#define HID_KEYBOARD_SC_F19                               110
#define HID_KEYBOARD_SC_F20                               111
#define HID_KEYBOARD_SC_F21                               112
#define HID_KEYBOARD_SC_F22                               113
#define HID_KEYBOARD_SC_F23                               114
#define HID_KEYBOARD_SC_F24                               115
#define HID_KEYBOARD_SC_EXECUTE                           116
#define HID_KEYBOARD_SC_HELP                              117
#define HID_KEYBOARD_SC_MANU                              118
#define HID_KEYBOARD_SC_SELECT                            119
#define HID_KEYBOARD_SC_STOP                              120
#define HID_KEYBOARD_SC_AGAIN                             121
#define HID_KEYBOARD_SC_UNDO                              122
#define HID_KEYBOARD_SC_CUT                               123
#define HID_KEYBOARD_SC_COPY                              124
#define HID_KEYBOARD_SC_PASTE                             125
#define HID_KEYBOARD_SC_FIND                              126
#define HID_KEYBOARD_SC_MUTE                              127
#define HID_KEYBOARD_SC_VOLUME_UP                         128
#define HID_KEYBOARD_SC_VOLUME_DOWN                       129
#define HID_KEYBOARD_SC_LOCKING_CAPS_LOCK                 130
#define HID_KEYBOARD_SC_LOCKING_NUM_LOCK                  131
#define HID_KEYBOARD_SC_LOCKING_SCROLL_LOCK               132
#define HID_KEYBOARD_SC_KEYPAD_COMMA                      133
#define HID_KEYBOARD_SC_KEYPAD_EQUAL_SIGN                 134
#define HID_KEYBOARD_SC_INTERNATIONAL1                    135
#define HID_KEYBOARD_SC_INTERNATIONAL2                    136
#define HID_KEYBOARD_SC_INTERNATIONAL3                    137
#define HID_KEYBOARD_SC_INTERNATIONAL4                    138
#define HID_KEYBOARD_SC_INTERNATIONAL5                    139
#define HID_KEYBOARD_SC_INTERNATIONAL6                    140
#define HID_KEYBOARD_SC_INTERNATIONAL7                    141
#define HID_KEYBOARD_SC_INTERNATIONAL8                    142
#define HID_KEYBOARD_SC_INTERNATIONAL9                    143
#define HID_KEYBOARD_SC_LANG1                             144
#define HID_KEYBOARD_SC_LANG2                             145
#define HID_KEYBOARD_SC_LANG3                             146
#define HID_KEYBOARD_SC_LANG4                             147
#define HID_KEYBOARD_SC_LANG5                             148
#define HID_KEYBOARD_SC_LANG6                             149
#define HID_KEYBOARD_SC_LANG7                             150
#define HID_KEYBOARD_SC_LANG8                             151
#define HID_KEYBOARD_SC_LANG9                             152
#define HID_KEYBOARD_SC_ALTERNATE_ERASE                   153
#define HID_KEYBOARD_SC_SISREQ                            154
#define HID_KEYBOARD_SC_CANCEL                            155
#define HID_KEYBOARD_SC_CLEAR                             156
#define HID_KEYBOARD_SC_PRIOR                             157
#define HID_KEYBOARD_SC_RETURN                            158
#define HID_KEYBOARD_SC_SEPARATOR                         159
#define HID_KEYBOARD_SC_OUT                               160
#define HID_KEYBOARD_SC_OPER                              161
#define HID_KEYBOARD_SC_CLEAR_AND_AGAIN                   162
#define HID_KEYBOARD_SC_CRSEL_ANDPROPS                    163
#define HID_KEYBOARD_SC_EXSEL                             164
#define HID_KEYBOARD_SC_KEYPAD_00                         176
#define HID_KEYBOARD_SC_KEYPAD_000                        177
#define HID_KEYBOARD_SC_THOUSANDS_SEPARATOR               178
#define HID_KEYBOARD_SC_DECIMAL_SEPARATOR                 179
#define HID_KEYBOARD_SC_CURRENCY_UNIT                     180
#define HID_KEYBOARD_SC_CURRENCY_SUB_UNIT                 181
#define HID_KEYBOARD_SC_KEYPAD_OPENING_PARENTHESIS        182
#define HID_KEYBOARD_SC_KEYPAD_CLOSING_PARENTHESIS        183
#define HID_KEYBOARD_SC_KEYPAD_OPENING_BRACE              184
#define HID_KEYBOARD_SC_KEYPAD_CLOSING_BRACE              185
#define HID_KEYBOARD_SC_KEYPAD_TAB                        186
#define HID_KEYBOARD_SC_KEYPAD_BACKSPACE                  187
#define HID_KEYBOARD_SC_KEYPAD_A                          188
#define HID_KEYBOARD_SC_KEYPAD_B                          189
#define HID_KEYBOARD_SC_KEYPAD_C                          190
#define HID_KEYBOARD_SC_KEYPAD_D                          191
#define HID_KEYBOARD_SC_KEYPAD_E                          192
#define HID_KEYBOARD_SC_KEYPAD_F                          193
#define HID_KEYBOARD_SC_KEYPAD_XOR                        194
#define HID_KEYBOARD_SC_KEYPAD_CARET                      195
#define HID_KEYBOARD_SC_KEYPAD_PERCENTAGE                 196
#define HID_KEYBOARD_SC_KEYPAD_LESS_THAN_SIGN             197
#define HID_KEYBOARD_SC_KEYPAD_GREATER_THAN_SIGN          198
#define HID_KEYBOARD_SC_KEYPAD_AMP                        199
#define HID_KEYBOARD_SC_KEYPAD_AMP_AMP                    200
#define HID_KEYBOARD_SC_KEYPAD_PIPE                       201
#define HID_KEYBOARD_SC_KEYPAD_PIPE_PIPE                  202
#define HID_KEYBOARD_SC_KEYPAD_COLON                      203
#define HID_KEYBOARD_SC_KEYPAD_HASHMARK                   204
#define HID_KEYBOARD_SC_KEYPAD_SPACE                      205
#define HID_KEYBOARD_SC_KEYPAD_AT                         206
#define HID_KEYBOARD_SC_KEYPAD_EXCLAMATION_SIGN           207
#define HID_KEYBOARD_SC_KEYPAD_MEMORY_STORE               208
#define HID_KEYBOARD_SC_KEYPAD_MEMORY_RECALL              209
#define HID_KEYBOARD_SC_KEYPAD_MEMORY_CLEAR               210
#define HID_KEYBOARD_SC_KEYPAD_MEMORY_ADD                 211
#define HID_KEYBOARD_SC_KEYPAD_MEMORY_SUBTRACT            212
#define HID_KEYBOARD_SC_KEYPAD_MEMORY_MULTIPLY            213
#define HID_KEYBOARD_SC_KEYPAD_MEMORY_DIVIDE              214
#define HID_KEYBOARD_SC_KEYPAD_PLUS_AND_MINUS             215
#define HID_KEYBOARD_SC_KEYPAD_CLEAR                      216
#define HID_KEYBOARD_SC_KEYPAD_CLEAR_ENTRY                217
#define HID_KEYBOARD_SC_KEYPAD_BINARY                     218
#define HID_KEYBOARD_SC_KEYPAD_OCTAL                      219
#define HID_KEYBOARD_SC_KEYPAD_DECIMAL                    220
#define HID_KEYBOARD_SC_KEYPAD_HEXADECIMAL                221
#define HID_KEYBOARD_SC_LEFT_CONTROL                      224
#define HID_KEYBOARD_SC_LEFT_SHIFT                        225
#define HID_KEYBOARD_SC_LEFT_ALT                          226
#define HID_KEYBOARD_SC_LEFT_GUI                          227
#define HID_KEYBOARD_SC_RIGHT_CONTROL                     228
#define HID_KEYBOARD_SC_RIGHT_SHIFT                       229
#define HID_KEYBOARD_SC_RIGHT_ALT                         230
#define HID_KEYBOARD_SC_RIGHT_GUI                         231

#define HID_DESCRIPTOR_JOYSTICK(NumAxis, MinAxisVal, MaxAxisVal, MinPhysicalVal, MaxPhysicalVal, Buttons) \
		HID_RPT_USAGE_PAGE      (8,  0x01), \
		HID_RPT_USAGE	       (8,  0x04), \
		HID_RPT_COLLECTION      (8,  0x01), \
			HID_RPT_USAGE           (8,  0x01), \
			HID_RPT_COLLECTION      (8,  0x00), \
			HID_RPT_USAGE_MINIMUM   (8,  0x30), \
			HID_RPT_USAGE_MAXIMUM   (8, (0x30 + (NumAxis - 1))), \
			HID_RPT_LOGICAL_MINIMUM (16, MinAxisVal), \
			HID_RPT_LOGICAL_MAXIMUM (16, MaxAxisVal), \
			HID_RPT_PHYSICAL_MINIMUM(16, MinPhysicalVal), \
			HID_RPT_PHYSICAL_MAXIMUM(16, MaxPhysicalVal), \
			HID_RPT_REPORT_COUNT    (8,  NumAxis), \
			HID_RPT_REPORT_SIZE     (8, ((((MinAxisVal >= -0xFF) && (MaxAxisVal <= 0xFF)) ? 8 : 16))), \
			HID_RPT_INPUT           (8,  HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE), \
			HID_RPT_END_COLLECTION  (0), \
			HID_RPT_USAGE_PAGE      (8,  0x09), \
			HID_RPT_USAGE_MINIMUM   (8,  0x01), \
			HID_RPT_USAGE_MAXIMUM   (8,  Buttons), \
			HID_RPT_LOGICAL_MINIMUM (8,  0x00), \
			HID_RPT_LOGICAL_MAXIMUM (8,  0x01), \
			HID_RPT_REPORT_SIZE     (8,  0x01), \
			HID_RPT_REPORT_COUNT    (8,  Buttons), \
			HID_RPT_INPUT           (8,  HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE), \
			HID_RPT_REPORT_SIZE     (8, (8 - (Buttons % 8))), \
			HID_RPT_REPORT_COUNT    (8,  0x01), \
			HID_RPT_INPUT           (8,  HID_IOF_CONSTANT), \
		HID_RPT_END_COLLECTION  (0)

#define HID_DESCRIPTOR_KEYBOARD(MaxKeys) \
		HID_RPT_USAGE_PAGE			(8, 0x01), \
		HID_RPT_USAGE				(8, 0x06), \
		HID_RPT_COLLECTION			(8, 0x01), \
			HID_RPT_USAGE_PAGE		(8, 0x07), \
		HID_RPT_USAGE_MINIMUM		(8, 0xE0), \
			HID_RPT_USAGE_MAXIMUM	(8, 0xE7), \
			HID_RPT_LOGICAL_MINIMUM	(8, 0x00), \
			HID_RPT_LOGICAL_MAXIMUM	(8, 0x01), \
			HID_RPT_REPORT_COUNT		(8, 0x08), \
			HID_RPT_REPORT_SIZE		(8, 0x01), \
			HID_RPT_INPUT			(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE), \
			HID_RPT_INPUT			(8, HID_IOF_CONSTANT | HID_IOF_VARIABLE), \
			HID_RPT_REPORT_COUNT		(8, 0x05), \
			HID_RPT_USAGE_PAGE		(8, 0x08), \
			HID_RPT_USAGE_MINIMUM	(8, 0x01), \
			HID_RPT_USAGE_MAXIMUM	(8, 0x05), \
			HID_RPT_OUTPUT			(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE), \
			HID_RPT_REPORT_COUNT		(8, 0x01), \
			HID_RPT_REPORT_SIZE		(8, 0x03), \
			HID_RPT_OUTPUT			(8, HID_IOF_CONSTANT), \
			HID_RPT_REPORT_COUNT		(8, 0x06), \
			HID_RPT_REPORT_SIZE		(8, 0x08), \
			HID_RPT_LOGICAL_MINIMUM	(8, 0x00), \
			0x26,	0xa4,	0x00, \
			HID_RPT_USAGE_PAGE		(8, 0x07), \
			HID_RPT_USAGE_MINIMUM	(8, 0x00), \
			0x2a,   0xa4,   0x00, \
			HID_RPT_INPUT			(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE), \
		HID_RPT_END_COLLECTION(0)

#define HID_DESCRIPTOR_MOUSE(MinAxisVal, MaxAxisVal, MinPhysicalVal, MaxPhysicalVal, Buttons, AbsoluteCoords) \
		HID_RPT_USAGE_PAGE				(8, 0x01), \
		HID_RPT_USAGE					(8, 0x02), \
		HID_RPT_COLLECTION				(8, 0x01), \
			HID_RPT_USAGE				(8, 0x01), \
			HID_RPT_COLLECTION			(8, 0x00), \
				HID_RPT_USAGE_PAGE		(8, 0x09), \
				HID_RPT_USAGE_MINIMUM	(8, 0x01), \
				HID_RPT_USAGE_MAXIMUM	(8, Buttons), \
				HID_RPT_LOGICAL_MINIMUM	(8, 0x00), \
				HID_RPT_LOGICAL_MAXIMUM	(8, 0x01), \
				HID_RPT_REPORT_COUNT		(8, Buttons), \
				HID_RPT_REPORT_SIZE		(8, 0x01), \
				HID_RPT_INPUT			(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE), \
				HID_RPT_REPORT_COUNT		(8, 0x01), \
				HID_RPT_REPORT_SIZE		(8, (8 - (Buttons % 8))), \
				HID_RPT_INPUT			(8, HID_IOF_CONSTANT), \
				HID_RPT_USAGE_PAGE		(8, 0x01), \
				HID_RPT_USAGE			(8, 0x30), \
				HID_RPT_USAGE			(8, 0x31), \
				HID_RPT_LOGICAL_MINIMUM	(16, MinAxisVal), \
				HID_RPT_LOGICAL_MAXIMUM	(16, MaxAxisVal), \
				HID_RPT_PHYSICAL_MINIMUM	(16, MinPhysicalVal), \
				HID_RPT_PHYSICAL_MAXIMUM	(16, MaxPhysicalVal), \
				HID_RPT_REPORT_COUNT		(8, 0x02), \
				HID_RPT_REPORT_SIZE		(8, ((((MinAxisVal >= -0xFF) && (MaxAxisVal <= 0xFF)) ? 8 : 16))), \
				HID_RPT_INPUT			(8, HID_IOF_DATA | HID_IOF_VARIABLE | (AbsoluteCoords ? HID_IOF_ABSOLUTE : HID_IOF_RELATIVE)), \
			HID_RPT_END_COLLECTION		(0), \
		HID_RPT_END_COLLECTION(0)

#define HID_DESCRIPTOR_VENDOR(VendorPageNum, CollectionUsage, DataINUsage, DataOUTUsage, NumBytes) \
		HID_RPT_USAGE_PAGE				(16, (0xFF00 | VendorPageNum)), \
		HID_RPT_USAGE					(8, CollectionUsage), \
		HID_RPT_COLLECTION				(8, 0x01), \
			HID_RPT_USAGE				(8, DataINUsage), \
			HID_RPT_LOGICAL_MINIMUM		(8, 0x00), \
			HID_RPT_LOGICAL_MAXIMUM		(8, 0xFF), \
			HID_RPT_REPORT_SIZE			(8, 0x08), \
			HID_RPT_REPORT_COUNT			(8, NumBytes), \
			HID_RPT_INPUT				(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE), \
			HID_RPT_USAGE				(8, DataOUTUsage), \
			HID_RPT_LOGICAL_MINIMUM		(8, 0x00), \
			HID_RPT_LOGICAL_MAXIMUM		(8, 0xFF), \
			HID_RPT_REPORT_SIZE			(8, 0x08), \
			HID_RPT_REPORT_COUNT			(8, NumBytes), \
			HID_RPT_OUTPUT				(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE), \
		HID_RPT_END_COLLECTION			(0)

enum HID_Descriptor_ClassSubclassProtocol_t
{
	HID_CSCP_NonBootSubclass = 0x00,
	HID_CSCP_NonBootProtocol = 0x00,
	HID_CSCP_BootSubclass = 0x01,
	HID_CSCP_KeyboardBootProtocol = 0x01,
	HID_CSCP_MouseBootProtocol,
	HID_CSCP_HIDClass,
};

enum HID_ClassRequests_t
{
	HID_REQ_GetReport = 0x01,
	HID_REQ_GetIdle,
	HID_REQ_GetProtocol,
	HID_REQ_SetReport = 0x09,
	HID_REQ_SetIdle,
	HID_REQ_SetProtocol,
};

enum HID_DescriptorTypes_t
{
	HID_DTYPE_HID = 0x21,
	HID_DTYPE_Report,
};

enum HID_ReportItemTypes_t
{
	HID_REPORT_ITEM_In,
	HID_REPORT_ITEM_Out,
	HID_REPORT_ITEM_Feature,
	HID_REPORT_CUSTOM,
};

typedef struct
{
	USB_Descriptor_Hdr_t Header;

	u16 HIDSpec;
	u8  CountryCode;
	u8 TotalReportDescriptors;
	u8 HIDReportType;
	u8 HIDReportLength[2];
}USB_HID_Descriptor_HID_t;

typedef struct
{
	u8 Length;
	u8 DescriptorType;
	u16 BcdHID;
	u8 CountryCode;
	u8 NumDescriptors;
	u8 DescriptorType2;
	u8 DescriptorLength[2];
}USB_HID_StdDescriptor_HID_t;

typedef struct
{
	u8 Button;
	s8  X;
	s8  Y;
}USB_MouseReport_Data_t;

typedef struct
{
	u8 Modifier;
	u8 Reserved;
	u8 KeyCode[6];
}USB_KeyboardReport_Data_t;

typedef u8 USB_Descriptor_HIDReport_Datatype_t;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

