/*
 * stcc4.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: pvvx
 */

#ifndef STCC4_H_
#define STCC4_H_

typedef struct _sensor_co2_t {
	volatile u32 measure_timeout;
	u32 time_measure;
	u8 th_measure_count;
} sensor_co2_t;

extern sensor_co2_t sensor_co2;

void init_co2_sensor(void);
int task_measure_co2(void);
void start_measure_co2(void);


#endif /* STCC4_H_ */
