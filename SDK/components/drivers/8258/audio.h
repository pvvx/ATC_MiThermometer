/********************************************************************************************************
 * @file     audio.h
 *
 * @brief    This is the Audio driver header file for TLSR8258
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

#ifndef audio_H
#define audio_H


#include "register.h"
#include "i2c.h"
#include "dfifo.h"

#define AUDIO_DBL_BUF_ENABLE	0

/**
 * define audio rate value.
 */
typedef enum{
	AUDIO_8K,
	AUDIO_16K,
	AUDIO_32K,
	RATE_SIZE
}AudioRate_Typedef;

/**
 * define audio input type.
 */
typedef enum{
	AMIC,
	DMIC,
	I2S_IN,
	USB_IN,
	BUF_IN,
}AudioInput_Typedef;

/**
 * define codec mode.
 */
typedef enum{
	CODEC_MODE_MICPHONE_TO_HEADPHONE_LINEOUT_I2S,
	CODEC_MODE_LINE_TO_HEADPHONE_LINEOUT_I2S,
	CODEC_MODE_I2S_TO_HEADPHONE_LINEOUT,
}CodecMode_Typedef;

/**
 * @brief     shut down audio and ADC  Module
 * @param[in] none.
 * @return    none
 */
void audio_stop(void);

/**
 * @brief     This function serves to reset audio Module
 * @param[in] none.
 * @return    none
 */
static inline void audio_reset(void)
{
	reg_rst2 = FLD_RST2_AUD;
	reg_rst2 = 0;
}

/**
 * 	@brief      This function serves to set the clock of dmic
 * 	@param[in]  step - the dividing factor of step.
 * 	@param[in]  mod - the dividing factor of mod.
 * 	@return     none
 */
static inline void audio_set_dmic_clk(unsigned char step,unsigned char mod)
{
	reg_dmic_step = step|FLD_DMIC_CLK_EN;
	reg_dmic_mod = mod;
}

/**
 * 	@brief      This function serves to set the clock of i2s
 * 	@param[in]  step - the dividing factor of step.
 * 	@param[in]  mod - the dividing factor of mod.
 * 	@return     none
 */
static inline void audio_set_i2s_clk(unsigned char step,unsigned char mod)
{
	reg_i2s_step = step|FLD_I2S_CLK_EN;
	reg_i2s_mod = mod;
}
/**
 * @brief      This function performs to read pointer/write pointer location.
 * @param[in]  none.
 * @return     the address of read/write.
 */
static inline unsigned short get_mic_wr_ptr (void)
{
	return reg_audio_wptr >>1;
}
/**
 * 	@brief     audio amic initial function. configure ADC corresponding parameters. set hpf,lpf and decimation ratio.
 * 	@param[in] Audio_Rate - audio rate value
 * 	@return    none
 */
void audio_amic_init(AudioRate_Typedef Audio_Rate);
/**
 * 	@brief     audio DMIC init function, config the speed of DMIC and downsample audio data to required speed.         actually audio data is dmic_speed/d_samp.
 * 	@param[in] Audio_Rate - set the DMIC speed. such as 1 indicate 1M and 2 indicate 2M.
 * 	@return    none.
 */
void audio_dmic_init(AudioRate_Typedef Audio_Rate);
/**
 * 	@brief     audio USB init function, config the speed of DMIC and downsample audio data to required speed.
 *            actually audio data is dmic_speed/d_samp.
 * 	@param[in] Audio_Rate - set the DMIC speed. such as 1 indicate 1M and 2 indicate 2M.
 * 	@return    none.
 */
void audio_usb_init(AudioRate_Typedef Audio_Rate);

/**
 * @brief     audio buff init function, config the speed of DMIC and downsample audio data to required speed.
 *            actually audio data is dmic_speed/d_samp.
 * @param[in] Audio_Rate  - audio rate.
 * @return    none.
 */
void audio_buff_init(AudioRate_Typedef Audio_Rate);

/**
 * 	@brief     audio I2S init in function, config the speed of i2s and MCLK to required speed.
 * 	@param[in] none.
 * 	@return    none.
 */
void audio_i2s_init(void);


/**
*	@brief	   sdm setting function, enable or disable the sdm output, configure SDM output paramaters.
*	@param[in]	InType -	  SDM input type, such as AMIC,DMIC,I2S_IN,USB_IN.
*	@param[in]	Audio_Rate - audio sampling rate, such as 16K,32k etc.
*	@param[in]	audio_out_en - audio output enable or disable set, '1' enable audio output; '0' disable output.
*	@return	none
*/
void audio_set_sdm_output(AudioInput_Typedef InType,AudioRate_Typedef Audio_Rate,unsigned char audio_out_en);

/**
 * @brief     This function servers to set USB input/output.
 * @param[in] none.
 * @return    none.
 */
void audio_set_usb_output(void);

/**
 * 	@brief	   	i2s setting function, enable or disable the i2s output, configure i2s output paramaters
 * 	@param[in] 	InType		- select audio input type including amic ,dmic ,i2s and usb
 * 	@param[in] 	Audio_Rate 	- select audio rate, which will decide on which adc sampling rate and relative decimation configuration will be chosen.
 * 	@return	  	none
 */
void audio_set_i2s_output(AudioInput_Typedef InType,AudioRate_Typedef Audio_Rate);

/**
 * 	@brief     This function serves to set I2S input.
 * 	@param[in] i2c_pin_group - select the pin for I2S.
 * 	@param[in] CodecMode - select I2S mode.
 * 	@param[in] sysclk - system clock.
 * 	@return    none.
 */
void audio_set_codec(I2C_GPIO_GroupTypeDef i2c_pin_group, CodecMode_Typedef CodecMode,unsigned sysclk);

#endif
