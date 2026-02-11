/*
 * bme280.h
 *
 */

#ifndef _BME280_H_
#define _BME280_H_

#define BMx280_I2C_ADDR1 0x76 // 0x76, 0x77
#define BMx280_I2C_ADDR2 0x77 // 0x76, 0x77
//
#define BMP280_ID 0x58
#define BME280_ID 0x60
//
// registers address
enum {
  BMx280_RA_DIG_T1 = 0x88,
  BMx280_RA_DIG_T2 = 0x8A,
  BMx280_RA_DIG_T3 = 0x8C,

  BMx280_RA_DIG_P1 = 0x8E,
  BMx280_RA_DIG_P2 = 0x90,
  BMx280_RA_DIG_P3 = 0x92,
  BMx280_RA_DIG_P4 = 0x94,
  BMx280_RA_DIG_P5 = 0x96,
  BMx280_RA_DIG_P6 = 0x98,
  BMx280_RA_DIG_P7 = 0x9A,
  BMx280_RA_DIG_P8 = 0x9C,
  BMx280_RA_DIG_P9 = 0x9E,

  BME280_RA_DIG_H1 = 0xA1,
  BME280_RA_DIG_H2 = 0xE1,
  BME280_RA_DIG_H3 = 0xE3,
  BME280_RA_DIG_H4 = 0xE4,
  BME280_RA_DIG_H5 = 0xE5,
  BME280_RA_DIG_H6 = 0xE7,

  BMx280_RA_CHIPID = 0xD0,
  BMx280_RA_VERSION = 0xD1,
  BMx280_RA_SOFTRESET = 0xE0,

  BME280_RA_CAL26 = 0xE1, // 0xE1..0xF0

  BME280_RA_CTRL_HUMI = 0xF2,
  BMx280_RA_STATUS = 0xF3,
  BMx280_RA_CTRL_MEAS = 0xF4,
  BMx280_RA_CONFIG = 0xF5,
  BMx280_RA_PRESSURE = 0xF7,
  BMx280_RA_TEMP = 0xFA,
  BME280_RA_HUMI = 0xFD
};

//
// RESET
#define BMx280_RESET			0xB6
// STATUS
#define BMx280_IM_UPDATE		1
#define BMx280_MEASURING		(1<<3)
// CTRL_HUMI
#define BME280_OVERSAMP_H_NONE	0
#define BME280_OVERSAMP_H_1		1
#define BME280_OVERSAMP_H_2		2
#define BME280_OVERSAMP_H_4		3
#define BME280_OVERSAMP_H_8		4
#define BME280_OVERSAMP_H_16	5
// CTRL_MEAS
#define BMx280_SLEEP_MODE		0
#define BMx280_FORCED_MODE		1
#define BMx280_NORMAL_MODE		3

#define BMx280_OVERSAMP_P_NONE	0
#define BMx280_OVERSAMP_P_1		(1<<2)
#define BMx280_OVERSAMP_P_2		(2<<2)
#define BMx280_OVERSAMP_P_4		(3<<2)
#define BMx280_OVERSAMP_P_8		(4<<2)
#define BMx280_OVERSAMP_P_16	(5<<2)

#define BMx280_OVERSAMP_T_NONE	0
#define BMx280_OVERSAMP_T_1		(1<<5)
#define BMx280_OVERSAMP_T_2		(2<<5)
#define BMx280_OVERSAMP_T_4		(3<<5)
#define BMx280_OVERSAMP_T_8		(4<<5)
#define BMx280_OVERSAMP_T_16	(5<<5)
// CONFIG
#define BMx280_SPI3W_DIS		0
#define BMx280_SPI3W_ENA		1
#define BMx280_FILER_OFF		0
#define BMx280_FILER_2			(1<<2)
#define BMx280_FILER_4			(2<<2)
#define BMx280_FILER_8			(3<<2)
#define BMx280_FILER_16			(4<<2)
#define BMx280_STANDBY_0T5MS	0
#define BMx280_STANDBY_62T5MS	(1<<5)
#define BMx280_STANDBY_125MS	(2<<5)
#define BMx280_STANDBY_250MS	(3<<5)
#define BMx280_STANDBY_500MS	(4<<5)
#define BMx280_STANDBY_1SEC		(5<<5)
#define BMx280_STANDBY_10MS		(6<<5)
#define BME280_STANDBY_20MS		(7<<5)
#define BMP280_STANDBY_2SEC		(6<<5)
#define BMP280_STANDBY_4SEC		(7<<5)
//
#define BME280_SET_CTRL_HUMI	(BME280_OVERSAMP_H_1)
#define BMx280_SET_CTRL_MEAS	(BMx280_OVERSAMP_T_1 | BMx280_OVERSAMP_P_1 | BMx280_FORCED_MODE)
#define BMx280_SET_CONFIG		(BMx280_FILER_OFF | BMx280_STANDBY_0T5MS | BMx280_SPI3W_DIS)
//
// struct resgisters BME280
typedef struct __attribute__ ((packed)) {
//	u8 status;
//	u8 ctrl_meas;
//	u8 config;
//	u8 none;
	u8 press[3];
	u8 temp[3];
#if USE_SENSOR_BME280
	u8 humi[2];
#endif
} bmx280_reg_t;
//
// struct resgisters OTP BME280
typedef struct {
	s32 t_fine ; /* t_fine carries fine temperature as global value */
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
	// конец чтения для BMP280 T1..xA0 = 24 байта
#if USE_SENSOR_BME280
	u8  xa0;
	u8  H1; // addr: 0xa1
	// конец чтения для BME280 T1..H2 = 26 байт
	s16 H2; // addr: 0xe1
	u8  H3; // addr: 0xe3
	s16 H4; // addr: 0xe4
	s16 H5; // addr: 0xe5
	s8  H6; // addr: 0xe7
#endif
} bmx280_dig_t;
//
#if USE_SENSOR_BME280
typedef struct _thsensor_coef_t {
	s32 val1_k;	// pressure_z
	u32 val2_k;	// humi_k 100% = 100*1024
	s16 val1_z;	// temp_z
	s16 val2_z;	// humi_z
} sensor_coef_t; // [12]

extern const sensor_coef_t def_bme_cfg;

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
	bmx280_dig_t dig;
} sensor_cfg_t;

#define sensor_cfg_send_size 18 //max 19

extern sensor_cfg_t sensor_cfg;

#endif

typedef struct _sensor_bmx280_t {
#if USE_SENSOR_BMP280
	s16 temp;
	u16 id;
	u8 i2c_addr;
#endif
	bmx280_dig_t dig;
}sensor_bmx280_t;

extern sensor_bmx280_t sensor_bmx280;

void init_sensor_bmx280(void);
int read_sensor_bmx280_cb(void);
int start_measure_sensor_bmx280_deep_sleep(void);

#if USE_SENSOR_BME280
#define init_sensor() init_sensor_bmx280()
#define read_sensor_cb() read_sensor_bmx280_cb()
#define start_measure_sensor_deep_sleep() start_measure_sensor_bmx280_deep_sleep()
#endif

#endif /* _BME280_H_ */
