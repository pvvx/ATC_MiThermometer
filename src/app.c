#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "cmd_parser.h"
#include "flash_eep.h"
#include "battery.h"
#include "ble.h"
#include "lcd.h"
#include "sensor.h"
#include "app.h"
#include "i2c.h"
#if	USE_TRIGGER_OUT
#include "trigger.h"
#endif
void app_enter_ota_mode(void);

RAM uint32_t vtime_count_us; // count validity time, in us
RAM uint32_t vtime_count_sec; // count validity time, in sec
RAM uint8_t show_stage; // count/stage update lcd code buffer

RAM measured_data_t measured_data;
RAM int16_t last_temp; // x0.1 C
RAM uint16_t last_humi; // x1 %
RAM uint8_t battery_level; // 0..100%

RAM volatile uint8_t tx_measures;

RAM volatile uint8_t start_measure; // start measure all
RAM volatile uint8_t wrk_measure;
RAM volatile uint8_t end_measure;
RAM uint32_t tim_last_chow; // timer show lcd >= 1.5 sec
RAM uint32_t tim_measure; // timer measurements >= 10 sec

RAM uint32_t adv_interval; // adv interval in 0.625 ms // cfg.advertising_interval * 100
RAM uint32_t connection_timeout; // connection timeout in 10 ms, Tdefault = connection_latency_ms * 4 = 2000 * 4 = 8000 ms
RAM uint32_t measurement_step_time; // adv_interval * measure_interval
RAM uint32_t min_step_time_update_lcd; // = cfg.min_step_time_update_lcd * 0.05 sec

void lcd(void);
RAM int16_t comfort_x[] = { 2000, 2560, 2700, 2500, 2050, 1700, 1600, 1750 };
RAM uint16_t comfort_y[] = { 2000, 1980, 3200, 6000, 8200, 8600, 7700, 3800 };

// Settings
const cfg_t def_cfg = {
		.flg.temp_F_or_C = false,
		.flg.blinking_smiley = false,
		.flg.comfort_smiley = false,
		.flg.show_batt_enabled = false,
		.flg.advertising_type = 3,
		.flg.tx_measures = false,
		.smiley = 0, // 0 = "     " off
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.measure_interval = 4, // * advertising_interval = 10 sec
		.rf_tx_power = RF_POWER_P3p01dBm,
		.connect_latency = 124, // (124+1)*1.25*16 = 2500 ms
		.min_step_time_update_lcd = 49 //x0.05 sec,   2.45 sec
		};
RAM cfg_t cfg;
static const external_data_t def_ext = {
		.big_number = 0,
		.small_number = 0,
		.vtime_sec = 60 * 10, // 10 minutes
		.flg.smiley = 7, // 7 = "(ooo)"
		.flg.percent_on = true,
		.flg.battery = false,
		.flg.temp_symbol = 5 // 5 = "°C", 3 = "°F", ... app.h
		};
RAM external_data_t ext;

