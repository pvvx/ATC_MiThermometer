#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#define VERSION 0x14
#define EEP_SUP_VER 0x09 // EEP data minimum supported version

#define DEVICE_LYWSD03MMC   0	// LCD display LYWSD03MMC
#define DEVICE_MHO_C401   	1	// E-Ink display MHO-C401

#define DEVICE_TYPE			DEVICE_LYWSD03MMC // DEVICE_LYWSD03MMC or DEVICE_MHO_C401

#define BLE_SECURITY_ENABLE 1
#define BLE_HOST_SMP_ENABLE BLE_SECURITY_ENABLE

#define USE_TRIGGER_OUT 	1 // use trigger out (GPIO_PA5)

#if DEVICE_TYPE == DEVICE_MHO_C401

#define EPD_SHD				GPIO_PA5 // should be high
#define PULL_WAKEUP_SRC_PC4 PM_PIN_PULLUP_10K

#define EPD_BUSY			GPIO_PA5
#define PULL_WAKEUP_SRC_PA5 PM_PIN_PULLUP_1M
#define PA5_INPUT_ENABLE	1
#define PA5_FUNC			AS_GPIO

#define EPD_RST				GPIO_PA6
#define PULL_WAKEUP_SRC_PA6 PM_PIN_PULLUP_1M
#define PA6_INPUT_ENABLE	1
#define PA6_DATA_OUT		1
#define PA6_OUTPUT_ENABLE	1
#define PA6_FUNC			AS_GPIO

#define EPD_CSB				GPIO_PD2
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLUP_1M
#define PD2_INPUT_ENABLE	1
#define PD2_DATA_OUT		1
#define PD2_OUTPUT_ENABLE	1
#define PD2_FUNC			AS_GPIO

#define EPD_SDA				GPIO_PB7
#define PULL_WAKEUP_SRC_PB7 PM_PIN_PULLUP_1M
#define PB7_INPUT_ENABLE	1
#define PB7_DATA_OUT		1
#define PB7_OUTPUT_ENABLE	1
#define PB7_FUNC			AS_GPIO

#define EPD_SCL				GPIO_PD7
#define PULL_WAKEUP_SRC_PD7 PM_PIN_PULLUP_1M
#define PD7_INPUT_ENABLE	1
#define PD7_DATA_OUT		0
#define PD7_OUTPUT_ENABLE	1
#define PD7_FUNC			AS_GPIO

#if USE_TRIGGER_OUT

#define GPIO_TRG			GPIO_PB6	// pcb mark "P5"
#define PB6_INPUT_ENABLE	1
#define PB6_DATA_OUT		0
#define PB6_OUTPUT_ENABLE	0
#define PB6_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PB6	PM_PIN_PULLDOWN_100K

#endif // USE_TRIGGER_OUT

#elif DEVICE_TYPE == DEVICE_LYWSD03MMC

#if USE_TRIGGER_OUT

#define GPIO_TRG			GPIO_PA5	// pcb mark "reset"
#define PA5_INPUT_ENABLE	1
#define PA5_DATA_OUT		0
#define PA5_OUTPUT_ENABLE	0
#define PA5_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA5	PM_PIN_PULLDOWN_100K

#endif // USE_TRIGGER_OUT
#endif // DEVICE_TYPE == DEVICE_LYWSD03MMC

/* DEVICE_LYWSD03MMC Average consumption (Show battery on, Comfort on, advertising 2 sec, measure 10 sec):
 * 16 MHz - 17.43 uA
 * 24 MHz - 17.28 uA
 * 32 MHz - 17.36 uA
 * Average consumption Original Xiaomi LYWSD03MMC (advertising 1700 ms, measure 6800 ms):
 * 18.64 uA
 */
#define CLOCK_SYS_CLOCK_HZ  	24000000 // 16000000, 24000000, 32000000, 48000000
enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};

#define pm_wait_ms(t) cpu_stall_wakeup_by_timer0(t*CLOCK_SYS_CLOCK_1MS);
#define pm_wait_us(t) cpu_stall_wakeup_by_timer0(t*CLOCK_SYS_CLOCK_1US);

#define RAM _attribute_data_retention_ // short version, this is needed to keep the values in ram after sleep

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

	//// gatt ////
	/**********************************************************************************************/
	GenericAttribute_PS_H,					//UUID: 2800, 	VALUE: uuid 1801
	GenericAttribute_ServiceChanged_CD_H,	//UUID: 2803, 	VALUE:  			Prop: Indicate
	GenericAttribute_ServiceChanged_DP_H,   //UUID:	2A05,	VALUE: service change
	GenericAttribute_ServiceChanged_CCB_H,	//UUID: 2902,	VALUE: serviceChangeCCC

	//// battery service ////
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

	//// Ota ////
	/**********************************************************************************************/
	OTA_PS_H, 								//UUID: 2800, 	VALUE: telink ota service uuid
	OTA_CMD_OUT_CD_H,						//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	OTA_CMD_OUT_DP_H,						//UUID: telink ota uuid,  VALUE: otaData
	OTA_CMD_OUT_DESC_H,						//UUID: 2901, 	VALUE: otaName

	//// RxTx ////
	/**********************************************************************************************/
	RxTx_PS_H, 								//UUID: 1F10, 	VALUE: RxTx service uuid
	RxTx_CMD_OUT_CD_H,						//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	RxTx_CMD_OUT_DP_H,						//UUID: 1F1F,  VALUE: RxTxData
	RxTx_CMD_OUT_DESC_H,					//UUID: 2902, 	VALUE: RxTxValueInCCC
	
	// Mi Advertising char
	Mi_PS_H, 								//UUID: , 	VALUE: 0xFE95 service uuid
	Mi_CMD_OUT_DESC_H,						//UUID: 2901, 	VALUE: my_MiName
	
	ATT_END_H,

}ATT_HANDLE;

#include "vendor/common/default_config.h"

#if defined(__cplusplus)
}
#endif
