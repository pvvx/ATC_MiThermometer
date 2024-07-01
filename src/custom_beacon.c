/*
 * custom_beacon.c
 *
 *  Created on: 07.03.2022
 *      Author: pvvx
 */
#include <stdint.h>
#include "tl_common.h"
#include "app_config.h"
#include "ble.h"
#include "app.h"
#include "trigger.h"
#if (DEV_SERVICES & SERVICE_RDS)
#include "rds_count.h"
#endif
#include "custom_beacon.h"
#include "ccm.h"

#if (DEV_SERVICES & SERVICE_BINDKEY)

/* Encrypted atc/custom nonce */
typedef struct __attribute__((packed)) _enc_beacon_nonce_t{
    uint8_t  MAC[6];
    adv_cust_head_t head;
} enc_beacon_nonce_t;

/* Create encrypted custom beacon packet
 * https://github.com/pvvx/ATC_MiThermometer/issues/94#issuecomment-842846036 */

__attribute__((optimize("-Os")))
void atc_encrypt_data_beacon(void) {
	padv_atc_enc_t p = (padv_atc_enc_t)&adv_buf.data;
	enc_beacon_nonce_t cbn;
	adv_atc_data_t data;
	uint8_t aad = 0x11;
	adv_buf.update_count = -1; // next call if next measured
	p->head.size = sizeof(adv_atc_enc_t) - 1;
	p->head.uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_CUSTOM_UUID16; // GATT Service 0x181A Environmental Sensing (little-endian) (or 0x181C 'User Data'?)
	p->head.counter = (uint8_t)adv_buf.send_count;
	data.temp = (measured_data.temp + 25) / 50 + 4000 / 50;
	data.humi = (measured_data.humi + 25) / 50;
	data.bat = measured_data.battery_level
#if (DEV_SERVICES & SERVICE_TH_TRG)
	| ((trg.flg.trigger_on)? 0x80 : 0)
#endif
	;
	memcpy(cbn.MAC, mac_public, sizeof(cbn.MAC));
	memcpy(&cbn.head, p, sizeof(cbn.head));
	aes_ccm_encrypt_and_tag((const unsigned char *)&bindkey,
					   (uint8_t*)&cbn, sizeof(cbn),
					   &aad, sizeof(aad),
					   (uint8_t *)&data, sizeof(data),
					   (uint8_t *)&p->data,
					   p->mic, 4);
}

__attribute__((optimize("-Os")))
void pvvx_encrypt_data_beacon(void) {
	padv_cust_enc_t p = (padv_cust_enc_t)&adv_buf.data;
	enc_beacon_nonce_t cbn;
	adv_cust_data_t data;
	uint8_t aad = 0x11;
	adv_buf.update_count = -1; // next call if next measured
	p->head.size = sizeof(adv_cust_enc_t) - 1;
	p->head.uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_CUSTOM_UUID16; // GATT Service 0x181A Environmental Sensing (little-endian) (or 0x181C 'User Data'?)
	p->head.counter = (uint8_t)adv_buf.send_count;
	data.temp = measured_data.temp;
	data.humi = measured_data.humi;
	data.bat = measured_data.battery_level;
#if (DEV_SERVICES & SERVICE_TH_TRG)
	data.trg = trg.flg_byte;
#else
	data.trg = 0;
#endif
	memcpy(cbn.MAC, mac_public, sizeof(cbn.MAC));
	memcpy(&cbn.head, p, sizeof(cbn.head));
	aes_ccm_encrypt_and_tag((const unsigned char *)&bindkey,
					   (uint8_t*)&cbn, sizeof(cbn),
					   &aad, sizeof(aad),
					   (uint8_t *)&data, sizeof(data),
					   (uint8_t *)&p->data,
					   p->mic, 4);
}

#endif // #if (DEV_SERVICES & SERVICE_BINDKEY)

