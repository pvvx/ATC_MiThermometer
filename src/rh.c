/*
 * rh.c
 *
 *  Created on: 05.04.2022
 *      Author: pvvx
 */
#include "tl_common.h"
#if (DEV_SERVICES & SERVICE_PLM) // USE_SENSOR_PWMRH
#include "drivers.h"
#include "battery.h"
#include "rh.h"
#include "app.h"


RAM rh_t rh;

static void pwm_init(void) {
	reg_clk_en0 |= FLD_CLK0_PWM_EN;
	reg_pwm_clk = 0; // = CLOCK_SYS_CLOCK_HZ 24Mhz
	pwm_set_mode(PWM_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM_ID, 18, 3); // 12..20/1
	pwm_start(PWM_ID);
	gpio_set_func(PWM_PIN, AS_PWMx);
}

//extern void adc_channel_init(ADC_InputPchTypeDef p_ain);

void get_adc_rh_ntc_mv(void) {
	gpio_set_output_en(GPIO_NTC_OFF, 1);

	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_UP_DOWN_FLOAT);
	pwm_init();
	u32 t0 = clock_time();
	while (clock_time() - t0 < rh.tic);
	rh.rh = get_adc_mv(CHNL_RHI);
	pwm_stop(PWM_ID);
	gpio_set_func(PWM_PIN, AS_GPIO);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);

	rh.ntc = get_adc_mv(CHNL_NTC);
	gpio_set_output_en(GPIO_NTC_OFF, 0);
}

void calibrate_rh(void) {
	u32 t0, tt;

	rh.tic = 0;

	rh.ubase = get_battery_mv();
	rh.ubase -= 700;
	rh.ubase >>= 1;
	rh.ubase -= rh.ubase >> 2;

	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_UP_DOWN_FLOAT);
	pwm_init();
	t0 = clock_time();
	do {
		tt = clock_time() - t0;
		rh.cal_mv = get_adc_mv(CHNL_RHI);
		if(rh.cal_mv >= rh.ubase) {
			rh.tic = tt;
			break;
		}
	} while(tt < 7 * CLOCK_16M_SYS_TIMER_CLK_1MS); // 25*CLOCK_16M_SYS_TIMER_CLK_1MS
	pwm_stop(PWM_ID);
	gpio_set_func(PWM_PIN, AS_GPIO);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);
}


int	read_sensor_cb(void) {
//	get_adc_rh_mv();
//	rh.rh_mv += 700 - (t+40) * 0.0034;
//
	get_adc_rh_ntc_mv();
	measured_data.humi = rh.rh;
	measured_data.temp = rh.ntc;
	measured_data.count++;
	return 1;
}

#endif // USE_SENSOR_PWMRH
