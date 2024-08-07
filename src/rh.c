/*
 * rh.c
 *
 *  Created on: 05.04.2022
 *      Author: pvvx
 */
#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "battery.h"
#include "rh.h"

RAM rh_t rh;

static void pwm_init(void) {
	reg_clk_en0 |= FLD_CLK0_PWM_EN;
	reg_pwm_clk = 0; // = CLOCK_SYS_CLOCK_HZ 24Mhz
	pwm_set_mode(PWM_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM_ID, 18, 1); // 12..20/1
	pwm_start(PWM_ID);
	gpio_set_func(PWM_PIN, AS_PWMx);
}

//extern void adc_channel_init(ADC_InputPchTypeDef p_ain);

// p_ain - ADC analog input positive channel
uint16_t get_adc_rh_mv(void) {
	ADC_InputPchTypeDef p_ain = CHNL_RHI;
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_UP_DOWN_FLOAT);
	pwm_init();
	u32 t0 = clock_time();
	while (clock_time() - t0 < rh.tic);
	rh.rh = get_adc_mv(p_ain);
	pwm_stop(PWM_ID);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);
	return rh.rh;
}


void calibrate_rh(void) {
	uint32_t t0, tt;

	rh.tic = 0;

	rh.ubat = get_battery_mv();
	rh.ubat -= 700;
	rh.ubat >>= 1;
	rh.ubat -= rh.ubat >> 3;

	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_UP_DOWN_FLOAT);
	pwm_init();
	t0 = clock_time();
	do {
		tt = clock_time() - t0;
		rh.mv = get_adc_mv(CHNL_RHI);
		if(rh.mv >= rh.ubat) {
			rh.tic = tt;
			break;
		}
	} while(tt < 25 * CLOCK_16M_SYS_TIMER_CLK_1MS); // 25*CLOCK_16M_SYS_TIMER_CLK_1MS
	pwm_stop(PWM_ID);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);
}
