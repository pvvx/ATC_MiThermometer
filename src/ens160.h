/*
 * ens160.h
 *
 *  Created on: 4 мар. 2025 г.
 *      Author: pvvx
 */

#ifndef _ENS160_H_
#define _ENS160_H_

#if !SENSOR_SLEEP_MEASURE
#define USE_ENS160_INT	1
#endif

// Possible Operating Mode defines:
#define ENS160_MODE_DEEP_SLEEP	0x00
#define ENS160_MODE_IDLE		0x01
#define ENS160_MODE_STANDARD	0x02
#define ENS160_MODE_RESET		0xF0

typedef struct {
    u8 i2c_address;
    u8 mode;	// Deep Sleep (0x00), Idle (0x01), Standard (0x02), Reset (0xF0)
    u8 status;	// Register DEVICE_STATUS
    // [0] 	 new data is available in the GPR_READx registers
    // [1]   new data is available in the DATA_x registers
    // [2:3] 0: Normal operation
    // 	     1: Warm-Up phase
    // 	     2: Initial Start-Up phase
    // 	     3: Invalid output
    // [6]   Invalid Operating Mode has been selected
    // [7]   OPMODE is running
	u8 aqi;		// 1 - Excellent, 2 - Good, 3 - Moderate, 4 - Poor, 5 - Unhealthy.
	u16 tvoc;	//
	u16 co2;	// ppm
	u8 flg;
#if USE_ENS160_INT
	u16 cnt;
	u32	co2_sum;
	u32	tvoc_sum;
#endif
} ens160_wrk_t;

extern ens160_wrk_t ens160;

int init_ens160(void);
int read_ens160(void);
int set_th_ens160(s16 tx100, u16 hx100);

#endif /* _ENS160_H_ */
