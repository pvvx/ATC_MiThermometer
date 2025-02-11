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

#ifndef RDS1_PULLUP
#define RDS1_PULLUP PM_PIN_PULLUP_1M
#endif

#ifndef RDS2_PULLUP
#define RDS2_PULLUP PM_PIN_PULLUP_1M
#endif

enum {
	RDS_NONE = 0,
	RDS_SWITCH,
	RDS_COUNTER,
	RDS_CONNECT // version 4.2+
} RDS_TYPES;

typedef struct _rds_count_t {
	u32 report_tick; // timer reed switch count report interval (wrk.utc_time_sec)
	union {				// rs1 counter pulses
		u8 count1_byte[4];
		u16 count1_short[2];
		u32 count1;
	};
/*
#ifdef GPIO_RDS2
	union {				// rs2 counter pulses
		u8 count2_byte[4];
		u16 count2_short[2];
		u32 count2;
	};
#endif
*/
	u8 event;  // Reed Switch event
} rds_count_t;
extern rds_count_t rds;		// Reed switch pulse counter

#ifdef GPIO_RDS1
static inline u8 get_rds1_input(void) {
	u8 r = BM_IS_SET(reg_gpio_in(GPIO_RDS1), GPIO_RDS1 & 0xff)? 1 : 0;
	if(trg.rds.rs1_invert)
		r ^= 1;
	return r;
}

static inline void rds1_input_on(void) {
	gpio_setup_up_down_resistor(GPIO_RDS1, RDS1_PULLUP);
}

static inline void rds1_input_off(void) {
	gpio_setup_up_down_resistor(GPIO_RDS1, PM_PIN_UP_DOWN_FLOAT);
}

#endif

#ifdef GPIO_RDS2
static inline u8 get_rds2_input(void) {
	u8 r = BM_IS_SET(reg_gpio_in(GPIO_RDS2), GPIO_RDS2 & 0xff)? 1 : 0;
	if(trg.rds.rs2_invert)
		r ^= 1;
	return r;
}

static inline void rds2_input_off(void) {
	gpio_setup_up_down_resistor(GPIO_RDS2, PM_PIN_UP_DOWN_FLOAT);
}

static inline void rds2_input_on(void) {
	gpio_setup_up_down_resistor(GPIO_RDS2, RDS2_PULLUP);
}
#endif


static inline void rds_input_on(void) {
#ifdef GPIO_RDS1
	gpio_setup_up_down_resistor(GPIO_RDS1, RDS1_PULLUP);
#endif
#ifdef GPIO_RDS2
	gpio_setup_up_down_resistor(GPIO_RDS2, RDS2_PULLUP);
#endif
}


void rds_init(void);
void rds_suspend(void);
void rds_task(void);

#endif // (DEV_SERVICES & SERVICE_RDS)

#endif /* RDS_COUNT_H_ */
