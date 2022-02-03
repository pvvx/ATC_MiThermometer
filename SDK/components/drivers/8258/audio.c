/********************************************************************************************************
 * @file     audio.c
 *
 * @brief    This is the Audio driver file for TLSR8258
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

#include "audio.h"
#include "pga.h"
#include "adc.h"
#include "irq.h"
#include "adc.h"
#include "register.h"


unsigned char AMIC_ADC_SampleLength[3] = {0xf0/*96K*/,0xac/*132K*/,0x75/*192K*/};


unsigned char DMIC_CLK_Mode[RATE_SIZE] = {	47/*8k	1021.277*/,
											47/*16k	1021.277*/,
										    47/*32k	1021.277*/};

unsigned char DMIC_CIC_Rate[RATE_SIZE] = {	0x23/*8k 7.978723*/,
											0x22/*16k 15.95745*/,
											0x11/*32k 31.91489*/};


unsigned char AMIC_CIC_Rate[RATE_SIZE] = {	0xab/*8k  96/12	*/,
											0x32/*16k 96/6	*/,
											0x42/*32k 96/3	*/};

/*  matrix used under condition: I2S = 2 MHz  */
unsigned long ASDM_Rate_Matching[RATE_SIZE] = {	0x00832001/*8k */,
												0x01063001/*16k*/,
												0x020C5001/*32k*/};

/*  matrix used under condition: I2S = 2 MHz  */
unsigned long DSDM_Rate_Matching[RATE_SIZE] = {	0x00820001/*8 k*/,
												0x01058001/*16k*/,
											    0x020AF001/*32k*/};

enum {
	AUD_DMIC,
	AUD_AMIC,
};

#define SET_PFM(v)     do{\
                           BM_CLR(reg_adc_period_chn0,FLD_ADC_CHNM_PERIOD);\
	              	   	   reg_adc_period_chn0 |= MASK_VAL(FLD_ADC_CHNM_PERIOD,v);\
                       }while(0)
#define SET_PFL(v)     do{\
                           BM_CLR(reg_adc_period_chn12,FLD_ADC_CHNLR_PERIOD);\
                           reg_adc_period_chn12 |= MASK_VAL(FLD_ADC_CHNLR_PERIOD,v);\
                       }while(0)

/**
 * @brief     shut down audio and ADC  Module
 * @param[in] none.
 * @return    none
 */
void audio_stop(void)
{
	adc_power_on_sar_adc(0);   //power off sar adc
	adc_enable_clk_24m_to_sar_adc(0);  //disable signal of 24M clock to sar adc
}

/**
 * 	@brief     audio amic initial function. configure ADC corresponding parameters. set hpf,lpf and decimation ratio.
 * 	@param[in] Audio_Rate - audio rate value
 * 	@return    none
 */
