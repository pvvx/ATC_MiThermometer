/*
 * rds_count.h
 *
 *  Created on: 20.02.2022
 *      Author: pvvx
 */

#ifndef RDS_COUNT_H_
#define RDS_COUNT_H_

#include "app_config.h"

#if (DEV_SERVICES & SERVICE_RDS)

//#include "mi_beacon.h"

#define EXT_ADV_INTERVAL ADV_INTERVAL_50MS
#define EXT_ADV_COUNT 6

enum {
	RDS_NONE = 0,
	RDS_SWITCH,
	RDS_COUNTER,
	RDS_CONNECT // version 4.2+
} RDS_TYPES;

typedef struct _rds_count_t {
	uint32_t report_tick; // timer reed switch count report interval (utc_time_sec)
	uint32_t adv_counter;
	union {				// rs counter pulses
		uint8_t count_byte[4];
		uint16_t count_short[2];
		uint32_t count;
	};
//	uint8_t type;	// RDS_TYPES: 0 - none, 1 - switch, 2 - counter, (3 - connect)
	uint8_t event;  // Reed Switch event
} rds_count_t;
extern rds_count_t rds;		// Reed switch pulse counter

#ifdef GPIO_RDS1
static inline uint8_t get_rds1_input(void) {
	uint8_t r = BM_IS_SET(reg_gpio_in(GPIO_RDS1), GPIO_RDS1 & 0xff)? 1 : 0;
	if(trg.rds.rs1_invert)
		r ^= 1;
	return r;
}

static inline void rds1_input_on(void) {
	gpio_setup_up_down_resistor(GPIO_RDS1, PM_PIN_PULLUP_1M);
}

static inline void rds1_input_off(void) {
	gpio_setup_up_down_resistor(GPIO_RDS1, PM_PIN_UP_DOWN_FLOAT);
}

#endif

#ifdef GPIO_RDS2
static inline uint8_t get_rds2_input(void) {
	uint8_t r = BM_IS_SET(reg_gpio_in(GPIO_RDS2), GPIO_RDS2 & 0xff)? 1 : 0;
	if(trg.rds.rs2_invert)
		r ^= 1;
	return r;
}

static inline void rds2_input_off(void) {
	gpio_setup_up_down_resistor(GPIO_RDS1, PM_PIN_UP_DOWN_FLOAT);
}

static inline void rds2_input_on(void) {
	gpio_setup_up_down_resistor(GPIO_RDS2, PM_PIN_PULLUP_1M);
}
#endif


static inline void rds_input_on(void) {
#ifdef GPIO_RDS1
	gpio_setup_up_down_resistor(GPIO_RDS1, PM_PIN_PULLUP_1M);
#endif
#ifdef GPIO_RDS2
	gpio_setup_up_down_resistor(GPIO_RDS2, PM_PIN_PULLUP_1M);
#endif
}


void rds_init(void);
void rds_suspend(void);
void rds_task(void);

#endif // (DEV_SERVICES & SERVICE_RDS)

#endif /* RDS_COUNT_H_ */
