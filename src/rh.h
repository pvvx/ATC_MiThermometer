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
	uint32_t val1_k;	// temp_k / current_k
	uint32_t val2_k;	// humi_k / voltage_k
	int16_t val1_z;		// temp_z / current_z
	int16_t val2_z;		// humi_z / voltage_z
} sensor_coef_t; // [12]

typedef struct _sensor_def_cfg_t {
	sensor_coef_t coef;
	uint32_t measure_timeout;
	uint8_t sensor_type; // SENSOR_TYPES
} sensor_def_cfg_t;

typedef struct _sensor_cfg_t {
	sensor_coef_t coef;
	uint32_t id;
	uint8_t i2c_addr;
	uint8_t sensor_type; // SENSOR_TYPES
	// not saved
#if SENSOR_SLEEP_MEASURE
	volatile uint32_t time_measure;
	uint32_t measure_timeout;
#endif
} sensor_cfg_t;

extern sensor_cfg_t sensor_cfg;
#define sensor_cfg_send_size 18 //max 19

#endif

typedef struct {
	uint32_t	tic;
	uint16_t	ubase;
	uint16_t	rh;
	uint16_t	ntc;
	uint16_t	cal_mv;
} rh_t;

extern rh_t rh;

//uint16_t get_adc_rh_mv(void);

void calibrate_rh(void);
int	read_sensor_cb(void);

#endif /* RH_H_ */
