/********************************************************************************************************
 * @file     irq.h 
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

#include "register.h"

/**
 * @brief      This function servers to enable IRQ.
 * @param[in]  none
 * @return     if return 1 is enable.else disable.
 */
static inline unsigned char irq_enable(void){
	unsigned char r = reg_irq_en;		// don't worry,  the compiler will optimize the return value if not used
	reg_irq_en = 1;
	return r;
}

/**
 * @brief      This function servers to disable IRQ.
 * @param[in]  none
 * @return     if return 0 is disable.else enable.
 */
static inline unsigned char irq_disable(void){
	unsigned char r = reg_irq_en;		// don't worry,  the compiler will optimize the return value if not used
	reg_irq_en = 0;
	return r;
}

/**
 * @brief      This function servers to restore IRQ.
 * @param[in]  none
 * @return     if return 1 is irg restore.
 */
static inline void irq_restore(unsigned char en){
	reg_irq_en = en;
}

/**
 * @brief      This function servers to set IRQ mask.
 * @param[in]  variable of msk.
 * @return     none.
 */

static inline void irq_enable_type(unsigned long msk)
{
	BM_SET(reg_irq_mask, msk);
}
/**
 * @brief      This function servers to set IRQ mask.
 * @param[in]  variable of msk.
 * @return     none.
 */
static inline void irq_set_mask(unsigned long msk){
	BM_SET(reg_irq_mask, msk);
}

/**
 * @brief      This function servers to enable one interrupt
 * @param[in]  none
 * @return     the value of IRQ register.
 */
static inline unsigned long irq_get_mask(void){
	return reg_irq_mask;
}

/**
 * @brief      This function servers to clear IRQ mask.
 * @param[in]  msk - variable of msk.
 * @return     none.
 */

static inline void irq_disable_type(unsigned long msk)
{
	BM_CLR(reg_irq_mask, msk);
}

/**
 * @brief      This function servers to get an IRQ source.
 * @param[in]  none.
 * @return     IRQ source.
 */
static inline unsigned long irq_get_src(){
	return reg_irq_src;
}

/**
 * @brief      This function servers to clear the specified IRQ source.
 * @param[in]  msk - variable of msk.
 * @return     none.
 */
static inline void irq_clr_src2(unsigned long msk)
{
    reg_irq_src |= msk;
}

/**
 * @brief      This function servers to clear all IRQ source.
 * @param[in]  none.
 * @return     none.
 */

static inline void irq_clr_src(void)
{
	reg_irq_src = 0xffffffff;
}

/**
 * @brief      This function servers to set the mask of RF IRQ.
 * @param[in]  msk - variable of msk.
 * @return     none.
 */
static inline void rf_irq_enable(unsigned int msk)
{
    reg_rf_irq_mask |= msk;
}

/**
 * @brief      This function servers to clear the mask of RF IRQ.
 * @param[in]  msk - variable of msk.
 * @return     none.
 */
static inline void rf_irq_disable(unsigned int msk)
{
    reg_rf_irq_mask &= (~msk);
}

/**
 * @brief      This function servers to get the RF IRQ source.
 * @param[in]  none.
 * @return     the state of RF IRQ register.
 */
static inline unsigned short rf_irq_src_get(void)
{
    return reg_rf_irq_status;
}

/**
 * @brief      This function servers to clear the RF IRQ source.
 * @param[in]  msk - variable of msk.
 * @return     none.
 */
static inline void rf_irq_clr_src(unsigned short msk)
{
    reg_rf_irq_status |= msk;
}

