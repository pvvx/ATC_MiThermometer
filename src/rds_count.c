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
#include "rds_count.h"


RAM	rds_count_t rds;		// Reed switch pulse counter

RAM struct {
	uint8_t		size;	// = 6
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0x2AEB - Count 24
	uint8_t	cnt[3];
} ext_adv;

void rds_init(void) {
	if(rds.type)
		rds_input_on();
#if USE_WK_RDS_COUNTER32 // save 32 bits?
	if((flash_read_cfg(&rds.count_short[1], EEP_ID_RPC, sizeof(rds.count_short[1])) != sizeof(rds.count_short[1])) {
		rds.count = 0;
	}
#endif
	rds.count_byte[0] = analog_read(DEEP_ANA_REG0);
	rds.count_byte[1] = analog_read(DEEP_ANA_REG1);
	rds.tim_disable = CLOCK_SYS_CLOCK_1S;
	rds.report_tick = utc_time_sec;
}

_attribute_ram_code_ static void rds_adv(void) {
	ext_adv.size = sizeof(ext_adv)-1;
	ext_adv.uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	ext_adv.UUID = 0x2AEB; // 0x2AEB - Count 24 (0x2AEA - Count 16)
	ext_adv.cnt[0] = rds.count_byte[2];
	ext_adv.cnt[1] = rds.count_byte[1];
	ext_adv.cnt[2] = rds.count_byte[0];
	start_ext_adv((u8 *)&ext_adv, sizeof(ext_adv));
}

_attribute_ram_code_ void rds_suspend(void) {
	if((!ble_connected) && rds.tick_disable == 0) {
		cpu_set_gpio_wakeup(GPIO_RDS, get_rds_input()? Level_Low : Level_High, 1);  // pad wakeup deepsleep enable
		bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
	} else {
		cpu_set_gpio_wakeup(GPIO_RDS, Level_Low, 0);  // pad wakeup suspend/deepsleep disable
	}
}

_attribute_ram_code_ void rds_task(void) {
	rds_input_on();
	if(rds.tick_disable && clock_time() -  rds.tick_disable > rds.tim_disable)
		rds.tick_disable = 0;
	if(get_rds_input()) {
		if(!trg.flg.rds_input) {
			trg.flg.rds_input = 1;
			rds.count++;
			// store low 16 bits rds.count
			analog_write(DEEP_ANA_REG0, rds.count);
			analog_write(DEEP_ANA_REG1, rds.count >> 8);
#if USE_WK_RDS_COUNTER32 // save 32 bits?
			if(rds.count_short[0] == 0) {
				flash_write_cfg(&rds.count_short[1], EEP_ID_RPC, sizeof(rds.count_short[1]));
			}
#endif
			if(rds.type == 1 && rds.tick_disable == 0){ // switch mode
				rds.tick_disable = clock_time() | 1;
				if(!ble_connected) {
					tim_measure = clock_time();
					rds_adv();
				}
			} else if(rds.type == 2) { // counter mode
				rds.tick_disable = 0;
				if((!ble_connected) && (rds.count & 0xffff) == 0) { // report 'overflow 16 bit count'
					tim_measure = clock_time();
					rds_adv();
				}
			}
		}
	} else {
		trg.flg.rds_input = 0;
	}
	if((!ble_connected) && trg.rds_time_report
		&& rds.type == 2 // counter mode
		&& utc_time_sec - rds.report_tick > trg.rds_time_report) {
				rds.report_tick = utc_time_sec;
				tim_measure = clock_time();
				rds_adv();
	}
}

#endif // USE_TRIGGER_OUT
