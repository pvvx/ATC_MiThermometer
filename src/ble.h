#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app.h"

extern uint8_t ota_is_working;
extern uint8_t ble_connected;
extern uint8_t adv_mi_count;
extern bool show_temp_humi_Mi;
extern u8 batteryValueInCCC[2];
extern u8 tempValueInCCC[2];
extern u8 temp2ValueInCCC[2];
extern u8 humiValueInCCC[2];
extern u8 RxTxValueInCCC[2];
extern uint8_t send_buf[16];

typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;
extern gap_periConnectParams_t my_periConnParameters;

// GATT Service 0x181A Environmental Sensing
// All data little-endian
typedef struct __attribute__((packed)) _adv_custom_t {
	uint8_t		size;	// = 19
	uint8_t		uid;	// = 0x16, 16-bit UUID
	uint16_t	UUID;	// = 0x181A, GATT Service 0x181A Environmental Sensing
	uint8_t		MAC[6]; // [0] - lo, .. [6] - hi digits
	int16_t		temperature; // x 0.1 degree
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


// UUID for Members 0xFE95 Xiaomi Inc. https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf
// All data little-endian, + https://github.com/Magalex2x14/LYWSD03MMC-info
typedef struct __attribute__((packed)) _adv_mi_t {
	uint8_t		size;	// = 21
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0xFE95, 16-bit UUID for Members 0xFE95 Xiaomi Inc.
	uint16_t	ctrl;	// = 0x3050 Frame ctrl
	uint16_t    dev_id; // = 0x055B	Device type
	uint8_t		counter; // 0..0xff..0 measurement count
	uint8_t		MAC[6];	// [0] - lo, .. [6] - hi digits
	// +15: 0x0A, 0x10, 0x01, t_lv, 0x02, b_lo, b_hi
	// +15: 0x0D, 0x10, 0x04, t_lo, t_hi, h_lo, h_hi
	uint8_t     data_id; 	// = 0x0A or 0x0D
	uint8_t     nx10; 		// = 0x10
	union {
		struct {
			uint8_t		len1; // = 0x01
			uint8_t		battery_level; // 0..100 %
			uint8_t		len2; // = 0x02
			uint16_t	battery_mv;
		}t0a;
		struct {
			uint8_t		len; // = 0x04
			int16_t		temperature; // x0.1 C
			uint16_t	humidity; // x0.1 %
		}t0d;
	};
} adv_mi_t, * padv_mi_t;

void set_adv_data(uint8_t adv_type); // 0 - atc1441, 1 - Custom, 2 - Mi, 3 - all

extern u8 my_RxTx_Data[16];

void init_ble();
bool ble_get_connected();
void ble_send_measures(void);
void ble_send_ext(void);
void ble_send_lcd(void);
#if USE_TRIGGER_OUT
void ble_send_trg(void);
void ble_send_trg_flg(void);
#endif
int otaWritePre(void * p);
int RxTxWrite(void * p);

inline void ble_send_temp(void) {
	bls_att_pushNotifyData(TEMP_LEVEL_INPUT_DP_H, (u8 *) &last_temp, 2);
}

inline void ble_send_temp2(void) {
	bls_att_pushNotifyData(TEMP2_LEVEL_INPUT_DP_H, (u8 *) &measured_data.temp, 2);
}

inline void ble_send_humi(void) {
	bls_att_pushNotifyData(HUMI_LEVEL_INPUT_DP_H, (u8 *) &measured_data.humi, 2);
}

inline void ble_send_battery(void) {
	bls_att_pushNotifyData(BATT_LEVEL_INPUT_DP_H, (u8 *) &battery_level, 1);
}

inline void ble_send_cfg(void) {
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, my_RxTx_Data, sizeof(cfg) + 2);
}
