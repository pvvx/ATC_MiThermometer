#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app.h"
#include "stack/ble/ble.h"

extern uint8_t mac_public[6], mac_random_static[6];
extern uint8_t ble_name[32];

extern uint8_t ota_is_working;
extern uint8_t ble_connected; // bit 0 - connected, bit 1 - conn_param_update, bit 2 - paring success, bit 7 - reset device on disconnect

//extern uint32_t adv_send_count;

#define ADV_BUFFER_SIZE		(31-3)
typedef struct __attribute__((packed)) _adv_buf_t {
	uint32_t send_count;
	uint8_t data_size;
	uint8_t flag[3];
	uint8_t data[ADV_BUFFER_SIZE];
}adv_buf_t;
extern adv_buf_t adv_buf;

extern u16 batteryValueInCCC;
extern u16 tempValueInCCC;
extern u16 temp2ValueInCCC;
extern u16 humiValueInCCC;
extern u16 RxTxValueInCCC;

#define SEND_BUFFER_SIZE	(ATT_MTU_SIZE-3) // = 20
extern uint8_t send_buf[SEND_BUFFER_SIZE];
extern u8 my_RxTx_Data[16];

#if DEVICE_TYPE == DEVICE_LYWSD03MMC
extern u8 my_HardStr[4];
#endif

#define CON_INERVAL_LAT		16 // 16*1.25 = 20 ms

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

#define ADV_HA_BLE_NS_UUID16 0x181C // 16-bit UUID Service 0x181C Test HA_BLE, no security
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
	HaBleID_opened,
	HaBleID_switch
} HaBleIDs_e;

// Type bit 5-7
typedef enum {
	HaBleType_uint = 0,	
	HaBleType_sint = (1<<5),
	HaBleType_float = (2<<5),
	HaBleType_string  = (3<<5),
	HaBleType_MAC  = (4<<5)
} HaBleTypes_e;

typedef struct __attribute__((packed)) _adv_na_ble_ns1_t {
	uint8_t		size;   // = 21?
	uint8_t		uid;	// = 0x16, 16-bit UUID
	uint16_t	UUID;	// = 0x181C, GATT Service HA_BLE
	uint8_t		p_st;
	uint8_t		p_id;	// = HaBleID_PacketId
	uint8_t		pid;	// PacketId (measurement count)
	uint8_t		t_st;
	uint8_t		t_id;	// = HaBleID_temperature
	int16_t		temperature; // x 0.01 degree
	uint8_t		h_st;
	uint8_t		h_id;	// = HaBleID_humidity
	uint16_t	humidity; // x 0.01 %
	uint8_t		b_st;
	uint8_t		b_id;	// = HaBleID_battery
	uint8_t		battery_level; // 0..100 %
	uint8_t		v_st;
	uint8_t		v_id;	// = HaBleID_voltage
	uint16_t	battery_mv; // mV
} adv_ha_ble_ns1_t, * padv_ha_ble_ns1_t;

typedef struct __attribute__((packed)) _adv_na_ble_ns2_t {
	uint8_t		size;	// = 15?
	uint8_t		uid;	// = 0x16, 16-bit UUID
	uint16_t	UUID;	// = 0x181C, GATT Service 0x181C
	uint8_t		p_st;
	uint8_t		p_id;	// = HaBleID_PacketId
	uint8_t		pid;	// PacketId (!= measurement count)
	uint8_t		s_st;
	uint8_t		s_id;	// = HaBleID_switch ?
	uint8_t		swtch;
	uint8_t		o_st;
	uint8_t		o_id;	// = HaBleID_opened ?
	uint8_t		opened;
	uint8_t		c_st;
	uint8_t		c_id;	// = HaBleID_count
	uint32_t	counter; // count
} adv_ha_ble_ns2_t, * padv_ha_ble_ns2_t;

#define ADV_CUSTOM_UUID16 0x181A // 16-bit UUID Service 0x181A Environmental Sensing

// GATT Service 0x181A Environmental Sensing
// All data little-endian
typedef struct __attribute__((packed)) _adv_custom_t {
	uint8_t		size;	// = 18
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

void set_adv_data(void);

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
void set_pvvx_adv_data(uint32_t cnt);
void set_atc_adv_data(uint32_t cnt);
void set_mi_adv_data(uint32_t cnt);

inline void ble_send_temp(void) {
	bls_att_pushNotifyData(TEMP_LEVEL_INPUT_DP_H, (u8 *) &measured_data.temp_x01, 2);
}

inline void ble_send_temp2(void) {
	bls_att_pushNotifyData(TEMP2_LEVEL_INPUT_DP_H, (u8 *) &measured_data.temp, 2);
}

inline void ble_send_humi(void) {
	bls_att_pushNotifyData(HUMI_LEVEL_INPUT_DP_H, (u8 *) &measured_data.humi, 2);
}

inline void ble_send_battery(void) {
	bls_att_pushNotifyData(BATT_LEVEL_INPUT_DP_H, (u8 *) &measured_data.battery_level, 1);
}

inline void ble_send_cfg(void) {
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, my_RxTx_Data, sizeof(cfg) + 3);
}
