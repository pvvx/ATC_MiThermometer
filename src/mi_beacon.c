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
#include "ccm.h"

typedef union _adv_mi_fctrl_t { // Frame Control
	struct __attribute__((packed)) {
		uint16_t Factory: 		1; // reserved text
		uint16_t Connected: 	1; // reserved text
		uint16_t Central: 		1; // Keep
		uint16_t isEncrypted: 	1; // 0: The package is not encrypted; 1: The package is encrypted
		uint16_t MACInclude: 	1; // 0: Does not include the MAC address; 1: includes a fixed MAC address (the MAC address is included for iOS to recognize this device and connect)
		uint16_t CapabilityInclude: 	1; // 0: does not include Capability; 1: includes Capability. Before the device is bound, this bit is forced to 1
		uint16_t ObjectInclude:	1; // 0: does not contain Object; 1: contains Object
		uint16_t Mesh: 			1; // 0: does not include Mesh; 1: includes Mesh. For standard BLE access products and high security level access, this item is mandatory to 0. This item is mandatory for Mesh access to 1.
		uint16_t registered:	1; // 0: The device is not bound; 1: The device is registered and bound.
		uint16_t solicited:		1; // 0: No operation; 1: Request APP to register and bind. It is only valid when the user confirms the pairing by selecting the device on the developer platform, otherwise set to 0. The original name of this item was bindingCfm, and it was renamed to solicited "actively request, solicit" APP for registration and binding
		uint16_t AuthMode:		2; // 0: old version certification; 1: safety certification; 2: standard certification; 3: reserved
		uint16_t version:		4; // Version number (currently v5)
	} bit;
	uint16_t	word;	// = 0x3050 Frame ctrl
} adv_mi_fctrl_t; // Frame Control

typedef struct __attribute__((packed)) _adv_mi_enc_t {
	uint8_t		size;	// = 27?
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0xFE95, 16-bit UUID for Members 0xFE95 Xiaomi Inc.
	adv_mi_fctrl_t fctrl;	// Frame Control
	uint16_t    dev_id;		// Device type (enum: XIAOMI_DEV_ID)
	uint8_t		counter;	// 0..0xff Measurement count, Serial number, used for de-duplication, different event or attribute reporting requires different Frame Counter
	uint8_t		MAC[6];		// [0] - lo, .. [6] - hi digits
	uint16_t     data_id; 	// = 0x1004, 0x1006, 0x100a (XIAOMI_DATA_ID)
	uint8_t 	 data_len;
} adv_mi_enc_t, * padv_mi_enc_t;

typedef struct __attribute__((packed)) _beacon_nonce_t{
    uint8_t  mac[6];
    uint16_t pid;
    uint8_t  cnt;
    uint8_t  ext_cnt[3];
} beacon_nonce_t, * pbeacon_nonce_t;

//// Init data
RAM uint8_t *pbindkey;
RAM uint8_t bindkey[16];

RAM beacon_nonce_t beacon_nonce;
//// Counters
RAM uint32_t adv_mi_cnt = 0xffffffff; // counter of measurement numbers from sensors
//// Buffers
extern uint8_t adv_buffer[28];
RAM uint8_t adv_crypt_buf[28];
/// Vars
RAM struct {
	int16_t temp;	// x0.1 C
	uint16_t humi;	// x0.1 %
	uint8_t batt;	// 0..100 %
} mi_beacon_data;

typedef struct _summ_data_t {
	uint32_t	batt; // mv
	int32_t		temp; // x 0.01 C
	uint32_t	humi; // x 0.01 %
	uint32_t 	count;
} mib_summ_data_t;
RAM mib_summ_data_t mib_summ_data;

