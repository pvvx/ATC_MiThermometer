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
#include "rds_count.h"
#endif
#if USE_FLASH_MEMO
#include "logger.h"
#endif
#if USE_MIHOME_BEACON
#include "mi_beacon.h"
#endif
#if USE_HA_BLE_BEACON
#include "ha_ble_beacon.h"
#endif

void app_enter_ota_mode(void);

RAM uint32_t chow_tick_clk; // count show validity time, in clock
RAM uint32_t chow_tick_sec; // count show validity time, in sec
RAM uint8_t show_stage; // count/stage update lcd code buffer
RAM lcd_flg_t lcd_flg;

RAM measured_data_t measured_data;

RAM volatile uint8_t tx_measures;

RAM volatile uint8_t start_measure; // start measure all
RAM volatile uint8_t wrk_measure;
RAM volatile uint8_t end_measure;
RAM uint32_t tim_last_chow; // timer show lcd >= 1.5 sec
RAM uint32_t tim_measure; // timer measurements >= 10 sec

RAM uint32_t adv_interval; // adv interval in 0.625 ms // = cfg.advertising_interval * 100
RAM uint32_t connection_timeout; // connection timeout in 10 ms, Tdefault = connection_latency_ms * 4 = 2000 * 4 = 8000 ms
RAM uint32_t measurement_step_time; // = adv_interval * measure_interval
RAM uint32_t min_step_time_update_lcd; // = cfg.min_step_time_update_lcd * 0.05 sec

RAM uint32_t utc_time_sec;	// clock in sec (= 0 1970-01-01 00:00:00)
RAM uint32_t utc_time_sec_tick;
#if USE_TIME_ADJUST
RAM uint32_t utc_time_tick_step = CLOCK_16M_SYS_TIMER_CLK_1S; // adjust time clock (in 1/16 us for 1 sec)
#else
#define utc_time_tick_step CLOCK_16M_SYS_TIMER_CLK_1S
#endif

#if USE_SECURITY_BEACON
RAM uint8_t bindkey[16];
#endif

RAM scomfort_t cmf;
const scomfort_t def_cmf = {
		.t = {2100,2600}, // x0.01 C
		.h = {3000,6000}  // x0.01 %
};

void lcd(void);

// Settings
const cfg_t def_cfg = {
		.flg.temp_F_or_C = false,
		.flg.comfort_smiley = true,
		.flg.blinking_time_smile = false,
		.flg.show_batt_enabled = false,
		.flg.advertising_type = ADV_TYPE_DEFAULT,
		.flg.tx_measures = false,
		.flg2.smiley = 0, // 0 = "     " off
		.flg2.bt5phy = 1, // support BT5.0
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
#if DEVICE_TYPE == DEVICE_LYWSD03MMC
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_cfg.hwver = 0,
#if USE_FLASH_MEMO
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_MHO_C401
		.flg.comfort_smiley = true,
		.measure_interval = 8, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 199, //x0.05 sec,   9.95 sec
		.hw_cfg.hwver = 1,
#if USE_FLASH_MEMO
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_CGG1
#if DEVICE_CGG1_ver == 2022
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_cfg.hwver = 7,
#if USE_FLASH_MEMO
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#else
		.flg.comfort_smiley = true,
		.measure_interval = 8, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 199, //x0.05 sec,   9.95 sec
		.hw_cfg.hwver = 2,
#if USE_FLASH_MEMO
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#endif
#elif DEVICE_TYPE == DEVICE_CGDK2
		.flg.comfort_smiley = false,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_cfg.hwver = 6,
#if USE_FLASH_MEMO
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#endif
		.rf_tx_power = RF_POWER_P0p04dBm, // RF_POWER_P3p01dBm,
		.connect_latency = (10000/(CON_INERVAL_LAT * 125)-1) // (49+1)*1.25*16 = 1000 ms
		};
