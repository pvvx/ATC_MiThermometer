#ifndef _BLE_H_
#define _BLE_H_

#include "app.h"
#include "stack/ble/ble.h"

#define BTHOME_UUID16 0xFCD2 // 16-bit UUID Service 0xFCD2 BTHOME

extern u8 mac_public[6];
extern u8 mac_random_static[6];
extern u8 ble_name[MAX_DEV_NAME_LEN + 2];

// adv_buf.ext_adv_init;
enum {
	EXT_ADV_Off = 0,	// Legacy
	EXT_ADV_Coded,		// LE long range (PHY Coded)
	EXT_ADV_1M			// Extension Advertise (PHY 1M)
};

//extern u32 adv_send_count;
#if (DEV_SERVICES & SERVICE_LE_LR)
#define ADV_BUFFER_SIZE		(62-3)
#else
#define ADV_BUFFER_SIZE		(31-3)
#endif
typedef struct _adv_buf_t {
	u32 send_count; // count & id advertise, = beacon_nonce.cnt32
	u8 meas_count;
	u8 call_count; 	// count 1..update_count
	u8 update_count;	// refresh adv_buf.data in next set_adv_data()
#if (DEV_SERVICES & SERVICE_LE_LR) // support extension advertise
	u8 ext_adv_init; 	// flag ext_adv init
#endif
	u8 data_size;		// Advertise data size
	u8 flag[3];		// Advertise type flags
	u8 data[ADV_BUFFER_SIZE];
}adv_buf_t;
extern adv_buf_t adv_buf;

extern u16 batteryValueInCCC;
extern u16 tempValueInCCC;
extern u16 temp2ValueInCCC;
extern u16 humiValueInCCC;
extern u16 anaValueInCCC;
extern u16 RxTxValueInCCC;

#define SEND_BUFFER_SIZE	(ATT_MTU_SIZE-3) // = 20
extern u8 send_buf[SEND_BUFFER_SIZE];
extern u8 my_RxTx_Data[sizeof(cfg) + 2];

#if defined(MI_HW_VER_FADDR) && (MI_HW_VER_FADDR)
// #if (DEVICE_TYPE == DEVICE_LYWSD03MMC) || (DEVICE_TYPE == DEVICE_MJWSD05MMC) || (DEVICE_TYPE == DEVICE_MHO_C401) || (DEVICE_TYPE == DEVICE_MJWSD05MMC_EN)
extern u8 my_HardStr[4];
extern u8 my_SerialStr[20];
#endif

#define DEF_CON_INERVAL		16 // in 1.25 ms -> 16*1.25 = 20 ms
#define DEF_CON_LAT_INERVAL	1000 // in 1 ms -> 1 sec
#define DEF_CONNECT_LATENCY (((int)(DEF_CON_LAT_INERVAL*100)/(int)(DEF_CON_INERVAL * 125))-1) // = 49, (49+1)*1.25*16 = 1000 ms)

#define CONNECTABLE_ADV_INERVAL 1600 //x0.625 ms = 1 sec
#define EXT_ADV_INTERVAL ADV_INTERVAL_50MS // event adv interval

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
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
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
#elif (DEV_SERVICES & SERVICE_18B20)
	//// Temp service ////
	/**********************************************************************************************/
	TEMP_PS_H, 								//UUID: 2800, 	VALUE: uuid 181A
	TEMP_LEVEL_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	TEMP_LEVEL_INPUT_DP_H,					//UUID: 2A1F 	VALUE: last_temp
	TEMP_LEVEL_INPUT_CCB_H,					//UUID: 2902, 	VALUE: tempValCCC

	TEMP2_LEVEL_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	TEMP2_LEVEL_INPUT_DP_H,					//UUID: 2A6E 	VALUE: measured_data.temp
	TEMP2_LEVEL_INPUT_CCB_H,				//UUID: 2902, 	VALUE: temp2ValCCC
#endif
#if (DEV_SERVICES & SERVICE_IUS)
	/**********************************************************************************************/
	ANA_PS_H, 								//UUID: 2800, 	VALUE: uuid 181A
	ANA_VALUE_INPUT_CD_H,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	ANA_VALUE_INPUT_DP_H,					//UUID: 2A1F 	VALUE: analog value (uint16)
	ANA_VALUE_INPUT_CCB_H,					//UUID: 2902, 	VALUE: anapValCCC
#endif
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

	// Mi Advertising char
	/**********************************************************************************************/
	Mi_PS_H, 								//UUID: 2800, 	VALUE: 0xFE95 service uuid
	Mi_CMD_OUT_DESC_H,						//UUID: 2901, 	VALUE: my_MiName

	ATT_END_H

} ATT_HANDLE;

void app_enter_ota_mode(void);
void set_adv_data(void);

void my_att_init();
void init_ble();
void ble_set_name(void);
void ble_send_measures(void);
void ble_send_ext(void);
void ble_send_lcd(void);
void ble_send_cmf(void);
#if (DEV_SERVICES & SERVICE_RDS) || (DEV_SERVICES & SERVICE_TH_TRG)
void ble_send_trg(void);
void ble_send_trg_flg(void);
#endif
#if (DEV_SERVICES & SERVICE_KEY) || (DEV_SERVICES & SERVICE_RDS)
void set_adv_con_time(int restore);
#endif
#if (DEV_SERVICES & SERVICE_HISTORY)
void send_memo_blk(void);
#endif
int otaWritePre(void * p);
#ifdef CHG_CONN_PARAM
int chgConnParameters(void * p);
#endif
int RxTxWrite(void * p);
void ev_adv_timeout(u8 e, u8 *p, int n);
void set_pvvx_adv_data(void);
void set_atc_adv_data(void);
void set_mi_adv_data(void);

#if (DEV_SERVICES & SERVICE_LE_LR)
void load_adv_data(void);
#endif

#if (DEV_SERVICES & (SERVICE_THS | SERVICE_18B20 | SERVICE_PLM))
inline void ble_send_temp01(void) {
	bls_att_pushNotifyData(TEMP_LEVEL_INPUT_DP_H, (u8 *) &measured_data.temp_x01, 2);
}

inline void ble_send_temp001(void) {
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
	bls_att_pushNotifyData(TEMP2_LEVEL_INPUT_DP_H, (u8 *) &measured_data.temp, 2);
#else
	bls_att_pushNotifyData(TEMP2_LEVEL_INPUT_DP_H, (u8 *) &measured_data.xtemp[0], 2);
#endif
}

#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
inline void ble_send_humi(void) {
	bls_att_pushNotifyData(HUMI_LEVEL_INPUT_DP_H, (u8 *) &measured_data.humi, 2);
}
#endif
#endif
#if (DEV_SERVICES & SERVICE_IUS)
inline void ble_send_ana(void) {
	bls_att_pushNotifyData(ANA_VALUE_INPUT_DP_H, (u8 *) &measured_data.voltage, 2);
}
#endif

inline void ble_send_battery(void) {
	bls_att_pushNotifyData(BATT_LEVEL_INPUT_DP_H, (u8 *) &measured_data.battery_level, 1);
}

inline void ble_send_cfg(void) {
	memcpy(&my_RxTx_Data[2], &cfg, sizeof(cfg));
	bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, my_RxTx_Data, sizeof(cfg) + 3);
}

#endif //_BLE_H_
