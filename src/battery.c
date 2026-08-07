#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "battery.h"

#ifndef ADC_BAT_VREF_MV
#define ADC_BAT_VREF_MV		1175 // default ADC ref voltage (unit:mV)
#endif

u8 adc_hw_initialized = 0;
u32 adc_average;

#define ADC_BUF_COUNT	8

// Process takes about 120 μs at CPU CLK 24Mhz.
_attribute_ram_code_
static void adc_channel_init(ADC_InputPchTypeDef p_ain) {
	adc_power_on_sar_adc(0);
	//adc_reset_adc_module(); // reset whole digital adc module
	//dfifo_disable_dfifo2(); // disable misc channel data dfifo
	adc_set_sample_clk(5);
	//adc_enable_clk_24m_to_sar_adc(1); // enable signal of 24M clock to sar adc
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
	u16 adc_sample[ADC_BUF_COUNT]; // = { 0 };
	u16 temp;
	u16 rp = 0;
	int i, j;
	if (adc_hw_initialized != p_ain) {
		adc_hw_initialized = p_ain;
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
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		adc_dat_buf[i] = 0;
	}
	adc_config_misc_channel_buf((u16 *) adc_dat_buf, sizeof(adc_dat_buf));
	dfifo_enable_dfifo2();
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		while(rp == reg_dfifo2_wptr);
		rp = reg_dfifo2_wptr; // 0,4,8,c,10,14,18,1c
		if (adc_dat_buf[i] & BIT(13)) {
			/* 14 bit resolution, BIT(13) is sign bit,
			 * 1 means negative voltage in differential_mode  */
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
	if(p_ain != SHL_ADC_VBAT) {
		return adc_average;
	} else
#endif
	return (adc_average * ADC_BAT_VREF_MV) >> 12; // adc_vref default: 1175 (mV)
}

#if !USE_NI_ZN_BATTERY
// Remaining battery capacity vs. voltage: a piecewise-linear fit of the
// Energizer CR2450 datasheet typical discharge curve (7.5 kΩ continuous
// load, 21°C): https://data.energizer.com/pdfs/cr2450.pdf
// The curve's normalized shape is set by the Li/MnO₂ chemistry, not the cell
// size, so the same table serves CR2032/CR2477 devices too.
//
// Entries are remaining charge in 0.1% units at 50 mV steps, from
// MIN_VBAT_MV (0%) to MAX_VBAT_MV (100%). Entries must be non-decreasing;
// the interpolation below assumes it. If MIN_VBAT_MV or MAX_VBAT_MV change,
// re-sample the datasheet curve at each step to rebuild the table.
#define VBAT2LEVEL_STEP_MV	50
static const u16 vbat2level_lut[] = {
	0, 6, 13, 22, 34, 49, 67, 91, 124, 160, 202, 257, 340, 465, 634, 821, 1000
};
STATIC_ASSERT_INT_DIV(MAX_VBAT_MV - MIN_VBAT_MV, VBAT2LEVEL_STEP_MV);
STATIC_ASSERT(sizeof(vbat2level_lut) / sizeof(vbat2level_lut[0])
	== (MAX_VBAT_MV - MIN_VBAT_MV) / VBAT2LEVEL_STEP_MV + 1);
#endif // !USE_NI_ZN_BATTERY

// MIN_VBAT_MV..MAX_VBAT_MV -> 0..1000 (0.1% units)
_attribute_ram_code_
u16 get_battery_level_x10(u16 battery_mv) {
	u16 battery_level_x10 = 1000;
	if (battery_mv < MAX_VBAT_MV) {
		if (battery_mv > MIN_VBAT_MV) {
#if USE_NI_ZN_BATTERY
			// no discharge-curve data for Ni-Zn: linear map, rounded to nearest
			battery_level_x10 = (u16)((((u32)(battery_mv - MIN_VBAT_MV) * 1000)
				+ (MAX_VBAT_MV - MIN_VBAT_MV) / 2) / (MAX_VBAT_MV - MIN_VBAT_MV));
#else
			u16 d = battery_mv - MIN_VBAT_MV;
			u16 i = d / VBAT2LEVEL_STEP_MV;
			u16 f = d - i * VBAT2LEVEL_STEP_MV; // d % step without a second divide
			u16 lo = vbat2level_lut[i];
			u16 hi = vbat2level_lut[i + 1];
			battery_level_x10 = lo + (u16)(((u32)(hi - lo) * f
				+ VBAT2LEVEL_STEP_MV / 2) / VBAT2LEVEL_STEP_MV);
#endif
		} else {
			battery_level_x10 = 0;
		}
	}
	return battery_level_x10;
}

// MIN_VBAT_MV..MAX_VBAT_MV -> 0..100%
_attribute_ram_code_
u8 get_battery_level(u16 battery_mv) {
	return (u8)((get_battery_level_x10(battery_mv) + 5) / 10);
}
