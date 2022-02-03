/********************************************************************************************************
 * @file     HIDReportData.h 
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

#define HID_RPT_DATA_SIZE_MASK                  (0x03)
#define HID_RPT_TYPE_MASK                       (0x0C)
#define HID_RPT_TAG_MASK                        (0xF0)

#define HID_RPT_TYPE_MAIN                       (0x00)
#define HID_RPT_TYPE_GLOBAL                     (0x04)
#define HID_RPT_TYPE_LOCAL                      (0x08)

#define HID_RPT_DATA_BITS_0                     (0x00)
#define HID_RPT_DATA_BITS_8                     (0x01)
#define HID_RPT_DATA_BITS_16                    (0x02)
#define HID_RPT_DATA_BITS_32                    (0x03)
#define HID_RPT_DATA_BITS(Data_bits)            HID_RPT_DATA_BITS_##Data_bits

#define _HID_RPT_DATA_ENCODE_0(Data)
#define _HID_RPT_DATA_ENCODE_8(Data)            , (Data & 0xFF)
#define _HID_RPT_DATA_ENCODE_16(Data)           _HID_RPT_DATA_ENCODE_8(Data)  _HID_RPT_DATA_ENCODE_8(Data>>8)
#define _HID_RPT_DATA_ENCODE_32(Data)           _HID_RPT_DATA_ENCODE_16(Data) _HID_RPT_DATA_ENCODE_16(Data>>16)
#define _HID_RPT_DATA_ENCODE(Data_bits, ...)    _HID_RPT_DATA_ENCODE_##Data_bits(__VA_ARGS__)


#define _HID_RPT_DATA_ENTRY(Type, Tag, Data_bits, ...)  \
												(Type | Tag | HID_RPT_DATA_BITS(Data_bits)) _HID_RPT_DATA_ENCODE(Data_bits, (__VA_ARGS__))


#define HID_IOF_CONSTANT                        BIT(0)
#define HID_IOF_DATA                            (0<<0)
#define HID_IOF_VARIABLE                        BIT(1)
#define HID_IOF_ARRAY                           (0<<1)
#define HID_IOF_RELATIVE                        BIT(2)
#define HID_IOF_ABSOLUTE                        (0<<2)
#define HID_IOF_WRAP                            BIT(3)
#define HID_IOF_NO_WRAP                         (0<<3)
#define HID_IOF_NON_LINEAR                      BIT(4)
#define HID_IOF_LINEAR                          (0<<4)
#define HID_IOF_NO_PREFERRED_STATE              BIT(5)
#define HID_IOF_PREFERRED_STATE                 (0<<5)
#define HID_IOF_NULLSTATE                       BIT(6)
#define HID_IOF_NO_NULL_POSITION                (0<<6)
#define HID_IOF_VOLATILE                        BIT(7)
#define HID_IOF_NON_VOLATILE                    (0<<7)
#define HID_IOF_BUFFERED_BYTES                  BIT(8)
#define HID_IOF_BITFIELD                        (0<<8)


#define HID_RPT_INPUT(Data_bits, ...)             _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0x80, Data_bits, __VA_ARGS__)
#define HID_RPT_OUTPUT(Data_bits, ...)            _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0x90, Data_bits, __VA_ARGS__)
#define HID_RPT_COLLECTION(Data_bits, ...)        _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0xA0, Data_bits, __VA_ARGS__)
#define HID_RPT_FEATURE(Data_bits, ...)           _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0xB0, Data_bits, __VA_ARGS__)
#define HID_RPT_END_COLLECTION(Data_bits, ...)    _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0xC0, Data_bits, __VA_ARGS__)
#define HID_RPT_USAGE_PAGE(Data_bits, ...)        _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x00, Data_bits, __VA_ARGS__)
#define HID_RPT_LOGICAL_MINIMUM(Data_bits, ...)   _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x10, Data_bits, __VA_ARGS__)
#define HID_RPT_LOGICAL_MAXIMUM(Data_bits, ...)   _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x20, Data_bits, __VA_ARGS__)
#define HID_RPT_PHYSICAL_MINIMUM(Data_bits, ...)  _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x30, Data_bits, __VA_ARGS__)
#define HID_RPT_PHYSICAL_MAXIMUM(Data_bits, ...)  _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x40, Data_bits, __VA_ARGS__)
#define HID_RPT_UNIT_EXPONENT(Data_bits, ...)     _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x50, Data_bits, __VA_ARGS__)
#define HID_RPT_UNIT(Data_bits, ...)              _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x60, Data_bits, __VA_ARGS__)
#define HID_RPT_REPORT_SIZE(Data_bits, ...)       _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x70, Data_bits, __VA_ARGS__)
#define HID_RPT_REPORT_ID(Data_bits, ...)         _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x80, Data_bits, __VA_ARGS__)
#define HID_RPT_REPORT_COUNT(Data_bits, ...)      _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x90, Data_bits, __VA_ARGS__)
#define HID_RPT_PUSH(Data_bits, ...)              _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0xA0, Data_bits, __VA_ARGS__)
#define HID_RPT_POP(Data_bits, ...)               _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0xB0, Data_bits, __VA_ARGS__)
#define HID_RPT_USAGE(Data_bits, ...)             _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_LOCAL , \
																0x00, Data_bits, __VA_ARGS__)
#define HID_RPT_USAGE_MINIMUM(Data_bits, ...)     _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_LOCAL , \
																0x10, Data_bits, __VA_ARGS__)
#define HID_RPT_USAGE_MAXIMUM(Data_bits, ...)     _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_LOCAL , \
																0x20, Data_bits, __VA_ARGS__)

