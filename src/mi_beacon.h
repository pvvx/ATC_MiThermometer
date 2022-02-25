/*
 * mi_beacon.h
 *
 *	Created on: 17.02.2021
 *		Author: pvvx
 */

#ifndef MI_BEACON_H_
#define MI_BEACON_H_

extern uint8_t bindkey[16];

#define ADV_XIAOMI_UUID16 0xFE95 // 16-bit UUID for Members 0xFE95 Xiaomi Inc.
#define XIAOMI_DEV_VERSION	5

enum {
	XIAOMI_DEV_ID_LYWSDCGQ       = 0x01AA,
	XIAOMI_DEV_ID_CGG1           = 0x0347,
	XIAOMI_DEV_ID_CGG1_ENCRYPTED = 0x0B48,
	XIAOMI_DEV_ID_CGDK2          = 0x066F,
	XIAOMI_DEV_ID_LYWSD02        = 0x045B,
	XIAOMI_DEV_ID_LYWSD03MMC     = 0x055B,
	XIAOMI_DEV_ID_CGD1           = 0x0576,
	XIAOMI_DEV_ID_MHO_C303       = 0x06d3,
	XIAOMI_DEV_ID_MHO_C401       = 0x0387,
	XIAOMI_DEV_ID_JQJCY01YM      = 0x02DF,
	XIAOMI_DEV_ID_HHCCJCY01      = 0x0098,
	XIAOMI_DEV_ID_GCLS002        = 0x03BC,
	XIAOMI_DEV_ID_HHCCPOT002     = 0x015D,
	XIAOMI_DEV_ID_WX08ZM         = 0x040A,
	XIAOMI_DEV_ID_MCCGQ02HL      = 0x098B,
	XIAOMI_DEV_ID_YM_K1501       = 0x0083,
	XIAOMI_DEV_ID_YM_K1501EU     = 0x0113,
	XIAOMI_DEV_ID_V_SK152        = 0x045C,
	XIAOMI_DEV_ID_SJWS01LM       = 0x0863,
	XIAOMI_DEV_ID_MJYD02YL       = 0x07F6
}XIAOMI_DEV_ID;

enum { // mijia ble version 5, General events
	XIAOMI_DATA_EV_Base					=0x0000,
	XIAOMI_DATA_EV_Connect				=0x0001,
	XIAOMI_DATA_EV_SimplrPair			=0x0002,
	XIAOMI_DATA_EV_Motion				=0x0003,
	XIAOMI_DATA_EV_KeepAway				=0x0004,
	XIAOMI_DATA_EV_LockObsolete			=0x0005,
	XIAOMI_DATA_EV_FingerPrint			=0x0006,
	XIAOMI_DATA_EV_Door					=0x0007,
	XIAOMI_DATA_EV_Armed				=0x0008,
	XIAOMI_DATA_EV_GestureController	=0x0009,
	XIAOMI_DATA_EV_BodyTemp				=0x000a,
	XIAOMI_DATA_EV_Lock					=0x000b,
	XIAOMI_DATA_EV_Flooding				=0x000c,
	XIAOMI_DATA_EV_Smoke				=0x000d,
	XIAOMI_DATA_EV_Gas					=0x000e,
	XIAOMI_DATA_EV_MovingWithLight		=0x000f, // Someone is moving (with light)
	XIAOMI_DATA_EV_ToothbrushIncident	=0x0010,
	XIAOMI_DATA_EV_CatEyeIncident		=0x0011,
	XIAOMI_DATA_EV_WeighingEvent		=0x0012,
	XIAOMI_DATA_EV_Button				=0x1001
} XIAOMI_DATA_EV;

enum { // mijia ble version 5, General attributes
	XIAOMI_DATA_ID_Sleep				=0x1002,
	XIAOMI_DATA_ID_RSSI					=0x1003,
	XIAOMI_DATA_ID_Temperature			=0x1004,
	XIAOMI_DATA_ID_Humidity				=0x1006,
	XIAOMI_DATA_ID_LightIlluminance		=0x1007,
	XIAOMI_DATA_ID_SoilMoisture			=0x1008,
	XIAOMI_DATA_ID_SoilECvalue			=0x1009,
	XIAOMI_DATA_ID_Power				=0x100A,
	XIAOMI_DATA_ID_TempAndHumidity		=0x100D,
	XIAOMI_DATA_ID_Lock					=0x100E,
	XIAOMI_DATA_ID_Gate					=0x100F,
	XIAOMI_DATA_ID_Formaldehyde			=0x1010,
	XIAOMI_DATA_ID_Bind					=0x1011,
	XIAOMI_DATA_ID_Switch				=0x1012,
	XIAOMI_DATA_ID_RemAmCons			=0x1013, // Remaining amount of consumables
	XIAOMI_DATA_ID_Flooding				=0x1014,
	XIAOMI_DATA_ID_Smoke				=0x1015,
	XIAOMI_DATA_ID_Gas					=0x1016,
	XIAOMI_DATA_ID_NoOneMoves			=0x1017,
	XIAOMI_DATA_ID_LightIntensity		=0x1018,
	XIAOMI_DATA_ID_DoorSensor			=0x1019,
	XIAOMI_DATA_ID_WeightAttributes		=0x101A,
	XIAOMI_DATA_ID_NoOneMovesOverTime	=0x101B, // No one moves over time
	XIAOMI_DATA_ID_SmartPillow			=0x101C
} XIAOMI_DATA_ID;

