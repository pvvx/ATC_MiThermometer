/*
 * rds_count.h
 *
 *  Created on: 20.02.2022
 *      Author: pvvx
 */

#ifndef RDS_COUNT_H_
#define RDS_COUNT_H_

#include "app_config.h"

#if	USE_TRIGGER_OUT
#ifdef GPIO_RDS

static inline uint8_t get_rds_input(void) {
	return (BM_IS_SET(reg_gpio_in(GPIO_RDS), GPIO_RDS & 0xff));
}

static inline void set_rds_input(void) {
	trg.flg.rds_input = ((BM_IS_SET(reg_gpio_in(GPIO_RDS), GPIO_RDS & 0xff))? 1 : 0);
}

static inline void rds_input_off(void) {
	gpio_setup_up_down_resistor(GPIO_RDS, PM_PIN_UP_DOWN_FLOAT);
}

static inline void rds_input_on(void) {
	gpio_setup_up_down_resistor(GPIO_RDS, PM_PIN_PULLUP_1M);
}

#if USE_WK_RDS_COUNTER
#include "mi_beacon.h"

#define EXT_ADV_INTERVAL ADV_INTERVAL_50MS
#define EXT_ADV_COUNT 5

#define ADV_UUID16_DigitalStateBits	0x2A56 // 16-bit UUID Digital bits, Out bits control (LEDs control)
#define ADV_UUID16_AnalogOutValues	0x2A58 // 16-bit UUID Analog values (DACs control)
#define ADV_UUID16_Aggregate		0x2A5A // 16-bit UUID Aggregate, The Aggregate Input is an aggregate of the Digital Input Characteristic value (if available) and ALL Analog Inputs available.
#define ADV_UUID16_Count24bits		0x2AEB // 16-bit UUID Count 24 bits
#define ADV_UUID16_Count16bits 		0x2AEA // 16-bit UUID Count 16 bits

typedef struct __attribute__((packed)) _ext_adv_cnt_t {
	uint8_t		size;	// = 6
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0x2AEB - Count 24
	uint8_t	cnt[3];
} ext_adv_cnt_t, * pext_adv_cnt_t;

typedef struct __attribute__((packed)) _ext_adv_digt_t {
	uint8_t		size;	// = 4
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0x2A56 - Digital State Bits
	uint8_t		bits;
} ext_adv_dig_t, * pext_adv_dig_t;

typedef struct __attribute__((packed)) _ext_adv_mi_t {
	adv_mi_head_t head;
	struct {
		uint16_t	 id;	// = 0x1004, 0x1006, 0x100a ... (XIAOMI_DATA_ID)
		uint8_t		 size;
		uint8_t		 value;
	} data;
} ext_adv_mi_t, * pext_adv_mi_t;

enum {
	RDS_NONE = 0,
	RDS_SWITCH,
	RDS_COUNTER
} RDS_TYPES;

typedef struct _rds_count_t {
	uint32_t report_tick; // timer reed switch count report interval (utc_time_sec)
	uint32_t adv_counter;
	union {				// rs counter pulses
		uint8_t count_byte[4];
		uint16_t count_short[2];
		uint32_t count;
	};
	uint8_t type;	// 0 - none, 1 - switch, 2 - counter
	uint8_t event;  // Reed Switch event
} rds_count_t;
extern rds_count_t rds;		// Reed switch pulse counter

void rds_init(void);
void rds_suspend(void);
void rds_task(void);
void set_rds_adv_data(void);

#endif // USE_WK_RDS_COUNTER

#endif // defined GPIO_RDS

#endif // USE_TRIGGER_OUT

#endif /* RDS_COUNT_H_ */
