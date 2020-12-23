#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"

#include "flash_eep.h"
#include "battery.h"
#include "ble.h"
#include "lcd.h"
#include "sensor.h"
#include "app.h"

RAM bool last_smiley;
RAM bool show_batt_or_humi;

RAM measured_data_t measured_data;
RAM volatile uint8_t tx_measures;

RAM int16_t last_temp; // x0.1 C
RAM uint16_t last_humi; // %
RAM uint8_t battery_level; // %

RAM volatile uint8_t loop_read_measure;
RAM volatile uint8_t start_measure; // start measure all
RAM volatile uint8_t wrk_measure;
RAM volatile uint8_t end_measure;
RAM uint32_t tim_last_chow; // timer show lcd >= 1.5 sec
RAM uint32_t tim_measure; // timer measurements >= 10 sec

RAM uint32_t adv_interval; // adv interval in 0.625 ms // cfg.advertising_interval * 100
RAM uint32_t measurement_step_time; // adv_interval * measure_interval

void lcd(void);

RAM int16_t comfort_x[] = {2000, 2560, 2700, 2500, 2050, 1700, 1600, 1750};
RAM uint16_t comfort_y[] = {2000, 1980, 3200, 6000, 8200, 8600, 7700, 3800};

// Settings
static const cfg_t def_cfg = {
	.flg.temp_C_or_F = false,
	.flg.blinking_smiley = false,
	.flg.comfort_smiley = true,
	.flg.show_batt_enabled = false,
	.flg.advertising_type = false,
	.flg.tx_measures = false,
	.advertising_interval = 32, // multiply by 62.5 ms  (2 sec)
	.measure_interval = 5, // * advertising_interval (10 sec)
	.rf_tx_power = RF_POWER_P3p01dBm,
	.connect_latency = 99
};
RAM cfg_t cfg;

