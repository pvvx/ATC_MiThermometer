#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"
#include "cmd_parser.h"
#include "flash_eep.h"
#include "battery.h"
#include "ble.h"
#include "lcd.h"
#include "app.h"
#include "i2c.h"
#include "sensor.h"
#if (DEV_SERVICES & SERVICE_PRESSURE)
#include "hx71x.h"
#endif
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
#include "rtc.h"
#endif
#if (DEV_SERVICES & SERVICE_18B20)
#include "my18b20.h"
#endif
#include "trigger.h"
#if (DEV_SERVICES & SERVICE_RDS)
#include "rds_count.h"
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
#include "logger.h"
#endif
#if (DEV_SERVICES & SERVICE_PLM)
#include "rh.h"
#endif
#if USE_MIHOME_BEACON
#include "mi_beacon.h"
#endif
#if USE_BTHOME_BEACON
#include "bthome_beacon.h"
#endif
#include "ext_ota.h"
#if USE_SYNC_SCAN
#include "scanning.h"
#endif
#if defined(USE_SENSOR_ENS160) && USE_SENSOR_ENS160
#include "ens160.h"
#endif
#if USE_SDM_OUT
#include "sdm_out.h"
#endif


void app_enter_ota_mode(void);

RAM measured_data_t measured_data;
RAM work_flg_t wrk;

#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)
RAM ext_key_t ext_key; // extension keys
#endif

#if (DEV_SERVICES & SERVICE_PINCODE)
RAM u32 pincode;
#endif

#if (DEV_SERVICES & SERVICE_BINDKEY)
RAM u8 bindkey[16];
#endif

#if (DEV_SERVICES & SERVICE_SCREEN)
RAM scomfort_t cmf;
const scomfort_t def_cmf = {
		.t = {2100,2600}, // x0.01 C
		.h = {3000,6000}  // x0.01 %
};
#endif

// Settings
const cfg_t def_cfg = {
		.flg.temp_F_or_C = false,
		.flg.comfort_smiley = true,
		.flg.lp_measures = true,
		.flg.advertising_type = ADV_TYPE_DEFAULT,
		.rf_tx_power = RF_POWER_P0p04dBm, // RF_POWER_P3p01dBm,
		.connect_latency = DEF_CONNECT_LATENCY, // (49+1)*1.25*16 = 1000 ms
		.event_adv_cnt = 6,
		.flg3.adv_interval_delay = 10,
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
		.advertising_interval = 80, // multiply by 62.5 ms = 5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 20 sec
		.hw_ver = HW_VER_MJWSD05MMC,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_LYWSD03MMC
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = HW_VER_LYWSD03MMC_B14,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_MHO_C401
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 8, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 99, //x0.05 sec,   4.95 sec
		.hw_ver = HW_VER_MHO_C401,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_MHO_C401N
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 8, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 99, //x0.05 sec,   4.95 sec
		.hw_ver = HW_VER_MHO_C401_2022,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_CGG1
#if DEVICE_CGG1_ver == 2022
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = HW_VER_CGG1_2022,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#else
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 8, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 99, //x0.05 sec,   4.95 sec
		.hw_ver = HW_VER_CGG1,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#endif
#elif DEVICE_TYPE == DEVICE_CGDK2
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = false,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = HW_VER_CGDK2,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_MHO_C122
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = HW_VER_MHO_C122,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#elif (DEVICE_TYPE == DEVICE_TNK01)
		.flg2.adv_flags = true,
		.advertising_interval = 24, // multiply by 62.5 ms = 1.5 sec
		.measure_interval = 2, // * advertising_interval = 3 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 200, // * measure_interval = 3 * 200 = 600 sec = 10 minutes
#endif
#elif (DEVICE_TYPE == DEVICE_TB03F)
		.flg2.adv_flags = true,
		.hw_ver = DEVICE_TYPE,
#if USE_SENSOR_INA3221
		.advertising_interval = 69, // multiply by 62.5 ms = 4312.5 sec  (замер 12767.232 ms)
		.measure_interval = 3, // * advertising_interval = 12.9375 sec
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 139, // * measure_interval = 12.9375 * 139 = 1798.3125 sec = 29.971875 minutes
#endif
#else
		.advertising_interval = 34, // multiply by 62.5 ms = 2.125 sec
		.measure_interval = 4, // * advertising_interval = 8500 sec
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 212, // * measure_interval = 8.5 * 212 = 1802 sec = 30 minutes
#endif
#endif
#elif (DEVICE_TYPE == DEVICE_TS0201) || (DEVICE_TYPE == DEVICE_TH03Z) || (DEVICE_TYPE == DEVICE_ZTH01) || (DEVICE_TYPE == DEVICE_ZTH02)
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#elif (DEVICE_TYPE == DEVICE_PLM1)
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif
#elif DEVICE_TYPE == DEVICE_ZTH03
		.flg2.adv_flags = true,
		.advertising_interval = 80, // multiply by 62.5 ms = 5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif

#elif DEVICE_TYPE == DEVICE_LKTMZL02
		.flg2.adv_flags = true,
		.advertising_interval = 80, // multiply by 62.5 ms = 5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif

#elif DEVICE_TYPE == DEVICE_ZTH05Z
		.flg2.adv_flags = true,
		.advertising_interval = 80, // multiply by 62.5 ms = 5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif

#elif (DEVICE_TYPE == DEVICE_ZYZTH01)
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.measure_interval = 4, // * advertising_interval = 10 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif

#elif (DEVICE_TYPE == DEVICE_ZYZTH02)
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif

#elif (DEVICE_TYPE == DEVICE_ZG_227Z)
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif

#elif (DEVICE_TYPE == DEVICE_ZBEACON_TH01)
		.flg2.adv_flags = true,
		.advertising_interval = 40, // multiply by 62.5 ms = 2.5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 10 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 180, // * measure_interval = 10 * 180 = 1800 sec = 30 minutes
#endif

#elif DEVICE_TYPE == DEVICE_MJWSD06MMC
		.flg2.adv_flags = true,
		.advertising_interval = 80, // multiply by 62.5 ms = 5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 20 sec
		.min_step_time_update_lcd = 49, //x0.05 sec,   2.45 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 90 = 1800 sec = 30 minutes
#endif
#elif (DEVICE_TYPE == DEVICE_ZG303Z)
		.flg2.adv_flags = true,
		.advertising_interval = 80, // multiply by 62.5 ms = 5 sec
		.flg.comfort_smiley = true,
		.measure_interval = 4, // * advertising_interval = 20 sec
		.hw_ver = DEVICE_TYPE,
#if (DEV_SERVICES & SERVICE_HISTORY)
		.averaging_measurements = 90, // * measure_interval = 20 * 180 = 1800 sec = 30 minutes
#endif
#else
#error "DEVICE_TYPE = ?"
#endif
		};
