#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "battery.h"

uint8_t adc_hw_initialized = 0;
#define ADC_BUF_COUNT	8
volatile unsigned int adc_dat_buf[ADC_BUF_COUNT];

_attribute_ram_code_ static void adc_channel_init(ADC_InputPchTypeDef p_ain) {
	adc_set_sample_clk(5);
	adc_set_left_right_gain_bias(GAIN_STAGE_BIAS_PER100, GAIN_STAGE_BIAS_PER100);
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);
	adc_set_state_length(240, 0, 10);
	analog_write(anareg_adc_res_m, RES14 | FLD_ADC_EN_DIFF_CHN_M);
	adc_set_ain_chn_misc(p_ain, GND);
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);
	adc_set_tsample_cycle_chn_misc(SAMPLING_CYCLES_6);
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
	//	adc_power_on_sar_adc(1);
}

// Process takes about 83 μs.
_attribute_ram_code_ uint16_t get_adc_mv(ADC_InputPchTypeDef p_ain) {
	uint16_t temp;
	int i, j;
	if (adc_hw_initialized != p_ain) {
		adc_hw_initialized = p_ain;
		adc_power_on_sar_adc(0);
		if(p_ain == B5P) {
			gpio_set_output_en(GPIO_PB5, 1);
			gpio_set_input_en(GPIO_PB5, 0);
			gpio_write(GPIO_PB5, 1);
		}
		adc_channel_init(p_ain);
	}
	adc_power_on_sar_adc(1); // + 0.4 mA
	adc_reset_adc_module();
	u32 t0 = clock_time();

	uint16_t adc_sample[8] = { 0 };
	u32 adc_average;
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		adc_dat_buf[i] = 0;
	}
	while (!clock_time_exceed(t0, 25))
		; //wait at least 2 sample cycle(f = 96K, T = 10.4us)
	adc_config_misc_channel_buf((uint16_t *) adc_dat_buf, sizeof(adc_dat_buf));
	dfifo_enable_dfifo2();
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		while (!adc_dat_buf[i])
			;
		if (adc_dat_buf[i] & BIT(13)) {
			adc_sample[i] = 0;
		} else {
			adc_sample[i] = ((uint16_t) adc_dat_buf[i] & 0x1FFF);
		}
		if (i) {
			if (adc_sample[i] < adc_sample[i - 1]) {
				temp = adc_sample[i];
				adc_sample[i] = adc_sample[i - 1];
				for (j = i - 1; j >= 0 && adc_sample[j] > temp; j--) {
					adc_sample[j + 1] = adc_sample[j];
				}
				adc_sample[j + 1] = temp;
			}
		}
	}
	dfifo_disable_dfifo2();
	//adc_set_ain_chn_misc(NOINPUTP, GND);
	adc_power_on_sar_adc(0); // - 0.4 mA
	adc_average = (adc_sample[2] + adc_sample[3] + adc_sample[4]
			+ adc_sample[5]) / 4;
	return (adc_average * adc_vref_cfg.adc_vref) >> 10; // adc_vref default: 1175 (mV)
}

// 2200..3100 mv - 0..100%
_attribute_ram_code_ uint8_t get_battery_level(uint16_t battery_mv) {
	uint8_t battery_level = 0;
	if (battery_mv > MIN_VBAT_MV) {
		battery_level = (battery_mv - MIN_VBAT_MV) / ((MAX_VBAT_MV
				- MIN_VBAT_MV) / 100);
		if (battery_level > 100)
			battery_level = 100;
	}
	return battery_level;
}
