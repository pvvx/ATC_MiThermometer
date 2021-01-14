/*
 * trigger.c
 *
 *  Created on: 02.01.2021
 *      Author: pvvx
 */
#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "app.h"
#if	USE_TRIGGER_OUT
#include "drivers.h"
#include "sensor.h"
#include "trigger.h"

const trigger_t def_trg = {
		.temp_threshold = 2100, // 21 °C
		.humi_threshold = 5000, // 50 %
		.temp_hysteresis = -5, // enable, low -> on , +-0.5 °C
		.humi_hysteresis = 0  // disable
};

RAM trigger_t trg;

_attribute_ram_code_ void test_trg_on(void) {
	if(trg.temp_hysteresis || trg.humi_hysteresis)
		trg.flg.trigger_on = true;
	else {
		trg.flg.trigger_on = false;
		if(trg.flg.trg_output)
			gpio_setup_up_down_resistor(GPIO_TRG, PM_PIN_PULLUP_10K);
		else
			gpio_setup_up_down_resistor(GPIO_TRG, PM_PIN_PULLDOWN_100K);
	}
}

_attribute_ram_code_ void set_trigger_out(void) {
	if(trg.temp_hysteresis) {
		if(trg.flg.temp_out_on) { // temp_out on
			if(trg.temp_hysteresis < 0) {
				if(measured_data.temp > trg.temp_threshold - trg.temp_hysteresis*10) {
					trg.flg.temp_out_on = false;
				}
			} else {
				if(measured_data.temp < trg.temp_threshold + trg.temp_hysteresis*10) {
					trg.flg.temp_out_on = false;
				}
			}
		} else { // temp_out off
			if(trg.temp_hysteresis < 0) {
				if(measured_data.temp < trg.temp_threshold + trg.temp_hysteresis*10) {
					trg.flg.temp_out_on = true;
				}
			} else {
				if(measured_data.temp > trg.temp_threshold - trg.temp_hysteresis*10) {
					trg.flg.temp_out_on = true;
				}
			}
		}
	} else trg.flg.temp_out_on = false;
	if(trg.humi_hysteresis) {
		if(trg.flg.humi_out_on) { // humi_out on
			if(trg.humi_hysteresis < 0) {
				if(measured_data.temp > trg.humi_threshold - trg.humi_hysteresis*10) {
					trg.flg.humi_out_on = false;
				}
			} else {
				if(measured_data.temp < trg.humi_threshold + trg.humi_hysteresis*10) {
					trg.flg.humi_out_on = false;
				}
			}
		} else { // humi_out off
			if(trg.humi_hysteresis < 0) {
				if(measured_data.temp < trg.humi_threshold + trg.humi_hysteresis*10) {
					trg.flg.humi_out_on = true;
				}
			} else {
				if(measured_data.temp > trg.humi_threshold - trg.humi_hysteresis*10) {
					trg.flg.humi_out_on = true;
				}
			}
		}
	} else trg.flg.humi_out_on = false;

	if(trg.flg.temp_out_on || trg.flg.humi_out_on) {
		gpio_setup_up_down_resistor(GPIO_TRG, PM_PIN_PULLUP_10K);
	} else {
		gpio_setup_up_down_resistor(GPIO_TRG, PM_PIN_PULLDOWN_100K);
	}
}

#endif	// USE_TRIGGER_OUT