void test_config(void) {
	cfg.rf_tx_power |= BIT(7);
	if (cfg.rf_tx_power < RF_POWER_N25p18dBm)
		cfg.rf_tx_power = RF_POWER_N25p18dBm;
	else if (cfg.rf_tx_power > RF_POWER_P3p01dBm)
		cfg.rf_tx_power = RF_POWER_P3p01dBm;
	if (cfg.measure_interval == 0)
		cfg.measure_interval = 1; // x1, T = cfg.measure_interval * advertising_interval_ms (ms),  Tmin = 1 * 1*62.5 = 62.5 ms / 1 * 160 * 62.5 = 10000 ms
	else if (cfg.measure_interval > 10)
		cfg.measure_interval = 10; // x10, T = cfg.measure_interval * advertising_interval_ms (ms),  Tmax = 10 * 1*62.5 = 625 ms / 10 * 160 * 62.5 = 100000 ms = 100 sec
	if (cfg.flg.tx_measures)
		tx_measures = 0xff;
	if (cfg.advertising_interval == 0) // 0 ?
		cfg.advertising_interval = 1; // 1*62.5 = 62.5 ms
	else if (cfg.advertising_interval > 160) // 160*62.5 = 10000 ms
		cfg.advertising_interval = 160;
	adv_interval = cfg.advertising_interval * 100; // Tadv_interval = adv_interval * 62.5 ms
	measurement_step_time = adv_interval * cfg.measure_interval * (625
			* sys_tick_per_us) - 250; // measurement_step_time = adv_interval * 62.5 * measure_interval
	/* interval = 16;
	 * connection_interval_ms = (interval * 125) / 100;
	 * connection_latency_ms = (cfg.connect_latency + 1) * connection_interval_ms = (16*125/100)*(99+1) = 2000;
	 * connection_timeout_ms = connection_latency_ms * 4 = 2000 * 4 = 8000;
	 */
	connection_timeout = ((cfg.connect_latency + 1) * 4 * 16 * 125) / 1000; // = 800, default = 8 sec
	if (connection_timeout > 32 * 100)
		connection_timeout = 32 * 100; //x10 ms, max 32 sec?
	else if(connection_timeout < 100)
		connection_timeout = 100;	//x10 ms,  1 sec
	if(!cfg.connect_latency) {
		my_periConnParameters.intervalMin = (cfg.advertising_interval * 625 / 30) - 1; // Tmin = 20*1.25 = 25 ms, Tмах = 3333*1.25 = 4166.25 ms
		my_periConnParameters.intervalMax = my_periConnParameters.intervalMin + 2;
		my_periConnParameters.latency = 0;
	} else {
		my_periConnParameters.intervalMin = 16; // 10*1.25 = 12.5 ms
		my_periConnParameters.intervalMax = 16; // 60*1.25 = 75 ms
		my_periConnParameters.latency = cfg.connect_latency;
	}
	my_periConnParameters.timeout = connection_timeout;
	if(cfg.min_step_time_update_lcd < 10)
		cfg.min_step_time_update_lcd = 10; // min 10*0.05 = 0.5 sec
	min_step_time_update_lcd = cfg.min_step_time_update_lcd * (100 * CLOCK_16M_SYS_TIMER_CLK_1MS);

	my_RxTx_Data[0] = 0x55;
	my_RxTx_Data[1] = VERSION;
	memcpy(&my_RxTx_Data[2], &cfg, sizeof(cfg));
}

_attribute_ram_code_ void WakeupLowPowerCb(int par) {
	(void) par;
	read_sensor_cb();
#if	USE_TRIGGER_OUT
	if(trg.flg.trigger_on)
		set_trigger_out();
#endif
	last_temp = measured_data.temp / 10;
	last_humi = measured_data.humi / 100;
	timer_measure_cb = 0;
	end_measure = 1;
	wrk_measure = 0;
	set_adv_data(cfg.flg.advertising_type);
}

_attribute_ram_code_ void ev_adv_timeout(u8 e, u8 *p, int n) {
	bls_ll_setAdvParam(adv_interval, adv_interval + 50,
			ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL,
			BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	bls_ll_setAdvEnable(1);
	start_measure = 1;
}
_attribute_ram_code_ void suspend_exit_cb(u8 e, u8 *p, int n) {
	if(timer_measure_cb)
		init_i2c();
}
_attribute_ram_code_ void suspend_enter_cb(u8 e, u8 *p, int n) {
	if (wrk_measure
		&& timer_measure_cb
		&& clock_time() - timer_measure_cb > SENSOR_MEASURING_TIMEOUT - 3*CLOCK_16M_SYS_TIMER_CLK_1MS) {
			WakeupLowPowerCb(0);
			bls_pm_setAppWakeupLowPower(0, 0);
	}
}

//------------------ user_init_normal -------------------
_attribute_ram_code_ void user_init_normal(void) {//this will get executed one time after power up
	random_generator_init(); //must
	// Read config
	if (flash_supported_eep_ver(EEP_SUP_VER, VERSION)) {
		if(flash_read_cfg(&cfg, EEP_ID_CFG, sizeof(cfg)) != sizeof(cfg))
			memcpy(&cfg, &def_cfg, sizeof(cfg));
#if	USE_TRIGGER_OUT
		if(flash_read_cfg(&trg, EEP_ID_TRG, FEEP_SAVE_SIZE_TRG) != FEEP_SAVE_SIZE_TRG)
			memcpy(&trg, &def_trg, sizeof(trg));
#endif
	} else {
		memcpy(&cfg, &def_cfg, sizeof(cfg));
#if	USE_TRIGGER_OUT
		memcpy(&trg, &def_trg, sizeof(trg));
#endif
	}
	test_config();
#if	USE_TRIGGER_OUT
	test_trg_on();
#endif
	memcpy(&ext, &def_ext, sizeof(ext));
	//	vtime_count_sec = ext.vtime;
	//	vtime_count_us = clock_time();
	init_ble();
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &suspend_exit_cb);
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_ENTER, &suspend_enter_cb);
	init_sensor();
	measured_data.battery_mv = get_battery_mv();
	battery_level = get_battery_level(measured_data.battery_mv);
	if (measured_data.battery_mv < 2000) {
		init_lcd();
		show_temp_symbol(0);
		show_big_number(measured_data.battery_mv * 10);
		show_small_number(-100, 1);
		show_battery_symbol(1);
		update_lcd();
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER,
				clock_time() + 120 * CLOCK_16M_SYS_TIMER_CLK_1S); // go deepsleep
	}
	init_lcd();
	//	show_atc_mac();
	ev_adv_timeout(0, 0, 0);
}