RAM cfg_t cfg;

#if (DEV_SERVICES & SERVICE_SCREEN)

static const external_data_t def_ext = {
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
		.number = 1234500,
		.vtime_sec = 30, // 30 sec
		.flg.smiley = 7, // 7 = "(ooo)"
		.flg.battery = false,
		.flg.temp_symbol = LCD_SYM_N // 0 = " ", ... app.h
#else
		.big_number = 0,
		.small_number = 0,
		.vtime_sec = 60, // 1 minutes
		.flg.smiley = 7, // 7 = "(ooo)"
		.flg.percent_on = true,
		.flg.battery = false,
		.flg.temp_symbol = 5 // 5 = "°C", ... app.h
#endif
		};

RAM external_data_t ext;
#endif

#if DEVICE_TYPE == DEVICE_LYWSD03MMC
/*	0 - LYWSD03MMC B1.4
	3 - LYWSD03MMC B1.9
	4 - LYWSD03MMC B1.6
	5 - LYWSD03MMC B1.7
	10 - LYWSD03MMC B1.5  &7=2
	14 - LYWSD03MMC NB1.6 &7=6 */
static const u8 id2hwver[8] = {
		//0  1   2   3   4   5   6   7
		'4','0','5','9','6','7','1','0'
};
#endif // DEVICE_TYPE == DEVICE_LYWSD03MMC

