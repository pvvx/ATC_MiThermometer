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
		.temp_hysteresis = -55, // enable, -0.55 °C
		.humi_hysteresis = 0  // disable
};

RAM trigger_t trg;

_attribute_ram_code_ void test_trg_on(void) {
	if(trg.temp_hysteresis || trg.humi_hysteresis) {
		trg.flg.trigger_on = true;
		trg.flg.trg_output = (trg.flg.humi_out_on || trg.flg.temp_out_on);
	} else {
		trg.flg.trigger_on = false;
	}
	gpio_setup_up_down_resistor(GPIO_TRG, trg.flg.trg_output ? PM_PIN_PULLUP_10K : PM_PIN_PULLDOWN_100K);
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) void set_trigger_out(void) {
	if(trg.temp_hysteresis) {
		if(trg.flg.temp_out_on) { // temp_out on
			if(trg.temp_hysteresis < 0) {
				if(measured_data.temp > trg.temp_threshold - trg.temp_hysteresis) {
					trg.flg.temp_out_on = false;
				}
			} else {
				if(measured_data.temp < trg.temp_threshold - trg.temp_hysteresis) {
					trg.flg.temp_out_on = false;
				}
			}
		} else { // temp_out off
			if(trg.temp_hysteresis < 0) {
				if(measured_data.temp < trg.temp_threshold + trg.temp_hysteresis) {
					trg.flg.temp_out_on = true;
				}
			} else {
				if(measured_data.temp > trg.temp_threshold + trg.temp_hysteresis) {
					trg.flg.temp_out_on = true;
				}
			}
		}
	} else trg.flg.temp_out_on = false;
	if(trg.humi_hysteresis) {
		if(trg.flg.humi_out_on) { // humi_out on
			if(trg.humi_hysteresis < 0) {
				if(measured_data.humi > trg.humi_threshold - trg.humi_hysteresis) {
					// humi > threshold
					trg.flg.humi_out_on = false;
				}
			} else { // hysteresis > 0
				if(measured_data.humi < trg.humi_threshold - trg.humi_hysteresis) {
					// humi < threshold
					trg.flg.humi_out_on = false;
				}
			}
		} else { // humi_out off
			if(trg.humi_hysteresis < 0) {
				if(measured_data.humi < trg.humi_threshold + trg.humi_hysteresis) {
					// humi < threshold
					trg.flg.humi_out_on = true;
				}
			} else { // hysteresis > 0
				if(measured_data.humi > trg.humi_threshold + trg.humi_hysteresis) {
					// humi > threshold
					trg.flg.humi_out_on = true;
				}
			}
		}
	} else trg.flg.humi_out_on = false;
	test_trg_on();
}

#endif	// USE_TRIGGER_OUT