//------------------ user_init_deepRetn -------------------
_attribute_ram_code_ void user_init_deepRetn(void) {//after sleep this will get executed
	adv_mi_count++;
	blc_ll_initBasicMCU();
	rf_set_power_level_index(cfg.rf_tx_power);
	blc_ll_recoverDeepRetention();
	bls_ota_registerStartCmdCb(app_enter_ota_mode);
}

_attribute_ram_code_ bool is_comfort(int16_t t, uint16_t h) {
	bool c = 0;
	uint8_t npol = sizeof(comfort_x);
	for (uint8_t i = 0, j = npol - 1; i < npol; j = i++) {
		if (((comfort_y[i] < comfort_y[j]) && (comfort_y[i] <= h) && (h
				<= comfort_y[j]) && ((comfort_y[j] - comfort_y[i]) * (t
				- comfort_x[i]) > (comfort_x[j] - comfort_x[i]) * (h
				- comfort_y[i]))) || ((comfort_y[i] > comfort_y[j])
				&& (comfort_y[j] <= h) && (h <= comfort_y[i]) && ((comfort_y[j]
				- comfort_y[i]) * (t - comfort_x[i]) < (comfort_x[j]
				- comfort_x[i]) * (h - comfort_y[i]))))
			c = !c;
	}
	return c;
}

_attribute_ram_code_ void lcd_set_ext_data(void) {
	show_battery_symbol(ext.flg.battery);
	show_small_number(ext.small_number, ext.flg.percent_on);
	show_temp_symbol(*((uint8_t *) &ext.flg));
	show_big_number(ext.big_number);
}