void audio_amic_init(AudioRate_Typedef Audio_Rate)
{

	/*******1.ADC setting for analog audio sample*************************
	 * 32K 16K   */
	adc_power_on_sar_adc(0);

	adc_reset_adc_module();   //reset whole digital adc module

	adc_set_sample_clk(5); //adc sample clk= 24M/(1+5)=4M

	//adc state machine state cnt 2( "set" stage and "capture" state for left channel)
	adc_set_chn_enable_and_max_state_cnt(ADC_LEFT_CHN, 2);//left channel enable and  state cnt= 2

	//set "capture state" length for misc channel: 240
	//set "set state" length for left channel: 10
	//adc state machine  period  = 24M/250 = 96K
	adc_set_state_length(0, 240, 10);	//	96K

	adc_set_input_mode(ADC_LEFT_CHN, DIFFERENTIAL_MODE); //left channel differential mode
	adc_set_ain_channel_differential_mode(ADC_LEFT_CHN, PGA0P, PGA0N); //left channel positive and negative data in

	adc_set_ref_voltage(ADC_RIGHT_CHN, ADC_VREF_0P6V);


	adc_set_resolution(ADC_LEFT_CHN, RES14);							//left channel resolution
	adc_set_tsample_cycle(ADC_LEFT_CHN, SAMPLING_CYCLES_6);				//left channel tsample

//	adc_set_vref_vbat_divider(ADC_VBAT_DIVIDER_OFF);                    //vbat vref divider
	adc_set_ain_pre_scaler(ADC_PRESCALER_1);  //ain pre scaler none

	adc_set_itrim_preamp(ADC_CUR_TRIM_PER100);
	adc_set_itrim_vrefbuf(ADC_CUR_TRIM_PER125);
	adc_set_itrim_vcmbuf(ADC_CUR_TRIM_PER100);
	//PGA0 left  C0/C1 ON,
	//PGA1 right C2/C3 OFF

	SET_PGA_LEFT_P_AIN(PGA_AIN_C1);
	SET_PGA_LEFT_N_AIN(PGA_AIN_C1);
	SET_PGA_RIGHT_P_AIN(PGA_AIN_C0);
	SET_PGA_RIGHT_N_AIN(PGA_AIN_C0);

	//
	adc_set_left_boost_bias(GAIN_STAGE_BIAS_PER75);

	analog_write (areg_adc_pga_ctrl, MASK_VAL( FLD_PGA_ITRIM_GAIN_L, GAIN_STAGE_BIAS_PER150, \
														  FLD_PGA_ITRIM_GAIN_R,GAIN_STAGE_BIAS_PER150, \
														  FLD_ADC_MODE, 0, \
														  FLD_SAR_ADC_POWER_DOWN, 1, \
														  FLD_POWER_DOWN_PGA_CHN_L, 0, \
														  FLD_POWER_DOWN_PGA_CHN_R, 1) );


	WriteAnalogReg(0xfe,0x05);		//0x80+126  = 0x05,0xfe default value is 0xe5,for output audio, must clear 0xfe<7:5>
/***********************************PGA gain setting*******************************************/
	//0xb63[7] 1b'1 enable gain mode to manual mode  manual mode 1

	reg_pga_fix_value = MASK_VAL(	FLD_PGA_POST_AMPLIFIER_GAIN,PGA_POST_GAIN_0DB,\
									FLD_PGA_PRE_AMPLIFIER_GAIN,PGA_PRE_GAIN_18DB,\
									FLD_PGA_GAIN_FIX_EN, 1);


	////////////////////////////// ALC HPF LPF setting /////////////////////////////////
	//enable hpf, enable lpf, anable alc, disable double_down_sampling


	reg_aud_alc_hpf_lpf_ctrl =   MASK_VAL( FLD_AUD_IN_HPF_SFT,  0x0b,   //different pcb may set different value.
										   FLD_AUD_IN_HPF_BYPASS, 0, \
										   FLD_AUD_IN_ALC_BYPASS, 0, \
										   FLD_AUD_IN_LPF_BYPASS, 0, \
										   FLD_DOUBLE_DOWN_SAMPLING_ON,\
										   (Audio_Rate==AUDIO_32K)?0:1);


	if(Audio_Rate==AUDIO_32K)
	{
		reg_dfifo_dec_ratio = AMIC_CIC_Rate[AUDIO_32K];
	}
	else
	{
		if(Audio_Rate==AUDIO_16K)
		{
			reg_dfifo_dec_ratio = AMIC_CIC_Rate[AUDIO_32K];//32k
		}
		else if(Audio_Rate==AUDIO_8K)
		{
			reg_dfifo_dec_ratio = AMIC_CIC_Rate[AUDIO_16K];//16k
		}
	}

	//alc mode select digital mode
	reg_aud_alc_cfg &= ~FLD_AUD_ALC_ANALOG_MODE_EN;
	//alc left channel select manual regulate, and set volume

	reg_aud_alc_vol_l_chn = MASK_VAL( FLD_AUD_ALC_MIN_VOLUME_IN_DIGITAL_MODE,  0x24, \
									  FLD_AUD_ALC_DIGITAL_MODE_AUTO_REGULATE_EN, 0);

	//2. Dfifo setting
	reg_clk_en2 |= FLD_CLK2_DFIFO_EN; //enable dfifo clock, this will be initialed in cpu_wakeup_int()
#if AUDIO_DBL_BUF_ENABLE
	reg_dfifo_mode = FLD_AUD_DFIFO1_IN;
#else
	reg_dfifo_mode = FLD_AUD_DFIFO0_IN;
#endif

	//amic input, mono mode, enable decimation filter
	reg_dfifo_ain =  MASK_VAL( FLD_AUD_DMIC0_DATA_IN_RISING_EDGE,AUDIO_DMIC_DATA_IN_FALLING_EDGE,\
									FLD_AUD_INPUT_SELECT, AUDIO_INPUT_AMIC, \
									FLD_AUD_INPUT_MONO_MODE, 1, \
									FLD_AUD_DECIMATION_FILTER_BYPASS, 0);


	reg_audio_dec_mode  |= FLD_AUD_LNR_VALID_SEL | FLD_AUD_CIC_MODE;

    adc_power_on_sar_adc(1);

}

