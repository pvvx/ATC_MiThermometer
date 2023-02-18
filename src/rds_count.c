/*
 * rds_count.c
 *
 *  Created on: 20.02.2022
 *      Author: pvvx
 */

#include <stdint.h>
#include "tl_common.h"
#if	USE_TRIGGER_OUT && USE_WK_RDS_COUNTER
#include "stack/ble/ble.h"
#include "app.h"
#include "drivers.h"
#include "sensor.h"
#include "trigger.h"
#include "ble.h"
#include "custom_beacon.h"
#if USE_MIHOME_BEACON
#include "mi_beacon.h"
#endif
#if USE_HA_BLE_BEACON
#include "ha_ble_beacon.h"
#endif
#include "rds_count.h"

#if (BLE_EXT_ADV)
extern u32 blt_advExpectTime;
extern u8 app_adv_set_param[];
#endif

RAM	rds_count_t rds;		// Reed switch pulse counter

void rds_init(void) {
	if (rds.type)
		rds_input_on();
#if USE_WK_RDS_COUNTER32 // save 32 bits?
	if ((flash_read_cfg(&rds.count_short[1], EEP_ID_RPC, sizeof(rds.count_short[1])) != sizeof(rds.count_short[1])) {
		rds.count = 0;
	}
#endif
	rds.report_tick = utc_time_sec;
}

//_attribute_ram_code_
__attribute__((optimize("-Os")))
void set_rds_adv_data(void) {
	adv_buf.send_count++;
	int advertising_type = cfg.flg.advertising_type;
#if	USE_SECURITY_BEACON
	if (cfg.flg2.adv_crypto) {
		if (advertising_type == ADV_TYPE_PVVX) {
#if USE_HA_BLE_BEACON
			ha_ble_encrypt_event_beacon(rds.type);
#else
			pvvx_encrypt_event_beacon(rds.type);
#endif
#if USE_HA_BLE_BEACON
		} else if (advertising_type == ADV_TYPE_HA_BLE) {
			ha_ble_encrypt_event_beacon(rds.type);
#endif
#if USE_MIHOME_BEACON
		} else if (advertising_type == ADV_TYPE_MI)  {
			mi_encrypt_event_beacon(rds.type);
#endif
		} else {
#if USE_HA_BLE_BEACON
			ha_ble_encrypt_event_beacon(rds.type);
#else
			atc_encrypt_event_beacon();
#endif
		}
	} else
#endif //	USE_SECURITY_BEACON
	{
		if (advertising_type == ADV_TYPE_PVVX) {
#if USE_HA_BLE_BEACON
			ha_ble_event_beacon(rds.type);
#else
			pvvx_event_beacon(rds.type);
#endif
#if USE_HA_BLE_BEACON
		} else if (advertising_type == ADV_TYPE_HA_BLE) {
			ha_ble_event_beacon(rds.type);
#endif
#if USE_MIHOME_BEACON
		} else if (advertising_type == ADV_TYPE_MI)  {
			mi_event_beacon(rds.type);
#endif
		} else {
#if USE_HA_BLE_BEACON
			ha_ble_event_beacon(rds.type);
#else
			atc_event_beacon();
#endif
		}
	}
	adv_buf.update_count = 0; // refresh adv_buf.data in next set_adv_data()
#if (BLE_EXT_ADV)
	if (ext_adv_init) { // support extension advertise
		if (cfg.flg2.adv_flags)
			blc_ll_setExtAdvData(ADV_HANDLE0, DATA_OPER_COMPLETE, DATA_FRAGM_ALLOWED, adv_buf.data_size + sizeof(adv_buf.flag), (u8 *)&adv_buf.flag);
		else
			blc_ll_setExtAdvData(ADV_HANDLE0, DATA_OPER_COMPLETE, DATA_FRAGM_ALLOWED, adv_buf.data_size, (u8 *)&adv_buf.data);

	} else
#endif
	{
		if (cfg.flg2.adv_flags) {
			bls_ll_setAdvData((u8 *)&adv_buf.flag, adv_buf.data_size + sizeof(adv_buf.flag));
		} else {
			bls_ll_setAdvData((u8 *)&adv_buf.data, adv_buf.data_size);
		}
	}
}

//_attribute_ram_code_
static void start_ext_adv(void) {
#if (BLE_EXT_ADV)
	if (ext_adv_init) { // support extension advertise
		set_rds_adv_data();
		blta.adv_duraton_en = EXT_ADV_COUNT;
		adv_buf.data_size = 0; // flag adv_buf.send_count++
		ll_ext_adv_t *p = (ll_ext_adv_t *)&app_adv_set_param;
		// patch: set time next ext.adv = 0
		blt_advExpectTime = reg_system_tick + 250*CLOCK_16M_SYS_TIMER_CLK_1US; // set time next ext.adv
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
		adv_buf.data_size = 0; // flag adv_buf.send_count++
		bls_ll_setAdvDuration(EXT_ADV_INTERVAL*EXT_ADV_COUNT*625+33, 1);
		blta.adv_interval = 0; // system tick
		bls_ll_setAdvEnable(BLC_ADV_ENABLE);  // adv enable
	}
}

_attribute_ram_code_ void rds_suspend(void) {
	if (!ble_connected) {
		/* TODO: if connection mode, gpio wakeup throws errors in sdk libs!
		   Work options: bls_pm_setSuspendMask(SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN);
		   No DEEPSLEEP_RETENTION_CONN */
		cpu_set_gpio_wakeup(GPIO_RDS, BM_IS_SET(reg_gpio_in(GPIO_RDS), GPIO_RDS & 0xff)? Level_Low : Level_High, 1);  // pad wakeup deepsleep enable
		bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
	} else {
		cpu_set_gpio_wakeup(GPIO_RDS, Level_Low, 0);  // pad wakeup suspend/deepsleep disable
	}
}

/* if (rds.type) // rds.type: switch or counter */

_attribute_ram_code_ __attribute__((optimize("-Os")))
void rds_task(void) {
//	rds_input_on(); // in "app_config.h" and WakeupLowPowerCb()
	if (get_rds_input()) {
		if (!trg.flg.rds_input) {
			trg.flg.rds_input = 1;
			if (rds.type == RDS_SWITCH) { // switch mode
				rds.event = rds.type;
			}
		}
	} else {
		if (trg.flg.rds_input) {
			trg.flg.rds_input = 0;
			rds.count++;
#if USE_WK_RDS_COUNTER32 // save 32 bits?
			if (rds.count_short[0] == 0) {
				flash_write_cfg(&rds.count_short[1], EEP_ID_RPC, sizeof(rds.count_short[1]));
			}
#endif
			if (rds.type == RDS_COUNTER) { // counter mode
				if ((rds.count & 0xffff) == 0) { // report 'overflow 16 bit count'
					rds.event = rds.type;
				}
			} else if (rds.type == RDS_SWITCH) { // switch mode
				rds.event = rds.type;
			}
		}
	}
	if (trg.rds_time_report
		&& utc_time_sec - rds.report_tick > trg.rds_time_report) {
				rds.event = rds.type;
	}
	if ((!ble_connected) && rds.event) {
		rds.adv_counter++;
		start_ext_adv();
		rds.event = RDS_NONE;
		rds.report_tick = utc_time_sec;
	}
}

#endif // USE_TRIGGER_OUT