RAM cfg_t cfg;
static const external_data_t def_ext = {
		.big_number = 0,
		.small_number = 0,
		.vtime_sec = 60, // 1 minutes
		.flg.smiley = 7, // 7 = "(ooo)"
		.flg.percent_on = true,
		.flg.battery = false,
		.flg.temp_symbol = 5 // 5 = "°C", ... app.h
		};
RAM external_data_t ext;
#if BLE_SECURITY_ENABLE
RAM uint32_t pincode;
#endif

__attribute__((optimize("-Os")))
void set_hw_version(void) {
	cfg.hw_cfg.reserved = 0;
	if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1))
		cfg.hw_cfg.shtc3 = 1; // = 1 - sensor SHTC3
	else
		cfg.hw_cfg.shtc3 = 0; // = 0 - sensor SHT4x or ?
#if DEVICE_TYPE == DEVICE_LYWSD03MMC
#if	USE_DEVICE_INFO_CHR_UUID
#else
	uint8_t my_HardStr[4];
#endif
	if (flash_read_cfg(&my_HardStr, EEP_ID_HWV, sizeof(my_HardStr)) != sizeof(my_HardStr)
	|| my_HardStr[0] != 'B'
	|| my_HardStr[2] != '.' ) {
		my_HardStr[0] = 'B';
		my_HardStr[1] = '1';
		my_HardStr[2] = '.';
		my_HardStr[3] = '?';
	}
	if (lcd_i2c_addr == (B14_I2C_ADDR << 1)) {
		if (cfg.hw_cfg.shtc3) { // sensor SHTC3 ?
			cfg.hw_cfg.hwver = 0; // HW:B1.4
			my_HardStr[3] = '4';
		} else { // sensor SHT4x or ?
			cfg.hw_cfg.hwver = 5; // HW:B1.7
			if (my_HardStr[1] == '1') // HW:B1.?
				my_HardStr[3] = '7';
		}
	} else if (lcd_i2c_addr == (B19_I2C_ADDR << 1)) {
		cfg.hw_cfg.hwver = 3; // HW:B1.9
		my_HardStr[3] = '9';
	} else {
		cfg.hw_cfg.hwver = 4; // HW:B1.6
		my_HardStr[3] = '6';
	}
#elif DEVICE_TYPE == DEVICE_MHO_C401
	cfg.hw_cfg.hwver = 1;
#elif DEVICE_TYPE == DEVICE_CGG1
#if DEVICE_CGG1_ver == 2022
	cfg.hw_cfg.hwver = 7;
#else
	cfg.hw_cfg.hwver = 2;
#endif
#elif DEVICE_TYPE == DEVICE_CGDK2
	cfg.hw_cfg.hwver = 6;
#else
	cfg.hw_cfg.hwver = 15;
#endif
}