void set_hw_version(void) {
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
#if (DEV_SERVICES & SERVICE_SCREEN)
#if	USE_DEVICE_INFO_CHR_UUID
#else
	u8 my_HardStr[4];
#endif // USE_DEVICE_INFO_CHR_UUID
	u8 hwver = 0;
	if (lcd_i2c_addr == (B14_I2C_ADDR << 1)) {
//		if (cfg.hw_cfg.shtc3) { // sensor SHTC3 ?
		if (sensor_cfg.sensor_type == TH_SENSOR_SHTC3) {
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
	} else if (lcd_i2c_addr == (B19_I2C_ADDR << 1)) {
		hwver = HW_VER_LYWSD03MMC_B19; // HW:B1.9
    } else if (lcd_i2c_addr == N16_I2C_ADDR) {
		hwver = HW_VER_LYWSD03MMC_NB16; // HW:B1.6 SPI
	} else { // UART
		// if(cfg.hw_cfg.shtc3)
		if (sensor_cfg.sensor_type == TH_SENSOR_SHTC3)
			hwver = HW_VER_LYWSD03MMC_B15; // HW:B1.5
		else
			hwver = HW_VER_LYWSD03MMC_B16; // HW:B1.6
	}
	my_HardStr[0] = 'B';
	my_HardStr[1] = '1';
	my_HardStr[2] = '.';
	my_HardStr[3] = id2hwver[hwver & 7];
	cfg.hw_ver = hwver;
	flash_write_cfg(&my_HardStr, EEP_ID_HWV, sizeof(my_HardStr));
#else // (DEV_SERVICES & SERVICE_SCREEN)
	cfg.hw_ver = DEVICE_TYPE;
#endif // (DEV_SERVICES & SERVICE_SCREEN)
	return;
#elif DEVICE_TYPE == DEVICE_CGG1
#if DEVICE_CGG1_ver == 2022
	cfg.hw_ver = HW_VER_CGG1_2022;
#else
	cfg.hw_ver = HW_VER_CGG1;
#endif // DEVICE_CGG1_ver
#else
	cfg.hw_ver = DEVICE_TYPE;
#endif
}

// go deep-sleep 
void go_sleep(u32 tik) {
#if USE_SENSOR_HX71X && (DEV_SERVICES & SERVICE_PRESSURE)
	hx711_go_sleep();
#endif
#if	(DEV_SERVICES & RDS)
	rds1_input_off();
#endif
#if USE_SENSOR_SHTC3
	send_i2c_word(0x70 << 1, 0x98b0); // SHTC3 go SLEEP: Sleep command of the sensor
#endif // USE_SENSOR_SHTC3
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
	if (cfg.event_adv_cnt < 6) {
		cfg.event_adv_cnt = 6;
	}
	if (cfg.flg.tx_measures)
		wrk.tx_measures = 0xff; // always notify
	if (cfg.advertising_interval == 0) // 0 ?
		cfg.advertising_interval = 1; // 1*62.5 = 62.5 ms
	else if (cfg.advertising_interval > 160) // max 160 : 160*62.5 = 10000 ms
		cfg.advertising_interval = 160; // 160*62.5 = 10000 ms
	wrk.adv_interval_delay = cfg.flg3.adv_interval_delay;
	wrk.adv_interval = cfg.advertising_interval * 100; // Tadv_interval = adv_interval * 62.5 ms , adv_interval in 0.625 ms

	// measurement_step_time = adv_interval * 62.5 * measure_interval, max 250 sec
	if (cfg.measure_interval < 2)
		cfg.measure_interval = 2; // T = cfg.measure_interval * advertising_interval_ms (ms),  Tmin = 1 * 1*62.5 = 62.5 ms / 1 * 160 * 62.5 = 10000 ms
#if USE_SENSOR_SCD41
	wrk.measurement_step_time = wrk.adv_interval * (625 * sys_tick_per_us) - 256;
#else
	wrk.measurement_step_time = wrk.adv_interval * (u32)cfg.measure_interval;
	// test overflow 250 sec
	u32 tmp = 400000 - wrk.adv_interval; // 250000000us/6250=400000
	if(wrk.measurement_step_time > tmp) {
		cfg.measure_interval = tmp / wrk.adv_interval;
		wrk.measurement_step_time = wrk.adv_interval * (u32)cfg.measure_interval;
	}
	wrk.measurement_step_time *= (625 * sys_tick_per_us);
	wrk.measurement_step_time -= 256; // us
#endif
	if(cfg.connect_latency > DEF_CONNECT_LATENCY
#if USE_AVERAGE_BATTERY
			&& measured_data.average_battery_mv < LOW_VBAT_MV)
#else
			&& measured_data.battery_mv < LOW_VBAT_MV)
#endif
		cfg.connect_latency = DEF_CONNECT_LATENCY;
	/* interval = 16;
	 * connection_interval_ms = (interval * 125) / 100;
	 * connection_latency_ms = (cfg.connect_latency + 1) * connection_interval_ms = (16*125/100)*(99+1) = 2000;
	 * connection_timeout_ms = connection_latency_ms * 4 = 2000 * 4 = 8000;
	 */
	wrk.connection_timeout = ((cfg.connect_latency + 1) * (4 * DEF_CON_INERVAL * 125)) / 1000; // = 800, default = 8 sec
	if (wrk.connection_timeout > 32 * 100)
		wrk.connection_timeout = 32 * 100; //x10 ms, max 32 sec?
	else if (wrk.connection_timeout < 100)
		wrk.connection_timeout = 100;	//x10 ms,  1 sec

	if (!cfg.connect_latency) {
		my_periConnParameters.intervalMin =	(cfg.advertising_interval * 625	/ 30) - 1; // Tmin = 20*1.25 = 25 ms, Tmax = 3333*1.25 = 4166.25 ms
		my_periConnParameters.intervalMax = my_periConnParameters.intervalMin + 5;
		my_periConnParameters.latency = 0;
	} else {
		my_periConnParameters.intervalMin = DEF_CON_INERVAL; // 16*1.25 = 20 ms
		my_periConnParameters.intervalMax = DEF_CON_INERVAL; // 16*1.25 = 20 ms
		my_periConnParameters.latency = cfg.connect_latency;
	}
	my_periConnParameters.timeout = wrk.connection_timeout;
#if (DEV_SERVICES & SERVICE_SCREEN)
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
#else
#if	USE_EPD
	if (cfg.min_step_time_update_lcd < USE_EPD) // min 0.5 sec: (10*50ms)
		cfg.min_step_time_update_lcd = USE_EPD;
#endif
	if (cfg.min_step_time_update_lcd < 10) // min 0.5 sec: (10*50ms)
		cfg.min_step_time_update_lcd = 10;
	lcd_flg.min_step_time_update_lcd = cfg.min_step_time_update_lcd * (50 * CLOCK_16M_SYS_TIMER_CLK_1MS);
#endif
#endif // (DEV_SERVICES & SERVICE_SCREEN)
	set_hw_version();
	my_RxTx_Data[0] = CMD_ID_CFG;
	my_RxTx_Data[1] = VERSION;
	memcpy(&my_RxTx_Data[2], &cfg, sizeof(cfg));
}

