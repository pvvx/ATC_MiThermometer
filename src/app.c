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
#if USE_RTC
#include "rtc.h"
#endif
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
#include "ext_ota.h"

void app_enter_ota_mode(void);

RAM measured_data_t measured_data;

RAM volatile uint8_t tx_measures; // measurement transfer counter, flag
RAM volatile uint8_t start_measure; // start measurements
RAM volatile uint8_t wrk_measure; // measurements in process
RAM uint8_t end_measure; // measurements completed
RAM uint32_t tim_measure; // measurement timer

RAM uint32_t adv_interval; // adv interval in 0.625 ms // = cfg.advertising_interval * 100
RAM uint32_t connection_timeout; // connection timeout in 10 ms, Tdefault = connection_latency_ms * 4 = 2000 * 4 = 8000 ms
RAM uint32_t measurement_step_time; // = adv_interval * measure_interval

RAM uint32_t utc_time_sec;	// clock in sec (= 0 1970-01-01 00:00:00)
RAM uint32_t utc_time_sec_tick; // clock counter in 1/16 us
#if USE_TIME_ADJUST
RAM uint32_t utc_time_tick_step = CLOCK_16M_SYS_TIMER_CLK_1S; // adjust time clock (in 1/16 us for 1 sec)
#else
#define utc_time_tick_step CLOCK_16M_SYS_TIMER_CLK_1S
#endif
#if defined(GPIO_KEY2) || USE_WK_RDS_COUNTER
RAM  ext_key_t ext_key; // extension keys
#endif

#if BLE_SECURITY_ENABLE
RAM uint32_t pincode;
#endif

#if USE_SECURITY_BEACON
RAM uint8_t bindkey[16];
#endif

RAM scomfort_t cmf;
const scomfort_t def_cmf = {
		.t = {2100,2600}, // x0.01 C
		.h = {3000,6000}  // x0.01 %
};

// Settings
const cfg_t def_cfg = {
		.flg.temp_F_or_C = false,
		.flg.comfort_smiley = true,
		.flg.advertising_type = ADV_TYPE_DEFAULT,
		.rf_tx_power = RF_POWER_P0p04dBm, // RF_POWER_P3p01dBm,
		.connect_latency = DEF_CONNECT_LATENCY, // (49+1)*1.25*16 = 1000 ms
#if DEVICE_TYPE == DEVICE_MJWSD05MMC
		.advertising_interval = 80, // multiply by 62.5 ms = 5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 20 sec
		.hw_cfg.hwver = HW_VER_MJWSD05MMC,
#if USE_FLASH_MEMO
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_LYWSD03MMC
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_cfg.hwver = HW_VER_LYWSD03MMC_B14,
#if USE_FLASH_MEMO
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_MHO_C401
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 8, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 99, //x0.05 sec,   4.95 sec
		.hw_cfg.hwver = HW_VER_MHO_C401,
#if USE_FLASH_MEMO
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_MHO_C401N
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 8, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 99, //x0.05 sec,   4.95 sec
		.hw_cfg.hwver = HW_VER_MHO_C401_2022,
#if USE_FLASH_MEMO
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_CGG1
#if DEVICE_CGG1_ver == 2022
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_cfg.hwver = HW_VER_CGG1_2022,
#if USE_FLASH_MEMO
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#else
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 8, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 99, //x0.05 sec,   4.95 sec
		.hw_cfg.hwver = HW_VER_CGG1,
#if USE_FLASH_MEMO
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#endif
#elif DEVICE_TYPE == DEVICE_CGDK2
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = false,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_cfg.hwver = HW_VER_CGDK2,
#if USE_FLASH_MEMO
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_MHO_C122
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_cfg.hwver = HW_VER_MHO_C122,
#if USE_FLASH_MEMO
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#endif
		};
RAM cfg_t cfg;