__attribute__((optimize("-Os"))) void test_config(void) {
	if (cfg.flg2.longrange) {
		cfg.flg2.bt5phy = 1;
		cfg.flg2.ext_adv = 1;
	} else 	if (cfg.flg2.ext_adv)
		cfg.flg2.bt5phy = 1;
	if (cfg.rf_tx_power & BIT(7)) {
		if (cfg.rf_tx_power < RF_POWER_N25p18dBm)
			cfg.rf_tx_power = RF_POWER_N25p18dBm;
		else if (cfg.rf_tx_power > RF_POWER_P3p01dBm)
			cfg.rf_tx_power = RF_POWER_P3p01dBm;
	} else {
		if (cfg.rf_tx_power < RF_POWER_P3p23dBm)
			cfg.rf_tx_power = RF_POWER_P3p23dBm;
		else if (cfg.rf_tx_power > RF_POWER_P10p46dBm)
			cfg.rf_tx_power = RF_POWER_P10p46dBm;
	}
	if (cfg.measure_interval < 2)
		cfg.measure_interval = 2; // T = cfg.measure_interval * advertising_interval_ms (ms),  Tmin = 1 * 1*62.5 = 62.5 ms / 1 * 160 * 62.5 = 10000 ms
	else if (cfg.measure_interval > 25) // max = (0x100000000-1.5*10000000*16)/(10000000*16) = 25.3435456
		cfg.measure_interval = 25; // T = cfg.measure_interval * advertising_interval_ms (ms),  Tmax = 25 * 160*62.5 = 250000 ms = 250 sec
	if (cfg.flg.tx_measures)
		tx_measures = 0xff; // always notify
	if (cfg.advertising_interval == 0) // 0 ?
		cfg.advertising_interval = 1; // 1*62.5 = 62.5 ms
	else if (cfg.advertising_interval > 160) // max 160 : 160*62.5 = 10000 ms
		cfg.advertising_interval = 160; // 160*62.5 = 10000 ms
	adv_interval = cfg.advertising_interval * 100; // Tadv_interval = adv_interval * 62.5 ms
	measurement_step_time = adv_interval * cfg.measure_interval * (625
			* sys_tick_per_us) - 250; // measurement_step_time = adv_interval * 62.5 * measure_interval, max 250 sec

	if(cfg.connect_latency > ((int)(1000*100)/(int)(CON_INERVAL_LAT * 125)-1) && measured_data.battery_mv < 2800)
		cfg.connect_latency = (int)(1000*100)/(int)(CON_INERVAL_LAT * 125)-1;
	/* interval = 16;
	 * connection_interval_ms = (interval * 125) / 100;
	 * connection_latency_ms = (cfg.connect_latency + 1) * connection_interval_ms = (16*125/100)*(99+1) = 2000;
	 * connection_timeout_ms = connection_latency_ms * 4 = 2000 * 4 = 8000;
	 */
	connection_timeout = ((cfg.connect_latency + 1) * (4 * CON_INERVAL_LAT * 125)) / 1000; // = 800, default = 8 sec
	if (connection_timeout > 32 * 100)
		connection_timeout = 32 * 100; //x10 ms, max 32 sec?
	else if (connection_timeout < 100)
		connection_timeout = 100;	//x10 ms,  1 sec

	if (!cfg.connect_latency) {
		my_periConnParameters.intervalMin =	(cfg.advertising_interval * 625	/ 30) - 1; // Tmin = 20*1.25 = 25 ms, Tmax = 3333*1.25 = 4166.25 ms
		my_periConnParameters.intervalMax = my_periConnParameters.intervalMin + 5;
		my_periConnParameters.latency = 0;
	} else {
		my_periConnParameters.intervalMin = CON_INERVAL_LAT; // 16*1.25 = 20 ms
		my_periConnParameters.intervalMax = CON_INERVAL_LAT; // 16*1.25 = 20 ms
		my_periConnParameters.latency = cfg.connect_latency;
	}
	my_periConnParameters.timeout = connection_timeout;
	if (cfg.min_step_time_update_lcd < 10)
		cfg.min_step_time_update_lcd = 10; // min 10*0.05 = 0.5 sec
	min_step_time_update_lcd = cfg.min_step_time_update_lcd * (100 * CLOCK_16M_SYS_TIMER_CLK_1MS);

	my_RxTx_Data[0] = CMD_ID_CFG;
	my_RxTx_Data[1] = VERSION;
	memcpy(&my_RxTx_Data[2], &cfg, sizeof(cfg));
}

void low_vbat(void) {
#if (DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_CGG1)
	while(task_lcd()) pm_wait_ms(10);
#endif
	show_temp_symbol(0);
#if (DEVICE_TYPE != DEVICE_CGDK2)
	show_smiley(0);
#endif
	show_big_number_x10(measured_data.battery_mv * 10);
#if (DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
	show_small_number_x10(-1023, 1); // "Lo"
#else
	show_small_number(-123, 1); // "Lo"
#endif
	show_battery_symbol(1);
	update_lcd();
#if (DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_CGG1)
	while(task_lcd()) pm_wait_ms(10);
#endif
	cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER,
			clock_time() + 120 * CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 2 minutes
	while(1);
}