void low_vbat(void) {
#if (DEVICE_TYPE != DEVICE_MJWSD06MMC)
#if (DEV_SERVICES & SERVICE_SCREEN)
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
	show_low_bat();
#else
#if (USE_EPD)
	while(task_lcd()) pm_wait_ms(10);
#endif
	show_temp_symbol(0);
#if	(SHOW_SMILEY)
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
#endif // USE_EPD
#endif // DEVICE_TYPE == DEVICE_MJWSD05MMC
#endif // (DEV_SERVICES & SERVICE_SCREEN)
#else
	send_i2c_byte(0x3E << 1, 0xEA); // BU9792AFUV reset
	send_i2c_byte(0x3E << 1, 0xD0);
#endif
	go_sleep(180u * CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 3 minutes
}

#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_18B20 | SERVICE_PLM))
#if SENSOR_SLEEP_MEASURE
_attribute_ram_code_
void WakeupLowPowerCb(int par) {
	(void) par;
	if (sensor_cfg.time_measure) {
#else
_attribute_ram_code_
void read_sensors(void) {
#endif
#if (DEV_SERVICES & SERVICE_RDS)
		rds_input_on();
#endif
#if (defined(CHL_ADC1) || defined(CHL_ADC1))
		if (1)
#else
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_PLM))
#if (DEV_SERVICES & SERVICE_18B20)
		if (read_sensor_cb() && my18b20.rd_ok)
#else
		if (read_sensor_cb())
#endif
#elif (DEV_SERVICES & SERVICE_18B20)
		if (my18b20.rd_ok)
#endif
#endif // (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_PLM))
		{
#ifdef CHL_ADC1 // DIY version only!
			measured_data.temp = get_adc_mv(CHL_ADC1);
#endif
#ifdef CHL_ADC2 // DIY version only!
			measured_data.humi = get_adc_mv(CHL_ADC2);
#endif
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
#if USE_SENSOR_PWMRH == 2
			read_rh_sensor();
#endif
			measured_data.temp_x01 = (measured_data.temp + 5)/ 10;
			measured_data.humi_x01 = (measured_data.humi + 5)/ 10;
			measured_data.humi_x1 = (measured_data.humi + 50)/ 100;
#elif (DEV_SERVICES & SERVICE_18B20)
			measured_data.temp_x01 = (measured_data.xtemp[0] + 5)/ 10;
#if (USE_SENSOR_MY18B20 == 2)
			// TODO: measured_data.humi_x1 = (measured_data.xtemp[1] + 50)/ 100;
#endif
#endif
#if defined(USE_SENSOR_ENS160) && USE_SENSOR_ENS160
#if !USE_ENS160_INT
			if (!read_ens160())
#endif
				set_th_ens160(measured_data.temp, measured_data.humi);
#endif
#if USE_SDM_OUT
			set_dac();
#endif
#if (DEV_SERVICES & SERVICE_TH_TRG)
			set_trigger_out();
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
			if (wrk.ota_is_working == 0 && cfg.averaging_measurements) {
				write_memo();
			}
#endif
#if (DEV_SERVICES & SERVICE_BINDKEY) && USE_MIHOME_BEACON
			if ((cfg.flg.advertising_type == ADV_TYPE_MI) && cfg.flg2.adv_crypto)
				mi_beacon_summ();
#endif
			wrk.msc.all_flgs = 0xff;
		}
#if (DEV_SERVICES & SERVICE_RDS)
		if (trg.rds.type1 == RDS_NONE) {
			trg.flg.rds1_input = get_rds1_input();
			rds1_input_off();
		}
#ifdef GPIO_RDS2
		if (trg.rds.type2 == RDS_NONE) {
			trg.flg.rds2_input = get_rds2_input();
			rds2_input_off();
		}
#endif
#endif // (DEV_SERVICES & SERVICE_RDS)
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
		SET_LCD_UPDATE();
#endif
#if SENSOR_SLEEP_MEASURE
		sensor_cfg.time_measure = 0;
	}
	bls_pm_setAppWakeupLowPower(0, 0); // clear callback
#endif
}
#endif // (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_18B20 | SERVICE_PLM))

_attribute_ram_code_
static void suspend_exit_cb(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
	rf_set_power_level_index(cfg.rf_tx_power);
}

#if !defined(SET_NO_SLEEP_MODE) && ( DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)   || (USE_SENSOR_HX71X && SENSOR_HX71X_WAKEAP)
_attribute_ram_code_
static void suspend_enter_cb(u8 e, u8 *p, int n) {
	(void) e; (void) p; (void) n;
#if (DEV_SERVICES & SERVICE_RDS)
	cpu_set_gpio_wakeup(GPIO_RDS1, BM_IS_SET(reg_gpio_in(GPIO_RDS1), GPIO_RDS1 & 0xff)? Level_Low : Level_High, trg.rds.type1 != RDS_NONE);  // pad wakeup deepsleep enable
#ifdef GPIO_RDS2
	cpu_set_gpio_wakeup(GPIO_RDS2, BM_IS_SET(reg_gpio_in(GPIO_RDS2), GPIO_RDS2 & 0xff)? Level_Low : Level_High, trg.rds.type2 != RDS_NONE);  // pad wakeup deepsleep enable
#endif
#endif
#if (USE_SENSOR_HX71X && SENSOR_HX71X_WAKEAP)
	cpu_set_gpio_wakeup(GPIO_HX71X_DOUT, Level_Low, wrk.ble_connected == 0);  // pad wakeup deepsleep enable
#endif
#if USE_ENS160_INT
	cpu_set_gpio_wakeup(GPIO_ENS160_INT, Level_High, ens160.mode == ENS160_MODE_STANDARD);  // pad wakeup deepsleep enable
#endif
#if (DEV_SERVICES & SERVICE_KEY)
	cpu_set_gpio_wakeup(GPIO_KEY2, BM_IS_SET(reg_gpio_in(GPIO_KEY2), GPIO_KEY2 & 0xff)? Level_Low : Level_High, 1);  // pad wakeup deepsleep enable
#endif
	bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
}
#endif // (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)

#if USE_AVERAGE_BATTERY
//--- check battery
#define BAT_AVERAGE_COUNT_SHL	9 // 4,5,6,7,8,9,10,11,12 -> 16,32,64,128,256,512,1024,2048,4096
RAM struct {
	u32	buf_sum;
	u32	count;
} bat_average;
#endif

_attribute_ram_code_
__attribute__((optimize("-Os")))
void check_battery(void) {
	measured_data.battery_mv = get_battery_mv();
	if (measured_data.battery_mv < END_VBAT_MV) // It is not recommended to write Flash below 2V
		low_vbat();
#if USE_AVERAGE_BATTERY
	bat_average.buf_sum += measured_data.battery_mv;
	bat_average.count++;
	if(bat_average.count >= (1<<BAT_AVERAGE_COUNT_SHL)) {
		measured_data.average_battery_mv = bat_average.buf_sum >> BAT_AVERAGE_COUNT_SHL;
		bat_average.buf_sum -= measured_data.average_battery_mv;
		bat_average.count--;
	} else {
		measured_data.average_battery_mv = bat_average.buf_sum / bat_average.count;
	}
	measured_data.battery_level = get_battery_level(measured_data.average_battery_mv);
#else
	measured_data.battery_level = get_battery_level(measured_data.battery_mv);
#endif
}