static const external_data_t def_ext = {
#if DEVICE_TYPE != DEVICE_MJWSD05MMC
		.big_number = 0,
		.small_number = 0,
		.vtime_sec = 60, // 1 minutes
		.flg.smiley = 7, // 7 = "(ooo)"
		.flg.percent_on = true,
		.flg.battery = false,
		.flg.temp_symbol = 5 // 5 = "°C", ... app.h
#else
		.number = 1234500,
		.vtime_sec = 30, // 30 sec
		.flg.smiley = 7, // 7 = "(ooo)"
		.flg.battery = false,
		.flg.temp_symbol = LCD_SYM_N // 0 = " ", ... app.h
#endif
		};
RAM external_data_t ext;

#if DEVICE_TYPE == DEVICE_LYWSD03MMC
/*	0 - LYWSD03MMC B1.4
	3 - LYWSD03MMC B1.9
	4 - LYWSD03MMC B1.6
	5 - LYWSD03MMC B1.7
	10 - LYWSD03MMC B1.5 */
static const uint8_t id2hwver[8] = {
		'4','0','5','9','6','7','0','0'
};
#endif // DEVICE_TYPE == DEVICE_LYWSD03MMC

void set_hw_version(void) {
	cfg.hw_cfg.reserved = 0;
	if (sensor_i2c_addr == (SHTC3_I2C_ADDR << 1))
		cfg.hw_cfg.shtc3 = 1; // = 1 - sensor SHTC3
	else
		cfg.hw_cfg.shtc3 = 0; // = 0 - sensor SHT4x or ?
#if DEVICE_TYPE == DEVICE_LYWSD03MMC
/*	The version is determined by the addresses of the display and sensor on I2C

	HW | LCD I2C   addr | SHTxxx   I2C addr | Note
	-- | -- | -- | --
	B1.4 | 0x3C | 0x70   (SHTC3) |  
	B1.5 | UART! | 0x70   (SHTC3) |  
	B1.6 | UART! | 0x44   (SHT4x) |  
	B1.7 | 0x3C | 0x44   (SHT4x) | Test   original string HW
	B1.9 | 0x3E | 0x44   (SHT4x) |  
	B2.0 | 0x3C | 0x44   (SHT4x) | Test   original string HW

	Version 1.7 or 2.0 is determined at first run by reading the HW line written in Flash.
	Display matrices or controllers are different for all versions, except B1.7 = B2.0. */
#if	USE_DEVICE_INFO_CHR_UUID
#else
	uint8_t my_HardStr[4];
#endif
	uint8_t hwver = 0;
	if (lcd_i2c_addr == (B14_I2C_ADDR << 1)) {
		if (cfg.hw_cfg.shtc3) { // sensor SHTC3 ?
			hwver = HW_VER_LYWSD03MMC_B14; // HW:B1.4
		} else { // sensor SHT4x or ?
			hwver = HW_VER_LYWSD03MMC_B17; // HW:B1.7 or B2.0
			if(flash_read_cfg(&my_HardStr, EEP_ID_HWV, sizeof(my_HardStr)) == sizeof(my_HardStr)
					&& my_HardStr[0] == 'B'
					&& my_HardStr[2] == '.') {
				if (my_HardStr[1] == '1')
					my_HardStr[3] = '7';
				else if(my_HardStr[1] == '2') {
					my_HardStr[3] = '0';
				}
				flash_write_cfg(&my_HardStr, EEP_ID_HWV, sizeof(my_HardStr));
				return;
			}
		}
	} else {
		if (lcd_i2c_addr == (B19_I2C_ADDR << 1))
			hwver = HW_VER_LYWSD03MMC_B19; // HW:B1.9
		else { // UART
			if(cfg.hw_cfg.shtc3)
				hwver = HW_VER_LYWSD03MMC_B15; // HW:B1.5
			else
				hwver = HW_VER_LYWSD03MMC_B16; // HW:B1.6
		}
	}
	my_HardStr[0] = 'B';
	my_HardStr[1] = '1';
	my_HardStr[2] = '.';
	my_HardStr[3] = id2hwver[hwver & 7];
	cfg.hw_cfg.hwver = hwver;
	flash_write_cfg(&my_HardStr, EEP_ID_HWV, sizeof(my_HardStr));
	return;
#elif DEVICE_TYPE == DEVICE_MHO_C401
	cfg.hw_cfg.hwver = HW_VER_MHO_C401;
#elif DEVICE_TYPE == DEVICE_CGG1
#if DEVICE_CGG1_ver == 2022
	cfg.hw_cfg.hwver = HW_VER_CGG1_2022;
#else
	cfg.hw_cfg.hwver = HW_VER_CGG1;
#endif
#elif DEVICE_TYPE == DEVICE_CGDK2
	cfg.hw_cfg.hwver = HW_VER_CGDK2;
#elif DEVICE_TYPE == DEVICE_MHO_C401N
	cfg.hw_cfg.hwver = HW_VER_MHO_C401_2022;
#elif DEVICE_TYPE == DEVICE_MJWSD05MMC
	cfg.hw_cfg.hwver = HW_VER_MJWSD05MMC;
#elif DEVICE_TYPE == DEVICE_MHO_C122
	cfg.hw_cfg.hwver = HW_VER_MHO_C122;
#else
	cfg.hw_cfg.hwver = HW_VER_UNKNOWN;
#endif
}

