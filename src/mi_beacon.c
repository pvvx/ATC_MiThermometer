/*
 * mi_beacon.c
 *
 *  Created on: 16.02.2021
 *      Author: pvvx
 */
#include <stdint.h>
#include "tl_common.h"
#if USE_MIHOME_BEACON
//#include "drivers.h"
#include "app_config.h"
#include "ble.h"
#include "cmd_parser.h"
#include "battery.h"
#include "app.h"
#include "flash_eep.h"
#if	USE_TRIGGER_OUT
#include "trigger.h"
#endif
#if USE_FLASH_MEMO
#include "logger.h"
#endif
#include "mi_beacon.h"
#include "ccm.h"


/* Encrypted mi nonce */
typedef struct __attribute__((packed)) _mi_beacon_nonce_t{
    uint8_t  mac[6];
	uint16_t pid;
	union {
		struct {
			uint8_t  cnt;
			uint8_t  ext_cnt[3];
		};
		uint32_t cnt32;
    };
} mi_beacon_nonce_t, * pmi_beacon_nonce_t;

/* Encrypted atc/custom nonce */
typedef struct __attribute__((packed)) _enc_beacon_nonce_t{
    uint8_t  MAC[6];
    adv_cust_head_t head;
} enc_beacon_nonce_t;

//// Init data
RAM uint8_t bindkey[16];
RAM mi_beacon_nonce_t beacon_nonce;
/// Vars
typedef struct _mi_beacon_data_t { // out data
	int16_t temp;	// x0.1 C
	uint16_t humi;	// x0.1 %
	uint8_t batt;	// 0..100 %
} mi_beacon_data_t;
RAM mi_beacon_data_t mi_beacon_data;

typedef struct _summ_data_t { // calk summ data
	uint32_t	batt; // mv
	int32_t		temp; // x 0.01 C
	uint32_t	humi; // x 0.01 %
	uint32_t 	count;
} mib_summ_data_t;
RAM mib_summ_data_t mib_summ_data;

/* Initializing data for mi beacon */
void mi_beacon_init(void) {
	uint32_t faddr = find_mi_keys(MI_KEYTBIND_ID, 1);
	if (faddr) {
		memcpy(&bindkey, &keybuf.data[12], sizeof(bindkey));
		faddr = find_mi_keys(MI_KEYSEQNUM_ID, 1);
		if (faddr)
			memcpy(&adv_buf.send_count, &keybuf.data, sizeof(adv_buf.send_count)); // BLE_GAP_AD_TYPE_FLAGS
	} else {
		if (flash_read_cfg(&bindkey, EEP_ID_KEY, sizeof(bindkey))
				!= sizeof(bindkey)) {
			generateRandomNum(sizeof(bindkey), (unsigned char *) &bindkey);
			flash_write_cfg(&bindkey, EEP_ID_KEY, sizeof(bindkey));
		}
	}
	memcpy(beacon_nonce.mac, mac_public, 6);
	beacon_nonce.pid = DEVICE_TYPE;
}

/* Averaging measurements */
_attribute_ram_code_
void mi_beacon_summ(void) {
	mib_summ_data.temp += measured_data.temp;
	mib_summ_data.humi += measured_data.humi;
	mib_summ_data.batt += measured_data.battery_mv;
	mib_summ_data.count++;
}

/* Create encrypted custom beacon packet
 * https://github.com/pvvx/ATC_MiThermometer/issues/94#issuecomment-842846036 */
__attribute__((optimize("-Os")))
void atc_encrypt_beacon() {
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
#if USE_TRIGGER_OUT
	| ((trg.flg.trigger_on)? 0x80 : 0)
#endif
	;
	memcpy(&cbn.MAC, mac_public, sizeof(cbn.MAC));
	memcpy(&cbn.head, p, sizeof(cbn.head));
	aes_ccm_encrypt_and_tag((const unsigned char *)&bindkey,
					   (uint8_t*)&cbn, sizeof(cbn),
					   &aad, sizeof(aad),
					   (uint8_t *)&data, sizeof(data),
					   (uint8_t *)&p->data,
					   p->mic, 4);
}

__attribute__((optimize("-Os")))
void pvvx_encrypt_beacon(void) {
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
#if	USE_TRIGGER_OUT
	data.trg = trg.flg_byte;
#else
	data.trg = 0;
#endif
	memcpy(&cbn.MAC, mac_public, sizeof(cbn.MAC));
	memcpy(&cbn.head, p, sizeof(cbn.head));
	aes_ccm_encrypt_and_tag((const unsigned char *)&bindkey,
					   (uint8_t*)&cbn, sizeof(cbn),
					   &aad, sizeof(aad),
					   (uint8_t *)&data, sizeof(data),
					   (uint8_t *)&p->data,
					   p->mic, 4);
}