/**
 * @brief     This function servers to receive data from buffer.
 * @param[in] buf - the buffer in which the data need to write
 * @param[in] len - the length of the buffer.
 * @return    none.
 */
void audio_rx_data_from_buff(signed char* buf,unsigned int len)
{
	signed short data;

	for (int i=0; i<len; i+=2)
	{
		if(i%4)
		{
			data = buf[i] & 0xff;
			data |= buf[i+1] << 8;
			reg_usb_mic_dat0 = data;
		}
		else
		{
			data = buf[i] & 0xff;
			data |= buf[i+1] << 8;
			reg_usb_mic_dat1 = data;
		}
	}
}

/**
 * @brief     audio DMIC init function, config the speed of DMIC and downsample audio data to required speed.
 *            actually audio data is dmic_speed/d_samp.
 * @param[in] Audio_Rate  - audio rate.
 * @return    none.
 */
void audio_dmic_init(AudioRate_Typedef Audio_Rate)
{
	//1. Dfifo setting
	reg_clk_en2 |= FLD_CLK2_DFIFO_EN; //enable dfifo clock, this will be initialed in cpu_wakeup_int()
	reg_dfifo_mode = FLD_AUD_DFIFO0_IN | FLD_AUD_DFIFO0_L_INT;

	//2. amic input, mono mode, enable decimation filter
	reg_dfifo_ain = 	  MASK_VAL( FLD_AUD_DMIC0_DATA_IN_RISING_EDGE,	AUDIO_DMIC_DATA_IN_RISING_EDGE,\
									FLD_AUD_INPUT_SELECT, 				AUDIO_INPUT_DMIC, \
									FLD_AUD_INPUT_MONO_MODE, 			1, \
									FLD_AUD_DECIMATION_FILTER_BYPASS, 	0, \
									FLD_AUD_DMIC_RISING_EDGE_BYPASS  ,	0, \
									FLD_AUD_DMIC_FALLING_EDGE_BYPASS ,	0);

	reg_dfifo_dec_ratio =DMIC_CIC_Rate[Audio_Rate];//by minghai.duan 2019.7.24
	//reg_dfifo_dec_ratio = 0x3a;

	////////////////////////////// ALC HPF LPF setting /////////////////////////////////
	//enable hpf, enable lpf, anable alc, disable double_down_sampling
	reg_aud_alc_hpf_lpf_ctrl = MASK_VAL( FLD_AUD_IN_HPF_SFT,  0x0b,   //different pcb may set different value.
										   FLD_AUD_IN_HPF_BYPASS, 0, \
										   FLD_AUD_IN_ALC_BYPASS, 1, \
										   FLD_AUD_IN_LPF_BYPASS, 1, \
										   FLD_DOUBLE_DOWN_SAMPLING_ON, 1);
	/*******1.Dmic setting for audio input**************************/
	reg_audio_ctrl = AUDIO_OUTPUT_OFF;

	audio_set_dmic_clk(0x8a,0xea);     //16K*2*64=2.048M,

}