__attribute__((optimize("-Os")))
static void start_tst_battery(void) {
	measured_data.battery_mv = get_battery_mv();
	if (measured_data.battery_mv < MIN_VBAT_MV) { // 2.2V
#if (DEVICE_TYPE ==	DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_CGDK2) || (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MHO_C122) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN) || (DEVICE_TYPE == DEVICE_MJWSD06MMC)
		// Set sleep power < 1 uA
		send_i2c_byte(0x3E << 1, 0xEA); // BU9792AFUV reset
#if (DEVICE_TYPE == DEVICE_MJWSD06MMC)
		send_i2c_byte(0x3E << 1, 0xD0);
#endif
#elif (DEVICE_TYPE == DEVICE_ZTH03) || (DEVICE_TYPE == DEVICE_LKTMZL02)
extern int soft_i2c_send_byte(u8 addr, u8 b);
		soft_i2c_send_byte(0x3E << 1, 0xD0);
#endif
		go_sleep(180u * CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 3 minutes
	}
#if USE_AVERAGE_BATTERY
	bat_average.buf_sum = measured_data.battery_mv;
	measured_data.average_battery_mv = measured_data.battery_mv;
	bat_average.count = 1;
#endif
	measured_data.battery_level = get_battery_level(measured_data.battery_mv);
}


#if (DEV_SERVICES & SERVICE_BINDKEY)
void bindkey_init(void) {
	if (flash_read_cfg(bindkey, EEP_ID_KEY, sizeof(bindkey))
			!= sizeof(bindkey)) {
		generateRandomNum(sizeof(bindkey), bindkey);
		flash_write_cfg(bindkey, EEP_ID_KEY, sizeof(bindkey));
	}
#if	USE_MIHOME_BEACON
	mi_beacon_init();
#endif // USE_MIHOME_BEACON
#if USE_BTHOME_BEACON
	bthome_beacon_init();
#endif
}
#endif // #if (DEV_SERVICES & SERVICE_BINDKEY)

#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)
void set_default_cfg(void) {
	memcpy(&cfg, &def_cfg, sizeof(cfg));
	test_config();
	flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
	SHOW_REBOOT_SCREEN();
	go_sleep(2*CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 2 sec
}
#endif // (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)

//=========================================================
//-------------------- user_init_normal -------------------
void user_init_normal(void) {//this will get executed one time after power up
	bool next_start = false;
	unsigned int old_ver;
	blc_ll_initBasicMCU(); //must
	adc_power_on_sar_adc(0); // - 0.4 mA
	lpc_power_down();
	start_tst_battery();
	flash_unlock();
	random_generator_init(); //must
#if !ZIGBEE_TUYA_OTA // USE_EXT_OTA
	big_to_low_ota(); // Correct FW OTA address? Reformat Big OTA to Low OTA
#endif
#if defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)
	u32 hw_ver = get_mi_hw_version();
#endif // (DEVICE_TYPE == DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC)
#if USE_SENSOR_HX71X && (DEV_SERVICES & SERVICE_PRESSURE)
	hx711_gpio_wakeup();
#endif
	// Read config
	if(flash_read_cfg(&old_ver, EEP_ID_VER, sizeof(old_ver)) != sizeof(old_ver))
		old_ver = 0;
	next_start = flash_supported_eep_ver(EEP_SUP_VER, VERSION);
	if (next_start) {
		if (flash_read_cfg(&cfg, EEP_ID_CFG, sizeof(cfg)) != sizeof(cfg))
			memcpy(&cfg, &def_cfg, sizeof(cfg));
#if (DEV_SERVICES & SERVICE_SCREEN)
		if (flash_read_cfg(&cmf, EEP_ID_CMF, sizeof(cmf)) != sizeof(cmf))
			memcpy(&cmf, &def_cmf, sizeof(cmf));
#endif
#if (DEV_SERVICES & SERVICE_TIME_ADJUST)
		if (flash_read_cfg(&wrk.utc_time_tick_step, EEP_ID_TIM,
				sizeof(wrk.utc_time_tick_step)) != sizeof(wrk.utc_time_tick_step))
#endif
			wrk.utc_time_tick_step = CLOCK_16M_SYS_TIMER_CLK_1S;
#if (DEV_SERVICES & SERVICE_PINCODE)
		if (flash_read_cfg(&pincode, EEP_ID_PCD, sizeof(pincode))
				!= sizeof(pincode))
			pincode = 0;
#endif
#if (DEV_SERVICES & SERVICE_TH_TRG)
		if (flash_read_cfg(&trg, EEP_ID_TRG, FEEP_SAVE_SIZE_TRG)
				!= FEEP_SAVE_SIZE_TRG)
			memcpy(&trg, &def_trg, FEEP_SAVE_SIZE_TRG);
#endif
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_PLM))
		if (flash_read_cfg(&sensor_cfg.coef, EEP_ID_CFS, sizeof(sensor_cfg.coef))
				!= sizeof(sensor_cfg.coef))
			memset(&sensor_cfg.coef, 0, sizeof(sensor_cfg.coef));
#endif
#if (DEV_SERVICES & SERVICE_18B20)
		if (flash_read_cfg(&my18b20.coef, EEP_ID_CMY, sizeof(my18b20.coef))
				!= sizeof(my18b20.coef))
			memset(&my18b20.coef, 0, sizeof(my18b20.coef));
#endif
#if (DEV_SERVICES & SERVICE_PRESSURE) && USE_SENSOR_HX71X
		if (flash_read_cfg(&hx71x.cfg, EEP_ID_HXC, sizeof(hx71x.cfg))
				!= sizeof(hx71x.cfg))
			memcpy(&hx71x.cfg, &def_hx71x_cfg, sizeof(hx71x.cfg));
#endif
		// if version < 4.2 -> clear cfg.flg2.longrange
		if (old_ver <= 0x41) {
			cfg.flg2.longrange = 0;
			flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
		}
	} else {
#if (DEV_SERVICES & SERVICE_PINCODE)
		pincode = 0;
#endif
		memcpy(&cfg, &def_cfg, sizeof(cfg));
#if (DEV_SERVICES & SERVICE_SCREEN)
		memcpy(&cmf, &def_cmf, sizeof(cmf));
#endif
#if (DEV_SERVICES & SERVICE_TH_TRG)
		memcpy(&trg, &def_trg, FEEP_SAVE_SIZE_TRG);
#endif
#if (DEV_SERVICES & SERVICE_PRESSURE) && USE_SENSOR_HX71X
		memcpy(&hx71x.cfg, &def_hx71x_cfg, sizeof(hx71x.cfg));
#endif
#if defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)
		if (hw_ver)
			flash_write_cfg(&hw_ver, EEP_ID_HWV, sizeof(hw_ver));
#endif
		wrk.utc_time_tick_step = CLOCK_16M_SYS_TIMER_CLK_1S;
	}
#if USE_SYNC_SCAN
	scan_init();
#endif
#if USE_SDM_OUT
	init_dac();
#endif
#if (DEV_SERVICES & SERVICE_RDS)
	rds_init();
#endif
#if (DEV_SERVICES & SERVICE_18B20)
	init_my18b20();
#endif
#ifdef I2C_GROUP
	init_i2c();
	reg_i2c_speed = (u8)(CLOCK_SYS_CLOCK_HZ/(4*100000)); // 100 kHz
#endif
	test_config();
#if (POWERUP_SCREEN) || (DEV_SERVICES & SERVICE_HARD_CLOCK) || (DEV_SERVICES & SERVICE_LE_LR)
	if(analog_read(DEEP_ANA_REG0) != 0x55) {
#if (DEV_SERVICES & SERVICE_LE_LR)
		cfg.flg2.longrange = 0;
		flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
#endif // #if (DEV_SERVICES & SERVICE_LE_LR)
		analog_write(DEEP_ANA_REG0, 0x55);
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
#if POWERUP_SCREEN
		init_lcd();
		SHOW_REBOOT_SCREEN();
#endif // POWERUP_SCREEN
		// RTC wakes up after powering on > 1 second.
		go_sleep(1500*CLOCK_16M_SYS_TIMER_CLK_1MS);  // go deep-sleep 1.5 sec
#endif // SHOW_REBOOT_SCREEN || (DEV_SERVICES & SERVICE_HARD_CLOCK)
	}
#endif // POWERUP_SCREEN || (DEV_SERVICES & SERVICE_HARD_CLOCK) || (DEV_SERVICES & SERVICE_LE_LR)
#if (DEV_SERVICES & SERVICE_SCREEN)
	memcpy(&ext, &def_ext, sizeof(ext));
#endif
	init_ble();
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &suspend_exit_cb);
#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS) || (USE_SENSOR_HX71X)
#if !defined(SET_NO_SLEEP_MODE)
	bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_ENTER, &suspend_enter_cb);
