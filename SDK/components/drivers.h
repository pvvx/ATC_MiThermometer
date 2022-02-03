/********************************************************************************************************
 * @file     drivers.h
 *
 * @brief    This is the header file for TLSR8258
 *
 * @author	 author@telink-semi.com;
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
 *
 *******************************************************************************************************/

#pragma once

#include "config.h"

#if(__TL_LIB_8255__ || (MCU_CORE_TYPE == MCU_CORE_8255))
#include "drivers/8255/driver_8255.h"
#elif(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258))
#include "drivers/8258/driver_8258.h"
#else
#include "drivers/8258/driver_8258.h"
#endif