/**
 * @brief     audio USB init function, config the speed of DMIC and downsample audio data to required speed.
 *            actually audio data is dmic_speed/d_samp.
 * @param[in] Audio_Rate  - audio rate.
 * @return    none.
 */
void audio_usb_init(AudioRate_Typedef Audio_Rate)
{
	//1. Dfifo setting
	reg_clk_en2 |= FLD_CLK2_DFIFO_EN; //enable dfifo clock, this will be initialed in cpu_wakeup_int()
	reg_dfifo_mode = FLD_AUD_DFIFO0_IN | FLD_AUD_DFIFO0_L_INT;

	//2. amic input, mono mode, enable decimation filter
	reg_dfifo_ain = 	  MASK_VAL( FLD_AUD_DMIC0_DATA_IN_RISING_EDGE,	AUDIO_DMIC_DATA_IN_RISING_EDGE,\
									FLD_AUD_INPUT_SELECT, 				AUDIO_INPUT_USB, \
									FLD_AUD_INPUT_MONO_MODE, 			0, \
									FLD_AUD_DECIMATION_FILTER_BYPASS, 	1, \
									FLD_AUD_DMIC_RISING_EDGE_BYPASS  ,	0, \
									FLD_AUD_DMIC_FALLING_EDGE_BYPASS ,	0);

	reg_dfifo_dec_ratio = 0x00;

	////////////////////////////// ALC HPF LPF setting /////////////////////////////////
	//enable hpf, enable lpf, anable alc, disable double_down_sampling
	reg_aud_alc_hpf_lpf_ctrl =   MASK_VAL( FLD_AUD_IN_HPF_SFT,  0x0b,   //different pcb may set different value.
										   FLD_AUD_IN_HPF_BYPASS, 1, \
										   FLD_AUD_IN_ALC_BYPASS, 1, \
										   FLD_AUD_IN_LPF_BYPASS, 1, \
										   FLD_DOUBLE_DOWN_SAMPLING_ON, 0);
	/*******1.Dmic setting for audio input**************************/
	reg_audio_ctrl = AUDIO_OUTPUT_OFF;

	reg_pga_fix_value = MASK_VAL(	FLD_PGA_POST_AMPLIFIER_GAIN, PGA_POST_GAIN_0DB,\
										FLD_PGA_PRE_AMPLIFIER_GAIN, PGA_PRE_GAIN_18DB,\
										FLD_PGA_GAIN_FIX_EN, 0);

	audio_set_dmic_clk(0x81,0x18);     //16K*2*64=2.048M,

}


/**
 * @brief     audio buff init function, config the speed of DMIC and downsample audio data to required speed.
 *            actually audio data is dmic_speed/d_samp.
 * @param[in] Audio_Rate  - audio rate.
 * @return    none.
 */
