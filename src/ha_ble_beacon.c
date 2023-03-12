/*
 * ha_ble_beacon.c
 *
 *  Created on: 07.03.2022
 *      Author: pvvx
 */

#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if USE_HA_BLE_BEACON
#include "ble.h"
#include "battery.h"
#include "app.h"
#if	USE_TRIGGER_OUT
#include "trigger.h"
#include "rds_count.h"
#endif
#include "ha_ble_beacon.h"
#include "ccm.h"

#if USE_SECURITY_BEACON

/* Encrypted ha-ble nonce */
typedef struct __attribute__((packed)) _ha_ble_beacon_nonce_t{
    uint8_t  mac[6];
    uint16_t uuid16;
	uint32_t cnt32;
} ha_ble_beacon_nonce_t, * pha_ble_beacon_nonce_t;

RAM ha_ble_beacon_nonce_t nonce;

void ha_ble_beacon_init(void) {
	SwapMacAddress(nonce.mac, mac_public);
	nonce.uuid16 = ADV_HA_BLE_SE_UUID16;
}

// HA_BLE adv security
typedef struct __attribute__((packed)) _adv_ha_ble_encrypt_t {
	adv_head_uuid16_t head;
	uint8_t data[31-4];
} adv_ha_ble_encrypt_t, * padv_ha_ble_encrypt_t;

/* Encrypt ha-ble data beacon packet */
_attribute_ram_code_ __attribute__((optimize("-Os")))
static void ha_ble_encrypt(uint8_t *pdata, uint32_t data_size) {
	padv_ha_ble_encrypt_t p = (padv_ha_ble_encrypt_t)&adv_buf.data;
	p->head.size = data_size + sizeof(p->head) + 7;
	adv_buf.data_size = p->head.size + 1;
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = nonce.uuid16;
	uint8_t *pmic = &adv_buf.data[data_size + sizeof(p->head)];
	*pmic++ = (uint8_t)adv_buf.send_count;
	*pmic++ = (uint8_t)(adv_buf.send_count>>8);
	*pmic++ = (uint8_t)(adv_buf.send_count>>16);
	*pmic++ = (uint8_t)(adv_buf.send_count>>24);
	nonce.cnt32 = adv_buf.send_count;
	uint8_t aad = 0x11;
	aes_ccm_encrypt_and_tag((const unsigned char *)&bindkey,
					   (uint8_t*)&nonce, sizeof(nonce),
					   &aad, sizeof(aad),
					   pdata, data_size,
					   p->data,
					   pmic, 4);
}

/* Create encrypted ha-ble data beacon packet */
_attribute_ram_code_ __attribute__((optimize("-Os")))
void ha_ble_encrypt_data_beacon(void) {
	uint8_t buf[20];
	if (adv_buf.call_count < cfg.measure_interval) {
		padv_ha_ble_data1_t p = (padv_ha_ble_data1_t)&buf;
		p->t_st = HaBleType_sint + sizeof(p->t_id) + sizeof(p->temperature);
		p->t_id = HaBleID_temperature;
		p->temperature = measured_data.temp; // x0.01 C
		p->h_st = HaBleType_uint + sizeof(p->h_id) + sizeof(p->humidity);
		p->h_id = HaBleID_humidity;
		p->humidity = measured_data.humi; // x0.01 %
		p->b_st = HaBleType_uint + sizeof(p->b_id) + sizeof(p->battery_level);
		p->b_id = HaBleID_battery;
		p->battery_level = measured_data.battery_level;
		ha_ble_encrypt(buf, sizeof(adv_ha_ble_data1_t));
	} else {
		adv_buf.call_count = 1;
		adv_buf.send_count++;
		padv_ha_ble_data2_t p = (padv_ha_ble_data2_t)&buf;
		p->s_st = HaBleType_uint + sizeof(p->s_id) + sizeof(p->swtch);
		p->s_id = HaBleID_switch;
		p->swtch = trg.flg.trg_output;
		p->v_st = HaBleType_uint + sizeof(p->v_id) + sizeof(p->battery_mv);
		p->v_id = HaBleID_voltage;
		p->battery_mv = measured_data.average_battery_mv; // x mV
		ha_ble_encrypt(buf, sizeof(adv_ha_ble_data2_t));
	}
}

