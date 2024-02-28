/*
 * ha_ble_beacon.h
 *
 *  Created on: 07.03.2022
 *      Author: pvvx
 */

#ifndef HA_BLE_BEACON_H_
#define HA_BLE_BEACON_H_

#define ADV_HA_BLE_NS_UUID16 0x181C // 16-bit UUID Service 0x181C HA_BLE, no security
#define ADV_HA_BLE_SE_UUID16 0x181E // 16-bit UUID Service 0x181E HA_BLE, security enable

// https://github.com/custom-components/ble_monitor/issues/548
typedef enum {
	HaBleID_PacketId = 0,	//0x00, uint8
	HaBleID_battery,      //0x01, uint8, %
	HaBleID_temperature,  //0x02, sint16, 0.01 °C
	HaBleID_humidity,     //0x03, uint16, 0.01 %
	HaBleID_pressure,     //0x04, uint24, 0.01 hPa
	HaBleID_illuminance,  //0x05, uint24, 0.01 lux
	HaBleID_weight,       //0x06, uint16, 0.01 kg
	HaBleID_weight_s,     //0x07, string, kg
	HaBleID_dewpoint,     //0x08, sint16, 0.01 °C
	HaBleID_count,        //0x09,	uint8/16/24/32
	HaBleID_energy,       //0x0A, uint24, 0.001 kWh
	HaBleID_power,        //0x0B, uint24, 0.01 W
	HaBleID_voltage,      //0x0C, uint16, 0.001 V
	HaBleID_pm2x5,        //0x0D, uint16, kg/m3
	HaBleID_pm10,         //0x0E, uint16, kg/m3
	HaBleID_boolean,      //0x0F, uint8
	HaBleID_switch,		  //0x10
	HaBleID_opened		  //0x11
} HaBleIDs_e;

// Type bit 5-7
typedef enum {
	HaBleType_uint = 0,
	HaBleType_sint = (1<<5),
	HaBleType_float = (2<<5),
	HaBleType_string  = (3<<5),
	HaBleType_MAC  = (4<<5)
} HaBleTypes_e;

typedef struct __attribute__((packed)) _adv_head_uuid16_t {
	uint8_t		size;   // =
	uint8_t		type;	// = 0x16, 16-bit UUID
	uint16_t	UUID;	// = 0x181C, GATT Service HA_BLE
} adv_head_uuid16_t, * padv_head_uuid16_t;

typedef struct __attribute__((packed)) _adv_ha_ble_data1_t {
	uint8_t		t_st;
	uint8_t		t_id;	// = HaBleID_temperature
	int16_t		temperature; // x 0.01 degree
	uint8_t		h_st;
	uint8_t		h_id;	// = HaBleID_humidity
	uint16_t	humidity; // x 0.01 %
	uint8_t		b_st;
	uint8_t		b_id;	// = HaBleID_battery
	uint8_t		battery_level; // 0..100 %
} adv_ha_ble_data1_t, * padv_ha_ble_data1_t;

typedef struct __attribute__((packed)) _adv_ha_ble_data2_t {
#if (DEV_SERVICES & SERVICE_TH_TRG)
	uint8_t		s_st;
	uint8_t		s_id;	// = HaBleID_switch ?
	uint8_t		swtch;
#endif
	uint8_t		v_st;
	uint8_t		v_id;	// = HaBleID_voltage
	uint16_t	battery_mv; // mV
} adv_ha_ble_data2_t, * padv_ha_ble_data2_t;

typedef struct __attribute__((packed)) _adv_ha_ble_event1_t {
	uint8_t		o_st;
	uint8_t		o_id;	// = HaBleID_opened ?
	uint8_t		opened;
	uint8_t		c_st;
	uint8_t		c_id;	// = HaBleID_count
	uint32_t	counter; // count
} adv_ha_ble_event1_t, * padv_ha_ble_event1_t;