void audio_buff_init(AudioRate_Typedef Audio_Rate)
{
	//1. Dfifo setting
	reg_clk_en2 |= FLD_CLK2_DFIFO_EN; //enable dfifo clock, this will be initialed in cpu_wakeup_int()
	reg_dfifo_mode = FLD_AUD_DFIFO0_IN | FLD_AUD_DFIFO0_L_INT;

	//2. amic input, mono mode, enable decimation filter
	reg_dfifo_ain = 	  MASK_VAL( FLD_AUD_DMIC0_DATA_IN_RISING_EDGE,	AUDIO_DMIC_DATA_IN_RISING_EDGE,\
									FLD_AUD_INPUT_SELECT, 				AUDIO_INPUT_USB, \
									FLD_AUD_INPUT_MONO_MODE, 			0, \
									FLD_AUD_DECIMATION_FILTER_BYPASS, 	1, \
									FLD_AUD_DMIC_RISING_EDGE_BYPASS  ,	0, \
									FLD_AUD_DMIC_FALLING_EDGE_BYPASS ,	0);

	reg_dfifo_dec_ratio = 0x00;

	////////////////////////////// ALC HPF LPF setting /////////////////////////////////
	//enable hpf, enable lpf, anable alc, disable double_down_sampling
	reg_aud_alc_hpf_lpf_ctrl =   MASK_VAL( FLD_AUD_IN_HPF_SFT,  0x0b,   //different pcb may set different value.
										   FLD_AUD_IN_HPF_BYPASS, 1, \
										   FLD_AUD_IN_ALC_BYPASS, 1, \
										   FLD_AUD_IN_LPF_BYPASS, 1, \
										   FLD_DOUBLE_DOWN_SAMPLING_ON, 0);
	/*******1.Dmic setting for audio input**************************/
	reg_audio_ctrl = AUDIO_OUTPUT_OFF;
  //set  PGA gain
	reg_pga_fix_value = MASK_VAL(	FLD_PGA_POST_AMPLIFIER_GAIN, PGA_POST_GAIN_0DB,\
										FLD_PGA_PRE_AMPLIFIER_GAIN, PGA_PRE_GAIN_18DB,\
										FLD_PGA_GAIN_FIX_EN, 0);

	audio_set_dmic_clk(0x81,0x18);     //16K*2*64=2.048M,

}

 /**
 *	@brief	   sdm setting function, enable or disable the sdm output, configure SDM output paramaters.
 *	@param[in]	InType -	  SDM input type, such as AMIC,DMIC,I2S_IN,USB_IN.
 *	@param[in]	Audio_Rate - audio sampling rate, such as 16K,32k etc.
 *	@param[in]	audio_out_en - audio output enable or disable set, '1' enable audio output; '0' disable output.
 *	@return	none
 */
void audio_set_sdm_output(AudioInput_Typedef InType,AudioRate_Typedef Audio_Rate,unsigned char audio_out_en)
{
	if(audio_out_en)
	{
		//SDM0  EVB(C1T139A30_V1.2) not used
//		gpio_set_func(GPIO_PB4, AS_SDM);
//		gpio_set_func(GPIO_PB5, AS_SDM);
		//SDM1
		gpio_set_func(GPIO_PB6, AS_SDM);
		gpio_set_func(GPIO_PB7, AS_SDM);

		reg_pwm_ctrl = MASK_VAL( 	FLD_PWM_MULTIPLY2,			0,\
									FLD_PWM_ENABLE,				0,\
									FLD_LINER_INTERPOLATE_EN,	1,\
									FLD_LEFT_SHAPING_EN,		0,\
									FLD_RIGTH_SHAPING_EN,		0);

		reg_pn2_right &= SDM_LEFT_CHN_CONST_EN;		//enable pn

		audio_set_i2s_clk(1,24);

		reg_ascl_tune = ASDM_Rate_Matching[Audio_Rate];


		reg_pn1_left =	MASK_VAL( 	PN1_LEFT_CHN_BITS,		6,\
									PN2_LEFT_CHN_EN, 		0,\
									PN1_LEFT_CHN_EN, 		0);
		reg_pn2_left =	MASK_VAL( 	PN2_LEFT_CHN_BITS,		6,\
									PN2_RIGHT_CHN_EN, 		0,\
									PN1_RIGHT_CHN_EN, 		0);
		reg_pn1_right =	MASK_VAL( 	PN1_RIGHT_CHN_BITS,		6,\
									CLK2A_AUDIO_CLK_EN, 	0,\
									EXCHANGE_SDM_DATA_EN,	0);
		reg_pn2_right = MASK_VAL( 	PN2_RIGHT_CHN_BITS,		6,\
									SDM_LEFT_CHN_CONST_EN, 	0,\
									SDM_RIGHT_CHN_CONST_EN, 0);

		reg_dfifo_mode |= FLD_AUD_DFIFO0_OUT;

		//config player mode
		if(InType == I2S_IN)
		{
			reg_audio_ctrl = (FLD_AUDIO_MONO_MODE|FLD_AUDIO_SDM_PLAYER_EN|FLD_AUDIO_I2S_RECORDER_EN|FLD_AUDIO_I2S_INTERFACE_EN);
		}
		else if(InType == USB_IN)
		{
			reg_audio_ctrl = (FLD_AUDIO_SDM_PLAYER_EN|FLD_AUDIO_HPF_EN);
		}
		else if(InType==BUF_IN)
		{
			reg_audio_ctrl = (FLD_AUDIO_MONO_MODE|FLD_AUDIO_SDM_PLAYER_EN);
		}
		else
		{
			reg_audio_ctrl = (FLD_AUDIO_SDM_PLAYER_EN);
		}
	}
	else
	{
		reg_audio_ctrl = AUDIO_OUTPUT_OFF;
	}
}

