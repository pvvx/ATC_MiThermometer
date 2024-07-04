/*
 * bthome_beacon.c
 *
 *  Created on: 17.10.23
 *      Author: pvvx
 */

#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#if USE_BTHOME_BEACON
#include "ble.h"
#include "battery.h"
#include "app.h"
#include "trigger.h"
#if (DEV_SERVICES & SERVICE_RDS)
#include "rds_count.h"
#endif
#include "bthome_beacon.h"
#include "ccm.h"

#define USE_OUT_AVERAGE_BATTERY 0

#if (DEV_SERVICES & SERVICE_BINDKEY)

/* Encrypted bthome nonce */
typedef struct __attribute__((packed)) _bthome_beacon_nonce_t{
    uint8_t  mac[6];
    uint16_t uuid16;	// = 0xfcd2
    uint8_t  info;		// = 0x41
	uint32_t cnt32;
} bthome_beacon_nonce_t, * pbthome_beacon_nonce_t;

RAM bthome_beacon_nonce_t bthome_nonce;

void bthome_beacon_init(void) {
	SwapMacAddress(bthome_nonce.mac, mac_public);
	bthome_nonce.uuid16 = ADV_BTHOME_UUID16;
	bthome_nonce.info = BtHomeID_Info_Encrypt;
}

// BTHOME adv security
typedef struct __attribute__((packed)) _adv_bthome_encrypt_t {
	adv_head_bth_t head; 	// 4 bytes
	uint8_t info;			// 1 byte
	uint8_t data[23];		// max 31-3-5 = 23 bytes
} adv_bthome_encrypt_t, * padv_bthome_encrypt_t;

/* Encrypt bthome data beacon packet */
_attribute_ram_code_ __attribute__((optimize("-Os")))
static void bthome_encrypt(uint8_t *pdata, uint32_t data_size) {
	padv_bthome_encrypt_t p = (padv_bthome_encrypt_t)&adv_buf.data;
	p->head.size = data_size + sizeof(p->head) - sizeof(p->head.size) + sizeof(p->info) + 4 + 4; // + mic + count
	adv_buf.data_size = p->head.size + 1;
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = bthome_nonce.uuid16;
	p->info = bthome_nonce.info;
	uint8_t *pmic = &adv_buf.data[data_size + sizeof(p->head) + sizeof(p->info)];
	*pmic++ = (uint8_t)adv_buf.send_count;
	*pmic++ = (uint8_t)(adv_buf.send_count>>8);
	*pmic++ = (uint8_t)(adv_buf.send_count>>16);
	*pmic++ = (uint8_t)(adv_buf.send_count>>24);
	bthome_nonce.cnt32 = adv_buf.send_count;
	aes_ccm_encrypt_and_tag((const unsigned char *)&bindkey,
					   (uint8_t*)&bthome_nonce, sizeof(bthome_nonce),
					   NULL, 0,
					   pdata, data_size,
					   p->data,
					   pmic, 4);
}

/* Create encrypted bthome data beacon packet */
_attribute_ram_code_ __attribute__((optimize("-Os")))
void bthome_encrypt_data_beacon(void) {
	uint8_t buf[16];
	if (adv_buf.call_count < cfg.measure_interval) {
		padv_bthome_data1_t p = (padv_bthome_data1_t)&buf;
		p->b_id = BtHomeID_battery;
		p->battery_level = measured_data.battery_level;
		p->t_id = BtHomeID_temperature;
		p->temperature = measured_data.temp; // x0.01 C
		p->h_id = BtHomeID_humidity;
		p->humidity = measured_data.humi; // x0.01 %
#if (DEV_SERVICES & SERVICE_PRESSURE)
		p->l_id = BtHomeID_volume32;
		p->volume = measured_data.pressure * 10UL; // 0.001 L
#endif
		bthome_encrypt(buf, sizeof(adv_bthome_data1_t));
	} else {
		adv_buf.call_count = 1;
		//adv_buf.send_count++;
		padv_bthome_data2_t p = (padv_bthome_data2_t)&buf;
		p->v_id = BtHomeID_voltage;
#if USE_OUT_AVERAGE_BATTERY
		p->battery_mv = measured_data.average_battery_mv; // x mV
#else
		p->battery_mv = measured_data.battery_mv; // x mV
#endif
#if (DEV_SERVICES & SERVICE_TH_TRG)
		p->s_id = BtHomeID_switch;
		p->swtch = trg.flg.trg_output;
#endif
#if (DEV_SERVICES & SERVICE_RDS)
		p->o1_id = BtHomeID_opened;
		p->opened1 = trg.flg.rds1_input;
#ifdef GPIO_RDS2
		p->o2_id = BtHomeID_opened;
		p->opened2 = trg.flg.rds2_input;
#endif
#endif
		bthome_encrypt(buf, sizeof(adv_bthome_data2_t));
	}
}

