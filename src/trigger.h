/*
 * tigger.h
 *
 *  Created on: 02.01.2021
 *      Author: pvvx
 */

#ifndef TIGGER_H_
#define TIGGER_H_

#include "app_config.h"

// GPIO_TRG pin (marking "reset" on circuit board) flags:
typedef struct __attribute__((packed)) _trigger_flg_t {
	uint8_t 	rds_input	:	1; // Reed Switch, input
	uint8_t 	trg_output	:	1; // GPIO_TRG pin output value (pull Up/Down)
	uint8_t 	trigger_on	:	1; // Output GPIO_TRG pin is controlled according to the set parameters threshold temperature or humidity
	uint8_t 	temp_out_on :	1; // Temperature trigger event
	uint8_t 	humi_out_on :	1; // Humidity trigger event
}trigger_flg_t;

typedef struct __attribute__((packed)) _trigger_t {
	int16_t temp_threshold; // x0.01°, Set temp threshold
	int16_t humi_threshold; // x0.01%, Set humi threshold
	int16_t temp_hysteresis; // Set temp hysteresis, -327.67..327.67 °
	int16_t humi_hysteresis; // Set humi hysteresis, -327.67..327.67 %
	union {
		trigger_flg_t flg;
		uint8_t	flg_byte;
	};
}trigger_t;

#define FEEP_SAVE_SIZE_TRG (sizeof(trg)-1)
extern trigger_t trg;
extern const trigger_t def_trg;

#if USE_WK_RDS_COUNTER
typedef union _rds_count_t {
	uint8_t count_byte[4];
	uint16_t count_short[2];
	uint32_t count;
}rds_count_t;
extern rds_count_t rds;		// Reed switch pulse counter
#endif

void set_trigger_out(void);
void test_trg_on(void);


#ifdef GPIO_RDS

static inline uint8_t get_rds_input(void) {
	return (BM_IS_SET(reg_gpio_in(GPIO_RDS), GPIO_RDS & 0xff));
}

static inline void save_rds_input(void) {
	trg.flg.rds_input = ((BM_IS_SET(reg_gpio_in(GPIO_RDS), GPIO_RDS & 0xff))? 1 : 0);
}

static inline void rds_input_off(void) {
	gpio_setup_up_down_resistor(GPIO_RDS, PM_PIN_UP_DOWN_FLOAT);
}

static inline void rds_input_on(void) {
	gpio_setup_up_down_resistor(GPIO_RDS, PM_PIN_PULLUP_1M);
}
#endif

#endif /* TIGGER_H_ */