/*
*	The software control interface may be operated using either a 3-wire (SPI-compatible) or 2-wire MPU
*	interface. Selection of interface format is achieved by setting the state of the MODE pin.
*		MODE	0	2 wire		default(hardware)
*				1	3 wire
*	In 2-wire mode, the state of CSB pin allows the user to select one of two addresses.
*		CSB		0	0011010		default(hardware)
*				1   0011011
*
*	note:
*		The WM8731 is not a standard I2C interface, so the Kite standard I2C driver is not used.
*/

#define		WM8731_LEFT_IN						0x00		//Left Line In
#define		WM8731_RIGHT_IN						0x02		//Right Line In
#define		WM8731_LEFT_HEADPHONE_OUT			0x04		//Left Headphone Out
#define		WM8731_RIGHT_HEADPHONE_OUT			0x06		//Right Headphone Out

#define		WM8731_ANA_AUDIO_PATH_CTRL			0x08		//Analogue Audio Path Control
#define		WM8731_DIG_AUDIO_PATH_CTRL			0x0a		//Digital Audio Path Control
#define		WM8731_POWER_DOWN_CTRL				0x0c		//Power Down Control
#define		WM8731_DIG_AUDIO_INTERFACE_FORMAT	0x0e		//Digital Audio Interface Format
#define		WM8731_SAMPLING_CTRL 				0x10		//Sampling Control
#define		WM8731_ACTIVE_CTRL 					0x12		//Active Control
#define		WM8731_RESET_CTRL 					0x1e		//Reset Register

unsigned char LineIn_To_I2S_CMD_TAB[9][2]={	{WM8731_RESET_CTRL, 				0x00},
											{WM8731_LEFT_IN,					0x17},
											{WM8731_RIGHT_IN,					0x17},
											{WM8731_ANA_AUDIO_PATH_CTRL,		0x02},
											{WM8731_DIG_AUDIO_PATH_CTRL,		0x09},
											{WM8731_POWER_DOWN_CTRL,			0x0a},
											{WM8731_DIG_AUDIO_INTERFACE_FORMAT,	0x02},
											{WM8731_SAMPLING_CTRL,				0x18},
											{WM8731_ACTIVE_CTRL,				0x01},
};

unsigned char I2S_To_HPout_CMD_TAB[7][2] ={	{WM8731_RESET_CTRL, 				0x00},
											{WM8731_ANA_AUDIO_PATH_CTRL,		0x12},
											{WM8731_DIG_AUDIO_PATH_CTRL,		0x00},
											{WM8731_POWER_DOWN_CTRL,			0x07},
											{WM8731_DIG_AUDIO_INTERFACE_FORMAT,	0x02},
											{WM8731_SAMPLING_CTRL,				0x18},
											{WM8731_ACTIVE_CTRL,				0x01},
};

