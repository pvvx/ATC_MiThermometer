/*
 * rh.c
 *
 *  Created on: 05.04.2022
 *      Author: pvvx
 */
#include "tl_common.h"
#if (DEV_SERVICES & SERVICE_PLM) &&  (USE_SENSOR_PWMRH == 1)
#include "drivers.h"
#include "battery.h"
#include "sensor.h"
#include "rh.h"
#include "app.h"
#include "flash_eep.h"

extern u64 mul32x32_shl32(u32 a, u32 b); // hard function (in div_mod.S)
// Humidity Slope factor: 3349874035, Zero offset: 13563

#define DEF_PWM_TIME_US	2000 // PWM time 2 ms, for R = 10k, С = 22 nF
#define TIM_PWM_MAX		(DEF_PWM_TIME_US*2*CLOCK_16M_SYS_TIMER_CLK_1US) // max time PWM
#define TIM_TUD_TST		(2000*CLOCK_16M_SYS_TIMER_CLK_1US) // time set "1" PWM
#define DEF_ADC_RH0		12400
#define DEF_ADC_RHK		80712
#define CONST_SHL_K		13

const sensor_coef_t rh_ntc_def = {
		.val1_k = 76500,
		.val1_z = -50,
		.val2_k = DEF_PWM_TIME_US | (DEF_ADC_RHK<<15),
		.val2_z = DEF_ADC_RH0
};

RAM sensor_cfg_t sensor_cfg;

/*  NTC1: 10k (25°С), R: 10k  */
const u16 ntc_table[] = {
//	-45		-40		-35		-30		-25		-20		-15		-10
	63320,	62603,	61703,	60592,	59239,	57627,	55730,	53555,
//	-5		0		+5		+10		+15		+20		+25		+30
	51096,	48390,	45463,	42378,	39187,	35963,	32768,	29661,
//	+35		+40		+45		+50		+55		+60		+65		+70
	26690,	23900,	21309,	18937,	16784,	14849,	13122,	11587,
//	+75		+80		+85		+90		+95		+100
	10232,	9037,	7988,	7068,	6261,	5554
};

#define NTC_TABLE_IDX_MAX ((sizeof(ntc_table) / sizeof(ntc_table[0])) - 1)

#define NTC_TABLE_T_MIN 	-4500 // x0.01C, ntc_table[0]
#define NTC_TABLE_T_MAX 	10000 // x0.01C, ntc_table[NTC_TABLE_IDX_MAX]
#define NTC_TABLE_T_STEP 	500 // x0.01C

s16 calc_temperature(u16 adc_val) {
	int i = 0;
	u16 d = ntc_table[0]; // -45C
	u16 m;
	s16 t = NTC_TABLE_T_MIN; // -50C

	if (adc_val <= ntc_table[NTC_TABLE_IDX_MAX]) {
		return NTC_TABLE_T_MAX;
	}
	if (adc_val >= ntc_table[0]) {
		return t;
	}

	while (i < NTC_TABLE_IDX_MAX) {
		++i;
		m = d; // old table value = ntc_table[i-1]
		d = ntc_table[i];
		t += NTC_TABLE_T_STEP;	// +5C
		if(adc_val >= d) // adc >= ntc_table[i]
			break;
	};
	m -= d; // ntc_table[i-1] - ntc_table[i]
	d = adc_val - d; // adc - ntc_table[i]
	if (d) {
		t -= (u32)(((u32)NTC_TABLE_T_STEP * (u32)d) + (u32)(m >> 1)) / (u32)m;
	}
	return t;
}


static void start_pwm(void) {
	reg_clk_en0 |= FLD_CLK0_PWM_EN;
	reg_pwm_clk = 0; // = CLOCK_SYS_CLOCK_HZ 24Mhz
	pwm_set_mode(PWM_ID, PWM_NORMAL_MODE);
	pwm_set_phase(PWM_ID, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(PWM_ID, 12, 3);
	pwm_start(PWM_ID);
	gpio_set_output_en(GPIO_RHI, 0);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_UP_DOWN_FLOAT);
	gpio_set_func(GPIO_RHI, AS_GPIO);
	gpio_set_func(PWM_PIN, AS_PWMx);
}