_attribute_ram_code_ void lcd(void) {
	bool set_small_number_and_bat = true;
	while (vtime_count_sec && clock_time() - vtime_count_us
			> CLOCK_16M_SYS_TIMER_CLK_1S) {
		vtime_count_us += CLOCK_16M_SYS_TIMER_CLK_1S;
		vtime_count_sec--;
	}
	show_stage++;
	if (vtime_count_sec && (show_stage & 2)) { // no blinking on + show ext data
		if (show_stage & 1) { // stage blinking or show battery
			if (cfg.flg.show_batt_enabled || battery_level <= 5) { // Battery
				show_smiley(0); // stage show battery
				show_battery_symbol(1);
				show_small_number((battery_level >= 100) ? 99 : battery_level, 1);
				set_small_number_and_bat = false;
			} else if (cfg.flg.blinking_smiley) // blinking on
				show_smiley(0); // stage blinking and blinking on
			else
				show_smiley(*((uint8_t *) &ext.flg));
		} else
			show_smiley(*((uint8_t *) &ext.flg));
		if (set_small_number_and_bat) {
			show_battery_symbol(ext.flg.battery);
			show_small_number(ext.small_number, ext.flg.percent_on);
		}
		show_temp_symbol(*((uint8_t *) &ext.flg));
		show_big_number(ext.big_number);
	} else {
		if (show_stage & 1) { // stage blinking or show battery
			if (cfg.flg.show_batt_enabled || battery_level <= 5) { // Battery
				show_smiley(0); // stage show battery
				show_battery_symbol(1);
				show_small_number((battery_level >= 100) ? 99 : battery_level, 1);
				set_small_number_and_bat = false;
			} else if (cfg.flg.blinking_smiley) // blinking on
				show_smiley(0); // stage blinking and blinking on
			else {
				if (cfg.flg.comfort_smiley) { // no blinking on + comfort
					if (is_comfort(measured_data.temp, measured_data.humi)) // blinking + comfort
						show_smiley(5);
					else
						show_smiley(6);
				} else
					show_smiley(cfg.smiley); // no blinking
			}
		} else {
			if (cfg.flg.comfort_smiley) { // no blinking on + comfort
				if (is_comfort(measured_data.temp, measured_data.humi)) // blinking + comfort
					show_smiley(5);
				else
					show_smiley(6);
			} else
				show_smiley(cfg.smiley); // no blinking
		}
		if (set_small_number_and_bat) {
			show_battery_symbol(0);
			show_small_number(last_humi, 1);
		}
		if (cfg.flg.temp_F_or_C) {
			show_temp_symbol(0x60); // "°F"
			show_big_number((((measured_data.temp / 5) * 9) + 3200) / 10); // convert C to F
		} else {
			show_temp_symbol(0xA0); // "°C"
			show_big_number(last_temp);
		}
	}
}
//----------------------- main_loop()
_attribute_ram_code_ void main_loop(void) {
	blt_sdk_main_loop();
	if (wrk_measure
		&& timer_measure_cb
		&& clock_time() - timer_measure_cb > SENSOR_MEASURING_TIMEOUT) {
			WakeupLowPowerCb(0);
			bls_pm_setAppWakeupLowPower(0, 0);
	}
	if (ota_is_working) {
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
		bls_pm_setManualLatency(0);
	} else {
		if (!wrk_measure) {
			if (start_measure) {
				wrk_measure = 1;
				start_measure = 0;
				if (cfg.flg.lp_measures) {
					read_sensor_low_power();
					measured_data.battery_mv = get_battery_mv();
					battery_level = get_battery_level(measured_data.battery_mv);
					WakeupLowPowerCb(0);
					bls_pm_setAppWakeupLowPower(0, 0);
				} else {
					read_sensor_deep_sleep();
					if (bls_pm_getSystemWakeupTick() - clock_time() > SENSOR_MEASURING_TIMEOUT + 5*CLOCK_16M_SYS_TIMER_CLK_1MS) {
						bls_pm_registerAppWakeupLowPowerCb(WakeupLowPowerCb);
						bls_pm_setAppWakeupLowPower(timer_measure_cb + SENSOR_MEASURING_TIMEOUT, 1);
//						timer_measure_cb = 0;
					} else {
						bls_pm_setAppWakeupLowPower(0, 0);
					}
				}
			} else {
				uint32_t new = clock_time();
				if (blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && blc_ll_getTxFifoNumber() < 9) {
					if (end_measure) {
						end_measure = 0;
						if (tx_measures && (RxTxValueInCCC[0] | RxTxValueInCCC[1])) {
							if (tx_measures != 0xff)
								tx_measures--;
							ble_send_measures();
						}
						if (batteryValueInCCC[0] | batteryValueInCCC[1])
							ble_send_battery();
						if (tempValueInCCC[0] | tempValueInCCC[1])
							ble_send_temp();
						if (temp2ValueInCCC[0] | temp2ValueInCCC[1])
							ble_send_temp2();
						if (humiValueInCCC[0] | humiValueInCCC[1])
							ble_send_humi();
					} else if (mi_key_stage) {
						mi_key_stage = get_mi_keys(mi_key_stage);
					}
				}
				if (new - tim_measure >= measurement_step_time) {
					start_measure = 1;
					tim_measure = new;
				}
				if (new - tim_last_chow >= min_step_time_update_lcd) {
					lcd();
					update_lcd();
					tim_last_chow = new;
				}
				bls_pm_setAppWakeupLowPower(0, 0);
			}
		} else {
//			bls_pm_setAppWakeupLowPower(0, 0);
		}
		if((cfg.flg.advertising_type & 1)
			&& blc_ll_getCurrentState() == BLS_LINK_STATE_ADV) {
			set_adv_data(cfg.flg.advertising_type);
		}
		bls_pm_setSuspendMask(
				SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN
						| DEEPSLEEP_RETENTION_CONN);
	}
}
