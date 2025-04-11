/*
 * bme280.h
 *
 */

#ifndef _BME280_H_
#define _BME280_H_

#define BME280_I2C_ADDR 0x76
//
#define BME280_ID 0x60
//
// registers address
enum {
  BME280_RA_DIG_T1 = 0x88,
  BME280_RA_DIG_T2 = 0x8A,
  BME280_RA_DIG_T3 = 0x8C,

  BME280_RA_DIG_P1 = 0x8E,
  BME280_RA_DIG_P2 = 0x90,
  BME280_RA_DIG_P3 = 0x92,
  BME280_RA_DIG_P4 = 0x94,
  BME280_RA_DIG_P5 = 0x96,
  BME280_RA_DIG_P6 = 0x98,
  BME280_RA_DIG_P7 = 0x9A,
  BME280_RA_DIG_P8 = 0x9C,
  BME280_RA_DIG_P9 = 0x9E,

  BME280_RA_DIG_H1 = 0xA1,
  BME280_RA_DIG_H2 = 0xE1,
  BME280_RA_DIG_H3 = 0xE3,
  BME280_RA_DIG_H4 = 0xE4,
  BME280_RA_DIG_H5 = 0xE5,
  BME280_RA_DIG_H6 = 0xE7,

  BME280_RA_CHIPID = 0xD0,
  BME280_RA_VERSION = 0xD1,
  BME280_RA_SOFTRESET = 0xE0,

  BME280_RA_CAL26 = 0xE1, // 0xE1..0xF0

  BME280_RA_CTRL_HUMI = 0xF2,
  BME280_RA_STATUS = 0xF3,
  BME280_RA_CTRL_MEAS = 0xF4,
  BME280_RA_CONFIG = 0xF5,
  BME280_RA_PRESSURE = 0xF7,
  BME280_RA_TEMP = 0xFA,
  BME280_RA_HUMI = 0xFD
};

//
// struct resgisters OTP BME280
typedef struct {
	u16 T1;	// addr: 0x88, data: 0x6fd1
	s16 T2; // addr: 0x8a, data: 0x6993
	s16 T3; // addr: 0x8c, data: 0x0032

	u16 P1; // addr: 0x8e, 0x8a62
	s16 P2; // addr: 0x90
	s16 P3; // addr: 0x92
	s16 P4; // addr: 0x94
	s16 P5; // addr: 0x96
	s16 P6; // addr: 0x98
	s16 P7; // addr: 0x9a
	s16 P8; // addr: 0x9c
	s16 P9; // addr: 0x9e

	u8  xa0;
	u8  H1; // addr: 0xa1

	s16 H2; // addr: 0xe1
	u8  H3; // addr: 0xe3
	s16 H4; // addr: 0xe4
	s16 H5; // addr: 0xe5
	s8  H6; // addr: 0xe7
} bme280_dig_t;
//
// STATUS
#define BME280_IM_UPDATE		1
#define BME280_MEASURING		(1<<3)
// CTRL_HUMI
#define BME280_OVERSAMP_H_NONE	0
#define BME280_OVERSAMP_H_1		1
#define BME280_OVERSAMP_H_2		2
#define BME280_OVERSAMP_H_4		3
#define BME280_OVERSAMP_H_8		4
#define BME280_OVERSAMP_H_16	5
// CTRL_MEAS
#define BME280_SLEEP_MODE		0
#define BME280_FORCED_MODE		1
#define BME280_NORMAL_MODE		3

#define BME280_OVERSAMP_P_NONE	0
#define BME280_OVERSAMP_P_1		(1<<2)
#define BME280_OVERSAMP_P_2		(2<<2)
#define BME280_OVERSAMP_P_4		(3<<2)
#define BME280_OVERSAMP_P_8		(4<<2)
#define BME280_OVERSAMP_P_16	(5<<2)

#define BME280_OVERSAMP_T_NONE	0
#define BME280_OVERSAMP_T_1		(1<<5)
#define BME280_OVERSAMP_T_2		(2<<5)
#define BME280_OVERSAMP_T_4		(3<<5)
#define BME280_OVERSAMP_T_8		(4<<5)
#define BME280_OVERSAMP_T_16	(5<<5)
// CONFIG
#define BME280_SPI3W_DIS		0
#define BME280_SPI3W_ENA		1
#define BME280_FILER_OFF		0
#define BME280_FILER_2			(1<<2)
#define BME280_FILER_4			(2<<2)
#define BME280_FILER_8			(3<<2)
#define BME280_FILER_16			(4<<2)
#define BME280_STANDBY_0T5MS	0
#define BME280_STANDBY_62T5MS	(1<<5)
#define BME280_STANDBY_125MS	(2<<5)
#define BME280_STANDBY_250MS	(3<<5)
#define BME280_STANDBY_500MS	(4<<5)
#define BME280_STANDBY_1SEC		(5<<5)
#define BME280_STANDBY_10MS		(6<<5)
#define BME280_STANDBY_20MS		(7<<5)
//
#define BME280_SET_CTRL_HUMI	(BME280_OVERSAMP_H_1)
#define BME280_SET_CTRL_MEAS	(BME280_OVERSAMP_T_1 | BME280_OVERSAMP_P_1 | BME280_FORCED_MODE)
#define BME280_SET_CONFIG		(BME280_FILER_OFF | BME280_STANDBY_0T5MS | BME280_SPI3W_DIS)
//
// struct resgisters BME280
typedef struct __attribute__ ((packed)) {
//	u8 status;
//	u8 ctrl_meas;
//	u8 config;
//	u8 none;
	u8 press[3];
	u8 temp[3];
	u8 humi[2];
} bme280_reg_t;
//
// struct BME280
typedef struct {
	s32 t_fine ; /* t_fine carries fine temperature as global value */
	bme280_dig_t dig;
} bme280_t;

typedef struct _thsensor_coef_t {
	u32 val1_k;	// temp_k
	u32 val2_k;	// humi_k
	s16 val1_z;	// temp_z
	s16 val2_z;	// humi_z
} sensor_coef_t; // [12]

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
	s32 t_fine ; /* t_fine carries fine temperature as global value */
	bme280_dig_t dig;
} sensor_cfg_t;

extern sensor_cfg_t sensor_cfg;
#define sensor_cfg_send_size 18 //max 19


#endif /* _BME280_H_ */