int get_adc_rh_ntc(void) {
	u32 adc_uint, adc_uz, t0, te;
	s32 adc_int;

	t0 = clock_time();

	// set pin PWM "1", ADC ADC PULLDOWN_100K
	gpio_set_func(PWM_PIN, AS_GPIO);
	gpio_write(PWM_PIN, 1); // set pin PWM "1"
	gpio_set_output_en(PWM_PIN, 1);
	gpio_setup_up_down_resistor(PWM_PIN, PM_PIN_UP_DOWN_FLOAT);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_UP_DOWN_FLOAT);

	gpio_set_output_en(GPIO_NTC_OFF, 1); // NTC power on

	measured_data.battery_mv = get_battery_mv(); // measure battery (in mV)

	adc_uint = (u32)get_adc_mv(CHNL_NTC);  // measure ntc, adc value x4

	gpio_set_output_en(GPIO_NTC_OFF, 0); // NTC power off (on..off interval ~500 us)

	// Get the ratio of voltage from NTC to battery voltage (* 1.1474609375)
	adc_uint <<= 14;
	adc_uint /= measured_data.battery_mv; // = adc / bat

#ifdef USE_AVERAGE_TH_SHL
	// Calculate the average value from (1<<USE_AVERAGE_TH_SHL) measurements
	sensor_cfg.summ_ntc += adc_uint;
	sensor_cfg.cnt_summ++;
	if(sensor_cfg.cnt_summ >= (1<<USE_AVERAGE_TH_SHL)) {
		adc_uint = sensor_cfg.summ_ntc >> USE_AVERAGE_TH_SHL;
		sensor_cfg.summ_ntc -= adc_uint;
	} else {
		adc_uint = sensor_cfg.summ_ntc / sensor_cfg.cnt_summ;
	}
#endif
	// adc_uint = 0..0xffff
	// Bring the ratio of the measurement to the table values
	adc_uint = adc_uint * sensor_cfg.coef.val1_k;
	adc_uint >>= 16;
	adc_int = (s32)adc_uint + (s32)sensor_cfg.coef.val1_z;
	// Limit value test
	if(adc_int > 0xffff)
		adc_int = 0xffff;
	else if (adc_int < 0)
		adc_int = 0;
	sensor_cfg.adc_ntc = adc_int; // save final NTC ADC NTC measurement

	while (clock_time() - t0 < TIM_TUD_TST);

	adc_uz = (u32)get_adc_mv(CHNL_RHI); // measure (+Vbat - Udiode)

	// discharge cap RHI
	gpio_write(PWM_PIN, 0);
	gpio_set_output_en(GPIO_RHI, 1);

	sleep_us(64); // Waiting discharge cap RHI

	start_pwm(); // Start PWM

	// Waiting for the end of the sensor capacitor charging interval
	t0 = clock_time();

	measured_data.temp = calc_temperature(adc_int);

	if(sensor_cfg.id == 1) {
		// Calibrate 0% (PWM time)
		adc_uz = (adc_uz >> 1) + (adc_uz >> 4); // * 0.75
		adc_uint = 0;
		te = 0;
		while (adc_uint < adc_uz && te < TIM_PWM_MAX) {  // measure rh, adc value x4
			te = clock_time() - t0;
			adc_uint = get_adc_mv(CHNL_RHI);
		}
		te >>= 4; // div CLOCK_16M_SYS_TIMER_CLK_1US
		pwm_stop(PWM_ID); // PMM Off
		gpio_set_output_en(PWM_PIN, 0);
		gpio_set_func(PWM_PIN, AS_GPIO);
		gpio_setup_up_down_resistor(PWM_PIN, PM_PIN_PULLDOWN_100K);
		gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);
		te |= sensor_cfg.coef.val2_k & (~((1 << CONST_SHL_K) - 1)); // ~0x01fff = 0xffffe000
		sensor_cfg.coef.val2_k = te;
		sensor_cfg.id = 0;
		flash_write_cfg(&sensor_cfg.coef, EEP_ID_CFS, sizeof(sensor_cfg.coef));
		return 0;
	}
	te = sensor_cfg.coef.val2_k & ((1 << CONST_SHL_K) - 1); // 0x01fff
	te <<= 4; // * CLOCK_16M_SYS_TIMER_CLK_1US
	// Waiting for the end of the sensor capacitor charging interval
	while (clock_time() - t0 < te);

	adc_uint = (u32)get_adc_mv(CHNL_RHI); // measure rh, adc value x4

	gpio_set_output_en(PWM_PIN, 0);
	pwm_stop(PWM_ID); // PMM Off
	gpio_set_func(PWM_PIN, AS_GPIO);
	gpio_setup_up_down_resistor(PWM_PIN, PM_PIN_PULLDOWN_100K);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);

	// компенсация диода
	if(measured_data.battery_mv < 3300)
		adc_uint *= 0x10000 + (3300 - measured_data.battery_mv)*10; // 0.07*65536/1000 = 4.58752
	else
		adc_uint <<= 16; // без компенсации

	adc_uint /= adc_uz;
	sensor_cfg.adc_rh = adc_uint;
	if(sensor_cfg.id == 2) { // Calibrate to 100%
		// Calibrate 100% (ADC zero)
		sensor_cfg.coef.val2_z = (u16)adc_uint;
		sensor_cfg.id = 0;
		flash_write_cfg(&sensor_cfg.coef, EEP_ID_CFS, sizeof(sensor_cfg.coef));
		return 0;
	}

	adc_uz = (u32)((u16)sensor_cfg.coef.val2_z);

	if(sensor_cfg.id == 3) {
		// Calibrate K
		if(adc_uint > adc_uz) {
			adc_uint -= adc_uz;
			adc_uint = (35020 << 16) / adc_uint;
		} else {
			adc_uint = 0x4000;
		}
		adc_uint <<= CONST_SHL_K;
		adc_uint |= sensor_cfg.coef.val2_k & ((1 << CONST_SHL_K) - 1); // 0x01fff
		sensor_cfg.coef.val2_k = adc_uint;
		sensor_cfg.id = 0;
		flash_write_cfg(&sensor_cfg.coef, EEP_ID_CFS, sizeof(sensor_cfg.coef));
		return 0;
	}
	// Normal operation
	if(adc_uint <= adc_uz) { // limit value test
		adc_uint = 10000; // 100.00%
	} else {
		adc_uint -= adc_uz;
		// Bring the ratio of the measurement to the 35020,30738793875
		adc_uint *= sensor_cfg.coef.val2_k >> CONST_SHL_K; // to 35020
		adc_uint >>= 16;
		if(adc_uint >= 35020) { // limit value test
			adc_uint = 0;
		} else {
			adc_uint = 35020 - adc_uint;
			// Raise the value to the power of 3 (adc_uint^3, (35020*35020*35020)>>32 = 9999)
			adc_uint = mul32x32_shl32(adc_uint * adc_uint, adc_uint); // = (adc_uint*adc_uint)>>32
		}
	}