#endif
#endif
#if defined(USE_SENSOR_ENS160) && USE_SENSOR_ENS160
	//pm_wait_ms(50);
	init_ens160();
#endif
	// start_tst_battery(); // step 2
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_PLM))
	init_sensor();
#endif
#if (DEV_SERVICES & SERVICE_PLM) && (USE_SENSOR_PWMRH == 2)
	init_rh_sensor();
#endif
#if USE_SENSOR_HX71X && (DEV_SERVICES & SERVICE_PRESSURE)
	hx71x_get_data(HX71XMODE_A128); // Start measure
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
	memo_init();
#endif
#if	(DEV_SERVICES & SERVICE_HARD_CLOCK)
	init_rtc();
#endif
	init_lcd();
	set_hw_version();
#if defined(GPIO_ADC1) || defined(GPIO_ADC2)
	sensor_go_sleep();
#endif
#if (DEV_SERVICES & SERVICE_BINDKEY)
	bindkey_init();
#endif
	check_battery();
#if USE_SENSOR_HX71X && (DEV_SERVICES & SERVICE_PRESSURE)
	hx71x_task();
#endif
	lcd();
#if (!USE_EPD)
//	update_lcd();
#endif
	if (!next_start) { // first start?
		flash_write_cfg(&cfg, EEP_ID_CFG, sizeof(cfg));
	}
	test_config();
#if defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)
	set_SerialStr();
#endif

	wrk.start_measure = 1;

#if (DEV_SERVICES & SERVICE_18B20)
	task_my18b20();
#endif

	bls_pm_setManualLatency(0);
}

//=========================================================
//------------------ user_init_deepRetn -------------------
_attribute_ram_code_
void user_init_deepRetn(void) {//after sleep this will get executed
	blc_ll_initBasicMCU();
	rf_set_power_level_index(cfg.rf_tx_power);
	blc_ll_recoverDeepRetention();
	bls_ota_registerStartCmdCb(app_enter_ota_mode);
#if USE_SYNC_SCAN
	if(scan.enabled)
		scan_wakeup();
#endif
}

