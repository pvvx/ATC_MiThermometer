/*
 * custom_beacon.h
 *
 *  Created on: 07.03.2022
 *      Author: pvvx
 */

#ifndef CUSTOM_BEACON_H_
#define CUSTOM_BEACON_H_

#define ADV_CUSTOM_UUID16 0x181A // 16-bit UUID Service 0x181A Environmental Sensing

#define ADV_UUID16_DigitalStateBits	0x2A56 // 16-bit UUID Digital bits, Out bits control (LEDs control)
#define ADV_UUID16_AnalogOutValues	0x2A58 // 16-bit UUID Analog values (DACs control)
#define ADV_UUID16_Aggregate		0x2A5A // 16-bit UUID Aggregate, The Aggregate Input is an aggregate of the Digital Input Characteristic value (if available) and ALL Analog Inputs available.
#define ADV_UUID16_Count24bits		0x2AEB // 16-bit UUID Count 24 bits
#define ADV_UUID16_Count16bits 		0x2AEA // 16-bit UUID Count 16 bits


// GATT Service 0x181A Environmental Sensing
// All data little-endian
typedef struct __attribute__((packed)) _adv_custom_t {
	uint8_t		size;	// = 18
	uint8_t		uid;	// = 0x16, 16-bit UUID
	uint16_t	UUID;	// = 0x181A, GATT Service 0x181A Environmental Sensing
	uint8_t		MAC[6]; // [0] - lo, .. [6] - hi digits
	int16_t		temperature; // x 0.01 degree
	uint16_t	humidity; // x 0.01 %
	uint16_t	battery_mv; // mV
	uint8_t		battery_level; // 0..100 %
	uint8_t		counter; // measurement count
	uint8_t		flags;
} adv_custom_t, * padv_custom_t;

// GATT Service 0x181A Environmental Sensing
// mixture of little-endian and big-endian!
typedef struct __attribute__((packed)) _adv_atc1441_t {
	uint8_t		size;	// = 16
	uint8_t		uid;	// = 0x16, 16-bit UUID
	uint16_t	UUID;	// = 0x181A, GATT Service 0x181A Environmental Sensing (little-endian)
	uint8_t		MAC[6]; // [0] - hi, .. [6] - lo digits (big-endian!)
	uint8_t		temperature[2]; // x 0.1 degree (big-endian!)
	uint8_t		humidity; // x 1 %
	uint8_t		battery_level; // 0..100 %
	uint8_t		battery_mv[2]; // mV (big-endian!)
	uint8_t		counter; // measurement count
} adv_atc1441_t, * padv_atc1441_t;

/* Encrypted custom beacon structs */
// https://github.com/pvvx/ATC_MiThermometer/issues/94#issuecomment-842846036

typedef struct __attribute__((packed)) _adv_cust_head_t {
	uint8_t		size;		//@0 = 11
	uint8_t		uid;		//@1 = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;		//@2..3 = GATT Service 0x181A Environmental Sensing (little-endian) (or 0x181C 'User Data'?)
	uint8_t		counter;	//@4 0..0xff Measurement count, Serial number, used for de-duplication, different event or attribute reporting requires different Frame Counter
} adv_cust_head_t, * padv_cust_head_t;

typedef struct __attribute__((packed)) _adv_cust_data_t {
	int16_t		temp;		//@0
	uint16_t	humi;		//@2
	uint8_t		bat;		//@4
	uint8_t		trg;		//@5
} adv_cust_data_t, * padv_cust_data_t;

typedef struct __attribute__((packed)) _adv_cust_enc_t {
	adv_cust_head_t head;
	adv_cust_data_t data;	//@5
	uint8_t		mic[4];		//@8..11
} adv_cust_enc_t, * padv_cust_enc_t;

/* Encrypted atc beacon structs */

typedef struct __attribute__((packed)) _adv_atc_data_t {
	uint8_t		temp;		//@0
	uint8_t		humi;		//@1
	uint8_t		bat;		//@2
} adv_atc_data_t, * padv_atc_data_t;

typedef struct __attribute__((packed)) _adv_atc_enc_t {
	adv_cust_head_t head;
	adv_atc_data_t data;   //@5
	uint8_t		mic[4];		//@8..11
} adv_atc_enc_t, * padv_atc_enc_t;


void pvvx_data_beacon(void);
void atc_data_beacon(void);
#if USE_TRIGGER_OUT
void pvvx_event_beacon(uint8_t n); // n = RDS_TYPES
void atc_event_beacon(void);
#endif

#if USE_SECURITY_BEACON
void pvvx_encrypt_data_beacon(void); // n = RDS_TYPES
void atc_encrypt_data_beacon(void);

#if USE_TRIGGER_OUT
void pvvx_encrypt_event_beacon(uint8_t n); // n = RDS_TYPES
void atc_encrypt_event_beacon(void); // n = RDS_TYPES
#endif
#endif // USE_SECURITY_BEACON

#endif /* CUSTOM_BEACON_H_ */
