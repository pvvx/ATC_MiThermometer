/*
 * sdm_out.c
 *
 *  SDMDAC (Sigma-Delta Modulation DAC)
 *
 *  Created on: 17 апр. 2025 г.
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app_config.h"

#define SDMDAC_MONO_MODE	1 // BIT(0) or 0

#if USE_SDM_OUT

#include "drivers.h"
#include "drivers/8258/gpio_8258.h"
#include "drivers/8258/pga.h"
#include "app.h"
#include "flash_eep.h"
#include "sensor.h"
#include "sdm_out.h"


#if	USE_SENSOR_SCD41
const cfg_dac_t sdmdac_cfg_def = {
		.in_min = 600,
		.in_max = 1000,
		.out_min = 6000,
		.out_max = 65535
}; // [5802 e803 7017 ffff 336b4a00]  k = 0x004a6b33
#define sdm_input_value measured_data.co2
#elif USE_SENSOR_ENS160
const cfg_dac_t sdmdac_cfg_def = {
		.in_min = 40,
		.in_max = 1000,
		.out_min = 6000,
		.out_max = 65535
};
#define sdm_input_value ens160.tvoc
#else
#error "Set sdm_input_value!"
#endif



RAM cfg_sdmdac_t sdmdac;

s16 sdm_out_buf[16]; // minimal size audio buf

void init_sdm(void) {

	//1. Dfifo setting
	reg_clk_en2 |= FLD_CLK2_AUD_EN | FLD_CLK2_DFIFO_EN; //enable dfifo clock, this will be initialed in cpu_wakeup_int()

	reg_pwm_ctrl = MASK_VAL( 	FLD_PWM_MULTIPLY2,			0,\
								FLD_PWM_ENABLE,				0,\
								FLD_LINER_INTERPOLATE_EN,	1,\
								FLD_LEFT_SHAPING_EN,		0,\
								FLD_RIGTH_SHAPING_EN,		0);

	audio_set_i2s_clk(1,24); // set the clock


	reg_ascl_tune = 0x01063001; // 8k: 0x00832001, 16k: 0x01063001, 32k: 0x020C5001;

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

	reg_dfifo_mode = FLD_AUD_DFIFO0_OUT;

	reg_audio_ctrl = FLD_AUDIO_SDM_PLAYER_EN; // | FLD_AUDIO_MONO_MODE ?
}

void sdm_off(void) {
	reg_audio_ctrl = AUDIO_OUTPUT_OFF;
	gpio_set_func(GPIO_PB6, AS_GPIO);
	gpio_set_func(GPIO_PB7, AS_GPIO);
	reg_clk_en2 &= ~FLD_CLK2_AUD_EN;
}

// Set SDMDAC:
// value_dac0 Out: GPIO_PB4/GPIO_PB5
// value_dac1 Out: GPIO_PB6/GPIO_PB7
void sdm_out(signed short value_dac0, signed short value_dac1) {
	if ((reg_clk_en2 & FLD_CLK2_AUD_EN) == 0) {
		dfifo_set_dfifo0((unsigned short*)sdm_out_buf,sizeof(sdm_out_buf));
		init_sdm();
#ifdef GPIO_SDM_P0
		gpio_set_func(GPIO_SDM_P0, AS_SDM);
#endif
#ifdef GPIO_SDM_N0
		gpio_set_func(GPIO_SDM_N0, AS_SDM);
#endif
#ifdef GPIO_SDM_P1
		gpio_set_func(GPIO_SDM_P1, AS_SDM);
#endif
#ifdef GPIO_SDM_N1
		gpio_set_func(GPIO_SDM_N1, AS_SDM);
#endif
	}
	for(int i = 0; i < 16; i += 2) {
		sdm_out_buf[i] = value_dac0;
		reg_usb_mic_dat0 = value_dac0;
		sdm_out_buf[i+1] = value_dac1;
		reg_usb_mic_dat1 = value_dac1;
	}
}


void init_dac(void) {
	s32 k;
	if(flash_read_cfg(&sdmdac.cfg, EEP_ID_DAC, sizeof(sdmdac.cfg)) != sizeof(sdmdac.cfg))
		memcpy(&sdmdac.cfg, &sdmdac_cfg_def,sizeof(sdmdac.cfg));
	if(sdmdac.cfg.in_max != sdmdac.cfg.in_min) {
		k = ((s32)sdmdac.cfg.out_max - (s32)sdmdac.cfg.out_min) << 15;
		k /= (s32)sdmdac.cfg.in_max - (s32)sdmdac.cfg.in_min;
	} else
		k = 1;
	sdmdac.k = k;
}

void set_dac(void) {
	s32 out = ((((s32)sdm_input_value - (s32)sdmdac.cfg.in_min) * sdmdac.k) >> 15) + sdmdac.cfg.out_min;
	if(sdmdac.cfg.out_max > sdmdac.cfg.out_min) {
		if(out > sdmdac.cfg.out_max)
			out = sdmdac.cfg.out_max;
		else if (out < sdmdac.cfg.out_min)
			out = sdmdac.cfg.out_min;
	} else { // sdmdac.cfg.out_max < sdmdac.cfg.out_min
		if(out > sdmdac.cfg.out_min)
			out = sdmdac.cfg.out_min;
		else if (out < sdmdac.cfg.out_max)
			out = sdmdac.cfg.out_max;
	}
	out -= 0x8000;
	sdm_out(out, out);
}


#endif // USE_SDM_OUT