#ifdef USE_AVERAGE_TH_SHL
	// Calculate the average value from (1<<USE_AVERAGE_TH_SHL) measurements
	sensor_cfg.summ_rh += adc_uint;
	if(sensor_cfg.cnt_summ >= (1<<USE_AVERAGE_TH_SHL)) {
		adc_uint = sensor_cfg.summ_rh >> USE_AVERAGE_TH_SHL;
		sensor_cfg.summ_rh -= adc_uint;
		sensor_cfg.cnt_summ--;
	} else {
		adc_uint = sensor_cfg.summ_rh / sensor_cfg.cnt_summ;
	}
#endif
	measured_data.humi = adc_uint;
	return 0;
}

#ifdef USE_AVERAGE_TH_SHL
void clr_rh_summ(void) {
	sensor_cfg.summ_ntc = 0;
	sensor_cfg.summ_rh = 0;
	sensor_cfg.cnt_summ = 0;
}
#else
#define clr_rh_summ()
#endif

int calibrate_rh_t(void) {
	clr_rh_summ();
	sensor_cfg.id = 1;
	return 0;
}

int calibrate_rh_100(void) {
	clr_rh_summ();
	sensor_cfg.id = 2;
	return 0;
}

int calibrate_rh_0(void) {
	clr_rh_summ();
	sensor_cfg.id = 3;
	return 0;
}

void init_sensor(void) {
	clr_rh_summ();
	if(sensor_cfg.coef.val1_k == 0) {
		memcpy(&sensor_cfg.coef, &rh_ntc_def, sizeof(sensor_cfg.coef));
	}
	sensor_cfg.sensor_type = ID_SENSOR_PWMRH;
}

int	read_sensor_cb(void) {
	get_adc_rh_ntc();
	measured_data.count++;
	return 1;
}

#endif // USE_SENSOR_PWMRH