RAM uint8_t adv_mi_crypt_num;
/* Create encrypted mi beacon packet */
__attribute__((optimize("-Os")))
void mi_encrypt_beacon(void) {
	beacon_nonce.cnt32 = adv_buf.send_count;
	adv_buf.update_count = -1; // next call if next measured
	adv_mi_crypt_num++;
	if (adv_mi_crypt_num > 2) {
		adv_mi_crypt_num = 0;
		if(mib_summ_data.count) {
			mi_beacon_data.temp = ((int16_t)(mib_summ_data.temp/(int32_t)mib_summ_data.count) + 5)/10;
			mi_beacon_data.humi = ((uint16_t)(mib_summ_data.humi/mib_summ_data.count)  + 5)/10;
			mi_beacon_data.batt = get_battery_level((uint16_t)(mib_summ_data.batt/mib_summ_data.count));
			memset(&mib_summ_data, 0, sizeof(mib_summ_data));
		} else {
			mi_beacon_data.temp = measured_data.temp_x01;
			mi_beacon_data.humi = measured_data.humi_x01;
			mi_beacon_data.batt = measured_data.battery_level;
		}
	}
	padv_mi_mac_beacon_t p = (padv_mi_mac_beacon_t)&adv_buf.data;
	p->head.uid = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT; // 16-bit UUID
	p->head.UUID = ADV_XIAOMI_UUID16; // 16-bit UUID for Members 0xFE95 Xiaomi Inc.
	p->head.dev_id = beacon_nonce.pid;
	p->head.counter = beacon_nonce.cnt;
	adv_mi_data_t data;
	memcpy(p->MAC, mac_public, 6);
	switch (adv_mi_crypt_num) {
		case 0:
			data.id = XIAOMI_DATA_ID_Temperature; // XIAOMI_DATA_ID
			data.size = 2;
			data.data_i16 = mi_beacon_data.temp;	// Temperature, Range: -400..+1000 (x0.1 C)
			break;
		case 1:
			data.id = XIAOMI_DATA_ID_Humidity; // byte XIAOMI_DATA_ID
			data.size = 2;
			data.data_u16 = mi_beacon_data.humi; // Humidity percentage, Range: 0..1000 (x0.1 %)
			break;
		case 2:
			data.id = XIAOMI_DATA_ID_Power; // XIAOMI_DATA_ID
			data.size = 1;
			data.data_u8 = mi_beacon_data.batt; // Battery percentage, Range: 0..100 %
			break;
/*
		case 3:
#if 0
			p->head.fctrl.word = 0;
			p->head.fctrl.bit.MACInclude = 1;
			p->head.fctrl.bit.CapabilityInclude = 1;
			p->head.fctrl.bit.registered = 1;
			p->head.fctrl.bit.AuthMode = 2;
			p->head.fctrl.bit.version = 5; // XIAOMI_DEV_VERSION
#else
			p->head.fctrl.word = 0x5830; // 0x5830
#endif
			p->capability = 0x08; // capability
			p->head.size = sizeof(p->head) - sizeof(p->head.size) + sizeof(p->MAC) + sizeof(p->capability);
			return;
*/
	}
#if 0
	p->head.fctrl.word = 0;
	p->head.fctrl.bit.isEncrypted = 1;
	p->head.fctrl.bit.MACInclude = 1;
	p->head.fctrl.bit.ObjectInclude = 1;
	p->head.fctrl.bit.registered = 1;
	p->head.fctrl.bit.AuthMode = 2;
	p->head.fctrl.bit.version = 5; // XIAOMI_DEV_VERSION
#else
	p->head.fctrl.word = 0x5858; // 0x5858
#endif
	p->head.size = data.size + sizeof(p->head) - sizeof(p->head.size) + sizeof(p->MAC) + sizeof(p->data.id) + sizeof(p->data.size) + sizeof(beacon_nonce.ext_cnt) + 4; //size data + size head + size MAC + size data head + size counter bit8..31 bits + size mic 32 bits - 1
	uint8_t * pmic = (uint8_t *)p;
	pmic += data.size + sizeof(p->head) + sizeof(p->MAC) + sizeof(p->data.id) + sizeof(p->data.size);
	*pmic++ = beacon_nonce.ext_cnt[0];
	*pmic++ = beacon_nonce.ext_cnt[1];
	*pmic++ = beacon_nonce.ext_cnt[2];
    uint8_t aad = 0x11;
	aes_ccm_encrypt_and_tag((const unsigned char *)&bindkey,
						   (uint8_t*)&beacon_nonce, sizeof(beacon_nonce),
						   &aad, sizeof(aad),
						   (uint8_t *)&data, data.size + sizeof(p->data.id) + sizeof(p->data.size), // + size data head
						   (uint8_t *)&p->data,
						   pmic, 4);
}

#endif // USE_MIHOME_BEACON