// go deep-sleep 
void go_sleep(uint32_t tik) {
	cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER,
				clock_time() + tik); 
	while(1);
}

__attribute__((optimize("-Os")))
void test_config(void) {
	if (cfg.flg2.longrange)
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

	if(cfg.connect_latency > DEF_CONNECT_LATENCY && measured_data.average_battery_mv < LOW_VBAT_MV)
		cfg.connect_latency = DEF_CONNECT_LATENCY;
	/* interval = 16;
	 * connection_interval_ms = (interval * 125) / 100;
	 * connection_latency_ms = (cfg.connect_latency + 1) * connection_interval_ms = (16*125/100)*(99+1) = 2000;
	 * connection_timeout_ms = connection_latency_ms * 4 = 2000 * 4 = 8000;
	 */
	connection_timeout = ((cfg.connect_latency + 1) * (4 * DEF_CON_INERVAL * 125)) / 1000; // = 800, default = 8 sec
	if (connection_timeout > 32 * 100)
		connection_timeout = 32 * 100; //x10 ms, max 32 sec?
	else if (connection_timeout < 100)
		connection_timeout = 100;	//x10 ms,  1 sec

	if (!cfg.connect_latency) {
		my_periConnParameters.intervalMin =	(cfg.advertising_interval * 625	/ 30) - 1; // Tmin = 20*1.25 = 25 ms, Tmax = 3333*1.25 = 4166.25 ms
		my_periConnParameters.intervalMax = my_periConnParameters.intervalMin + 5;
		my_periConnParameters.latency = 0;
	} else {
		my_periConnParameters.intervalMin = DEF_CON_INERVAL; // 16*1.25 = 20 ms
		my_periConnParameters.intervalMax = DEF_CON_INERVAL; // 16*1.25 = 20 ms
		my_periConnParameters.latency = cfg.connect_latency;
	}
	my_periConnParameters.timeout = connection_timeout;
#if DEVICE_TYPE != DEVICE_MJWSD05MMC
#if	USE_EPD
	if (cfg.min_step_time_update_lcd < USE_EPD) // min 0.5 sec: (10*50ms)
		cfg.min_step_time_update_lcd = USE_EPD;
#endif
	if (cfg.min_step_time_update_lcd < 10) // min 0.5 sec: (10*50ms)
		cfg.min_step_time_update_lcd = 10;
	lcd_flg.min_step_time_update_lcd = cfg.min_step_time_update_lcd * (50 * CLOCK_16M_SYS_TIMER_CLK_1MS);
#endif
	set_hw_version();
	my_RxTx_Data[0] = CMD_ID_CFG;
	my_RxTx_Data[1] = VERSION;
	memcpy(&my_RxTx_Data[2], &cfg, sizeof(cfg));
}

