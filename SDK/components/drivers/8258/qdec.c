/********************************************************************************************************
 * @file     putchar.h
 *
 * @brief    This is the source file for TLSR8258
 *
 * @author	 Driver Group
 * @date     December 5, 2018
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
#include "qdec.h"

/**
 * @brief      This function servers to set input port.
 * @param[in]  QDEC_InputAchTypeDef - input types of A channel.
 * @param[in]  QDEC_InputBchTypeDef - input types of A channel.
 * @return     none.
 */
void qdec_set_pin(QDEC_InputAchTypeDef channelA,QDEC_InputBchTypeDef channelB)
{
	write_reg8(reg_qdec_channel_a,channelA);
	write_reg8(reg_qdec_channel_b,channelB);
}

/**
 * @brief      This function servers to set qdec's mode.
 * @param[in]  QDEC_ModeTypeDef - mode type to select.
 * @return     none.
 */
void qdec_set_mode(QDEC_ModeTypeDef mode)
{
	write_reg8(reg_qdec_mode,(read_reg8(reg_qdec_mode)&0xfe)|mode);
}

/**
 * @brief      This function servers to initials qedc source clock.
 * @param[in]  none.
 * @return     none.
 */
void qdec_clk_en(void)
{
	rc_32k_cal();
	BM_SET(reg_clk_en0,FLD_CLK0_QDEC_EN);
}

volatile int total_count =0;

/**
 * @brief      This function servers to read hardware counting value.
 * @param[in]  none.
 * @return     hardware counting value.
 */
unsigned char qdec_get_count_value(void)
{
	char tmp;
	unsigned int pol = 0x100;
	write_reg8(rge_qdec_load,0x01);
	while(read_reg8(rge_qdec_load) & 0x01);

	tmp = read_reg8(reg_qdec_count);

	if((tmp >> 7) == 0x01)
		total_count-= (pol -tmp);
	else
		total_count += read_reg8(reg_qdec_count);

	return tmp;
}

/**
 * @brief      This function servers to reset the counter and the QDEC Counter value is cleared zero.
 * @param[in]  none.
 * @return     none.
 */
void qdec_clear_conuter(void)
{
	write_reg8(reg_qdec_reset,read_reg8(reg_qdec_reset)&0xfe);
}

/**
 * @brief      This function servers to set hardware debouncing.
 * @param[in]  thrsh - lower the value of thrsh,will be regard as jitter.
 * @return     none.
 */
void qdec_set_debouncing(char thrsh)
{
	write_reg8(reg_qdec_set,(read_reg8(reg_qdec_set)&0xf8)|(thrsh&0x07));
}