//=========================================================
//----------------------- main_loop() ---------------------
_attribute_ram_code_
void main_loop(void) {
	blt_sdk_main_loop();
	while (clock_time() -  wrk.utc_time_sec_tick > wrk.utc_time_tick_step) {
		wrk.utc_time_sec_tick += wrk.utc_time_tick_step;
		wrk.utc_time_sec++; // + 1 sec
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
		if(++rtc.seconds >= 60) {
			rtc.seconds = 0;
			if(++rtc.minutes >= 60) {
				rtc.minutes = 0;
				rtc_sync_utime = wrk.utc_time_sec;
			}
			SET_LCD_UPDATE();
		}
#endif
	}
#if SENSOR_SLEEP_MEASURE
	if(sensor_cfg.time_measure && (clock_time() - sensor_cfg.time_measure > sensor_cfg.measure_timeout))
		WakeupLowPowerCb(0);
#endif
#if (DEV_SERVICES & SERVICE_RDS)
#ifndef GPIO_RDS2
		if(trg.rds.type1 != RDS_NONE) // rds: switch or counter
#endif
			rds_task();
#endif // (DEV_SERVICES & SERVICE_RDS)
#if USE_SENSOR_HX71X && (DEV_SERVICES & SERVICE_PRESSURE)
	hx71x_task();
#endif
#if USE_ENS160_INT
	if (gpio_read(GPIO_ENS160_INT))
		read_ens160();
#endif
	if (wrk.ota_is_working) {
#if (DEV_SERVICES & SERVICE_OTA_EXT)
		if(wrk.ota_is_working == OTA_EXTENDED) {
			bls_pm_setManualLatency(3);
			clear_ota_area();
		} else
#endif
		{
			if ((wrk.ble_connected & BIT(CONNECTED_FLG_PAR_UPDATE))==0)
				bls_pm_setManualLatency(0);
		}
#ifdef	SET_NO_SLEEP_MODE
		bls_pm_setSuspendMask(SET_NO_SLEEP_MODE);
#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
#endif
	} else {
#if (DEV_SERVICES & SERVICE_18B20)
		task_my18b20();
#endif
		u32 new = clock_time();
#if (DEV_SERVICES & SERVICE_KEY)
		if(!get_key2_pressed()) {
			if(!ext_key.key2pressed) {
				// key2 on
				ext_key.key2pressed = 1;
				ext_key.key_pressed_tik1 = new;
				ext_key.key_pressed_tik2 = new;
				set_adv_con_time(0); // set connection adv.
				SET_LCD_UPDATE();
#if (DEV_SERVICES & SERVICE_RDS) || (DEV_SERVICES & SERVICE_TH_TRG)
				trg.flg.key_pressed = 1;
#endif
#if (DEV_SERVICES & SERVICE_LED)
				// led on
				gpio_is_output_en(GPIO_LED);
#if LED_ON
				gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLUP_10K);
#else
				gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLDOWN_100K);
#endif // LED_ON
#endif // (DEV_SERVICES & SERVICE_LED)
			}
			else {
				if(new - ext_key.key_pressed_tik1 > 1750*CLOCK_16M_SYS_TIMER_CLK_1MS) {
					ext_key.key_pressed_tik1 = new;
#if (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
					if(++cfg.flg2.screen_type > SCR_TYPE_EXT)
						cfg.flg2.screen_type = SCR_TYPE_TIME;
#elif (DEV_SERVICES & SERVICE_SCREEN)
					cfg.flg.temp_F_or_C ^= 1;
#endif
					if(ext_key.rest_adv_int_tad) {
						set_adv_con_time(1); // restore default adv.
						ext_key.rest_adv_int_tad = 0;
					}
					SET_LCD_UPDATE();
#if (DEV_SERVICES & SERVICE_LED)
					// led off
#if LED_ON
					gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLDOWN_100K);
#else
					gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLUP_1M);
#endif // LED_ON
#endif // (DEV_SERVICES & SERVICE_LED)
				}
#ifdef GPIO_KEY1
				if(new - ext_key.key_pressed_tik2 > 5*CLOCK_16M_SYS_TIMER_CLK_1S) {
					if((reg_gpio_in(GPIO_KEY1) & (GPIO_KEY1 & 0xff))==0)
#else
				if(new - ext_key.key_pressed_tik2 > 7*CLOCK_16M_SYS_TIMER_CLK_1S) {
#endif // GPIO_KEY1
						set_default_cfg();
#if (DEV_SERVICES & SERVICE_RDS) || (DEV_SERVICES & SERVICE_TH_TRG)
					trg.flg.key_pressed = 0;
#endif
#if (DEV_SERVICES & SERVICE_LED)
					// led on
					gpio_is_output_en(GPIO_LED);
#if LED_ON
					gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLUP_10K);
#else
					gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLDOWN_100K);
#endif // LED_ON
#endif
				}
			}
		}
		else {
			// key2 off
			ext_key.key2pressed = 0;
			ext_key.key_pressed_tik1 = new;
			ext_key.key_pressed_tik2 = new;
#if (DEV_SERVICES & SERVICE_LED)
			// led off
#if LED_ON
			gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLDOWN_100K);
#else
			gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLUP_1M);
#endif
#endif
		}
#endif // (DEV_SERVICES & SERVICE_KEY)
#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)
		if(ext_key.rest_adv_int_tad < -80) {
			set_adv_con_time(1); // restore default adv.
			SET_LCD_UPDATE();
		}
#endif
#if SENSOR_SLEEP_MEASURE
		if (wrk.start_measure
			&& sensor_cfg.time_measure == 0
//			&& bls_pm_getSystemWakeupTick() - new > sensor_cfg.measure_timeout + 5*CLOCK_16M_SYS_TIMER_CLK_1MS // есть время на замер ?
			) {

			wrk.start_measure = 0;
			bls_pm_setSuspendMask(SUSPEND_DISABLE);

#if defined(GPIO_ADC1) || defined(GPIO_ADC2)
			check_battery();
			WakeupLowPowerCb(0);
#else
			check_battery();
			start_measure_sensor_deep_sleep();
			sensor_cfg.time_measure = clock_time() | 1;
#if (DEV_SERVICES & SERVICE_PRESSURE) && USE_SENSOR_HX71X
			measured_data.pressure = hx71x_get_volume();
#endif
			if(cfg.flg.lp_measures == 0
#if USE_SENSOR_SHTC3
				|| sensor_cfg.sensor_type == TH_SENSOR_SHTC3
#endif
				) {
				if(clock_time() - sensor_cfg.time_measure > sensor_cfg.measure_timeout - 3)
					WakeupLowPowerCb(0);
				else {
					bls_pm_registerAppWakeupLowPowerCb(WakeupLowPowerCb);
					bls_pm_setAppWakeupLowPower(sensor_cfg.time_measure + sensor_cfg.measure_timeout, 1);
				}
			}
#endif
		} else
