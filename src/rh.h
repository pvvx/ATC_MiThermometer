/*
 * rh.h
 *
 *  Created on: 24 июл. 2024 г.
 *      Author: pvvx
 */

#ifndef _RH_H_
#define _RH_H_

#define USE_RH_SENSOR	1

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

// PWM
#define PWM_PIN		GPIO_PB5
#define AS_PWMx		AS_PWM5
#define PWM_ID		PWM5_ID
// ADC
#define GPIO_RHI	GPIO_PB1
#define CHNL_RHI	B1P

typedef struct {
	uint32_t	tic;
	uint16_t	ubat;
	uint16_t	rh;
	uint16_t	mv;
} rh_t;

extern rh_t rh;

uint16_t get_adc_rh_mv(void);

void calibrate_rh(void);

#endif /* RH_H_ */
