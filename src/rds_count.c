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


_attribute_ram_code_ __attribute__((optimize("-Os")))
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
#if 0 // Not Work! SDK only 1 ext_adv...
	if (ext_adv_init) { // support extension advertise
		blc_ll_setExtAdvData(ADV_HANDLE0, DATA_OPER_COMPLETE, DATA_FRAGM_ALLOWED,
				adv_buf.data_size, (u8 *)&adv_buf.data);
	} else
#endif
#endif
	bls_ll_setAdvData((u8 *)&adv_buf.data, adv_buf.data_size);
}

_attribute_ram_code_ static void start_ext_adv(void) {

#if (BLE_EXT_ADV)
#if 0 // Not Work! SDK only 1 ext_adv... 
	if (ext_adv_init) { // support extension advertise
		blc_ll_setExtAdvEnable_1(BLC_ADV_DISABLE, 1, ADV_HANDLE0, 0, 0);
		//blc_ll_removeAdvSet(ADV_HANDLE0);
		//blc_ll_clearAdvSets();
		//adv_set: Extended, Connectable_scannable
		blc_ll_setExtAdvParam(ADV_HANDLE0,
				ADV_EVT_PROP_LEGACY_CONNECTABLE_SCANNABLE_UNDIRECTED,
				EXT_ADV_INTERVAL, EXT_ADV_INTERVAL,
				BLT_ENABLE_ADV_ALL, // primary advertising channel map
				OWN_ADDRESS_PUBLIC, // own address type
				BLE_ADDR_PUBLIC, // peer address type
				NULL, // * peer address
				ADV_FP_NONE, // advertising filter policy
				TX_POWER_0dBm, // TODO: advertising TX power cfg.rf_tx_power
				BLE_PHY_1M, // primary advertising channel PHY type
				0, // secondary advertising minimum skip number
				BLE_PHY_CODED, // primary advertising channel PHY type
				ADV_SID_0,
				0); // scan response notify enable ?

		blc_ll_setExtScanRspData(ADV_HANDLE0, DATA_OPER_COMPLETE, DATA_FRAGM_ALLOWED,
				ble_name[0]+1, (uint8_t *) ble_name);

		set_rds_adv_data();
		adv_buf.data_size = 0; // flag adv_buf.send_count++ in app_advertise_prepare_handler()

		// if Coded PHY is used, this API set default S2/S8 mode for Extended ADV
		//blc_ll_setDefaultExtAdvCodingIndication(ADV_HANDLE0, CODED_PHY_PREFER_S8);

		// debug!!!
		//blc_ll_setAuxAdvChnIdxByCustomers(20); // auxiliary data channel, must be range of 0~36

		blta.adv_duraton_en = EXT_ADV_COUNT-2;
		blta.adv_interval = 0; // system tick
		// Time = N * 10 ms, Time Range: 10 ms to 655,350 ms
		blc_ll_setExtAdvEnable_1(BLC_ADV_ENABLE, 1, ADV_HANDLE0, (EXT_ADV_INTERVAL*(EXT_ADV_COUNT-1)*625+35)/10000 , EXT_ADV_COUNT);
	} else

#endif
#endif
	{
		bls_ll_setAdvEnable(BLC_ADV_DISABLE);  // adv disable
		bls_ll_setAdvParam(EXT_ADV_INTERVAL, EXT_ADV_INTERVAL,
				ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL,
				BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
		set_rds_adv_data();
		adv_buf.data_size = 0; // flag adv_buf.send_count++
		bls_ll_setAdvDuration(EXT_ADV_INTERVAL*(EXT_ADV_COUNT-1)*625+33, 1);
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