void test_config(void) {
	if (cfg.humi_offset < -50)
		cfg.humi_offset = -50;
	if (cfg.humi_offset > 50)
		cfg.humi_offset = 50;
	if (cfg.measure_interval == 0)
		cfg.measure_interval = 1; // x1
	else if (cfg.measure_interval > 10)
		cfg.measure_interval = 10; // x10
	if(!cfg.flg.blinking_smiley && !cfg.flg.comfort_smiley)
		show_smiley(cfg.flg.smiley);
	if(cfg.flg.tx_measures)
		tx_measures = 1;
	adv_interval = cfg.advertising_interval * 100; // t = adv_interval * 0.625 ms
	measurement_step_time = adv_interval * cfg.measure_interval * 625 * sys_tick_per_us;
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

_attribute_ram_code_ void WakeupLowPowerCb(int par) {
	(void) par;
	read_sensor_cb();
	last_temp = measured_data.temp/10;
	last_humi = measured_data.humi/100;
	set_adv_data(last_temp, last_humi, battery_level, measured_data.battery_mv);
	end_measure = 1;
	wrk_measure = 0;
}

_attribute_ram_code_ void app_suspend_enter(void) {
	if (ota_is_working) {
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
		bls_pm_setManualLatency(0);
		if(timer_measure_cb) {
			bls_pm_setAppWakeupLowPower(0, 0);
			read_sensor_cb();
			timer_measure_cb = 0;
		}
	} else {
		if (timer_measure_cb) {
				bls_pm_registerAppWakeupLowPowerCb(WakeupLowPowerCb);
				bls_pm_setAppWakeupLowPower(timer_measure_cb, 1);
				timer_measure_cb = 0;
		} else if(!wrk_measure)
			bls_pm_setAppWakeupLowPower(0, 0);
		bls_pm_setSuspendMask(
				SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN
						| DEEPSLEEP_RETENTION_CONN);
	}
}

_attribute_ram_code_ void ev_adv_timeout(u8 e, u8 *p, int n) {
	bls_ll_setAdvParam(adv_interval, adv_interval + 50, ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0,  NULL, BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	bls_ll_setAdvEnable(1);
	start_measure = 1;
}

void user_init_normal(void) {//this will get executed one time after power up
	random_generator_init(); //must
	// Read config
	if (flash_read_cfg(&cfg, EEP_ID_CFG, sizeof(cfg)) != sizeof(cfg)) {
		memcpy(&cfg, &def_cfg, sizeof(cfg));
	}
	test_config();
	init_ble();
	init_sensor();
	measured_data.battery_mv = get_battery_mv();
	battery_level = get_battery_level(measured_data.battery_mv);
	if(measured_data.battery_mv < 1950) {
		init_lcd();
		show_big_number(0, 1);
		show_small_number(0, 1);
		show_battery_symbol(1);
		update_lcd();
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time() + 30 * CLOCK_16M_SYS_TIMER_CLK_1S);  //deepsleep
	}
	init_lcd();
	//	show_atc_mac();
	ev_adv_timeout(0,0,0);
}

void app_enter_ota_mode(void);

_attribute_ram_code_ void user_init_deepRetn(void) {//after sleep this will get executed
	blc_ll_initBasicMCU();
	rf_set_power_level_index(RF_POWER_P3p01dBm);
	blc_ll_recoverDeepRetention();
	bls_ota_registerStartCmdCb(app_enter_ota_mode);
}

void lcd(void) {
	if (cfg.flg.temp_C_or_F) {
		show_temp_symbol(2);
		show_big_number((((measured_data.temp / 5) * 9) + 3200) / 10, 1); // convert C to F
	} else {
		show_temp_symbol(1);
		show_big_number(last_temp, 1);
	}

	if (cfg.flg.comfort_smiley) {
		if (is_comfort(measured_data.temp, measured_data.humi)) {
			show_smiley(1);
		} else {
			show_smiley(2);
		}
	}
	if (cfg.flg.blinking_smiley) { // If Smiley should blink do it
		last_smiley = !last_smiley;
		show_smiley(last_smiley);
	}

	if (!cfg.flg.show_batt_enabled) {
		if(battery_level < 5)
			cfg.flg.show_batt_enabled = true;
		else
			show_batt_or_humi = true;
	}
	if (show_batt_or_humi) { // Change between Humidity displaying and battery level if show_batt_enabled=true
		show_small_number(last_humi, 1);
		show_battery_symbol(0);
	} else {
		show_small_number((battery_level == 100) ? 99 : battery_level, 1);
		show_battery_symbol(1);
	}
	show_batt_or_humi = !show_batt_or_humi;
}

void main_loop() {
	blt_sdk_main_loop();
	if(loop_read_measure) {
		loop_read_measure = 0;
		WakeupLowPowerCb(0);
	}
	if(!ota_is_working){
		if(start_measure) {
			wrk_measure = 1;
			start_measure = 0;
			read_sensor_deep_sleep();
			measured_data.battery_mv = get_battery_mv();
			battery_level = get_battery_level(measured_data.battery_mv);
		} else {
			if(!wrk_measure) {
				if(ble_connected) {
					if (end_measure) {
						end_measure = 0;
						if(tx_measures && (RxTxValueInCCC[0] | RxTxValueInCCC[1]))
							ble_send_all();
						if(batteryValueInCCC[0] | batteryValueInCCC[1])
							ble_send_battery(battery_level);
						if(tempValueInCCC[0] | tempValueInCCC[1])
							ble_send_temp(last_temp);
						if(humiValueInCCC[0] | humiValueInCCC[1])
							ble_send_humi(measured_data.humi);
					}
				}
				uint32_t new = clock_time();
				if(new - tim_measure >= measurement_step_time) {
					start_measure = 1;
					tim_measure = new;
				}
				if (new - tim_last_chow >= TIME_UPDATE_LCD) {
					lcd();
					update_lcd();
					tim_last_chow = new;
				}
			}
		}
	}
	app_suspend_enter();
}
