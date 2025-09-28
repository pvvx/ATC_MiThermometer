/*
 * trigger.c
 *
 *  Created on: 02.01.2021
 *      Author: pvvx
 */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "app.h"
#include "drivers.h"
#include "sensor.h"
#include "trigger.h"
#include "rds_count.h"

#if (DEV_SERVICES & SERVICE_TH_TRG) || (DEV_SERVICES & SERVICE_RDS)

const trigger_t def_trg = {
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_18B20))
#if USE_SENSOR_INA3221
#ifdef	GPIO_TRG2
		.temp_threshold = 20800, // 20.80V
		.humi_threshold = 20850, // 20.80V
		.temp_hysteresis = -100, // enable, 20.70/20.90V
		.humi_hysteresis = -50,  // enable, 20.8/20.90V
#else
		.temp_threshold = 20800, // 20.80V
		.humi_threshold = 1000, // 1A
		.temp_hysteresis = -100, // enable, 20.70/20.90V
		.humi_hysteresis = 0,  // disable
#endif

#else
		.temp_threshold = 2100, // 21 °C
		.humi_threshold = 5000, // 50 %
		.temp_hysteresis = -55, // enable, -0.55 °C
		.humi_hysteresis = 0,  // disable
#endif
#endif
#if (DEV_SERVICES & SERVICE_RDS)
		.rds_time_report = 3600, // 1 hours
#if (DEV_SERVICES & SERVICE_KEY)
		.rds.type1 = RDS_SWITCH,
#else
		.rds.type1 = RDS_CONNECT,
#endif
		.rds.type2 = RDS_NONE
#endif
};

RAM trigger_t trg;


#if (DEV_SERVICES & SERVICE_TH_TRG)
_attribute_ram_code_
void test_trg_on(void) {
	if (trg.temp_hysteresis || trg.humi_hysteresis) {
		trg.flg.trigger_on = true;
		trg.flg.trg_output = (trg.flg.humi_out_on || trg.flg.temp_out_on);
	} else {
		trg.flg.trigger_on = false;
	}
#ifdef	GPIO_TRG2
	gpio_setup_up_down_resistor(GPIO_TRG, trg.flg.temp_out_on ? PM_PIN_PULLUP_10K : PM_PIN_PULLDOWN_100K);
	gpio_setup_up_down_resistor(GPIO_TRG2, trg.flg.humi_out_on ? PM_PIN_PULLUP_10K : PM_PIN_PULLDOWN_100K);
#else
	gpio_setup_up_down_resistor(GPIO_TRG, trg.flg.trg_output ? PM_PIN_PULLUP_10K : PM_PIN_PULLDOWN_100K);
#endif
}
#endif

#if (DEV_SERVICES & SERVICE_THS)
#if USE_SENSOR_HT && USE_SENSOR_MY18B20
#define measured_val1	measured_data.temp
#define measured_val2	measured_data.xtemp[0]
#else
#define measured_val1	measured_data.temp
#if (DEV_SERVICES & SERVICE_PLM) && (USE_SENSOR_PWMRH == 2)
#define measured_val2	measured_data.mois
#else
#define measured_val2	measured_data.humi
#endif
#endif
#elif (DEV_SERVICES & SERVICE_18B20)
#if USE_SENSOR_HT && USE_SENSOR_MY18B20 == 1 
#define measured_val1	measured_data.temp
#define measured_val2	measured_data.xtemp[0]
#else
#define measured_val1	measured_data.xtemp[0]
#define measured_val2	measured_data.xtemp[1]
#endif
#elif (DEV_SERVICES & SERVICE_IUS)
#if USE_SENSOR_INA3221
#ifdef	GPIO_TRG2
#define measured_val1	measured_data.voltage[1]
#define measured_val2	measured_data.voltage[1]
#else
#define measured_val2	measured_data.voltage[1]
#define measured_val1	measured_data.current[1]
#endif
#else
#define measured_val1	measured_data.current
#define measured_val2	measured_data.voltage
#endif // USE_SENSOR_INA3221
#elif (DEV_SERVICES & SERVICE_PLM)
#define measured_val1	measured_data.temp
#define measured_val2	measured_data.humi
#endif

_attribute_ram_code_
__attribute__((optimize("-Os")))
void set_trigger_out(void) {
	if (trg.temp_hysteresis) {
		if (trg.flg.temp_out_on) { // temp_out on
			if (trg.temp_hysteresis < 0) {
				if (measured_val1 > trg.temp_threshold - trg.temp_hysteresis) {
					trg.flg.temp_out_on = false;
				}
			} else {
				if (measured_val1 < trg.temp_threshold - trg.temp_hysteresis) {
					trg.flg.temp_out_on = false;
				}
			}
		} else { // temp_out off
			if (trg.temp_hysteresis < 0) {
				if (measured_val1 < trg.temp_threshold + trg.temp_hysteresis) {
					trg.flg.temp_out_on = true;
				}
			} else {
				if (measured_val1 > trg.temp_threshold + trg.temp_hysteresis) {
					trg.flg.temp_out_on = true;
				}
			}
		}
	} else trg.flg.temp_out_on = false;
	if (trg.humi_hysteresis) {
		if (trg.flg.humi_out_on) { // humi_out on
			if (trg.humi_hysteresis < 0) {
				if (measured_val2 > trg.humi_threshold - trg.humi_hysteresis) {
					// humi > threshold
					trg.flg.humi_out_on = false;
				}
			} else { // hysteresis > 0
				if (measured_val2 < trg.humi_threshold - trg.humi_hysteresis) {
					// humi < threshold
					trg.flg.humi_out_on = false;
				}
			}
		} else { // humi_out off
			if (trg.humi_hysteresis < 0) {
				if (measured_val2 < trg.humi_threshold + trg.humi_hysteresis) {
					// humi < threshold
					trg.flg.humi_out_on = true;
				}
			} else { // hysteresis > 0
				if (measured_val2 > trg.humi_threshold + trg.humi_hysteresis) {
					// humi > threshold
					trg.flg.humi_out_on = true;
				}
			}
		}
	} else trg.flg.humi_out_on = false;

	test_trg_on();
}

#endif  // (DEV_SERVICES & SERVICE_TH_TRG) || (DEV_SERVICES & SERVICE_RDS)
