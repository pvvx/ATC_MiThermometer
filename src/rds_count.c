/*
 * rds_count.c
 *
 *  Created on: 20.02.2022
 *      Author: pvvx
 */

#include "tl_common.h"
#if (DEV_SERVICES & SERVICE_RDS)
#include "stack/ble/ble.h"
#include "app.h"
#include "ble.h"
#include "drivers.h"
#include "sensor.h"
#include "trigger.h"
#include "ble.h"
#include "lcd.h"
#include "custom_beacon.h"
#if USE_MIHOME_BEACON
#include "mi_beacon.h"
#endif
#if USE_BTHOME_BEACON
#include "bthome_beacon.h"
#endif
#include "rds_count.h"
#if (USE_SENSOR_HX71X)
#include "hx71x.h"
#endif

#if (DEV_SERVICES & SERVICE_LE_LR)
extern u32 blt_advExpectTime;
extern u8 app_adv_set_param[];
#endif

RAM	rds_count_t rds;		// Reed switch pulse counter

_attribute_ram_code_
void rds_input_off(void) {
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
}


void rds_init(void) {
	if (trg.rds.type1) {
#if (DEV_SERVICES & SERVICE_KEY)  // defined GPIO_KEY2
		if(trg.rds.type1 == RDS_CONNECT) {
			trg.rds.type1 = RDS_SWITCH;
		}
#else
		if(trg.rds.type1 == RDS_CONNECT)
			trg.rds.rs1_invert = 1;
#endif
		rds1_input_on();
	} else
		cpu_set_gpio_wakeup(GPIO_RDS1, Level_Low, 0);  // pad wakeup deepsleep disable
#ifdef GPIO_RDS2
	if (trg.rds.type2) {
#if (DEV_SERVICES & SERVICE_KEY)  // defined GPIO_KEY2
		if(trg.rds.type2 == RDS_CONNECT) {
			trg.rds.type2 = RDS_SWITCH;
		}
#else
		if(trg.rds.type2 == RDS_CONNECT)
			trg.rds.rs2_invert = 1;
#endif
		rds2_input_on();
	} else
		cpu_set_gpio_wakeup(GPIO_RDS2, Level_Low, 0);  // pad wakeup deepsleep disable
#endif
#if USE_WK_RDS_COUNTER32 // save 32 bits?
	if ((flash_read_cfg(&rds.count_short[1], EEP_ID_RPC, sizeof(rds.count_short[1])) != sizeof(rds.count_short[1])) {
		rds.count = 0;
	}
#endif
	rds.report_tick = wrk.utc_time_sec;
}

//_attribute_ram_code_
__attribute__((optimize("-Os")))
static void set_rds_adv_data(void) {
	adv_buf.send_count++;
#if (USE_CUSTOM_BEACON + USE_BTHOME_BEACON + USE_MIHOME_BEACON + USE_ATC_BEACON) > 1
	int advertising_type = cfg.flg.advertising_type;
#endif
#if (DEV_SERVICES & SERVICE_BINDKEY)
	if (cfg.flg2.adv_crypto) {
#if (USE_CUSTOM_BEACON + USE_BTHOME_BEACON + USE_MIHOME_BEACON + USE_ATC_BEACON) > 1
#if USE_CUSTOM_BEACON
		if (advertising_type == ADV_TYPE_PVVX)
			pvvx_encrypt_event_beacon(rds.event);
		else
#endif
#if USE_BTHOME_BEACON
		if (advertising_type == ADV_TYPE_BTHOME)
			bthome_encrypt_event_beacon(rds.event);
		else
#endif
#if USE_MIHOME_BEACON
		if (advertising_type == ADV_TYPE_MI)
			mi_encrypt_event_beacon(rds.event);
		else
#endif
#if USE_ATC_BEACON
		if (advertising_type == ADV_TYPE_ATC)
			default_event_beacon();
		else
#endif
		{}
#else
		bthome_encrypt_event_beacon(rds.event);
#endif // (USE_CUSTOM_BEACON + USE_BTHOME_BEACON + USE_MIHOME_BEACON + USE_ATC_BEACON) > 1
	} else
#endif //	#if (DEV_SERVICES & SERVICE_BINDKEY)
	{
#if (USE_CUSTOM_BEACON + USE_BTHOME_BEACON + USE_MIHOME_BEACON + USE_ATC_BEACON) > 1
#if USE_CUSTOM_BEACON
		if (advertising_type == ADV_TYPE_PVVX)
			pvvx_event_beacon(rds.event);
		else
#endif
#if USE_MIHOME_BEACON
		if (advertising_type == ADV_TYPE_MI)
			mi_event_beacon(rds.event);
		else
#endif
#if USE_BTHOME_BEACON
		if (advertising_type == ADV_TYPE_BTHOME)
			bthome_event_beacon(rds.event);
		else
#endif
#if USE_ATC_BEACON
		if (advertising_type == ADV_TYPE_ATC)
			default_event_beacon();
		else
#endif
		{}
#else
		bthome_event_beacon(rds.event);
#endif // (USE_CUSTOM_BEACON + USE_BTHOME_BEACON + USE_MIHOME_BEACON + USE_ATC_BEACON) > 1
	}
	adv_buf.update_count = 0; // refresh adv_buf.data in next set_adv_data()
	load_adv_data();
}

//_attribute_ram_code_
static void start_ext_adv(void) {
#if (DEV_SERVICES & SERVICE_LE_LR)
	if (adv_buf.ext_adv_init != EXT_ADV_Off) { // support extension advertise
		set_rds_adv_data();
		blta.adv_duraton_en = EXT_ADV_COUNT;
		adv_buf.data_size = 0; // flag adv_buf.send_count++ over adv.event
		ll_ext_adv_t *p = (ll_ext_adv_t *)&app_adv_set_param;
		// patch: set time next ext.adv = 0
		blt_advExpectTime = clock_time() + 250*CLOCK_16M_SYS_TIMER_CLK_1US; // set time next ext.adv
		p->adv_event_tick = blt_advExpectTime;
		p->advInt_use = EXT_ADV_INTERVAL; // new adv. interval
	} else
#endif
	{
		bls_ll_setAdvEnable(BLC_ADV_DISABLE);  // adv disable
		bls_ll_setAdvParam(EXT_ADV_INTERVAL, EXT_ADV_INTERVAL,
				ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL,
				BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
		set_rds_adv_data();
		adv_buf.data_size = 0; // flag adv_buf.send_count++ over adv.event
		bls_ll_setAdvDuration(EXT_ADV_INTERVAL*EXT_ADV_COUNT*625+33, 1);
		blta.adv_interval = 0; // system tick
		bls_ll_setAdvEnable(BLC_ADV_ENABLE);  // adv enable
	}
}

/* if (trg.rds.type1) //  switch or counter [or connect] */
_attribute_ram_code_
__attribute__((optimize("-Os")))
void rds_task(void) {
#ifdef GPIO_RDS2
	if(trg.rds.type2 != RDS_NONE) { // rds: switch or counter
		if (get_rds2_input()) {
			if (!trg.flg.rds2_input) {
				// RDS2 on event
				trg.flg.rds2_input = 1;
				if (trg.rds.type2 == RDS_SWITCH) { // switch mode, send event if on
					rds.event = trg.rds.type2;
				}
			}
		} else {
			if (trg.flg.rds2_input) {
				// RDS2 off event, empty tank
				trg.flg.rds2_input = 0;
				if(!trg.flg.rds2_input) {
/*
					if (trg.rds.type2 == RDS_COUNTER) { // counter mode
						if ((rds.count2 & 0xffff) == 0) { // report 'overflow 16 bit count'
							rds.event = trg.rds.type2;
						}
					} else
*/
						if (trg.rds.type2 == RDS_SWITCH) { // switch mode
						rds.event = trg.rds.type2;
					}
				}
			}
		}
	}
	if(trg.rds.type1 != RDS_NONE) { // rds: switch or counter
#endif // GPIO_RDS2
		if (get_rds1_input()) { // key on
			if (!trg.flg.rds1_input) {
				// RDS1 on event, keypress event
				trg.flg.rds1_input = 1;
#if (USE_SENSOR_HX71X) && (DEVICE_TYPE == DEVICE_TNK01)
				hx71x_calibration();
#endif
				if (trg.rds.type1 == RDS_SWITCH) { // switch mode
					rds.event = trg.rds.type1;
				}
#if !(DEV_SERVICES & SERVICE_KEY)
				else if (trg.rds.type1 == RDS_CONNECT) { // connect mode
					// connect keypress event
					u32 new = clock_time();
					ext_key.key_pressed_tik1 = new;
					ext_key.key_pressed_tik2 = new;
					set_adv_con_time(0); // set connection adv.
					SET_LCD_UPDATE();
				}
#endif
			}
#if !(DEV_SERVICES & SERVICE_KEY)
			else if (trg.rds.type1 == RDS_CONNECT) { // connect mode
				// connection key held
				u32 new = clock_time();
				if(new - ext_key.key_pressed_tik1 > 1750*CLOCK_16M_SYS_TIMER_CLK_1MS) {
					ext_key.key_pressed_tik1 = new;
					cfg.flg.temp_F_or_C ^= 1;
					if(ext_key.rest_adv_int_tad) {
						set_adv_con_time(1); // restore default adv.
						ext_key.rest_adv_int_tad = 0;
					}
					SET_LCD_UPDATE();
				}
				if(new - ext_key.key_pressed_tik2 > 20*CLOCK_16M_SYS_TIMER_CLK_1S) {
					set_default_cfg();
				}
			}
#endif
		} else { // key off
			if (trg.flg.rds1_input) {
				// RDS1 off event, key released event
				trg.flg.rds1_input = 0;
				rds.count1++;
#if USE_WK_RDS_COUNTER32 // save 32 bits?
				if (rds.count_short[0] == 0) {
					flash_write_cfg(&rds.count_short[1], EEP_ID_RPC, sizeof(rds.count_short[1]));
				}
#endif
				if (trg.rds.type1 == RDS_COUNTER) { // counter mode
					if ((rds.count1 & 0xffff) == 0) { // report 'overflow 16 bit count'
						rds.event = trg.rds.type1;
					}
				} else if (trg.rds.type1 == RDS_SWITCH) { // switch mode
					rds.event = trg.rds.type1;
				}
			}
#if !(DEV_SERVICES & SERVICE_KEY)
			if (trg.rds.type1 == RDS_CONNECT) { // connect mode
					// connection key released
					u32 new = clock_time();
					ext_key.key_pressed_tik1 = new;
					ext_key.key_pressed_tik2 = new;
/* in app.c: main_loop()
					if(ext_key.rest_adv_int_tad < -80) {
						set_adv_con_time(1); // restore default adv.
						SET_LCD_UPDATE();
					}
*/
			}
#endif
		}
#ifdef GPIO_RDS2
	}
#endif
	if (trg.rds_time_report
			&& wrk.utc_time_sec - rds.report_tick > trg.rds_time_report) {
		rds.event = trg.rds.type1;
		rds.report_tick = wrk.utc_time_sec;
	}
	if (rds.event != RDS_NONE) {
		if(wrk.ble_connected) {
			if (rds.event == RDS_SWITCH) { // switch mode
				if(blc_ll_getTxFifoNumber() < 9)
					ble_send_trg_flg();
			}
		} else {
			start_ext_adv();
		}
		rds.event = RDS_NONE;
	}
}

#endif // #if (DEV_SERVICES & SERVICE_RDS)
