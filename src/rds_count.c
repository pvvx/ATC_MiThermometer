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
#include "mi_beacon.h"
#include "rds_count.h"

RAM	rds_count_t rds;		// Reed switch pulse counter

void rds_init(void) {
	if (rds.type)
		rds_input_on();
#if USE_WK_RDS_COUNTER32 // save 32 bits?
	if ((flash_read_cfg(&rds.count_short[1], EEP_ID_RPC, sizeof(rds.count_short[1])) != sizeof(rds.count_short[1])) {
		rds.count = 0;
	}
#endif
	rds.count_byte[0] = analog_read(DEEP_ANA_REG0);
	rds.count_byte[1] = analog_read(DEEP_ANA_REG1);
	rds.report_tick = utc_time_sec;
}

__attribute__((optimize("-Os")))
_attribute_ram_code_ void set_rds_adv_data(void) {
	pext_adv_cnt_t v = (pext_adv_cnt_t)&adv_buf.data;
	adv_buf.data_size = 0;
	if (rds.type == RDS_SWITCH) {
		rds.adv_counter &= 0x1f;
		rds.adv_counter |= ((uint8_t)measured_data.count ^ 0xffffffe0) & 0xffffffe0;
		if ((cfg.flg.advertising_type == ADV_TYPE_PVVX) || (cfg.flg.advertising_type == ADV_TYPE_ALL)) { // 0 - atc1441, 1 - pvvx, 2 - Mi, 3 - all
			set_pvvx_adv_data(rds.adv_counter);
			adv_buf.data_size = adv_buf.data[0] + 1;
			v = (pext_adv_cnt_t)&adv_buf.data[adv_buf.data_size];
		} else if (cfg.flg.advertising_type == ADV_TYPE_MI)  {
			pext_adv_mi_t p = (pext_adv_mi_t)&adv_buf.data;
			p->head.size = sizeof(ext_adv_mi_t) - sizeof(p->head.size);
			p->head.uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
			p->head.UUID = ADV_XIAOMI_UUID16; // 16-bit UUID for Members 0xFE95 Xiaomi Inc.
			p->head.fctrl.word = 0x5040; // 0x5040 version = 5, no MACInclude, ObjectInclude
			p->head.dev_id = DEVICE_TYPE;
			p->head.counter = (uint8_t)rds.adv_counter;
#if	0
			p->data.id = XIAOMI_DATA_ID_Switch;
			p->data.size = 1;
			p->data.value = 1;
#else
			p->data.id = XIAOMI_DATA_ID_DoorSensor;
			p->data.size = 1;
			p->data.value = ! trg.flg.rds_input;
#endif
			adv_buf.data_size = sizeof(ext_adv_mi_t);
			v = (pext_adv_cnt_t)&adv_buf.data[sizeof(ext_adv_mi_t)];
		} else {
			pext_adv_dig_t p = (pext_adv_dig_t)&adv_buf.data;
			p->size = sizeof(ext_adv_dig_t) - sizeof(p->size);
			p->uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
			p->UUID = ADV_UUID16_DigitalStateBits; // = 0x2A56, 16-bit UUID Digital bits, Out bits control (LEDs control)
			p->bits = trg.flg_byte;
			adv_buf.data_size = sizeof(ext_adv_dig_t);
			v = (pext_adv_cnt_t)&adv_buf.data[sizeof(ext_adv_dig_t)];
		}
	}
	v->size = 6;
	v->uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	v->UUID = ADV_UUID16_Count24bits;
	v->cnt[0] = rds.count_byte[2];
	v->cnt[1] = rds.count_byte[1];
	v->cnt[2] = rds.count_byte[0];
	adv_buf.data_size += 7;
	bls_ll_setAdvData((u8 *)&adv_buf.data, adv_buf.data_size);
}

#define EXT_ADV_INTERVAL ADV_INTERVAL_50MS
#define EXT_ADV_COUNT 8

_attribute_ram_code_ static void start_ext_adv(void) {
	bls_ll_setAdvEnable(0);  // adv disable
	bls_ll_setAdvParam(EXT_ADV_INTERVAL, EXT_ADV_INTERVAL,
//			ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL,
			ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL,
			BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	set_rds_adv_data();
	bls_ll_setAdvDuration(EXT_ADV_INTERVAL*EXT_ADV_COUNT*625+16, 1);
	bls_ll_setAdvEnable(1);  // adv enable
}

_attribute_ram_code_ void rds_suspend(void) {
	if(!ble_connected) {
		/* TODO: if connection mode, gpio wakeup throws errors in sdk libs!
		   Work options: bls_pm_setSuspendMask(SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN);
		   No DEEPSLEEP_RETENTION_CONN */
		cpu_set_gpio_wakeup(GPIO_RDS, get_rds_input()? Level_Low : Level_High, 1);  // pad wakeup deepsleep enable
		bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
	} else {
		cpu_set_gpio_wakeup(GPIO_RDS, Level_Low, 0);  // pad wakeup suspend/deepsleep disable
	}
}

/* if(rds.type) // rds.type: switch or counter */
__attribute__((optimize("-Os")))
_attribute_ram_code_ void rds_task(void) {
	rds_input_on();
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
			// store low 16 bits rds.count
			analog_write(DEEP_ANA_REG0, rds.count);
			analog_write(DEEP_ANA_REG1, rds.count >> 8);
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