#else // ! SENSOR_SLEEP_MEASURE
#if USE_SENSOR_SHTC3
#error "SHTC3: set SENSOR_SLEEP_MEASURE!"
#endif
		if (wrk.start_measure) {
			wrk.start_measure = 0;
			check_battery();
			read_sensors();
#if (DEV_SERVICES & SERVICE_THS) && (!USE_SENSOR_SHTC3) && !USE_SENSOR_SCD41
			start_measure_sensor_deep_sleep();
#endif
#if (DEV_SERVICES & SERVICE_PRESSURE)
			measured_data.pressure = hx71x_get_volume();
#endif
		} else
#endif
		{
			if (wrk.ble_connected && blc_ll_getTxFifoNumber() < 9) {
				// if wrk.ble_connected & TxFifo ready
				if (wrk.msc.b.send_measure) {
					wrk.msc.b.send_measure = 0;
					if (RxTxValueInCCC && wrk.tx_measures) {
						if (wrk.tx_measures != 0xff)
							wrk.tx_measures--;
						ble_send_measures();
					}
					if (batteryValueInCCC)
						ble_send_battery();
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_18B20 | SERVICE_PLM))
					if (tempValueInCCC)
						ble_send_temp01();
					if (temp2ValueInCCC)
						ble_send_temp001();
#endif
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
					if (humiValueInCCC)
						ble_send_humi();
#endif
#if (DEV_SERVICES & SERVICE_IUS)
					if (anaValueInCCC)
						ble_send_ana();
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
				} else if (rd_memo.cnt) {
					send_memo_blk();
#endif
#if (DEV_SERVICES & SERVICE_MI_KEYS)
				} else if (mi_key_stage) {
					mi_key_stage = get_mi_keys(mi_key_stage);
#endif
#if (DEV_SERVICES & SERVICE_SCREEN)
				} else if (RxTxValueInCCC) {
					if (lcd_flg.b.send_notify) {
						// LCD for send notify
						lcd_flg.b.send_notify = 0;
						ble_send_lcd();
					}
#endif
				}
			}
#if (DEV_SERVICES & SERVICE_HARD_CLOCK)
			else if(rtc_sync_utime) {
				rtc_sync_utime = 0;
				wrk.utc_time_sec = rtc_get_utime();
			}
#endif // (DEV_SERVICES & SERVICE_HARD_CLOCK)
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_IUS | SERVICE_18B20 | SERVICE_PLM))
#if SENSOR_SLEEP_MEASURE
			if(sensor_cfg.time_measure == 0)
#endif
			{
				if(wrk.ble_connected) {
					if (new - wrk.tim_measure >= wrk.measurement_step_time) {
						wrk.tim_measure = new;
						adv_buf.meas_count = 0; // counter of advertising broadcasts until the start of the next measurement
						wrk.start_measure = 1;
					}
				}
			}
#endif
#if (DEV_SERVICES & SERVICE_SCREEN)
			if(!cfg.flg2.screen_off) {
#if (USE_EPD)
				if ((!stage_lcd) && (new - lcd_flg.tim_last_chow >= lcd_flg.min_step_time_update_lcd)) {
					lcd_flg.tim_last_chow = new;
					lcd_flg.show_stage++;
					if(lcd_flg.update_next_measure) {
						lcd_flg.update = wrk.msc.b.update_lcd;
						wrk.msc.b.update_lcd = 0;
					} else
						lcd_flg.update = 1;
				}
#elif !((DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN))
				if (new - lcd_flg.tim_last_chow >= lcd_flg.min_step_time_update_lcd) {
					lcd_flg.tim_last_chow = new;
					lcd_flg.show_stage++;
					if(lcd_flg.update_next_measure) {
						lcd_flg.update = wrk.msc.b.update_lcd;
						wrk.msc.b.update_lcd = 0;
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
#endif // #if (DEV_SERVICES & SERVICE_SCREEN)
#if USE_SYNC_SCAN
			if (scan.cfg.interval
			&& wrk.utc_time_sec - scan.start_time > scan.cfg.interval
			&& adv_buf.ext_adv_init == EXT_ADV_Off // not support extension advertise
			&& !wrk.ble_connected
			&& !blta.adv_duraton_en) {
				scan.start_time = wrk.utc_time_sec;
				scan_start();
			}
#endif
		}
#if USE_SYNC_SCAN
		if(scan.start_tik) {
			scan_task();
		} else
#endif
#ifdef	SET_NO_SLEEP_MODE
		bls_pm_setSuspendMask(SET_NO_SLEEP_MODE);
#else
		bls_pm_setSuspendMask(
				SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
#endif
	}
#if (DEV_SERVICES & SERVICE_SCREEN)
#if (USE_EPD)
	if (stage_lcd) {
		if (task_lcd()) {
			if(!gpio_read(EPD_BUSY)) {
				cpu_set_gpio_wakeup(EPD_BUSY, Level_High, 1);  // pad high wakeup deepsleep enable
#if !((DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS) || (USE_SENSOR_HX71X))
				bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
#endif
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
#endif // (DEV_SERVICES & SERVICE_SCREEN)
}
