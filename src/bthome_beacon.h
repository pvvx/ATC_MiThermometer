/*
 * bthome_beacon.h
 *
 *  Created on: 17.10.23
 *      Author: pvvx
 */

#ifndef BTHOME_BEACON_H_
#define BTHOME_BEACON_H_

#include "app_config.h"

#define ADV_BTHOME_UUID16 0xFCD2 // 16-bit UUID Service 0xFCD2 BTHOME

#define BtHomeID_Info	0x40
#define BtHomeID_Info_Encrypt	0x41


// https://github.com/custom-components/ble_monitor/issues/548
typedef enum {
	BtHomeID_PacketId = 0,			//0x00, uint8
	BtHomeID_battery = 0x01,		//0x01, uint8, %
	BtHomeID_temperature = 0x02,	//0x02, sint16, 0.01 °C
	BtHomeID_humidity = 0x03,		//0x03, uint16, 0.01 %
	BtHomeID_pressure = 0x04,		//0x04, uint24, 0.01 hPa
	BtHomeID_illuminance = 0x05,	//0x05, uint24, 0.01 lux
	BtHomeID_weight	= 0x06,			//0x06, uint16, 0.01 kg
	BtHomeID_weight_lb = 0x07,      //0x07, uint16, 0.01 lb
	BtHomeID_dewpoint = 0x08,		//0x08, sint16, 0.01 °C
	BtHomeID_count8 = 0x09,			//0x09,	uint8
	BtHomeID_energy24 = 0x0a,		//0x0A, uint24, 0.001 kWh
	BtHomeID_power24 = 0x0b,		//0x0B, uint24, 0.01 W
	BtHomeID_voltage = 0x0c,		//0x0C, uint16, 0.001 V
	BtHomeID_pm2x5 = 0x0d,			//0x0D, uint16, kg/m3
	BtHomeID_pm10 = 0x0e,			//0x0E, uint16, kg/m3
	BtHomeID_boolean = 0x0f,		//0x0F, uint8, generic boolean
	BtHomeID_switch = 0x10,		  	//0x10, uint8, power on/off
	BtHomeID_opened = 0x11,			//0x11, uint8, opening =0 Closed, = 1 Open
	BtHomeID_co2 = 0x12,			//0x12, uint16
	BtHomeID_tvoc = 0x13,			//0x13, uint16
	BtHomeID_moisture16 = 0x14,		//0x14, uint16, 0.01
	BtHomeID_low_battery = 0x15,	//0x15, uint8, =1 low
	BtHomeID_chg_battery = 0x16,	//0x16, uint8, battery charging
	BtHomeID_carbon_monoxide = 0x17,//0x17, uint8, carbon monoxide
	BtHomeID_cold = 0x18,			//0x18, uint8
	BtHomeID_connectivity = 0x19,	//0x19, uint8
	BtHomeID_door = 0x1a,			//0x1a, uint8, =0 Closed, =1 Open
	BtHomeID_garage_door = 0x1b,	//0x1b, uint8, =0 Closed, =1 Open
	BtHomeID_gas = 0x1c,			//0x1c, uint8, =1 Detected
	BtHomeID_heat = 0x1d,			//0x1d, uint8, =1 Hot
	BtHomeID_light = 0x1e,			//0x1e, uint8, =1 Light detected
	BtHomeID_lock = 0x1f,			//0x1f, uint8, =1 Unlocked
	BtHomeID_moisture_b = 0x20,		//0x20, uint8, =0 Dry, =1 Wet
	BtHomeID_motion = 0x21,			//0x21, uint8, =0 Clear, =1 Detected
	BtHomeID_moving = 0x22,			//0x22, uint8, =1 Moving
	BtHomeID_occupancy = 0x23,		//0x23, uint8, =1 Detected
	BtHomeID_plug = 0x24,			//0x24, uint8, =0 Unplugged, =1 Plugged in
	BtHomeID_presence = 0x25,		//0x25, uint8, =0 Away, =1 Home
	BtHomeID_problem = 0x26,		//0x26, uint8, =0 Ok, =1 Problem
	BtHomeID_running = 0x27,		//0x27, uint8, =0 Not Running, =1 Running
	BtHomeID_safety = 0x28,			//0x28, uint8, =0 Unsafe, =1 Safe
	BtHomeID_smoke = 0x29,			//0x29, uint8, =0 Clear, =1 Detected
	BtHomeID_sound = 0x2a,			//0x2a, uint8, =0 Clear, =1 Detected
	BtHomeID_tamper = 0x2b,			//0x2b, uint8, =0 Off, =1 On
	BtHomeID_vibration = 0x2c,		//0x2c, uint8, =0 Clear, =1 Detected
	BtHomeID_window = 0x2d,			//0x2d, uint8, =0 Closed, =1 Open
	BtHomeID_humidity8 = 0x2e,		//0x2e, uint8
	BtHomeID_moisture8 = 0x2f,		//0x2f, uint8
	BtHomeID_0x30 = 0x30,		//0x30, uint8
	BtHomeID_0x31 = 0x31,		//0x31, uint8
	BtHomeID_0x32 = 0x32,		//0x32, uint8
	BtHomeID_0x33 = 0x33,		//0x33, uint8
	BtHomeID_0x34 = 0x34,		//0x34, uint8
	BtHomeID_0x35 = 0x35,		//0x35, uint8
	BtHomeID_0x36 = 0x36,		//0x36, uint8
	BtHomeID_0x37 = 0x37,		//0x37, uint8
	BtHomeID_0x38 = 0x38,		//0x38, uint8
	BtHomeID_0x39 = 0x39,		//0x39, uint8
	BtHomeID_button = 0x3a,			//0x3a, uint8, =1 press, =2 double_press ... https://bthome.io/format/
	BtHomeID_0x3b = 0x3b,		//0x3b, uint8
	BtHomeID_dimmer = 0x3c,			//0x3c, uint16 ?, =1 rotate left 3 steps, ... https://bthome.io/format/
	BtHomeID_count16 = 0x3d,		//0x3d, uint16
	BtHomeID_count32 = 0x3e,		//0x3e, uint32
	BtHomeID_rotation = 0x3f,		//0x3f, sint16, 0.1
	BtHomeID_distance_mm  = 0x40,	//0x40, uint16, mm
	BtHomeID_distance_m = 0x41,		//0x41, uint16, m, 0.1
	BtHomeID_duration = 0x42,		//0x42, uint24, 0.01
	BtHomeID_current = 0x43,		//0x43, uint16, 0.001 A
	BtHomeID_speed = 0x44,			//0x44, uint16, 0.01
	BtHomeID_temperature_01 = 0x45,	//0x45, sint16, 0.1
	BtHomeID_UV_index = 0x46,		//0x46, uint8, 0.1
	BtHomeID_volume16_01 = 0x47,	//0x47, uint16, 0.1 L
	BtHomeID_volume16 = 0x48,		//0x48, uint16, 1 L
	BtHomeID_Flow_Rate = 0x49,		//0x49, uint16, 0.001
	BtHomeID_voltage_01 = 0x4a,		//0x4a, uint16, 0.1 V
	BtHomeID_gas24 = 0x4b,			//0x4b, uint24, 0.001
	BtHomeID_gas32 = 0x4c,			//0x4c, uint32, 0.001
	BtHomeID_energy32 = 0x4d,		//0x4d, uint32, 0.001 kWh
	BtHomeID_volume32 = 0x4e,		//0x4e, uint32, 0.001 L
	BtHomeID_water32 = 0x4f,		//0x4f, uint32, 0.001
	BtHomeID_timestamp = 0x50,		//0x50, uint48
	BtHomeID_acceleration = 0x51,	//0x51, uint16, 0.001
	BtHomeID_gyroscope = 0x52,		//0x52, uint16, 0.001
	BtHomeID_text = 0x53,			//0x53, size uint8, uint8[]
	BtHomeID_raw = 0x54				//0x54, size uint8, uint8[]
} BtHomeIDs_e;