#if (DEV_SERVICES & SERVICE_RDS)
// n = RDS_TYPES
_attribute_ram_code_ __attribute__((optimize("-Os")))
void bthome_encrypt_event_beacon(uint8_t n) {
	uint8_t buf[16];
	padv_bthome_event1_t p = (padv_bthome_event1_t)&buf;
	p->o1_id = BtHomeID_opened;
	p->opened1 = trg.flg.rds1_input;
#ifdef GPIO_RDS2
	p->o2_id = BtHomeID_opened;
	p->opened2 = trg.flg.rds2_input;
#endif
	p->c_id = BtHomeID_count32;
	p->counter = rds.count1;
	bthome_encrypt(buf, sizeof(adv_bthome_event1_t));
}
#endif

#endif // #if (DEV_SERVICES & SERVICE_BINDKEY)

_attribute_ram_code_ __attribute__((optimize("-Os")))
void bthome_data_beacon(void) {
	padv_bthome_ns1_t p = (padv_bthome_ns1_t)&adv_buf.data;
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_BTHOME_UUID16;
	p->info = BtHomeID_Info;
	p->p_id = BtHomeID_PacketId;
	if (adv_buf.call_count < cfg.measure_interval) {
		p->pid = (uint8_t)adv_buf.send_count;
		p->data.b_id = BtHomeID_battery;
		p->data.battery_level = measured_data.battery_level;
		p->data.t_id = BtHomeID_temperature;
		p->data.temperature = measured_data.temp; // x0.01 C
		p->data.h_id = BtHomeID_humidity;
		p->data.humidity = measured_data.humi; // x0.01 %
#if (DEV_SERVICES & SERVICE_PRESSURE)
		p->data.l_id = BtHomeID_volume32;
		p->data.volume = measured_data.pressure * 10UL; // 0.001 L
#endif
		p->head.size = sizeof(adv_bthome_ns1_t) - sizeof(p->head.size);
	} else {
		padv_bthome_ns2_t p = (padv_bthome_ns2_t)&adv_buf.data;
		adv_buf.call_count = 1;
		//adv_buf.send_count++;
		p->pid = (uint8_t)adv_buf.send_count;
		p->data.v_id = BtHomeID_voltage;
#if USE_OUT_AVERAGE_BATTERY
		p->data.battery_mv = measured_data.average_battery_mv; // x mV
#else
		p->data.battery_mv = measured_data.battery_mv; // x mV
#endif
#if (DEV_SERVICES & SERVICE_TH_TRG)
		p->data.s_id = BtHomeID_switch;
		p->data.swtch = trg.flg.trg_output;
#endif
#if (DEV_SERVICES & SERVICE_RDS)
		p->data.o1_id = BtHomeID_opened;
		p->data.opened1 = trg.flg.rds1_input;
#ifdef GPIO_RDS2
		p->data.o2_id = BtHomeID_opened;
		p->data.opened2 = trg.flg.rds2_input;
#endif
#endif
		p->head.size = sizeof(adv_bthome_ns2_t) - sizeof(p->head.size);
	}
}

#if (DEV_SERVICES & SERVICE_RDS)
// n = RDS_TYPES
_attribute_ram_code_ __attribute__((optimize("-Os")))
void bthome_event_beacon(uint8_t n) {
	padv_bthome_ns_ev1_t p = (padv_bthome_ns_ev1_t)&adv_buf.data;
	p->head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_BTHOME_UUID16;
	p->info = BtHomeID_Info;
	p->p_id = BtHomeID_PacketId;
	p->pid = (uint8_t)adv_buf.send_count;
	p->head.size = sizeof(adv_bthome_ns_ev1_t) - sizeof(p->head.size);
	p->data.o1_id = BtHomeID_opened;
	p->data.opened1 = trg.flg.rds1_input;
#ifdef GPIO_RDS2
	p->data.o2_id = BtHomeID_opened;
	p->data.opened2 = trg.flg.rds2_input;
#endif
	p->data.c_id = BtHomeID_count32;
	p->data.counter = rds.count1;
	adv_buf.data_size = sizeof(adv_bthome_ns_ev1_t);
}
#endif

#endif // USE_BTHOME_BEACON