_attribute_ram_code_
__attribute__((optimize("-Os")))
void pvvx_data_beacon(void) {
	padv_custom_t p = (padv_custom_t)&adv_buf.data;
	memcpy(p->MAC, mac_public, 6);
#if (DEV_SERVICES & SERVICE_TH_TRG)
	p->size = sizeof(adv_custom_t) - 1;
#else
	p->size = sizeof(adv_custom_t) - 2;
#endif
	p->uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->UUID = ADV_CUSTOM_UUID16; // GATT Service 0x181A Environmental Sensing (little-endian)
	p->temperature = measured_data.temp; // x0.01 C
	p->humidity = measured_data.humi; // x0.01 %
#if USE_AVERAGE_BATTERY
	p->battery_mv = measured_data.average_battery_mv; // x mV
#else
	p->battery_mv = measured_data.battery_mv; // x mV
#endif
	p->battery_level = measured_data.battery_level; // x1 %
	p->counter = (uint8_t)adv_buf.send_count;
#if (DEV_SERVICES & SERVICE_TH_TRG)
	p->flags = trg.flg_byte;
#endif
}

_attribute_ram_code_
__attribute__((optimize("-Os")))
void atc_data_beacon(void) {
	padv_atc1441_t p = (padv_atc1441_t)&adv_buf.data;
	p->size = sizeof(adv_atc1441_t) - 1;
	p->uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->UUID = ADV_CUSTOM_UUID16; // GATT Service 0x181A Environmental Sensing (little-endian)
#if 1
	SwapMacAddress(p->MAC, mac_public);
#else
	p->MAC[0] = mac_public[5];
	p->MAC[1] = mac_public[4];
	p->MAC[2] = mac_public[3];
	p->MAC[3] = mac_public[2];
	p->MAC[4] = mac_public[1];
	p->MAC[5] = mac_public[0];
#endif
	p->temperature[0] = (uint8_t)(measured_data.temp_x01 >> 8);
	p->temperature[1] = (uint8_t)measured_data.temp_x01; // x0.1 C
	p->humidity = measured_data.humi_x1; // x1 %
	p->battery_level = measured_data.battery_level; // x1 %
#if USE_AVERAGE_BATTERY
	p->battery_mv[0] = (uint8_t)(measured_data.average_battery_mv >> 8);
	p->battery_mv[1] = (uint8_t)measured_data.average_battery_mv; // x1 mV
#else
	p->battery_mv[0] = (uint8_t)(measured_data.battery_mv >> 8);
	p->battery_mv[1] = (uint8_t)measured_data.battery_mv; // x1 mV
#endif
	p->counter = (uint8_t)adv_buf.send_count;
}


#if (DEV_SERVICES & SERVICE_RDS)

typedef struct __attribute__((packed)) _ext_adv_cnt_t {
	uint8_t		size;	// = 6
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0x2AEB - Count 24
	uint8_t		cnt[3];
} ext_adv_cnt_t, * pext_adv_cnt_t;

typedef struct __attribute__((packed)) _ext_adv_digt_t {
	uint8_t		size;	// = 4
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0x2A56 - Digital State Bits
	uint8_t		bits;
} ext_adv_dig_t, * pext_adv_dig_t;

typedef struct __attribute__((packed)) _adv_event_t {
	ext_adv_dig_t dig;
	ext_adv_cnt_t cnt;
} adv_event_t, * padv_event_t;

void default_event_beacon(void){
	padv_event_t p = (padv_event_t)&adv_buf.data;
	p->dig.size = sizeof(p->dig) - sizeof(p->dig.size);
	p->dig.uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->dig.UUID = ADV_UUID16_DigitalStateBits;
	p->dig.bits = trg.flg_byte;
	p->cnt.size = sizeof(p->cnt) - sizeof(p->cnt.size);
	p->cnt.uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->cnt.UUID = ADV_UUID16_Count24bits;
	p->cnt.cnt[0] = rds.count_byte[2];
	p->cnt.cnt[1] = rds.count_byte[1];
	p->cnt.cnt[2] = rds.count_byte[0];
	adv_buf.data_size = sizeof(adv_event_t);
}

void pvvx_event_beacon(uint8_t n){
	if (n == RDS_SWITCH) {
		pvvx_data_beacon();
		adv_buf.data_size = adv_buf.data[0] + 1;
	} else
		default_event_beacon();
}

#if (DEV_SERVICES & SERVICE_BINDKEY)

void pvvx_encrypt_event_beacon(uint8_t n){
	if (n == RDS_SWITCH) {
		pvvx_encrypt_data_beacon();
		adv_buf.data_size = adv_buf.data[0] + 1;
	} else
		default_event_beacon();
}

#endif // (DEV_SERVICES & SERVICE_RDS)
#endif // #if (DEV_SERVICES & SERVICE_BINDKEY)
