/*
 * tigger.h
 *
 *  Created on: 02.01.2021
 *      Author: pvvx
 */

#ifndef _TIGGER_H_
#define _TIGGER_H_

#include "app_config.h"

#if (DEV_SERVICES & SERVICE_TH_TRG) || (DEV_SERVICES & SERVICE_RDS)

typedef struct __attribute__((packed)) _trigger_flg_t {
	uint8_t 	rds1_input	:	1; // Reed Switch, input
	uint8_t 	trg_output	:	1; // GPIO_TRG pin output value (pull Up/Down)
	uint8_t 	trigger_on	:	1; // Output GPIO_TRG pin is controlled according to the set parameters threshold temperature or humidity
	uint8_t 	temp_out_on :	1; // Temperature trigger event
	uint8_t 	humi_out_on :	1; // Humidity trigger event
	uint8_t 	key_pressed	:	1; // key2 pressed (Connect/Reset)
	uint8_t 	rds2_input 	:	1; // Reed Switch 2, input
}trigger_flg_t;

#if (DEV_SERVICES & SERVICE_RDS)

typedef struct __attribute__((packed)) _rds_type_t {
	uint8_t type1		: 2;		// RDS_TYPES, Reed switch 1 types: 0 - none, 1 - switch, 2 - counter [3 - connect]
	uint8_t type2		: 2;		// RDS_TYPES, Reed switch 2 types: 0 - none, 1 - switch, 2 - counter [3 - connect]
	// version 3.9+
	uint8_t rs1_invert	: 1;		// GPIO events (Reed switch 1): 0 - rising, 1 - falling
	uint8_t rs2_invert	: 1;		// GPIO events (Reed switch 2): 0 - rising, 1 - falling
}rds_type_t;

#endif

typedef struct __attribute__((packed)) _trigger_t {
#if (DEV_SERVICES & SERVICE_TH_TRG)
	int16_t temp_threshold; // x0.01°, Set temp threshold
	int16_t humi_threshold; // x0.01%, Set humi threshold
	int16_t temp_hysteresis; // Set temp hysteresis, -327.67..327.67 °
	int16_t humi_hysteresis; // Set humi hysteresis, -327.67..327.67 %
#endif
#if (DEV_SERVICES & SERVICE_RDS)
	// version 3.6+
	uint16_t rds_time_report; // Reed switch count report interval (sec)
	rds_type_t rds;	  		  // type Reed switch
#endif
	union {
		trigger_flg_t flg;
		uint8_t	flg_byte;
	};
}trigger_t;

#define FEEP_SAVE_SIZE_TRG (sizeof(trg)-1)

extern trigger_t trg;
extern const trigger_t def_trg;

#if (DEV_SERVICES & SERVICE_TH_TRG)
void set_trigger_out(void);
void test_trg_on(void);
#endif

#endif // (DEV_SERVICES & SERVICE_TH_TRG) || (DEV_SERVICES & SERVICE_RDS)

#endif /* _TIGGER_H_ */