_attribute_ram_code_ void WakeupLowPowerCb(int par) {
	(void) par;
	if (wrk_measure) {
#if	USE_TRIGGER_OUT && defined(GPIO_RDS)
#if USE_WK_RDS_COUNTER
		if (rds.type == 0)
#endif
			rds_input_on();
#endif
#if (defined(CHL_ADC1) || defined(CHL_ADC1))
		if (1) {
#else
		if (read_sensor_cb()) {
#endif
			measured_data.count++;
#ifdef CHL_ADC1 // Test only!
			measured_data.temp = get_adc_mv(CHL_ADC1);
#endif
#ifdef CHL_ADC2 // Test only!
			measured_data.humi = get_adc_mv(CHL_ADC2);
#endif
			measured_data.temp_x01 = (measured_data.temp + 5)/ 10;
			measured_data.humi_x01 = (measured_data.humi + 5)/ 10;
			measured_data.humi_x1 = (measured_data.humi + 50)/ 100;
#if	USE_TRIGGER_OUT
			set_trigger_out();
#endif
#if USE_FLASH_MEMO
			if (cfg.averaging_measurements)
				write_memo();
#endif
#if	USE_MIHOME_BEACON && USE_SECURITY_BEACON
			if ((cfg.flg.advertising_type == ADV_TYPE_MI) && cfg.flg2.adv_crypto)
				mi_beacon_summ();
#endif
		}
#if USE_TRIGGER_OUT && defined(GPIO_RDS)
#if USE_WK_RDS_COUNTER
		if (rds.type == 0)
#endif
		{
			trg.flg.rds_input = get_rds_input();
			rds_input_off();
		}
#endif
		end_measure = 1;

		wrk_measure = 0;
	}	
	timer_measure_cb = 0;
	bls_pm_setAppWakeupLowPower(0, 0); // clear callback

	if (measured_data.battery_mv < 2000) // It is not recommended to write Flash below 2V
		low_vbat();
}

_attribute_ram_code_ static void suspend_exit_cb(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
	if (timer_measure_cb)
		init_i2c();
	rf_set_power_level_index(cfg.rf_tx_power);
}

#if USE_WK_RDS_COUNTER
_attribute_ram_code_ static void suspend_enter_cb(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
	if (rds.type) // rds: switch or counter
		rds_suspend();
}
#endif

//--- check battery
_attribute_ram_code_ void check_battery(void) {
	measured_data.battery_mv = get_battery_mv();
	measured_data.battery_level = get_battery_level(measured_data.battery_mv);
}

#if DEVICE_TYPE == DEVICE_LYWSD03MMC
/*
 * Read HW version
 * Flash:
 * 00055000:  42 31 2E 34 46 31 2E 30 2D 43 46 4D 4B 2D 4C 42  B1.4F1.0-CFMK-LB
 * 00055010:  2D 5A 43 58 54 4A 2D 2D FF FF FF FF FF FF FF FF  -ZCXTJ--
 */
uint32_t get_mi_hw_version(void) {
	uint32_t hw;
	flash_read_page(0x55000, sizeof(hw), (unsigned char *) &hw);
	if ((hw & 0xf0fff0ff) != 0x302E3042) {
		if (flash_read_cfg(&hw, EEP_ID_HWV, sizeof(hw)) != sizeof(hw))
			hw = 0;
		else if ((hw & 0xf0fff0ff) != 0x302E3042)
			hw = 0;
	}
	return hw;
}
#endif // DEVICE_TYPE == DEVICE_LYWSD03MMC

#if USE_SECURITY_BEACON
void bindkey_init(void) {
#if	USE_MIHOME_BEACON
	uint32_t faddr = find_mi_keys(MI_KEYTBIND_ID, 1);
	if (faddr) {
		memcpy(&bindkey, &keybuf.data[12], sizeof(bindkey));
		faddr = find_mi_keys(MI_KEYSEQNUM_ID, 1);
		if (faddr)
			memcpy(&adv_buf.send_count, &keybuf.data, sizeof(adv_buf.send_count)); // BLE_GAP_AD_TYPE_FLAGS
	} else {
		if (flash_read_cfg(&bindkey, EEP_ID_KEY, sizeof(bindkey))
				!= sizeof(bindkey)) {
			generateRandomNum(sizeof(bindkey), (unsigned char *) &bindkey);
			flash_write_cfg(&bindkey, EEP_ID_KEY, sizeof(bindkey));
		}
	}
	mi_beacon_init();
#else
	if (flash_read_cfg(&bindkey, EEP_ID_KEY, sizeof(bindkey))
			!= sizeof(bindkey)) {
		generateRandomNum(sizeof(bindkey), (unsigned char *) &bindkey);
		flash_write_cfg(&bindkey, EEP_ID_KEY, sizeof(bindkey));
	}
#endif // USE_MIHOME_BEACON
#if USE_HA_BLE_BEACON
	ha_ble_beacon_init();
#endif
}
#endif // USE_SECURITY_BEACON

//------------------ user_init_normal -------------------
void user_init_normal(void) {//this will get executed one time after power up
	bool next_start = false;
	adc_power_on_sar_adc(0); // - 0.4 mA
	lpc_power_down();
	if (get_battery_mv() < MIN_VBAT_MV) { // 2.2V
		sensor_go_sleep(); // SHTC3 go SLEEP
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER,
				clock_time() + 120 * CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 2 minutes
	}
	random_generator_init(); //must
#if DEVICE_TYPE == DEVICE_LYWSD03MMC
	uint32_t hw_ver = get_mi_hw_version();
#endif
	// Read config
	next_start = flash_supported_eep_ver(EEP_SUP_VER, VERSION);
	if (next_start) {
		if (flash_read_cfg(&cfg, EEP_ID_CFG, sizeof(cfg)) != sizeof(cfg))
			memcpy(&cfg, &def_cfg, sizeof(cfg));
		if (flash_read_cfg(&cmf, EEP_ID_CMF, sizeof(cmf)) != sizeof(cmf))
			memcpy(&cmf, &def_cmf, sizeof(cmf));
#if USE_TIME_ADJUST
		if (flash_read_cfg(&utc_time_tick_step, EEP_ID_TIM,
				sizeof(utc_time_tick_step)) != sizeof(utc_time_tick_step))
			utc_time_tick_step = CLOCK_16M_SYS_TIMER_CLK_1S;
#endif
#if BLE_SECURITY_ENABLE
		if (flash_read_cfg(&pincode, EEP_ID_PCD, sizeof(pincode))
				!= sizeof(pincode))
			pincode = 0;
#endif
#if	USE_TRIGGER_OUT
		if (flash_read_cfg(&trg, EEP_ID_TRG, FEEP_SAVE_SIZE_TRG)
				!= FEEP_SAVE_SIZE_TRG)
			memcpy(&trg, &def_trg, FEEP_SAVE_SIZE_TRG);
#if USE_WK_RDS_COUNTER
		rds.type = trg.rds.type;
#endif
#endif
	} else {
		memcpy(&cfg, &def_cfg, sizeof(cfg));
		memcpy(&cmf, &def_cmf, sizeof(cmf));
#if BLE_SECURITY_ENABLE
		pincode = 0;
#endif
#if	USE_TRIGGER_OUT
		memcpy(&trg, &def_trg, FEEP_SAVE_SIZE_TRG);
#endif
#if DEVICE_TYPE == DEVICE_LYWSD03MMC
		if (hw_ver)
			flash_write_cfg(&hw_ver, EEP_ID_HWV, sizeof(hw_ver));
#endif
	}
#if USE_WK_RDS_COUNTER
	rds_init();
#endif
	if(analog_read(DEEP_ANA_REG0) != 0x55) {
		cfg.flg2.ext_adv = 0;
		cfg.flg2.longrange = 0;
		analog_write(DEEP_ANA_REG0, 0x55);
	}

	test_config();
	memcpy(&ext, &def_ext, sizeof(ext));
	init_ble();
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &suspend_exit_cb);
#if USE_WK_RDS_COUNTER
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_ENTER, &suspend_enter_cb);
#endif
	init_sensor();
