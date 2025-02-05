/*
 * rh.h
 *
 *  Created on: 24 июл. 2024 г.
 *      Author: pvvx
 */

#ifndef _RH_H_
#define _RH_H_

// R = 7.5 kOm, КД521 (BAV99), C 4.7 nF

/*********************************************************************************
    PWM0   :  PA2.  PC1.  PC2.	PD5
    PWM1   :  PA3.  PC3.
    PWM2   :  PA4.  PC4.
    PWM3   :  PB0.  PD2.
    PWM4   :  PB1.  PB4.
    PWM5   :  PB2.  PB5.
    PWM0_N :  PA0.  PB3.  PC4	PD5
    PWM1_N :  PC1.  PD3.
    PWM2_N :  PD4.
    PWM3_N :  PC5.
    PWM4_N :  PC0.  PC6.
    PWM5_N :  PC7.
 *********************************************************************************/

/*
// PWM
#define PWM_PIN		GPIO_PB4
#define AS_PWMx		AS_PWM4
#define PWM_ID		PWM4_ID
// ADC
#define GPIO_RHI	GPIO_PB5
#define CHNL_RHI	B5P
*/
#if (DEV_SERVICES & SERVICE_PLM)

typedef struct _thsensor_coef_t {
	u32 val1_k;	// temp_k / current_k
	u32 val2_k;	// humi_k / voltage_k
	s16 val1_z;		// temp_z / current_z
	s16 val2_z;		// humi_z / voltage_z
} sensor_coef_t; // [12]

typedef struct _sensor_def_cfg_t {
	sensor_coef_t coef;
	u32 measure_timeout;
	u8 sensor_type; // SENSOR_TYPES
} sensor_def_cfg_t;

typedef struct _sensor_cfg_t {
	sensor_coef_t coef;
	u32 id;
	u8 i2c_addr;
	u8 sensor_type; // SENSOR_TYPES
	// not saved
#if SENSOR_SLEEP_MEASURE
	volatile u32 time_measure;
	u32 measure_timeout;
#endif
} sensor_cfg_t;

extern sensor_cfg_t sensor_cfg;
#define sensor_cfg_send_size 18 //max 19

#endif

typedef struct {
	u32	tic;
	u16	ubase;
	u16	rh;
	u16	ntc;
	u16	cal_mv;
} rh_t;

extern rh_t rh;

//u16 get_adc_rh_mv(void);

void calibrate_rh(void);
int	read_sensor_cb(void);

#endif /* RH_H_ */
