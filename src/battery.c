#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "battery.h"

u8 adc_hw_initialized = 0;
#define ADC_BUF_COUNT	8

// Process takes about 120 μs at CPU CLK 24Mhz.
_attribute_ram_code_
static void adc_channel_init(ADC_InputPchTypeDef p_ain) {
	adc_set_sample_clk(5);
	adc_set_left_right_gain_bias(GAIN_STAGE_BIAS_PER100, GAIN_STAGE_BIAS_PER100);
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);
	adc_set_state_length(240, 0, 10);
	analog_write(anareg_adc_res_m, RES14 | FLD_ADC_EN_DIFF_CHN_M);
	adc_set_ain_chn_misc(p_ain, GND);
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);
	adc_set_tsample_cycle_chn_misc(SAMPLING_CYCLES_6);
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
}

// Process takes about 260 μs at CPU CLK 24Mhz.
_attribute_ram_code_
u16 get_adc_mv(u32 p_ain) { // ADC_InputPchTypeDef
	volatile unsigned int adc_dat_buf[ADC_BUF_COUNT];
	u16 temp;
	int i, j;
	if (adc_hw_initialized != p_ain) {
		adc_hw_initialized = p_ain;
		adc_power_on_sar_adc(0);
#if 0 // gpio set in app_config.h
		if(p_ain == SHL_ADC_VBAT) {
			// Set missing pin on case TLSR8251F512ET24/TLSR8253F512ET32
			gpio_set_output_en(GPIO_VBAT, 1);
			gpio_set_input_en(GPIO_VBAT, 0);
			gpio_write(GPIO_VBAT, 1);
		}
#endif
		adc_channel_init(p_ain);
	}
	adc_power_on_sar_adc(1); // + 0.4 mA
	adc_reset_adc_module();
	u32 t0 = clock_time();

	u16 adc_sample[ADC_BUF_COUNT]; // = { 0 };
	u32 adc_average;
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		adc_dat_buf[i] = 0;
	}
	while (!clock_time_exceed(t0, 25)); //wait at least 2 sample cycle(f = 96K, T = 10.4us)
	adc_config_misc_channel_buf((u16 *) adc_dat_buf, sizeof(adc_dat_buf));
	dfifo_enable_dfifo2();
	sleep_us(20);
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		while (!adc_dat_buf[i]);
		if (adc_dat_buf[i] & BIT(13)) {
			adc_sample[i] = 0;
		} else {
			adc_sample[i] = ((u16) adc_dat_buf[i] & 0x1FFF);
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
	adc_power_on_sar_adc(0); // - 0.4 mA
	adc_average = (adc_sample[2] + adc_sample[3] + adc_sample[4]
				+ adc_sample[5]);
#if (DEV_SERVICES & SERVICE_PLM)
	if(p_ain != SHL_ADC_VBAT)
		return adc_average;
	else
		return (adc_average * 1175) >> 12; // adc_vref default: 1175 (mV)
#else
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
	return (adc_average * 1686) >> 12; // adc_vref default: 1175 (mV)
#else
	return (adc_average * 1175) >> 12; // adc_vref default: 1175 (mV)
#endif
#endif
}

// 2200..3000 mv - 0..100%
_attribute_ram_code_
u8 get_battery_level(u16 battery_mv) {
    if (battery_mv >= MAX_VBAT_MV) return 100;
    if (battery_mv <= MIN_VBAT_MV) return 0;

    // Keep the /100 in the denominator to avoid an u16 overflow
    return (battery_mv - MIN_VBAT_MV) / ((MAX_VBAT_MV - MIN_VBAT_MV) / 100);
}