#if USE_FLASH_MEMO
	memo_init();
#endif
	init_lcd();
	set_hw_version();
	wrk_measure = 1;
#if defined(GPIO_ADC1) || defined(GPIO_ADC2)
	sensor_go_sleep();
#else
	start_measure_sensor_low_power();
#endif
#if USE_SECURITY_BEACON
	bindkey_init();
#endif
	check_battery();
	WakeupLowPowerCb(0);
	lcd();
#if (DEVICE_TYPE == DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_CGDK2)
	update_lcd();
#endif
	if (!next_start) { // first start?
		if (!cfg.hw_cfg.shtc3) // sensor SHT4x ?
			cfg.flg.lp_measures = 1;
		flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
	}
	test_config();
	start_measure = 1;
}

//------------------ user_init_deepRetn -------------------
_attribute_ram_code_ void user_init_deepRetn(void) {//after sleep this will get executed
	blc_ll_initBasicMCU();
	rf_set_power_level_index(cfg.rf_tx_power);
	blc_ll_recoverDeepRetention();
	bls_ota_registerStartCmdCb(app_enter_ota_mode);
}

_attribute_ram_code_ uint8_t is_comfort(int16_t t, uint16_t h) {
	uint8_t ret = SMILE_SAD;
	if (t >= cmf.t[0] && t <= cmf.t[1] && h >= cmf.h[0] && h <= cmf.h[1])
		ret = SMILE_HAPPY;
	return ret;
}