typedef struct __attribute__((packed)) _adv_head_bth_t {
	uint8_t		size;   // =
	uint8_t		type;	// = 0x16, 16-bit UUID
	uint16_t	UUID;	// = 0xFCD2, GATT Service BTHome
} adv_head_bth_t, * padv_head_bth_t;

typedef struct __attribute__((packed)) _adv_bthome_data1_t {
	uint8_t		b_id;	// = BtHomeID_battery
	uint8_t		battery_level; // 0..100 %
#if (DEV_SERVICES & SERVICE_THS)
	uint8_t		t_id;	// = BtHomeID_temperature
	int16_t		temperature; // x 0.01 degree
	uint8_t		h_id;	// = BtHomeID_humidity
	uint16_t	humidity; // x 0.01 %
#endif
#if (DEV_SERVICES & SERVICE_PRESSURE)
	uint8_t		l_id;	// = BtHomeID_volume16_01
	uint32_t	volume; // x 0.001 l
#endif
#if (DEV_SERVICES & SERVICE_IUS)
	uint8_t		u_id;	// BtHomeID_voltage
	uint16_t	voltage; // x 0.001 V
	uint8_t		i_id; 	// BtHomeID_current
	uint16_t	current; // x 0.001 A
#endif
} adv_bthome_data1_t, * padv_bthome_data1_t; // max 15 bytes!