void low_vbat(void) {
#if DEVICE_TYPE == DEVICE_MJWSD05MMC
	show_low_bat();
#else
#if (USE_EPD)
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
#if (USE_EPD)
	while(task_lcd()) pm_wait_ms(10);
#endif
#endif
	go_sleep(120 * CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 2 minutes
}

_attribute_ram_code_
void WakeupLowPowerCb(int par) {
	(void) par;
	if (wrk_measure) {
#if	USE_TRIGGER_OUT && defined(GPIO_RDS)
			rds_input_on();
#endif
#if (defined(CHL_ADC1) || defined(CHL_ADC1))
		if (1) {
#else
		if (read_sensor_cb()) {
#endif
			measured_data.count++;
#ifdef CHL_ADC1 // DIY version only!
			measured_data.temp = get_adc_mv(CHL_ADC1);
#endif
#ifdef CHL_ADC2 // DIY version only!
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
		end_measure = 0xff;
		wrk_measure = 0;
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC)
		SET_LCD_UPDATE();
#endif
	}
	timer_measure_cb = 0;
	bls_pm_setAppWakeupLowPower(0, 0); // clear callback
}

_attribute_ram_code_
static void suspend_exit_cb(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
	rf_set_power_level_index(cfg.rf_tx_power);
}

#if defined(GPIO_KEY2) || USE_WK_RDS_COUNTER
_attribute_ram_code_
static void suspend_enter_cb(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
#if USE_WK_RDS_COUNTER
	if (rds.type) // rds: switch or counter
		rds_suspend();
#endif
#ifdef GPIO_KEY2
	if (1) { // !ble_connected) {
		/* TODO: if connection mode, gpio wakeup throws errors in sdk libs!
		   Work options: bls_pm_setSuspendMask(SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN);
		   No DEEPSLEEP_RETENTION_CONN */
		cpu_set_gpio_wakeup(GPIO_KEY2, BM_IS_SET(reg_gpio_in(GPIO_KEY2), GPIO_KEY2 & 0xff)? Level_Low : Level_High, 1);  // pad wakeup deepsleep enable
		bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
	} else {
		cpu_set_gpio_wakeup(GPIO_KEY2, Level_Low, 0);  // pad wakeup suspend/deepsleep disable
	}
#endif // GPIO_KEY2
}
#endif // GPIO_KEY2 || (USE_WK_RDS_COUNTER)

//--- check battery
#define BAT_AVERAGE_SHL		4 // 16*16 = 256 ( 256*10/60 = 42.7 min)
#define BAT_AVERAGE_COUNT	(1 << BAT_AVERAGE_SHL) // 8
RAM struct {
	uint32_t buf2[BAT_AVERAGE_COUNT];
	uint16_t buf1[BAT_AVERAGE_COUNT];
	uint8_t index1;
	uint8_t index2;
} bat_average;

_attribute_ram_code_
__attribute__((optimize("-Os")))
void check_battery(void) {
	uint32_t i;
	uint32_t summ = 0;
	measured_data.battery_mv = get_battery_mv();
	if (measured_data.battery_mv < END_VBAT_MV) // It is not recommended to write Flash below 2V
		low_vbat();
	bat_average.index1++;
	bat_average.index1 &= BAT_AVERAGE_COUNT - 1;
	if(bat_average.index1 == 0) {
		bat_average.index2++;
		bat_average.index2 &= BAT_AVERAGE_COUNT - 1;
	}
	bat_average.buf1[bat_average.index1] = measured_data.battery_mv;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		summ += bat_average.buf1[i];
	bat_average.buf2[bat_average.index2] = summ;
	summ = 0;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		summ += bat_average.buf2[i];
	measured_data.average_battery_mv = summ >> (2*BAT_AVERAGE_SHL);
	measured_data.battery_level = get_battery_level(measured_data.average_battery_mv);
}

__attribute__((optimize("-Os")))
static void start_tst_battery(void) {
	int i;
	uint16_t avr_mv = get_battery_mv();
	measured_data.battery_mv = avr_mv;
	if (avr_mv < MIN_VBAT_MV) { // 2.2V
#if (DEVICE_TYPE ==	DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_CGDK2) || (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MHO_C122)
		// Set sleep power < 1 uA
#if (DEVICE_TYPE ==	DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_MHO_C122)
		send_i2c_word(0x70 << 1, 0x98b0); // SHTC3 go SLEEP: Sleep command of the sensor
#endif
		send_i2c_byte(0x3E << 1, 0xEA); // BU9792AFUV reset
#endif
		go_sleep(120 * CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 2 minutes
	}
	measured_data.average_battery_mv = avr_mv;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		bat_average.buf1[i] = avr_mv;
	avr_mv <<= BAT_AVERAGE_SHL;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		bat_average.buf2[i] = avr_mv;
}


#if USE_SECURITY_BEACON
void bindkey_init(void) {
#if	0 // USE_MIHOME_BEACON
	uint32_t faddr = find_mi_keys(MI_KEYTBIND_ID, 1);
	if (faddr) {
		memcpy(bindkey, &keybuf.data[12], sizeof(bindkey));
		faddr = find_mi_keys(MI_KEYSEQNUM_ID, 1);
		if (faddr)
			memcpy(&adv_buf.send_count, keybuf.data, sizeof(adv_buf.send_count)); // BLE_GAP_AD_TYPE_FLAGS
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

#if defined(GPIO_KEY2) || USE_WK_RDS_COUNTER
void set_default_cfg(void) {
	memcpy(&cfg, &def_cfg, sizeof(cfg));
	test_config();
	if(!cfg.hw_cfg.shtc3)
		cfg.flg.lp_measures = 1;
	flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
	SHOW_REBOOT_SCREEN();
	go_sleep(2*CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 2 sec
}
#endif // GPIO_KEY2 || USE_WK_RDS_COUNTER

//=========================================================
//-------------------- user_init_normal -------------------
void user_init_normal(void) {//this will get executed one time after power up
	bool next_start = false;
	unsigned int old_ver;
	adc_power_on_sar_adc(0); // - 0.4 mA
	lpc_power_down();
	blc_ll_initBasicMCU(); //must
	start_tst_battery();
	flash_unlock();
	random_generator_init(); //must
#if	(DEVICE_TYPE == DEVICE_MJWSD05MMC)
	test_first_ota(); // MJWSD05MMC: Correct FW OTA address? Reformat Big OTA to Low OTA
#endif // (DEVICE_TYPE == DEVICE_MJWSD05MMC)
#if defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)
	uint32_t hw_ver = get_mi_hw_version();
#endif // (DEVICE_TYPE == DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC)
	// Read config
	if(flash_read_cfg(&old_ver, EEP_ID_VER, sizeof(old_ver)) != sizeof(old_ver))
		old_ver = 0;
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
#endif
		// if version < 4.2 -> clear cfg.flg2.longrange
		if (old_ver <= 0x41) {
			cfg.flg2.longrange = 0;
			flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
		}
	} else {
#if BLE_SECURITY_ENABLE
		pincode = 0;
#endif
		memcpy(&cfg, &def_cfg, sizeof(cfg));
		memcpy(&cmf, &def_cmf, sizeof(cmf));
#if	USE_TRIGGER_OUT
		memcpy(&trg, &def_trg, FEEP_SAVE_SIZE_TRG);
#endif
#if defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)
		if (hw_ver)
			flash_write_cfg(&hw_ver, EEP_ID_HWV, sizeof(hw_ver));
#endif
	}
#if USE_WK_RDS_COUNTER
	rds.type = trg.rds.type;
	rds_init();
#endif
	init_i2c();
	reg_i2c_speed = (uint8_t)(CLOCK_SYS_CLOCK_HZ/(4*100000)); // 100 kHz
	test_config();
#if (POWERUP_SCREEN) || (USE_RTC) || (BLE_EXT_ADV)
	if(analog_read(DEEP_ANA_REG0) != 0x55) {
#if (BLE_EXT_ADV)
		cfg.flg2.longrange = 0;
		flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
#endif // BLE_EXT_ADV
		analog_write(DEEP_ANA_REG0, 0x55);
#if (USE_RTC)
#if POWERUP_SCREEN
		init_lcd();
		SHOW_REBOOT_SCREEN();
#endif // POWERUP_SCREEN
		// RTC wakes up after powering on > 1 second.
		go_sleep(1500*CLOCK_16M_SYS_TIMER_CLK_1MS);  // go deep-sleep 1.5 sec
#endif // SHOW_REBOOT_SCREEN || (USE_RTC)
#endif // POWERUP_SCREEN || (USE_RTC) || (BLE_EXT_ADV)
	}
	memcpy(&ext, &def_ext, sizeof(ext));
	init_ble();
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &suspend_exit_cb);
#if defined(GPIO_KEY2) || USE_WK_RDS_COUNTER
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_ENTER, &suspend_enter_cb);
#endif
	start_tst_battery();
	init_sensor();
#if USE_FLASH_MEMO
	memo_init();
#endif
#if	USE_RTC
	init_rtc();
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
#if (!USE_EPD)
	update_lcd();
#endif
	if (!next_start) { // first start?
		if(!cfg.hw_cfg.shtc3)
			cfg.flg.lp_measures = 1;
		flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
	}
	test_config();
#if defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)
	set_SerialStr();
#endif
	start_measure = 1;
}

//=========================================================
//------------------ user_init_deepRetn -------------------
_attribute_ram_code_
void user_init_deepRetn(void) {//after sleep this will get executed
	blc_ll_initBasicMCU();
	rf_set_power_level_index(cfg.rf_tx_power);
	blc_ll_recoverDeepRetention();
	bls_ota_registerStartCmdCb(app_enter_ota_mode);
}

//=========================================================
//----------------------- main_loop() ---------------------
_attribute_ram_code_
void main_loop(void) {
	blt_sdk_main_loop();
	while (clock_time() -  utc_time_sec_tick > utc_time_tick_step) {
		utc_time_sec_tick += utc_time_tick_step;
		utc_time_sec++; // + 1 sec
#if USE_RTC
		if(++rtc.seconds >= 60) {
			rtc.seconds = 0;
			if(++rtc.minutes >= 60) {
				rtc.minutes = 0;
				rtc_sync_utime = utc_time_sec;
			}
			SET_LCD_UPDATE();
		}
#endif
	}
	// instability workaround bls_pm_setAppWakeupLowPower()
	if(timer_measure_cb && clock_time() - timer_measure_cb > SENSOR_MEASURING_TIMEOUT)
		WakeupLowPowerCb(0);
	if (ota_is_working) {
#if USE_EXT_OTA
		if(ota_is_working == OTA_EXTENDED) {
			bls_pm_setManualLatency(3);
			clear_ota_area();
		} else
#endif
		{
			if ((ble_connected & BIT(CONNECTED_FLG_PAR_UPDATE))==0)
				bls_pm_setManualLatency(0);
		}
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	} else {
#if USE_WK_RDS_COUNTER
		if (rds.type) // rds: switch or counter
			rds_task();
#endif
		uint32_t new = clock_time();
#ifdef GPIO_KEY2
		if(!get_key2_pressed()) {
			if(!ext_key.key2pressed) {
				// key2 on
				ext_key.key2pressed = 1;
				ext_key.key_pressed_tik1 = new;
				ext_key.key_pressed_tik2 = new;
				set_adv_con_time(0); // set connection adv.
				SET_LCD_UPDATE();
			}
			else {
				if(new - ext_key.key_pressed_tik1 > 1750*CLOCK_16M_SYS_TIMER_CLK_1MS) {
					ext_key.key_pressed_tik1 = new;
					if(++cfg.flg2.screen_type > SCR_TYPE_EXT)
						cfg.flg2.screen_type = SCR_TYPE_TIME;
					if(ext_key.rest_adv_int_tad) {
						set_adv_con_time(1); // restore default adv.
						ext_key.rest_adv_int_tad = 0;
					}
					SET_LCD_UPDATE();
				}
				if(new - ext_key.key_pressed_tik2 > 5*CLOCK_16M_SYS_TIMER_CLK_1S) {
					if((reg_gpio_in(GPIO_KEY1) & (GPIO_KEY1 & 0xff))==0)
						set_default_cfg();
				}
			}
		}
		else {
			// key2 off
			ext_key.key2pressed = 0;
			ext_key.key_pressed_tik1 = new;
			ext_key.key_pressed_tik2 = new;
		}
#endif // GPIO_KEY2
#if defined(GPIO_KEY2) || USE_WK_RDS_COUNTER
		if(ext_key.rest_adv_int_tad < -80) {
			set_adv_con_time(1); // restore default adv.
			SET_LCD_UPDATE();
		}
#endif
		if (!wrk_measure) {
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
					// sensor SHT4x or cfg.flg.lp_measures
					if (cfg.hw_cfg.shtc3) {
						start_measure_sensor_low_power();
						check_battery();
						WakeupLowPowerCb(0);
					} else	{
						// if sensor SHT4x - > no callback, data read sensor is next cycle
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
				bls_pm_setAppWakeupLowPower(0, 0); // clear callback
				if (ble_connected && blc_ll_getTxFifoNumber() < 9) {  
					// connect, TxFifo ready
					if (end_measure & 1) {
						end_measure &= ~1;
						if (RxTxValueInCCC && tx_measures) {
							if (tx_measures != 0xff)
								tx_measures--;
							ble_send_measures();
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
					} else if (RxTxValueInCCC) {
						if (lcd_flg.b.send_notify) {
							// LCD for send notify
							lcd_flg.b.send_notify = 0;
							ble_send_lcd();
						}
					}
				}
#if USE_RTC
				else if(rtc_sync_utime) {
					rtc_sync_utime = 0;
					utc_time_sec = rtc_get_utime();
				}
#endif // USE_RTC
				if (new - tim_measure >= measurement_step_time) {
					tim_measure = new;
					start_measure = 1;
				}
				if(!cfg.flg2.screen_off) {
#if (USE_EPD)
					if ((!stage_lcd) && (new - lcd_flg.tim_last_chow >= lcd_flg.min_step_time_update_lcd)) {
						lcd_flg.tim_last_chow = new;
						lcd_flg.show_stage++;
						if(lcd_flg.update_next_measure) {
							lcd_flg.update = end_measure & 2;
							end_measure &= ~2;
						} else
							lcd_flg.update = 1;
					}
#elif (DEVICE_TYPE != DEVICE_MJWSD05MMC)
					if (new - lcd_flg.tim_last_chow >= lcd_flg.min_step_time_update_lcd) {
						lcd_flg.tim_last_chow = new;
						lcd_flg.show_stage++;
						if(lcd_flg.update_next_measure) {
							lcd_flg.update = end_measure & 2;
							end_measure &= ~2;
						} else
							lcd_flg.update = 1;
					}
#endif
					if (lcd_flg.update) {
						lcd_flg.update = 0;
						if (!lcd_flg.b.ext_data_buf) { // LCD show external data ? No
							lcd();
						}
						update_lcd();
					}
				}
			}
		}
#if (USE_EPD)
		if (wrk_measure == 0 && stage_lcd) {
			if (task_lcd()) {
				if(!gpio_read(EPD_BUSY)) {
//					if ((bls_pm_getSystemWakeupTick() - clock_time()) > 25 * CLOCK_16M_SYS_TIMER_CLK_1MS)
					{
						cpu_set_gpio_wakeup(EPD_BUSY, Level_High, 1);  // pad high wakeup deepsleep enable
						bls_pm_setWakeupSource(PM_WAKEUP_PAD);  // gpio pad wakeup suspend/deepsleep
					}
				} else {
					cpu_set_gpio_wakeup(EPD_BUSY, Level_High, 0);  // pad high wakeup deepsleep disable
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