/**
 * @brief     This function servers to config I2S input.
 * @param[in] i2c_pin_group - select the pin for I2S.
 * @param[in] CodecMode - select I2S mode.
 * @param[in] sysclk - system clock.
 * @return    none.
 */
void audio_set_codec( I2C_GPIO_GroupTypeDef i2c_pin_group , CodecMode_Typedef CodecMode,unsigned sysclk)
{

	unsigned char i = 0;

	//I2C pin set
	i2c_gpio_set(i2c_pin_group);  	//SDA/CK : A3/A4
	i2c_master_init(0x34, (unsigned char)(sysclk/(4*200000)) );		//i2c clock 200K, only master need set i2c clock

	if(CodecMode == CODEC_MODE_LINE_TO_HEADPHONE_LINEOUT_I2S)
	{
		for(i=0;i<9;i++)
		{
			do
			{
				write_reg8(0x04,LineIn_To_I2S_CMD_TAB[i][0]);
				write_reg8(0x05,LineIn_To_I2S_CMD_TAB[i][1]);
				write_reg8(0x07,0x37);
				while(read_reg8(0x02)&0x01);
			}
			while(read_reg8(0x02)&0x04);
		}
	}
	else if(CodecMode == CODEC_MODE_I2S_TO_HEADPHONE_LINEOUT)
	{
		for(i=0;i<7;i++)
		{
			do
			{
				write_reg8(0x04,I2S_To_HPout_CMD_TAB[i][0]);
				write_reg8(0x05,I2S_To_HPout_CMD_TAB[i][1]);
				write_reg8(0x07,0x37);
				while(read_reg8(0x02)&0x01);
			}
			while(read_reg8(0x02)&0x04);
		}
	}
	else
	{

	}
}

/**
 * @brief     audio I2S init in function, config the speed of i2s and MCLK to required speed.
 * @param[in] none.
 * @return    none.
 */
void audio_i2s_init(void)
{
	/*******1.I2S setting for audio input**************************/
	reg_audio_ctrl = AUDIO_OUTPUT_OFF;

	//config dmic clock for 12Mhz to offer the MCLK of CORDEC
	gpio_set_func(GPIO_PA1, AS_DMIC);		//XTI/MCLK from DMIC clk
	audio_set_dmic_clk(0x81,0x4);

	//config i2s clock for 1Mhz to offer the clk of CORDEC
	gpio_set_func(GPIO_PD7, AS_I2S);		//I2S_BCK
	gpio_set_func(GPIO_PD2, AS_I2S);		//I2S_ADC_LRC  I2S_DAC_LRC
	gpio_set_func(GPIO_PD3, AS_I2S);		//I2S_ADC_DAT
	gpio_set_func(GPIO_PD4, AS_I2S);		//I2S_DAC_DAT
	gpio_set_input_en(GPIO_PD3, 1);

	audio_set_i2s_clk(0x81,0x18);

	reg_dfifo_dec_ratio = 0;

	reg_clk_en2 |= FLD_CLK2_DFIFO_EN; //enable dfifo clock, this will be initialed in cpu_wakeup_int()

	//2. amic input, mono mode, enable decimation filter
	reg_dfifo_ain = 	  MASK_VAL( FLD_AUD_DMIC0_DATA_IN_RISING_EDGE,	AUDIO_DMIC_DATA_IN_RISING_EDGE,\
									FLD_AUD_INPUT_SELECT, 				AUDIO_INPUT_I2S, \
									FLD_AUD_INPUT_MONO_MODE, 			1, \
									FLD_AUD_DECIMATION_FILTER_BYPASS, 	1, \
									FLD_AUD_DMIC_RISING_EDGE_BYPASS  ,	0, \
									FLD_AUD_DMIC_FALLING_EDGE_BYPASS ,	0);

	reg_audio_ctrl |= FLD_AUDIO_I2S_INTERFACE_EN|FLD_AUDIO_I2S_RECORDER_EN;

	////////////////////////////// ALC HPF LPF setting /////////////////////////////////
	//enable hpf, enable lpf, anable alc, disable double_down_sampling
	reg_aud_alc_hpf_lpf_ctrl =   MASK_VAL( FLD_AUD_IN_HPF_SFT,  0x0b,   //different pcb may set different value.
										   FLD_AUD_IN_HPF_BYPASS, 1, \
										   FLD_AUD_IN_ALC_BYPASS, 1, \
										   FLD_AUD_IN_LPF_BYPASS, 1, \
										   FLD_DOUBLE_DOWN_SAMPLING_ON, 1);

	reg_dfifo_dec_ratio = 0x00;


	reg_dfifo_mode = FLD_AUD_DFIFO0_IN | FLD_AUD_DFIFO0_OUT;
}


