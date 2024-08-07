/*
 * my18b20.h
 *
 *  Created on: 3 авг. 2024 г.
 *      Author: pvvx
 */

#ifndef _MY18B20_H_
#define _MY18B20_H_

typedef struct _my18b20_coef_t {
	u32 val1_k;	// temp_k / current_k
#if	USE_SENSOR_MY18B20 == 2
	u32 val2_k;	// humi_k / voltage_k
#endif
	s16 val1_z;		// temp_z / current_z
#if	USE_SENSOR_MY18B20 == 2
	s16 val2_z;		// humi_z / voltage_z
#endif
} my18b20_coef_t; // [12]

// extern my18b20_coef_t def_coef_my18b20;

typedef struct {
	my18b20_coef_t coef;
	u32 id;
	u8	res;
	u8	type;
	u8	rd_ok;
	u8	stage;
	u32 tick;
	u32 timeout;
	s16 temp[USE_SENSOR_MY18B20];
} my18b20_t;

extern my18b20_t my18b20;
#define my18b20_send_size (sizeof(my18b20.coef) + 6)

void init_my18b20(void);
void task_my18b20(void);
int read_sensor_cb(void);

#endif /* _MY18B20_H_ */