typedef struct __attribute__((packed)) _adv_ha_ble_event2_t {
	uint8_t		size;	// = 12
	uint8_t		uid;	// = 0x16, 16-bit UUID
	uint16_t	UUID;	// = 0x181C, GATT Service 0x181C
	uint8_t		p_st;
	uint8_t		p_id;	// = HaBleID_PacketId
	uint8_t		pid;	// PacketId (!= measurement count)
	uint8_t		c_st;
	uint8_t		c_id;	// = HaBleID_count
	uint32_t	counter; // count
} adv_ha_ble_event2_t, * padv_ha_ble_event2_t;

// HA_BLE data1, no security
typedef struct __attribute__((packed)) _adv_ha_ble_ns1_t {
	adv_head_uuid16_t head;
	uint8_t		p_st;
	uint8_t		p_id;	// = HaBleID_PacketId
	uint8_t		pid;	// PacketId (measurement count)
	adv_ha_ble_data1_t data;
} adv_ha_ble_ns1_t, * padv_ha_ble_ns1_t;

// HA_BLE data2, no security
typedef struct __attribute__((packed)) _adv_ha_ble_ns2_t {
	adv_head_uuid16_t head;
	uint8_t		p_st;
	uint8_t		p_id;	// = HaBleID_PacketId
	uint8_t		pid;	// PacketId (measurement count)
	adv_ha_ble_data2_t data;
} adv_ha_ble_ns2_t, * padv_ha_ble_ns2_t;

// HA_BLE event1, no security
typedef struct __attribute__((packed)) _adv_ha_ble_ns_ev1_t {
	adv_head_uuid16_t head;
	uint8_t		p_st;
	uint8_t		p_id;	// = HaBleID_PacketId
	uint8_t		pid;	// PacketId (!= measurement count)
	adv_ha_ble_event1_t data;
} adv_ha_ble_ns_ev1_t, * padv_ha_ble_ns_ev1_t;

// HA_BLE event2, no security
typedef struct __attribute__((packed)) _adv_ha_ble_ns_ev2_t {
	adv_head_uuid16_t head;
	uint8_t		p_st;
	uint8_t		p_id;	// = HaBleID_PacketId
	uint8_t		pid;	// PacketId (!= measurement count)
	adv_ha_ble_event2_t data;
} adv_ha_ble_ns_ev2_t, * padv_ha_ble_ns_ev2_t;

#if (DEV_SERVICES & SERVICE_BINDKEY)

// HA_BLE data1, security
typedef struct __attribute__((packed)) _adv_ha_ble_d1_t {
	adv_head_uuid16_t head;
	adv_ha_ble_data1_t data;
	uint32_t	count_id;
	uint8_t		mic[4];
} adv_ha_ble_1_t, * padv_ha_ble_d1_t;

// HA_BLE data2, security
typedef struct __attribute__((packed)) _adv_ha_ble_d2_t {
	adv_head_uuid16_t head;
	adv_ha_ble_data2_t data;
	uint32_t	count_id;
	uint8_t		mic[4];
} adv_ha_ble_d2_t, * padv_ha_ble_d2_t;

// HA_BLE event1, security
typedef struct __attribute__((packed)) _adv_ha_ble_ev1_t {
	adv_head_uuid16_t head;
	adv_ha_ble_event1_t data;
	uint32_t	count_id;
	uint8_t		mic[4];
} adv_ha_ble_ev1_t, * padv_ha_ble_ev1_t;

// HA_BLE event1, security
typedef struct __attribute__((packed)) _adv_ha_ble_ev2_t {
	adv_head_uuid16_t head;
	adv_ha_ble_event2_t data;
	uint32_t	count_id;
	uint8_t		mic[4];
} adv_ha_ble_ev2_t, * padv_ha_ble_ev2_t;

void ha_ble_beacon_init(void);
void ha_ble_encrypt_data_beacon(void);
#if (DEV_SERVICES & SERVICE_TH_TRG)
void ha_ble_encrypt_event_beacon(uint8_t n); // n = RDS_TYPES
#endif
#endif // #if (DEV_SERVICES & SERVICE_BINDKEY)

void ha_ble_data_beacon(void);
#if (DEV_SERVICES & SERVICE_TH_TRG)
void ha_ble_event_beacon(uint8_t n); // n = RDS_TYPES
#endif
#endif /* HA_BLE_BEACON_H_ */
