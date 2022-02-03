/********************************************************************************************************
 * @file     lpc.c
 *
 * @brief    This is the source file for TLSR8258
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

#include "lpc.h"
/**
 * @brief This function powers down low power comparator.
 * @param[in] none
 * @return none
 */
void lpc_power_down(void)
{
	analog_write(0x07,analog_read(0x07)|0x08);
}
/**
 * @brief This function provides power for low power comparator.
 * @param[in] none
 * @return none
 */
void lpc_power_on (void)
{
    analog_write(0x07,analog_read(0x07)&0xf7);
}
/**
 * @brief This function selects input channel for low power comparator .
 * @param[in] pin-selected input channel.Input derived from external PortB(PB<1>~PB<7>).
 * @return none
 */
void lpc_set_input_chn(LPC_Input_Channel_TypeDef pin)
{
	 analog_write(0x0d,(analog_read(0x0d)&0xf8)|pin);
}

/**
 * @brief This function selects input reference voltage for low power comparator .
 * @param[in] mode-lower power comparator working mode includes normal mode and low power mode.
 * @param[in] ref- selected input reference voltage.
 * @return none
 */
void lpc_set_input_ref(LPC_Mode_TypeDef mode, LPC_Reference_TypeDef ref)
{
    if(mode == LPC_NORMAL)
    {
    	analog_write(0x0b,analog_read(0x0b)&0xf7);
    	analog_write(0x0d,analog_read(0x0d)&0x7f);
    }
    else if (mode == LPC_LOWPOWER){

    	analog_write(0x0b,analog_read(0x0b)|0x08);
    	analog_write(0x0d,analog_read(0x0d)|0x80);
    }

    analog_write(0x0d,(analog_read(0x0d)&0x8f)|(ref<<4));

}
/**
 * @brief This function sets scaling_coefficient  for low power comparator .
 * @param[in] divider-selected scaling coefficient.(%25,%50,%75,%100)
 * @return none
 */
void lpc_set_scaling_coeff(LPC_Scaling_TypeDef divider)
{
	analog_write(0x0b,(analog_read(0x0b)&0xcf)|(divider<<4));
}


