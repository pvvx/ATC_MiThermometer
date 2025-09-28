/*
 * rh.c
 *
 *  Created on: 05.04.2022
 *      Author: pvvx
 */
#include "tl_common.h"
#if (DEV_SERVICES & SERVICE_PLM) && (USE_SENSOR_PWMRH == 2)
#include "drivers.h"
#include "battery.h"
#include "sensor.h"
#include "rh.h"
#include "app.h"
#include "flash_eep.h"

#define DEF_ADC_DIODE	8810 // ADC value GPIO PWM = "1", Ubat: 2995 mV
#define DEF_ADC_RH_Z	820  // ADC value at 100%
#define DEF_RH_K		3100000 // (ADC value at 0% - DEF_ADC_HZ) * 1000

#define PWM_CYCLE		18   // F=24000/PWM_CYCLE, 12->2MHz, 18->1.33MHz 48->500 кГц
#define PWM_PULSE		3	 // 8 MHz

const sensor_rh_coef_t rh_coef_def = {
		.k = DEF_RH_K,
		.z = DEF_ADC_RH_Z,
		.d = DEF_ADC_DIODE
};

RAM sensor_rh_t sensor_rh;

static void start_pwm(void) {
	reg_clk_en0 |= FLD_CLK0_PWM_EN;
	reg_pwm_clk = 0; // = CLOCK_SYS_CLOCK_HZ 24Mhz
	pwm_set_mode(PWM_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM_ID, PWM_CYCLE, PWM_PULSE); // 12..20/3
	pwm_start(PWM_ID);
	gpio_set_output_en(GPIO_RHI, 0);
	gpio_set_func(GPIO_RHI, AS_GPIO);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_UP_DOWN_FLOAT);
	gpio_set_func(PWM_PIN, AS_PWMx);
}


static void	discharge_c_rh(void) {
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);
	gpio_write(PWM_PIN, 0);
	gpio_set_func(PWM_PIN, AS_GPIO);
	gpio_set_output_en(GPIO_RHI, 1);
}


static u32 get_rh(void) {
	u32 k, z, val;
	// discharge cap RHI
	discharge_c_rh();

	sleep_us(64);

	start_pwm();

	measured_data.battery_mv = get_battery_mv(); // measure battery (in mV)
	sensor_rh.adc_rh = get_adc_mv(CHNL_RHI); // measure rh, adc value x4

	pwm_stop(PWM_ID); // PMM Off

	// set pin PWM "1"
	gpio_set_func(PWM_PIN, AS_GPIO);
	gpio_write(PWM_PIN, 1);
	gpio_set_output_en(PWM_PIN, 1);

	sleep_us(128);
	// get ADC value RHI (+Vbat - Udiode)
	sensor_rh.adc_d = get_adc_mv(CHNL_RHI); // measure rh, adc value x4

	// set PWM OUT "0"
	gpio_write(PWM_PIN, 0);
	discharge_c_rh();

	k = sensor_rh.coef.d << 16;
	k /= sensor_rh.adc_d;
	z = sensor_rh.coef.z << 16;
	z /= k;
	val = sensor_rh.adc_rh * k;
	val >>= 16;
	val -= z;
	val = sensor_rh.coef.k / val;
	if(val >= 11000)
		return 10000; // 100.00%
	if(val <= 1000)
		return 0; // 0.00%
	return val - 1000;
}


#ifdef USE_AVERAGE_TH_SHL
void clr_rh_summ(void) {
	sensor_rh.summ_rh = 0;
	sensor_rh.cnt_summ = 0;
}
#else
#define clr_rh_summ()
#endif

int calibrate_rh_0(void) {
	get_rh(); // save final RH ADC measurement
	return 1;
}

int calibrate_rh_100(void) {
	get_rh(); // save final RH ADC measurement
	return 1;
}

void init_rh_sensor(void) {
	clr_rh_summ();
	if(sensor_rh.coef.k == 0) {
		memcpy(&sensor_rh.coef, &rh_coef_def, sizeof(sensor_rh.coef));
	}
}

int	read_rh_sensor(void) {
	u32 adc_uint = get_rh();
#ifdef USE_AVERAGE_TH_SHL
	// Calculate the average value from (1<<USE_AVERAGE_TH_SHL) measurements
	sensor_rh.summ_rh += adc_uint;
	if(++sensor_rh.cnt_summ >= (1<<USE_AVERAGE_TH_SHL)) {
		adc_uint = sensor_rh.summ_rh >> USE_AVERAGE_TH_SHL;
		sensor_rh.summ_rh -= adc_uint;
		sensor_rh.cnt_summ--;
	} else {
		adc_uint = sensor_rh.summ_rh / sensor_rh.cnt_summ;
	}
#endif
	measured_data.mois = (u16)adc_uint;
	return 1;
}

#endif // USE_SENSOR_PWMRH
