#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app.h"
#include "stack/ble/ble.h"

extern uint8_t ota_is_working;
extern uint8_t ble_connected; // bit 0 - connected, bit 1 - conn_param_update, bit 2 - paring success, bit 7 - reset device on disconnect
extern uint8_t adv_mi_count;
extern bool show_temp_humi_Mi;
extern u8 batteryValueInCCC[2];
extern u8 tempValueInCCC[2];
extern u8 temp2ValueInCCC[2];
extern u8 humiValueInCCC[2];
extern u8 RxTxValueInCCC[2];
extern uint8_t send_buf[20];
extern uint8_t mac_public[6];
extern uint8_t mac_random_static[6];
extern uint8_t ble_name[12];

#define XIAOMI_DEV_VERSION	5

enum { // mijia ble version 5
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

enum { // mijia ble version 5
	XIAOMI_DATA_ID_Sleep				=0x1002,
	XIAOMI_DATA_ID_RSSI					=0x1003,
	XIAOMI_DATA_ID_Temperature			=0x1004,
	XIAOMI_DATA_ID_Humidity				=0x1006,
	XIAOMI_DATA_ID_LightIlluminance		=0x1007,
	XIAOMI_DATA_ID_SoilMoisture			=0x1008,
	XIAOMI_DATA_ID_SoilECvalue			=0x1009,
	XIAOMI_DATA_ID_Power				=0x100A,
	XIAOMI_DATA_ID_TempAndHumidity		=0x100D, // added pvvx - non-standard ?
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
	XIAOMI_DATA_ID_NoOneMovesOverTime 	=0x101B, // No one moves over time
	XIAOMI_DATA_ID_SmartPillow			=0x101C
} XIAOMI_DATA_ID;

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

// UUID for Members 0xFE95 Xiaomi Inc. https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf
// All data little-endian, + https://github.com/pvvx/ATC_MiThermometer/tree/master/InfoMijiaBLE
typedef struct __attribute__((packed)) _adv_mi_t {
	uint8_t		size;	// = 21
	uint8_t		uid;	// = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	uint16_t	UUID;	// = 0xFE95, 16-bit UUID for Members 0xFE95 Xiaomi Inc.
#if 1
	union { // Frame Control
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
	} ctrl; // Frame Control
#else
	uint16_t	ctrl;	// = 0x3050 Frame ctrl
#endif
	uint16_t    dev_id;		// Device type (enum: XIAOMI_DEV_ID)
	uint8_t		counter;	// 0..0xff Measurement count, Serial number, used for de-duplication, different event or attribute reporting requires different Frame Counter
	uint8_t		MAC[6];		// [0] - lo, .. [6] - hi digits
	uint8_t     data_id; 	// = 0x0A or 0x0D (lo byte XIAOMI_DATA_ID)
	uint8_t     nx10; 		// = 0x10 (hi byte XIAOMI_DATA_ID)
	union {
		struct __attribute__((packed)) {
			uint8_t		len1; // = 0x01
			uint8_t		battery_level; // 0..100 %
			uint8_t		len2; // = 0x02
			uint16_t	battery_mv;
		}t0a;
		struct __attribute__((packed)) {
			uint8_t		len; // = 0x04
			int16_t		temperature; // x0.1 C
			uint16_t	humidity; // x0.1 %
		}t0d;
	};
} adv_mi_t, * padv_mi_t;

///////////////////////////////////// ATT  HANDLER define ///////////////////////////////////////
typedef enum
{
	ATT_H_START = 0,

	//// Gap ////
	/**********************************************************************************************/
	GenericAccess_PS_H, 					//UUID: 2800, 	VALUE: uuid 1800
	GenericAccess_DeviceName_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	GenericAccess_DeviceName_DP_H,			//UUID: 2A00,   VALUE: device name
	GenericAccess_Appearance_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	GenericAccess_Appearance_DP_H,			//UUID: 2A01,	VALUE: appearance
	CONN_PARAM_CD_H,						//UUID: 2803, 	VALUE:  			Prop: Read
	CONN_PARAM_DP_H,						//UUID: 2A04,   VALUE: connParameter

	//// Gatt ////
	/**********************************************************************************************/
	GenericAttribute_PS_H,					//UUID: 2800, 	VALUE: uuid 1801
	GenericAttribute_ServiceChanged_CD_H,	//UUID: 2803, 	VALUE:  			Prop: Indicate
	GenericAttribute_ServiceChanged_DP_H,   //UUID:	2A05,	VALUE: service change
	GenericAttribute_ServiceChanged_CCB_H,	//UUID: 2902,	VALUE: serviceChangeCCC

#if USE_DEVICE_INFO_CHR_UUID
	//// device information ////
	/**********************************************************************************************/
	DeviceInformation_PS_H,					//UUID: 2800, 	VALUE: uuid 180A
	DeviceInformation_ModName_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_ModName_DP_H,			//UUID: 2A24,	VALUE: Model Number String
	DeviceInformation_SerialN_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_SerialN_DP_H,			//UUID: 2A25,	VALUE: Serial Number String
	DeviceInformation_FirmRev_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_FirmRev_DP_H,			//UUID: 2A26,	VALUE: Firmware Revision String
	DeviceInformation_HardRev_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_HardRev_DP_H,			//UUID: 2A27,	VALUE: Hardware Revision String
	DeviceInformation_SoftRev_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_SoftRev_DP_H,			//UUID: 2A28,	VALUE: Software Revision String
	DeviceInformation_ManName_CD_H,			//UUID: 2803, 	VALUE:  			Prop: Read
	DeviceInformation_ManName_DP_H,			//UUID: 2A29,	VALUE: Manufacturer Name String
#endif
	//// Battery service ////
	/**********************************************************************************************/
	BATT_PS_H, 								//UUID: 2800, 	VALUE: uuid 180f
	BATT_LEVEL_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	BATT_LEVEL_INPUT_DP_H,					//UUID: 2A19 	VALUE: batVal
	BATT_LEVEL_INPUT_CCB_H,					//UUID: 2902, 	VALUE: batValCCC

	//// Temp/Humi service ////
	/**********************************************************************************************/
	TEMP_PS_H, 								//UUID: 2800, 	VALUE: uuid 181A
	TEMP_LEVEL_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	TEMP_LEVEL_INPUT_DP_H,					//UUID: 2A1F 	VALUE: last_temp
	TEMP_LEVEL_INPUT_CCB_H,					//UUID: 2902, 	VALUE: tempValCCC

	TEMP2_LEVEL_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	TEMP2_LEVEL_INPUT_DP_H,					//UUID: 2A6E 	VALUE: measured_data.temp
	TEMP2_LEVEL_INPUT_CCB_H,				//UUID: 2902, 	VALUE: temp2ValCCC

	HUMI_LEVEL_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	HUMI_LEVEL_INPUT_DP_H,					//UUID: 2A6F 	VALUE: measured_data.humi
	HUMI_LEVEL_INPUT_CCB_H,					//UUID: 2902, 	VALUE: humiValCCC

	//// Telink OTA ////
	/**********************************************************************************************/
	OTA_PS_H, 								//UUID: 2800, 	VALUE: telink ota service uuid
	OTA_CMD_OUT_CD_H,						//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	OTA_CMD_OUT_DP_H,						//UUID: telink ota uuid,  VALUE: otaData
	OTA_CMD_OUT_DESC_H,						//UUID: 2901, 	VALUE: otaName

	//// Custom RxTx ////
	/**********************************************************************************************/
	RxTx_PS_H, 								//UUID: 2800, 	VALUE: 1F10 RxTx service uuid
	RxTx_CMD_OUT_CD_H,						//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	RxTx_CMD_OUT_DP_H,						//UUID: 1F1F,  VALUE: RxTxData
	RxTx_CMD_OUT_DESC_H,					//UUID: 2902, 	VALUE: RxTxValueInCCC

#if USE_MIHOME_SERVICE
	// Mi Service
	/**********************************************************************************************/
	Mi_Service_PS_H, 						//UUID: 2800, 	VALUE: 0xFE95 service uuid
	Mi_Version_CD_H,						//UUID: 2803, 	VALUE: prop
	Mi_Version_DP_H,						//UUID: 0004,   VALUE: //value "1.0.0_0001"
	Mi_Version_DESC_H,						//UUID: 2902, 	VALUE: BLE_UUID_MI_VERS // "Version"

	Mi_Authentication_CD_H,					//UUID: 2803, 	VALUE: prop
	Mi_Authentication_DP_H,					//UUID: 0010,   VALUE: //value "1.0.0_0001"
	Mi_Authentication_DESC_H,				//UUID: 2901, 	VALUE: // "Authentication"
	Mi_Authentication_CCB_H,				//UUID: 2902, 	VALUE: CCC

	Mi_OTA_Ctrl_CD_H,						//UUID: 2803, 	VALUE: prop
	Mi_OTA_Ctrl_DP_H,						//UUID: 0017,   VALUE: //value
	Mi_OTA_Ctrl_DESC_H,						//UUID: 2901, 	VALUE: // "ota_ctrl"
	Mi_OTA_Ctrl_CCB_H,						//UUID: 2902, 	VALUE: CCC

	Mi_OTA_data_CD_H,						//UUID: 2803, 	VALUE: prop
	Mi_OTA_data_DP_H,						//UUID: 0018,   VALUE: //value
	Mi_OTA_data_DESC_H,						//UUID: 2901, 	VALUE: // "ota_data"
	Mi_OTA_data_CCB_H,						//UUID: 2902, 	VALUE: CCC

	Mi_Standard_CD_H,						//UUID: 2803, 	VALUE: prop
	Mi_Standard_DP_H,						//UUID: 0019,   VALUE: //value
	Mi_Standard_DESC_H,						//UUID: 2901, 	VALUE: // "standard"
	Mi_Standard_CCB_H,						//UUID: 2902, 	VALUE: CCC

	// Mi STDIO Service
	/**********************************************************************************************/
	Mi_STDIO_PS_H,							//UUID: 2800, 	VALUE: stdio_uuid @0100
	Mi_STDIO_RX_CD_H,						//UUID: 2803, 	VALUE: prop
	Mi_STDIO_RX_DP_H,						//UUID: @1100,  VALUE: //value
	Mi_STDIO_RX_DESC_H,						//UUID: 2901, 	VALUE: // "STDIO_RX"
	Mi_STDIO_RX_CCB_H,						//UUID: 2902, 	VALUE: CCC

	Mi_STDIO_TX_CD_H,						//UUID: 2803, 	VALUE: prop
	Mi_STDIO_TX_DP_H,						//UUID: @2100,  VALUE: //value
	Mi_STDIO_TX_DESC_H,						//UUID: 2901, 	VALUE: // "STDIO_TX"
	Mi_STDIO_TX_CCB_H,						//UUID: 2902, 	VALUE: CCC

#else
	// Mi Advertising char
	Mi_PS_H, 								//UUID: 2800, 	VALUE: 0xFE95 service uuid
	Mi_CMD_OUT_DESC_H,						//UUID: 2901, 	VALUE: my_MiName
#endif
	ATT_END_H,

}ATT_HANDLE;

void set_adv_data(uint8_t adv_type); // 0 - atc1441, 1 - Custom, 2 - Mi, 3 - all

extern u8 my_RxTx_Data[16];

void my_att_init();
void init_ble();
void ble_get_name(void);
bool ble_get_connected();
void ble_send_measures(void);
void ble_send_ext(void);
void ble_send_lcd(void);
void ble_send_cmf(void);
#if USE_TRIGGER_OUT
void ble_send_trg(void);
void ble_send_trg_flg(void);
#endif
#if USE_FLASH_MEMO
void send_memo_blk(void);
#endif
int otaWritePre(void * p);
int RxTxWrite(void * p);
void ev_adv_timeout(u8 e, u8 *p, int n);

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
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, my_RxTx_Data, sizeof(cfg) + 3);
}