#if	USE_TRIGGER_OUT && USE_WK_RDS_COUNTER
_attribute_ram_code_ __attribute__((optimize("-Os")))
void ha_ble_encrypt_event_beacon(uint8_t n) {
	uint8_t buf[20];
	if (n == RDS_SWITCH) {
		padv_ha_ble_event1_t p = (padv_ha_ble_event1_t)&buf;
		p->o_st = HaBleType_uint + sizeof(p->o_id) + sizeof(p->opened);
		p->o_id = HaBleID_opened;
		p->opened = trg.flg.rds_input;
		p->c_st = HaBleType_uint + sizeof(p->c_id) + sizeof(p->counter);
		p->c_id = HaBleID_count;
		p->counter = rds.count;
		ha_ble_encrypt(buf, sizeof(adv_ha_ble_event1_t));
	} else {
		padv_ha_ble_event2_t p = (padv_ha_ble_event2_t)&buf;
		p->c_st = HaBleType_uint + sizeof(p->c_id) + sizeof(p->counter);
		p->c_id = HaBleID_count;
		p->counter = rds.count;
		ha_ble_encrypt(buf, sizeof(adv_ha_ble_event2_t));
	}
}
#endif // USE_TRIGGER_OUT

#endif // USE_SECURITY_BEACON

_attribute_ram_code_ __attribute__((optimize("-Os")))
void ha_ble_data_beacon(void) {
	padv_ha_ble_ns1_t p = (padv_ha_ble_ns1_t)&adv_buf.data;
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_HA_BLE_NS_UUID16;
	p->p_st = HaBleType_uint + sizeof(p->p_id) + sizeof(p->pid);
	p->p_id = HaBleID_PacketId;
	if (adv_buf.call_count < cfg.measure_interval) {
		p->head.size = sizeof(adv_ha_ble_ns1_t) - sizeof(p->head.size);
		p->pid = (uint8_t)adv_buf.send_count;
		p->data.t_st = HaBleType_sint + sizeof(p->data.t_id) + sizeof(p->data.temperature);
		p->data.t_id = HaBleID_temperature;
		p->data.temperature = measured_data.temp; // x0.01 C
		p->data.h_st = HaBleType_uint + sizeof(p->data.h_id) + sizeof(p->data.humidity);
		p->data.h_id = HaBleID_humidity;
		p->data.humidity = measured_data.humi; // x0.01 %
		p->data.b_st = HaBleType_uint + sizeof(p->data.b_id) + sizeof(p->data.battery_level);
		p->data.b_id = HaBleID_battery;
		p->data.battery_level = measured_data.battery_level;
	} else {
		p->head.size = sizeof(adv_ha_ble_ns2_t) - sizeof(p->head.size);
		padv_ha_ble_ns2_t p = (padv_ha_ble_ns2_t)&adv_buf.data;
		adv_buf.call_count = 1;
		adv_buf.send_count++;
		p->pid = (uint8_t)adv_buf.send_count;
		p->data.s_st = HaBleType_uint + sizeof(p->data.s_id) + sizeof(p->data.swtch);
		p->data.s_id = HaBleID_switch;
		p->data.swtch = trg.flg.trg_output;
		p->data.v_st = HaBleType_uint + sizeof(p->data.v_id) + sizeof(p->data.battery_mv);
		p->data.v_id = HaBleID_voltage;
		p->data.battery_mv = measured_data.average_battery_mv; // x mV
	}
}

#if	USE_TRIGGER_OUT && USE_WK_RDS_COUNTER
_attribute_ram_code_ __attribute__((optimize("-Os")))
void ha_ble_event_beacon(uint8_t n) {
	padv_ha_ble_ns_ev1_t p = (padv_ha_ble_ns_ev1_t)&adv_buf.data;
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_HA_BLE_NS_UUID16;
	p->p_st = HaBleType_uint + sizeof(p->p_id) + sizeof(p->pid);
	p->p_id = HaBleID_PacketId;
	p->pid = (uint8_t)adv_buf.send_count;
	if (n == RDS_SWITCH) {
		p->head.size = sizeof(adv_ha_ble_ns_ev1_t) - sizeof(p->head.size);
		p->data.o_st = HaBleType_uint + sizeof(p->data.o_id) + sizeof(p->data.opened);
		p->data.o_id = HaBleID_opened;
		p->data.opened = trg.flg.rds_input;
		p->data.c_st = HaBleType_uint + sizeof(p->data.c_id) + sizeof(p->data.counter);
		p->data.c_id = HaBleID_count;
		p->data.counter = rds.count;
		adv_buf.data_size = sizeof(adv_ha_ble_ns_ev1_t);
	} else {
		padv_ha_ble_ns_ev2_t p = (padv_ha_ble_ns_ev2_t)&adv_buf.data;
		p->head.size = sizeof(adv_ha_ble_ns_ev2_t) - sizeof(p->head.size);
		p->data.c_st = HaBleType_uint + sizeof(p->data.c_id) + sizeof(p->data.counter);
		p->data.c_id = HaBleID_count;
		p->data.counter = rds.count;
		adv_buf.data_size = sizeof(adv_ha_ble_ns_ev2_t);
	}
}
#endif // USE_TRIGGER_OUT

#endif // USE_HA_BLE_BEACON