/* Encrypted mi beacon structs */
// UUID for Members 0xFE95 Xiaomi Inc. https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf
// All data little-endian, + https://github.com/pvvx/ATC_MiThermometer/tree/master/InfoMijiaBLE

typedef union _adv_mi_fctrl_t { // Frame Control
	struct __attribute__((packed)) {
		uint16_t Factory:		1; // reserved text
		uint16_t Connected:		1; // reserved text
		uint16_t Central:		1; // Keep
		uint16_t isEncrypted:	1; // 0: The package is not encrypted; 1: The package is encrypted
		uint16_t MACInclude:	1; // 0: Does not include the MAC address; 1: includes a fixed MAC address (the MAC address is included for iOS to recognize this device and connect)
		uint16_t CapabilityInclude:		1; // 0: does not include Capability; 1: includes Capability. Before the device is bound, this bit is forced to 1
		uint16_t ObjectInclude:	1; // 0: does not contain Object; 1: contains Object
		uint16_t Mesh:			1; // 0: does not include Mesh; 1: includes Mesh. For standard BLE access products and high security level access, this item is mandatory to 0. This item is mandatory for Mesh access to 1.
		uint16_t registered:	1; // 0: The device is not bound; 1: The device is registered and bound.
		uint16_t solicited:		1; // 0: No operation; 1: Request APP to register and bind. It is only valid when the user confirms the pairing by selecting the device on the developer platform, otherwise set to 0. The original name of this item was bindingCfm, and it was renamed to solicited "actively request, solicit" APP for registration and binding
		uint16_t AuthMode:		2; // 0: old version certification; 1: safety certification; 2: standard certification; 3: reserved
		uint16_t version:		4; // Version number (currently v5)
	} bit;
	uint16_t	word;	// = 0x3050 Frame ctrl
} adv_mi_fctrl_t; // Frame Control

typedef struct __attribute__((packed)) _adv_mi_head_t {
	uint8_t		size;	// = 27?
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0xFE95, 16-bit UUID for Members 0xFE95 Xiaomi Inc.
	adv_mi_fctrl_t fctrl;	// Frame Control
	uint16_t	dev_id;		// Device type (enum: XIAOMI_DEV_ID)
	uint8_t		counter;	// 0..0xff Measurement count, Serial number, used for de-duplication, different event or attribute reporting requires different Frame Counter
} adv_mi_head_t, * padv_head_enc_t;

typedef struct __attribute__((packed)) _adv_mi_data_t {
	uint16_t	 id;	// = 0x1004, 0x1006, 0x100a (XIAOMI_DATA_ID)
	uint8_t		 size;
	union {
		int16_t		 data_i16;
		uint16_t	 data_u16;
		uint8_t		 data_u8;
		struct __attribute__((packed)) {
			uint8_t		battery_level; // 0..100 %
		}t0a;
		struct __attribute__((packed)) {
			int16_t		temperature; // x0.1 C
			uint16_t	humidity; // x0.1 %
		}t0d;
	};
} adv_mi_data_t, * padv_mi_data_t;

typedef struct __attribute__((packed)) _adv_mi_mac_beacon_t {
	adv_mi_head_t head;
	uint8_t		MAC[6];		// [0] - lo, .. [6] - hi digits
	union {
		adv_mi_data_t data;
		uint8_t capability;
	};
} adv_mi_mac_beacon_t, * padv_mi_mac_beacon_t;

typedef struct __attribute__((packed)) _adv_mi_no_mac_beacon_t {
	adv_mi_head_t head;
	union {
		adv_mi_data_t data;
		uint8_t capability;
	};
} adv_mi_no_mac_beacon_t, * padv_mi_no_mac_beacon_t;

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


void mi_beacon_summ(void); // averaging measurements
void mi_encrypt_beacon(uint32_t cnt);
void mi_beacon_init(void);

void atc_encrypt_beacon(uint32_t cnt);
void pvvx_encrypt_beacon(uint32_t cnt);

#endif /* MI_BEACON_H_ */