_attribute_ram_code_ __attribute__((optimize("-Os"))) void lcd(void) {
	bool set_small_number_and_bat = true;
	while (chow_tick_sec && clock_time() - chow_tick_clk
			> CLOCK_16M_SYS_TIMER_CLK_1S) {
		chow_tick_clk += CLOCK_16M_SYS_TIMER_CLK_1S;
		chow_tick_sec--;
	}
	show_stage++;
	if (chow_tick_sec && (show_stage & 2)) { // show ext data
		if (show_stage & 1) { // stage blinking or show battery or clock
			if (cfg.flg.show_batt_enabled
#if	(DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
#else
				|| measured_data.battery_level <= 15
#endif
				) { // Battery
#if	(DEVICE_TYPE != DEVICE_CGDK2)
				show_smiley(0); // stage show battery
#endif
				show_battery_symbol(1);
#if	(DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
#if DEVICE_TYPE == DEVICE_CGG1
				show_batt_cgg1();
#else
				show_batt_cgdk2();
#endif
#else
				show_small_number((measured_data.battery_level >= 100) ? 99 : measured_data.battery_level, 1);
#endif // (DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
				set_small_number_and_bat = false;
			} else if (cfg.flg.blinking_time_smile) { // blinking on
#if	USE_CLOCK
				show_clock(); // stage clock
				show_ble_symbol(ble_connected);
				return;
#else
#if	(DEVICE_TYPE != DEVICE_CGDK2)
				show_smiley(0); // stage blinking and blinking on
#endif
#endif
			}
#if	(DEVICE_TYPE != DEVICE_CGDK2)
			else
				show_smiley(*((uint8_t *) &ext.flg));
#endif
		}
#if	(DEVICE_TYPE != DEVICE_CGDK2)
		else
			show_smiley(*((uint8_t *) &ext.flg));
#endif
		if (set_small_number_and_bat) {
			show_battery_symbol(ext.flg.battery);
#if	(DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
			show_small_number_x10(ext.small_number, ext.flg.percent_on);
#else
			show_small_number(ext.small_number, ext.flg.percent_on);
#endif
		}
		show_temp_symbol(*((uint8_t *) &ext.flg));
		show_big_number_x10(ext.big_number);
	} else {
		if (show_stage & 1) { // stage blinking or show battery
#if	USE_CLOCK
			if (cfg.flg.blinking_time_smile && (show_stage & 2)) {
				show_clock(); // stage clock
				show_ble_symbol(ble_connected);
				return;
			}
#endif
			if (cfg.flg.show_batt_enabled
#if	(DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
#else
				|| measured_data.battery_level <= 15
#endif
				) { // Battery
#if	(DEVICE_TYPE != DEVICE_CGDK2)
				show_smiley(0); // stage show battery
#endif
				show_battery_symbol(1);
#if	(DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
#if DEVICE_TYPE == DEVICE_CGG1
				show_batt_cgg1();
#else
				show_batt_cgdk2();
#endif
#else
				show_small_number((measured_data.battery_level >= 100) ? 99 : measured_data.battery_level, 1);
#endif // (DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
				set_small_number_and_bat = false;
			} else if (cfg.flg.blinking_time_smile) { // blinking on
#if	USE_CLOCK
				show_clock(); // stage clock
				show_ble_symbol(ble_connected);
				return;
#else
#if	(DEVICE_TYPE != DEVICE_CGDK2)
				show_smiley(0); // stage blinking and blinking on
#endif
#endif
			} else {
#if	(DEVICE_TYPE != DEVICE_CGDK2)
				if (cfg.flg.comfort_smiley) { // no blinking on + comfort
					show_smiley(is_comfort(measured_data.temp, measured_data.humi));
				} else
					show_smiley(cfg.flg2.smiley); // no blinking
#endif
			}
		} else {
#if	(DEVICE_TYPE != DEVICE_CGDK2)
			if (cfg.flg.comfort_smiley) { // no blinking on + comfort
				show_smiley(is_comfort(measured_data.temp, measured_data.humi));
			} else
				show_smiley(cfg.flg2.smiley); // no blinking
#endif
		}
		if (set_small_number_and_bat) {
#if	(DEVICE_TYPE == DEVICE_CGG1) || (DEVICE_TYPE == DEVICE_CGDK2)
			show_battery_symbol(!cfg.flg.show_batt_enabled);
			show_small_number_x10(measured_data.humi_x01, 1);
#else
			show_battery_symbol(0);
			show_small_number(measured_data.humi_x1, 1);
#endif
		}
		if (cfg.flg.temp_F_or_C) {
			show_temp_symbol(TMP_SYM_F); // "°F"
			show_big_number_x10((((measured_data.temp / 5) * 9) + 3200) / 10); // convert C to F
		} else {
			show_temp_symbol(TMP_SYM_C); // "°C"
			show_big_number_x10(measured_data.temp_x01);
		}
	}
	show_ble_symbol(ble_connected);
}

