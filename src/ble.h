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
typedef struct _adv_buf_t {
	uint32_t send_count; // count & id advertise, = beacon_nonce.cnt32
	uint16_t old_measured_count; // old measured_data.count
	uint8_t update_count;	// refresh adv_buf.data in next set_adv_data()
	uint8_t call_count; 	// count 1..cfg.measure_interval
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
void set_pvvx_adv_data(void);
void set_atc_adv_data(void);
void set_mi_adv_data(void);

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
