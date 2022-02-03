/********************************************************************************************************
 * @file     pga.h 
 *
 * @brief    This is the header file for TLSR8258
 *
 * @author	 Driver Group
 * @date     May 8, 2018
 *
 * @par      Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 * @par      History:
 * 			 1.initial release(DEC. 26 2018)
 *
 * @version  A001
 *         
 *******************************************************************************************************/
#ifndef		pga_H
#define		pga_H

#include "bsp.h"


enum{
	PGA_AIN_C0,
	PGA_AIN_C1,
	PGA_AIN_C2,
	PGA_AIN_C3,
};


//set FLD_PGA_PRE_AMPLIFIER_GAIN of reg_pga_fix_value (0xb63<6>) 26dB OR 46dB

typedef enum{
	PGA_PRE_GAIN_18DB = 0,		//26dB  18dB
	PGA_PRE_GAIN_38DB = 1,		//46dB   38dB
}PGA_PreAmplifierTypeDef;


///set FLD_PGA_POST_AMPLIFIER_GAIN of reg_pga_fix_value (0xb63<0:5>) range -10~14dB
typedef enum{
	PGA_POST_GAIN_m10DB = 0,		//-10dB
	PGA_POST_GAIN_m9P5DB,
	PGA_POST_GAIN_m9DB,
	PGA_POST_GAIN_m8P5DB,
	PGA_POST_GAIN_m8DB,
	PGA_POST_GAIN_m7P5DB,
	PGA_POST_GAIN_m7DB,
	PGA_POST_GAIN_m6P5DB,
	PGA_POST_GAIN_m6DB,
	PGA_POST_GAIN_m5P5DB,
	PGA_POST_GAIN_m5DB,
	PGA_POST_GAIN_m4P5DB,
	PGA_POST_GAIN_m4DB,
	PGA_POST_GAIN_m3P5DB,
	PGA_POST_GAIN_m3DB,
	PGA_POST_GAIN_m2P5DB,
	PGA_POST_GAIN_m2DB,
	PGA_POST_GAIN_m1P5DB,
	PGA_POST_GAIN_m1DB,
	PGA_POST_GAIN_m0P5DB,
	PGA_POST_GAIN_0DB,				//0dB
	PGA_POST_GAIN_0P5DB,
	PGA_POST_GAIN_1DB,
	PGA_POST_GAIN_1P5DB,
	PGA_POST_GAIN_2DB,
	PGA_POST_GAIN_2P5DB,
	PGA_POST_GAIN_3DB,
	PGA_POST_GAIN_3P5DB,
	PGA_POST_GAIN_4DB,
	PGA_POST_GAIN_4P5DB,
	PGA_POST_GAIN_5DB,
	PGA_POST_GAIN_5P5DB,
	PGA_POST_GAIN_6DB,
	PGA_POST_GAIN_6P5DB,
	PGA_POST_GAIN_7DB,
	PGA_POST_GAIN_7P5DB,
	PGA_POST_GAIN_8DB,
	PGA_POST_GAIN_8P5DB,
	PGA_POST_GAIN_9DB,
	PGA_POST_GAIN_9P5DB,
	PGA_POST_GAIN_10DB,				//10dB
	PGA_POST_GAIN_10P5DB,
	PGA_POST_GAIN_11DB,
	PGA_POST_GAIN_11P5DB,
	PGA_POST_GAIN_12DB,
	PGA_POST_GAIN_12P5DB,
	PGA_POST_GAIN_13DB,
	PGA_POST_GAIN_13P5DB,
	PGA_POST_GAIN_14DB,				//14dB
}PGA_PostAmplifierTypeDef;


#define		SET_PGA_CHN_ON(v)				do{\
												unsigned char val=analog_read(0x80+124);\
												val &= (~(v));\
												analog_write(0x80+124,val);\
											}while(0)
#define		SET_PGA_LEFT_P_AIN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+125);\
												val &= 0xfc;\
												val |= (((unsigned char)(v))&0x03);\
												WriteAnalogReg(0x80+125,val);\
											}while(0)

#define		SET_PGA_LEFT_N_AIN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+125);\
												val &= 0xf3;\
												val |= (((unsigned char)(v)<<2)&0x0c);\
												WriteAnalogReg(0x80+125,val);\
											}while(0)

#define		SET_PGA_RIGHT_P_AIN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+125);\
												val &= 0xcf;\
												val |= (((unsigned char)(v)<<4)&0x30);\
												WriteAnalogReg(0x80+125,val);\
											}while(0)

#define		SET_PGA_RIGHT_N_AIN(v)			do{\
												unsigned char val=ReadAnalogReg(0x80+125);\
												val &= 0x3f;\
												val |= (((unsigned char)(v)<<6)&0xc0);\
												WriteAnalogReg(0x80+125,val);\
											}while(0)

#define		SET_PGA_GAIN_FIX_VALUE(v)		do{\
												unsigned char val=0;\
												val |= (((unsigned char)(v))&0x7f);\
												write_reg8(0xb63,val|0x80);\
											}while(0)


#endif