//----------------------- main_loop()
_attribute_ram_code_ void main_loop(void) {
	blt_sdk_main_loop();
	while (clock_time() -  utc_time_sec_tick > utc_time_tick_step) {
		utc_time_sec_tick += utc_time_tick_step;
		utc_time_sec++; // + 1 sec
	}
	// instability workaround bls_pm_setAppWakeupLowPower()
	if(timer_measure_cb && clock_time() - timer_measure_cb > SENSOR_MEASURING_TIMEOUT)
		WakeupLowPowerCb(0);
	if (ota_is_working) {
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN); // SUSPEND_DISABLE
		if ((ble_connected&2)==0)
			bls_pm_setManualLatency(0);
	} else {
#if USE_WK_RDS_COUNTER
		if (rds.type) // rds: switch or counter
			rds_task();
#endif
		if (!wrk_measure) {
			uint32_t new = clock_time();
			if (start_measure
//				&& sensor_i2c_addr
				&&	bls_pm_getSystemWakeupTick() - new > SENSOR_MEASURING_TIMEOUT + 5*CLOCK_16M_SYS_TIMER_CLK_1MS) {

				bls_pm_setSuspendMask(SUSPEND_DISABLE);

				wrk_measure = 1;
				start_measure = 0;
#if defined(GPIO_ADC1) || defined(GPIO_ADC2)
				check_battery();
				WakeupLowPowerCb(0);
#else
				if (cfg.flg.lp_measures) {
					if (cfg.hw_cfg.shtc3) {
						start_measure_sensor_low_power();
						check_battery();
						WakeupLowPowerCb(0);
					} else { // sensor SHT4x
						// no callback, data read sensor is next cycle
						WakeupLowPowerCb(0);
						check_battery();
						start_measure_sensor_deep_sleep();
					}
				} else {
					start_measure_sensor_deep_sleep();
					check_battery();
					// Sleep transition instability workaround bls_pm_setAppWakeupLowPower()
					if(clock_time() - timer_measure_cb > SENSOR_MEASURING_TIMEOUT - 3)
						WakeupLowPowerCb(0);
					else {
						bls_pm_registerAppWakeupLowPowerCb(WakeupLowPowerCb);
						bls_pm_setAppWakeupLowPower(timer_measure_cb + SENSOR_MEASURING_TIMEOUT, 1);
					}
				}
#endif
			} else {
				if ((blc_ll_getCurrentState() & BLS_LINK_STATE_CONN) && blc_ll_getTxFifoNumber() < 9) {
					if (end_measure) {
						end_measure = 0;
						if (RxTxValueInCCC) {
							if (tx_measures) {
								if (tx_measures != 0xff)
									tx_measures--;
								ble_send_measures();
							}
							if (lcd_flg.b.new_update) {
								lcd_flg.b.new_update = 0;
								ble_send_lcd();
							}
						}
						if (batteryValueInCCC)
							ble_send_battery();
						if (tempValueInCCC)
							ble_send_temp();
						if (temp2ValueInCCC)
							ble_send_temp2();
						if (humiValueInCCC)
							ble_send_humi();
					} else if (mi_key_stage) {
						mi_key_stage = get_mi_keys(mi_key_stage);
#if USE_FLASH_MEMO
					} else if (rd_memo.cnt) {
						send_memo_blk();
#endif
					}
				}
#if 0 // USE_WK_RDS_COUNTER
				if ((!blta.adv_duraton_en) && new - tim_measure >= measurement_step_time) {
#else
				if (new - tim_measure >= measurement_step_time) {
#endif
					tim_measure = new;
					start_measure = 1;
				}
#if (DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_CGG1)
				else
				if ((!stage_lcd) && (new - tim_last_chow >= min_step_time_update_lcd)) {
#else
				if (new - tim_last_chow >= min_step_time_update_lcd) {
#endif
					if (!lcd_flg.b.ext_data) {
						lcd_flg.b.new_update = lcd_flg.b.notify_on;
						lcd();
					}
					update_lcd();
					tim_last_chow = new;
				}
				bls_pm_setAppWakeupLowPower(0, 0); // clear callback
			}
		}
#if (DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_CGG1)
		if (wrk_measure == 0 && stage_lcd) {
			if (task_lcd()) {
				if(!gpio_read(EPD_BUSY)) {
//					if ((bls_pm_getSystemWakeupTick() - clock_time()) > 25 * CLOCK_16M_SYS_TIMER_CLK_1MS) {
						cpu_set_gpio_wakeup(EPD_BUSY, Level_High, 1);  // pad high wakeup deepsleep enable
						bls_pm_setWakeupSource(PM_WAKEUP_PAD);  // gpio pad wakeup suspend/deepsleep
//					}
				} else {
					bls_pm_setSuspendMask(SUSPEND_DISABLE);
					return;
				}
			} else {
				cpu_set_gpio_wakeup(EPD_BUSY, Level_High, 0);  // pad high wakeup deepsleep disable
			}
		}
#endif
		bls_pm_setSuspendMask(
				SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
	}
}
