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

#define DEF_PWM_TIK		54000 // DEF_PWM_TIK/CLOCK_16M_SYS_TIMER_CLK_1US = 4000 us, for С = 47 nF
#define DEF_U_POWER		3100 // 3300 mV
#define U_DIODE			1350 // sensor_cfg.coef.val1_z //  Diode Voltage drop at 8 MHz (mV). Using a diode marked "T4"
#define DEF_ADC_RH0		1180 // ADC value at 100% and supply voltage 3.300V
#define VAL_RH0			3475 // The cube root of 41943040000 is: 3474.454549241, (3475*3475*3475)>>22 = 10004

#define TIM_PWM_MAX		(7 * CLOCK_16M_SYS_TIMER_CLK_1MS) // min time PWM
#define TIM_PWM_MIN		(2 * CLOCK_16M_SYS_TIMER_CLK_1MS) // max time PWM
#define ADC_RH0_MIN		400  // min ADC value at 100% and supply voltage 3.100V
#define ADC_RH0_MAX		1200 // max ADC value at 100% and supply voltage 3.100V


const sensor_coef_t rh_ntc_def = {
		.val1_k = 75000,
		.val1_z = 80,
		.val2_k = DEF_PWM_TIK,
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
	pwm_set_cycle_and_duty(PWM_ID, 18, 3); // 12..20/1
	pwm_start(PWM_ID);
	gpio_set_output_en(GPIO_RHI, 0);
	gpio_set_func(GPIO_RHI, AS_GPIO);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_UP_DOWN_FLOAT);
	gpio_set_func(PWM_PIN, AS_PWMx);
}


static void stop_pwm(void) {
	pwm_stop(PWM_ID);
	gpio_set_func(PWM_PIN, AS_GPIO);
	gpio_set_output_en(PWM_PIN, 1);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);
}


u32 get_adc_rh_ntc(void) {
	u32 k, t0;
	u32 adc_uint;
	s32 adc_int;
	gpio_set_output_en(GPIO_NTC_OFF, 1); // NTC power on

	start_pwm();

	t0 = clock_time(); // save start time

	measured_data.battery_mv = get_battery_mv(); // measure battery (in mV)

	adc_uint = get_adc_mv(CHNL_NTC);  // measure ntc, adc value x4

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

	// Waiting for the end of the sensor capacitor charging interval
	while (clock_time() - t0 < sensor_cfg.coef.val2_k);

	adc_uint = get_adc_mv(CHNL_RHI); // measure rh, adc value x4

	// measure battery (in mV)
	k = measured_data.battery_mv + get_battery_mv(); // start + end battery_mv = 2 x Ubat

	stop_pwm(); // PMM Off

	// coefficient for ADC RH to the current supply voltage
	k = ((DEF_U_POWER - U_DIODE) << 17) / (k - (U_DIODE*2));

	adc_uint = (adc_uint * k) >> 16;

	sensor_cfg.adc_rh = adc_uint; // save final RH ADC measurement

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
	// maximum value to calculate 0% RH
	k = VAL_RH0 + sensor_cfg.coef.val2_z; // max ADC value  (= 0%)

	// Limit value test
	if(adc_uint <= sensor_cfg.coef.val2_z)
		adc_uint = 10000; // 100.00%
	else if(adc_uint >= k)
		adc_uint = 0; // 0%
	else {
		// Get the reciprocal value
		adc_uint = k - adc_uint;
		// Raise the value to the power of 3 (adc_int^3, (3475*3475*3475)>>22 = 10004)
		adc_int = (((adc_uint * adc_uint) >> 11) * adc_uint) >> 11;
		// Limit value test
		if(adc_uint < 0)
			adc_uint = 0; // 0%
		else if(adc_uint > 10000)
			adc_uint = 10000; // 100.00%
	}
	return adc_uint;
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

static void	discharge_c_rh(void) {
	gpio_set_func(PWM_PIN, AS_GPIO);
	gpio_setup_up_down_resistor(GPIO_RHI, PM_PIN_PULLDOWN_100K);
	gpio_set_output_en(GPIO_RHI, 1);
	measured_data.battery_mv = get_battery_mv();
}

int calibrate_rh_0(void) {
	discharge_c_rh();
	get_adc_rh_ntc();
	if(sensor_cfg.adc_rh >= ADC_RH0_MIN && sensor_cfg.adc_rh <= ADC_RH0_MAX) {
		sensor_cfg.coef.val2_z = sensor_cfg.adc_rh;
		flash_write_cfg(&sensor_cfg.coef, EEP_ID_CFS, sizeof(sensor_cfg.coef));
		clr_rh_summ();
		return 0;
	}
	return 1;
}

int calibrate_rh_100(void) {
	u32 t0, tt = 0;
	u32 tst_val;

	discharge_c_rh();

	sensor_cfg.coef.val2_k = DEF_PWM_TIK;
	if(sensor_cfg.coef.val2_z > ADC_RH0_MAX || sensor_cfg.coef.val2_z < ADC_RH0_MIN)
		sensor_cfg.coef.val2_z = DEF_ADC_RH0;

	start_pwm();
	t0 = clock_time();

	tst_val = measured_data.battery_mv + get_battery_mv();
	tst_val >>= 1;
	tst_val = (VAL_RH0 + sensor_cfg.coef.val2_z)*(tst_val  - U_DIODE);
	tst_val /= (DEF_U_POWER - U_DIODE);
	sensor_cfg.id = tst_val; // debug
	do {
		tt = clock_time() - t0;
		if(get_adc_mv(CHNL_RHI) >= tst_val) {
			break;
		}
	} while(tt < TIM_PWM_MAX);

	stop_pwm();

	if(tt > TIM_PWM_MIN && tt < TIM_PWM_MAX) {
		sensor_cfg.coef.val2_k = tt;
		flash_write_cfg(&sensor_cfg.coef, EEP_ID_CFS, sizeof(sensor_cfg.coef));
		clr_rh_summ();
		return 0;
	}
	return 1;
}

void init_sensor(void) {
	clr_rh_summ();
	if(sensor_cfg.coef.val1_k == 0) {
		memcpy(&sensor_cfg.coef, &rh_ntc_def, sizeof(sensor_cfg.coef));
	}
	sensor_cfg.sensor_type = IU_SENSOR_PWMRH;
}

int	read_sensor_cb(void) {
	measured_data.humi = get_adc_rh_ntc();
	measured_data.temp = calc_temperature(sensor_cfg.adc_ntc);
	measured_data.count++;
	return 1;
}

#endif // USE_SENSOR_PWMRH