/**
 *
 * @brief	   	i2s setting function, enable or disable the i2s output, configure i2s output paramaters
 * @param[in] 	InType		- select audio input type including amic ,dmic ,i2s and usb
 * @param[in] 	Audio_Rate 	- select audio rate, which will decide on which adc sampling rate and relative decimation configuration will be chosen.
 * @return	  	none
 */
void audio_set_i2s_output(AudioInput_Typedef InType,AudioRate_Typedef Audio_Rate)
{
	//config dmic clock for 12Mhz to offer the MCLK of CORDEC
	gpio_set_func(GPIO_PA1, AS_DMIC);		//XTI/MCLK from DMIC clk
	audio_set_dmic_clk(0x81,0x4);

	//config i2s clock for 1Mhz to offer the clk of CORDEC
	gpio_set_func(GPIO_PD7, AS_I2S);		//I2S_BCK
	gpio_set_func(GPIO_PD2, AS_I2S);		//I2S_ADC_LRC  I2S_DAC_LRC
	gpio_set_func(GPIO_PD3, AS_I2S);		//I2S_ADC_DAT
	gpio_set_func(GPIO_PD4, AS_I2S);		//I2S_DAC_DAT
	gpio_set_input_en(GPIO_PD3, 1);
	audio_set_i2s_clk(1,0x18);

	reg_pwm_ctrl = MASK_VAL( 	FLD_PWM_MULTIPLY2,			0,\
								FLD_PWM_ENABLE,				0,\
								FLD_LINER_INTERPOLATE_EN,	1,\
								FLD_LEFT_SHAPING_EN,		0,\
								FLD_RIGTH_SHAPING_EN,		0);


	if(InType==AMIC)
	{
		reg_ascl_tune = 0x80000001;
	}
	else
	{
		reg_ascl_tune = DSDM_Rate_Matching[Audio_Rate];
	}

	//config player mode
	if(InType == I2S_IN)
	{
		reg_audio_ctrl = (FLD_AUDIO_MONO_MODE|FLD_AUDIO_SDM_PLAYER_EN|FLD_AUDIO_I2S_RECORDER_EN|FLD_AUDIO_I2S_INTERFACE_EN);
	}
	else if(InType == USB_IN)
	{
		reg_audio_ctrl = (FLD_AUDIO_MONO_MODE|FLD_AUDIO_SDM_PLAYER_EN|FLD_AUDIO_ISO_PLAYER_EN);
	}
	else
	{
		reg_audio_ctrl = (FLD_AUDIO_MONO_MODE|FLD_AUDIO_I2S_INTERFACE_EN|FLD_AUDIO_I2S_PLAYER_EN);
	}

	reg_dfifo_mode |= FLD_AUD_DFIFO0_OUT;
}

/**
 * @brief     This function servers to set USB input/output.
 * @param[in] none.
 * @return    none.
 */
void audio_set_usb_output(void)
{
	write_reg8(0xb10,0xf9);
}