/* Initializing mi beacon */
void mi_beacon_init(void) {
	uint8_t *p_key = find_mi_keys(MI_KEYTBIND_ID, 1);
	if(p_key) {
		pbindkey = p_key + 12;
		p_key = find_mi_keys(MI_KEYSEQNUM_ID, 1);
		if(p_key)
			memcpy(&beacon_nonce.cnt, p_key, 4);
	} else {
		pbindkey = bindkey;
		if(flash_read_cfg(pbindkey, EEP_ID_KEY, sizeof(bindkey)) != sizeof(bindkey)) {
			generateRandomNum(sizeof(bindkey), pbindkey);
			flash_write_cfg(pbindkey, EEP_ID_KEY, sizeof(bindkey));
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

/* Create encrypted mi beacon packet */
__attribute__((optimize("-Os"))) void mi_encrypt_beacon(uint32_t cnt) {
	if(adv_mi_cnt == cnt) { // measurement counter update?
		memcpy(adv_buffer, adv_crypt_buf, sizeof(adv_buffer));
		return;
	}
	adv_mi_cnt = cnt; // new counter
	beacon_nonce.cnt = cnt;
	if((cnt & 3) == 0) { // Data are averaged over a period of 16 measurements (cnt*4)
		mi_beacon_data.temp = ((int16_t)(mib_summ_data.temp/(int32_t)mib_summ_data.count))/10;
		mi_beacon_data.humi = ((uint16_t)(mib_summ_data.humi/mib_summ_data.count))/10;
		mi_beacon_data.batt = get_battery_level((uint16_t)(mib_summ_data.batt/mib_summ_data.count));
		memset(&mib_summ_data, 0, sizeof(mib_summ_data));
	}
	padv_mi_enc_t p = (padv_mi_enc_t)adv_crypt_buf;
	p->uid = 0x16; // 16-bit UUID
	p->UUID = 0xFE95; // 16-bit UUID for Members 0xFE95 Xiaomi Inc.
	p->dev_id = beacon_nonce.pid;
	p->counter = cnt;
	memcpy(p->MAC, mac_public, 6);
	uint8_t *mic = (uint8_t *)p;
	mic += sizeof(adv_mi_enc_t);
	switch(cnt & 3) {
		case 0:
			p->data_id = XIAOMI_DATA_ID_Temperature; // XIAOMI_DATA_ID
			p->data_len = 2;
			*mic++ = mi_beacon_data.temp;	// Temperature, Range: -400..+1000 (x0.1 C)
			*mic++ = mi_beacon_data.temp >> 8;
			break;
		case 1:
			p->data_id = 0x08;
			p->fctrl.word = 0x5830; // 0x5830
			p->size = sizeof(adv_mi_enc_t) - 2 - 1;
			return;
		case 2:
			p->data_id = XIAOMI_DATA_ID_Humidity; // byte XIAOMI_DATA_ID
			p->data_len = 2;
			*mic++ = mi_beacon_data.humi; // Humidity percentage, Range: 0..1000 (x0.1 %)
			*mic++ = mi_beacon_data.humi >> 8;
			break;
		case 3:
			p->data_id = XIAOMI_DATA_ID_Power; // XIAOMI_DATA_ID
			p->data_len = 1;
			*mic++ = mi_beacon_data.batt; // Battery percentage, Range: 0..100 %
			break;
	}
#if 0
	p->fctrl.word = 0;
	p->fctrl.bit.isEncrypted = 1;
	p->fctrl.bit.MACInclude = 1;
	p->fctrl.bit.ObjectInclude = 1;
	p->fctrl.bit.registered = 1;
	p->fctrl.bit.AuthMode = 2;
	p->fctrl.bit.version = 5; // XIAOMI_DEV_VERSION
#else
	p->fctrl.word = 0x5858; // 0x5830
#endif
	p->size = p->data_len + sizeof(adv_mi_enc_t) + 3 + 4 - 1;
	*mic++ = beacon_nonce.ext_cnt[0];
	*mic++ = beacon_nonce.ext_cnt[1];
	*mic++ = beacon_nonce.ext_cnt[2];
    uint8_t aad = 0x11;
	aes_ccm_encrypt_and_tag(pbindkey,
						   (uint8_t*)&beacon_nonce, sizeof(beacon_nonce),
						   &aad, sizeof(aad),
						   (uint8_t *)&p->data_id, p->data_len + 3,
						   (uint8_t *)&p->data_id,
						   mic, 4);
}

#endif // USE_MIHOME_BEACON
