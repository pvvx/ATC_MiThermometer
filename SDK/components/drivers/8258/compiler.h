/********************************************************************************************************
 * @file     compiler.h 
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

#pragma once


#define _attribute_packed_				__attribute__((packed))
#define _attribute_aligned_(s)			__attribute__((aligned(s)))
#define _attribute_session_(s)			__attribute__((section(s)))
#define _attribute_ram_code_  			_attribute_session_(".ram_code")
#define _attribute_my_ram_code_  		_attribute_session_(".my_ram_code")
#define _attribute_custom_code_  		_attribute_session_(".custom") volatile
#define _attribute_no_inline_   		__attribute__((noinline))
#define _inline_ 						inline

#ifndef	BLC_PM_DEEP_RETENTION_MODE_EN
#define	BLC_PM_DEEP_RETENTION_MODE_EN		1
#endif

#if (BLC_PM_DEEP_RETENTION_MODE_EN)
	#define _attribute_data_retention_   	_attribute_session_(".retention_data")
#else
    #define _attribute_data_retention_
#endif