typedef struct __attribute__((packed)) _adv_bthome_data2_t {
#if (DEV_SERVICES & SERVICE_IUS)
	uint8_t		e_id; // BtHomeID_energy32
	uint32_t	energy; // x 0.001 W
#else
	uint8_t		v_id;	// = BtHomeID_voltage
	uint16_t	battery_mv; // mV
#endif
#if (DEV_SERVICES & SERVICE_TH_TRG)
	uint8_t		s_id;	// = BtHomeID_opened / BtHomeID_switch ?
	uint8_t		swtch;
#endif
#if (DEV_SERVICES & SERVICE_RDS)
	uint8_t		o1_id;	// = BtHomeID_opened ?
	uint8_t		opened1;
#ifdef GPIO_RDS2
	uint8_t		o2_id;	// = BtHomeID_opened ?
	uint8_t		opened2;
#endif
#endif
} adv_bthome_data2_t, * padv_bthome_data2_t; // max 15 bytes!

typedef struct __attribute__((packed)) _adv_bthome_event1_t {
	uint8_t		o1_id;	// = BtHomeID_opened ?
	uint8_t		opened1;
#ifdef GPIO_RDS2
	uint8_t		o2_id;	// = BtHomeID_opened ?
	uint8_t		opened2;
#endif
	uint8_t		c_id;	// = BtHomeID_count32
	uint32_t	counter;
} adv_bthome_event1_t, * padv_bthome_event1_t; // max 15 bytes!

// BTHOME data1, no security
typedef struct __attribute__((packed)) _adv_bthome_ns1_t {
	adv_head_bth_t head;
	uint8_t		info;	// = 0x40 BtHomeID_Info
	uint8_t		p_id;	// = BtHomeID_PacketId
	uint8_t		pid;	// PacketId (measurement count)
	adv_bthome_data1_t data; // max 28 - 7 = 21 bytes
} adv_bthome_ns1_t, * padv_bthome_ns1_t; // max 31 - 3(BLE flags) = 28 bytes!

// BTHOME data2, no security
typedef struct __attribute__((packed)) _adv_bthome_ns2_t {
	adv_head_bth_t head;
	uint8_t		info;	// = 0x40 BtHomeID_Info
	uint8_t		p_id;	// = BtHomeID_PacketId
	uint8_t		pid;	// PacketId (measurement count)
	adv_bthome_data2_t data; // max 28 - 13 = 15 bytes
} adv_bthome_ns2_t, * padv_bthome_ns2_t;

// BTHOME event1, no security
typedef struct __attribute__((packed)) _adv_bthome_ns_ev1_t {
	adv_head_bth_t head;
	uint8_t		info;	// = 0x40 BtHomeID_Info
	uint8_t		p_id;	// = BtHomeID_PacketId
	uint8_t		pid;	// PacketId (!= measurement count)
	adv_bthome_event1_t data;
} adv_bthome_ns_ev1_t, * padv_bthome_ns_ev1_t;

#if (DEV_SERVICES & SERVICE_BINDKEY)

// BTHOME data1, security
typedef struct __attribute__((packed)) _adv_bthome_d1_t {
	adv_head_bth_t head; // 4 bytes
	uint8_t		info;	 // = 0x41 BtHomeID_Info_Encrypt
	adv_bthome_data1_t data; // max 28 - 13 = 15 bytes
	uint32_t	count_id; //  4 bytes
	uint8_t		mic[4]; //  4 bytes
} adv_bthome_1_t, * padv_bthome_d1_t; // max 31 - 3(BLE flags) = 28 bytes!

// BTHOME data2, security
typedef struct __attribute__((packed)) _adv_bthome_d2_t {
	adv_head_bth_t head;
	uint8_t		info;	// = 0x41 BtHomeID_Info_Encrypt
	adv_bthome_data2_t data; // max 28 - 13 = 15 bytes
	uint32_t	count_id;
	uint8_t		mic[4];
} adv_bthome_d2_t, * padv_bthome_d2_t;

// BTHOME event1, security
typedef struct __attribute__((packed)) _adv_bthome_ev1_t {
	adv_head_bth_t head;
	uint8_t		info;	// = 0x41 BtHomeID_Info_Encrypt
	adv_bthome_event1_t data;
	uint32_t	count_id;
	uint8_t		mic[4];
} adv_bthome_ev1_t, * padv_bthome_ev1_t;

void bthome_beacon_init(void);
void bthome_encrypt_data_beacon(void);
#if (DEV_SERVICES & SERVICE_RDS)
void bthome_encrypt_event_beacon(uint8_t n); // n = RDS_TYPES
#endif
#endif // #if (DEV_SERVICES & SERVICE_BINDKEY)

void bthome_data_beacon(void);
#if (DEV_SERVICES & SERVICE_RDS)
void bthome_event_beacon(uint8_t n); // n = RDS_TYPES
#endif
#endif /* BTHOME_BEACON_H_ */
