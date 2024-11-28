#include "tl_common.h"

#include "stack/ble/ble.h"
#include "ble.h"

static const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG; // 2902

//static const u16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF; // 2907

//static const u16 reportRefUUID = GATT_UUID_REPORT_REF; // 2908

static const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT; // 2904

static const u16 userdesc_UUID	= GATT_UUID_CHAR_USER_DESC; // 2901

static const u16 serviceChangeUUID = GATT_UUID_SERVICE_CHANGE; // 2a05 https://github.com/oesmith/gatt-xml/blob/master/org.bluetooth.characteristic.gatt.service_changed.xml

static const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE; // 2800

static const u16 my_characterUUID = GATT_UUID_CHARACTER; // 2803

static const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION; // 180A

//static const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID; // 2A50

static const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME; // 2A00

static const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS; // 1800

static const u16 my_appearanceUIID = GATT_UUID_APPEARANCE; // 2A01

static const u16 my_periConnParamUUID = GATT_UUID_PERI_CONN_PARAM; // 2A04

// https://github.com/sputnikdev/bluetooth-gatt-parser/blob/master/src/main/resources/gatt/characteristic/org.bluetooth.characteristic.gap.appearance.xml
static const u16 my_appearance = 768; // value="Generic Thermometer"

static const u16 my_gattServiceUUID = SERVICE_UUID_GENERIC_ATTRIBUTE; // 1801


#if USE_DEVICE_INFO_CHR_UUID

//#define CHARACTERISTIC_UUID_SYSTEM_ID			0x2A23 // System ID
#define CHARACTERISTIC_UUID_MODEL_NUMBER		0x2A24 // Model Number String: LYWSD03MMC
#define CHARACTERISTIC_UUID_SERIAL_NUMBER		0x2A25 // Serial Number String: F1.0-CFMK-LB-ZCXTJ--
#define CHARACTERISTIC_UUID_FIRMWARE_REV		0x2A26 // Firmware Revision String: 1.0.0_0109
#define CHARACTERISTIC_UUID_HARDWARE_REV		0x2A27 // Hardware Revision String: B1.4
#define CHARACTERISTIC_UUID_SOFTWARE_REV		0x2A28 // Software Revision String: 0x109
#define CHARACTERISTIC_UUID_MANUFACTURER_NAME	0x2A29 // Manufacturer Name String: miaomiaoce.com

//// device Information  attribute values
//static const u16 my_UUID_SYSTEM_ID		    = CHARACTERISTIC_UUID_SYSTEM_ID;
static const u16 my_UUID_MODEL_NUMBER	    = CHARACTERISTIC_UUID_MODEL_NUMBER;
static const u16 my_UUID_SERIAL_NUMBER	    = CHARACTERISTIC_UUID_SERIAL_NUMBER;
static const u16 my_UUID_FIRMWARE_REV	    = CHARACTERISTIC_UUID_FIRMWARE_REV;
static const u16 my_UUID_HARDWARE_REV	    = CHARACTERISTIC_UUID_HARDWARE_REV;
static const u16 my_UUID_SOFTWARE_REV	    = CHARACTERISTIC_UUID_SOFTWARE_REV;
static const u16 my_UUID_MANUFACTURER_NAME  = CHARACTERISTIC_UUID_MANUFACTURER_NAME;
static const u8 my_ModCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_HardRev_DP_H), U16_HI(DeviceInformation_HardRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HARDWARE_REV), U16_HI(CHARACTERISTIC_UUID_HARDWARE_REV)
};
static const u8 my_SerialCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_FirmRev_DP_H), U16_HI(DeviceInformation_FirmRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_SERIAL_NUMBER), U16_HI(CHARACTERISTIC_UUID_SERIAL_NUMBER)
};
static const u8 my_FirmCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_FirmRev_DP_H), U16_HI(DeviceInformation_FirmRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_FIRMWARE_REV), U16_HI(CHARACTERISTIC_UUID_FIRMWARE_REV)
};
static const u8 my_HardCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_HardRev_DP_H), U16_HI(DeviceInformation_HardRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HARDWARE_REV), U16_HI(CHARACTERISTIC_UUID_HARDWARE_REV)
};
static const u8 my_SoftCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_SoftRev_DP_H), U16_HI(DeviceInformation_SoftRev_DP_H),
	U16_LO(CHARACTERISTIC_UUID_SOFTWARE_REV), U16_HI(CHARACTERISTIC_UUID_SOFTWARE_REV)
};
static const u8 my_ManCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_ManName_DP_H), U16_HI(DeviceInformation_ManName_DP_H),
	U16_LO(CHARACTERISTIC_UUID_MANUFACTURER_NAME), U16_HI(CHARACTERISTIC_UUID_MANUFACTURER_NAME)
};
static const u8 my_FirmStr[] = {"github.com/pvvx"};
#if USE_FLASH_SERIAL_UID
RAM uint8_t my_SerialStr[20]; // "556202-C86013-123456"
#endif

#if DEVICE_TYPE == DEVICE_MJWSD05MMC
static const u8 my_ModelStr[] = {"MJWSD05MMC"};
RAM u8 my_HardStr[4];// = {"V2.3"}
RAM u8 my_SerialStr[20]; // = {"F2.0-CFMK-LB-TMDZ---"}
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0026"
static const u8 my_ManStr[] = {"miaomiaoce.com"};

#elif DEVICE_TYPE == DEVICE_MHO_C401
static const u8 my_ModelStr[] = {"MHO-C401"};
RAM u8 my_HardStr[4]; // = {"2020"};
RAM u8 my_SerialStr[20]; // "0000-0000-0000-00000"
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0110"
static const u8 my_ManStr[] = {"miaomiaoce.com"};

#elif DEVICE_TYPE == DEVICE_MHO_C401N
static const u8 my_ModelStr[] = {"MHO-C401N"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0008"}; // "0000-0000-0000-00000"
#endif
static const u8 my_HardStr[] = {"2022"};
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0110"
static const u8 my_ManStr[] = {"miaomiaoce.com"};

#elif DEVICE_TYPE == DEVICE_LYWSD03MMC
static const u8 my_ModelStr[] = {"LYWSD03MMC"};
RAM u8 my_HardStr[4];// = {"B1.4"};
RAM u8 my_SerialStr[20]; // = {"F1.0-CFMK-LB-ZCXTJ--"}; // B1.5 "F2.0-CFMK-LB-JHBD---", B1.9 "F1.0-CFMK-LB-FLD----"
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"miaomiaoce.com"};

#elif DEVICE_TYPE == DEVICE_CGG1
static const u8 my_ModelStr[] = {"CGG1"};
#if DEVICE_CGG1_ver == 2022
static const u8 my_HardStr[] = {"2022"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0007"}; // "0000-0000-0000-00000"
#endif
#else
static const u8 my_HardStr[] = {"0001"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0002"}; // "0000-0000-0000-00000"
#endif
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"Qingping Technology (Beijing) Co., Ltd."};

#elif DEVICE_TYPE == DEVICE_CGDK2
static const u8 my_ModelStr[] = {"CGDK2"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0006"}; // "0000-0000-0000-00000"
#endif
static const u8 my_HardStr[] = {"2.1.0"};
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)};
static const u8 my_ManStr[] = {"Qingping Technology (Beijing) Co., Ltd."};

#elif DEVICE_TYPE == DEVICE_MHO_C122
static const u8 my_ModelStr[] = {"MHO-C122"};
static const u8 my_HardStr[] = {"V1.1-202106"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"2AWMOMHOC122"};
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)};
static const u8 my_ManStr[] = {"MiaoMiaoCe Technology (Beijing) Co., Ltd."};

#elif DEVICE_TYPE == DEVICE_TS0201
static const u8 my_ModelStr[] = {"TS0201"};
static const u8 my_HardStr[4] = {"V1.0"};// = {"V1.0"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0001"}; // "0000-0000-0000-00000"
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"DIY.home"};

#elif DEVICE_TYPE == DEVICE_TH03Z
static const u8 my_ModelStr[] = {"TH03Z"};
static const u8 my_HardStr[4] = {"V1.0"};// = {"V1.0"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0002"}; // "0000-0000-0000-00000"
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"DIY.home"};

#elif DEVICE_TYPE == DEVICE_ZTH01
static const u8 my_ModelStr[] = {"ZTH01"};
static const u8 my_HardStr[4] = {"V1.0"};// = {"V1.0"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0003"}; // "0000-0000-0000-00000"
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"DIY.home"};

#elif DEVICE_TYPE == DEVICE_ZTH02
static const u8 my_ModelStr[] = {"ZTH02"};
static const u8 my_HardStr[4] = {"V1.0"};// = {"V1.0"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0004"}; // "0000-0000-0000-00000"
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"DIY.home"};

#elif DEVICE_TYPE == DEVICE_TNK01
static const u8 my_ModelStr[] = {"TNK01"};
static const u8 my_HardStr[4] = {"V1.0"};// = {"V1.0"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0005"}; // "0000-0000-0000-00000"
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"DIY.home"};

#elif DEVICE_TYPE == DEVICE_TB03F
static const u8 my_ModelStr[] = {"TB-03F"};
static const u8 my_HardStr[4] = {"V1.0"};// = {"V1.0"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0006"}; // "0000-0000-0000-00000"
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"DIY.home"};

#elif DEVICE_TYPE == DEVICE_PLM1
static const u8 my_ModelStr[] = {"PLM-1"};
static const u8 my_HardStr[4] = {"V1.3"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0007"}; // "0000-0000-0000-00000"
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"Tuya"};

#elif DEVICE_TYPE == DEVICE_ZTH03
static const u8 my_ModelStr[] = {"TH03"};
static const u8 my_HardStr[4] = {"V1.1"};
#if !USE_FLASH_SERIAL_UID
static const u8 my_SerialStr[] = {"0000-0000-0000-0008"}; // "0000-0000-0000-00000"
#endif
static const u8 my_SoftStr[] = {'V','0'+(VERSION>>4),'.','0'+(VERSION&0x0f)}; // "0109"
static const u8 my_ManStr[] = {"Tuya"};
#else
#error "DEVICE_TYPE = ?"
#endif
//------------------
#endif // USE_DEVICE_INFO_CHR_UUID

#ifdef CHG_CONN_PARAM
RAM gap_periConnectParams_t my_periConnParameters = {DEF_CON_INERVAL, DEF_CON_INERVAL, 0, DEF_CON_INERVAL*250};
#else
static const gap_periConnectParams_t def_periConnParameters = {DEF_CON_INERVAL, DEF_CON_INERVAL, DEF_CONNECT_LATENCY , DEF_CON_INERVAL*(DEF_CONNECT_LATENCY)*3};
RAM gap_periConnectParams_t my_periConnParameters = {DEF_CON_INERVAL, DEF_CON_INERVAL+3, 0, DEF_CON_INERVAL*125};
#endif

static u32 serviceChangeVal = 0; // uint16 1..65535 "Start of Affected Attribute Handle Range", uint16 1..65535 "End of Affected Attribute Handle Range"
static u16 serviceChangeCCC = 0;

//////////////////////// Battery /////////////////////////////////////////////////
static const u16 my_batServiceUUID        = SERVICE_UUID_BATTERY;
static const u16 my_batCharUUID       	  = CHARACTERISTIC_UUID_BATTERY_LEVEL;
RAM u16 batteryValueInCCC;

//////////////////////// Temp/Hum /////////////////////////////////////////////////
#define CHARACTERISTIC_UUID_TEMPERATYRE 0x2A6E // https://github.com/oesmith/gatt-xml/blob/master/org.bluetooth.characteristic.temperature.xml
#define CHARACTERISTIC_UUID_TEMPERATYRE2 0x2A1F
#define CHARACTERISTIC_UUID_HUMIDITY 0x2A6F // https://github.com/oesmith/gatt-xml/blob/master/org.bluetooth.characteristic.humidity.xml

const u16 my_envServiceUUID       = 0x181A; // environmental_sensing
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_18B20 | SERVICE_PLM))
static const u16 my_tempCharUUID       	  = CHARACTERISTIC_UUID_TEMPERATYRE2;
static const u16 my_temp2CharUUID      	  = CHARACTERISTIC_UUID_TEMPERATYRE;
RAM u16 tempValueInCCC;
RAM u16 temp2ValueInCCC;
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
static const u16 my_humiCharUUID       	  = CHARACTERISTIC_UUID_HUMIDITY;
RAM u16 humiValueInCCC;
#endif
#endif

#if (DEV_SERVICES & SERVICE_IUS)
#define CHARACTERISTIC_UUID_ANALOG	0x2A58 // https://github.com/oesmith/gatt-xml/blob/master/org.bluetooth.characteristic.analog.xml
const u16 my_anaCharUUID	= CHARACTERISTIC_UUID_ANALOG;
RAM u16 anaValueInCCC;
#endif

///////////////////////// OTA ////////////////////////////////
static const  u8 my_OtaUUID[16]					    = TELINK_SPP_DATA_OTA;
static const  u8 my_OtaServiceUUID[16]				= TELINK_OTA_UUID_SERVICE;
static u8 my_OtaData 						        = 0x00;
static const u8  my_OtaName[] = {'O', 'T', 'A'};

/////////////// RxTx/CMD Char ///////////////
#define  COMMAND_UUID16_SERVICE 0x1F10
#define  COMMAND_UUID16_CHARACTERISTIC 0x1F1F

static const  u16 my_RxTxUUID				= COMMAND_UUID16_CHARACTERISTIC;
static const  u16 my_RxTx_ServiceUUID		= COMMAND_UUID16_SERVICE;
RAM u8 my_RxTx_Data[sizeof(cfg) + 2];
RAM u16 RxTxValueInCCC;

// MI 0x95FE
static const u16 mi_primary_service_uuid = 0xfe95;
#if (!defined(USE_MIHOME) || USE_MIHOME == 0)
static const u8  my_MiName[] = {'M', 'i'};
#endif

//// GAP attribute values
static const u8 my_devNameCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(GenericAccess_DeviceName_DP_H), U16_HI(GenericAccess_DeviceName_DP_H),
	U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
};
static const u8 my_appearanceCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(GenericAccess_Appearance_DP_H), U16_HI(GenericAccess_Appearance_DP_H),
	U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
};
static const u8 my_periConnParamCharVal[5] = {
#ifdef CHG_CONN_PARAM
	CHAR_PROP_READ | CHAR_PROP_WRITE,
#else
	CHAR_PROP_READ,
#endif
	U16_LO(CONN_PARAM_DP_H), U16_HI(CONN_PARAM_DP_H),
	U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
};
//// GATT attribute values
static const u8 my_serviceChangeCharVal[5] = {
	CHAR_PROP_INDICATE,
	U16_LO(GenericAttribute_ServiceChanged_DP_H), U16_HI(GenericAttribute_ServiceChanged_DP_H),
	U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
};

//// Battery attribute values
static const u8 my_batCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(BATT_LEVEL_INPUT_DP_H), U16_HI(BATT_LEVEL_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_BATTERY_LEVEL), U16_HI(CHARACTERISTIC_UUID_BATTERY_LEVEL)
};

#if (DEV_SERVICES & (SERVICE_THS | SERVICE_18B20 | SERVICE_PLM))
//// Temp attribute values
static const u8 my_tempCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(TEMP_LEVEL_INPUT_DP_H), U16_HI(TEMP_LEVEL_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_TEMPERATYRE2), U16_HI(CHARACTERISTIC_UUID_TEMPERATYRE2)
};
static const u8 my_temp2CharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(TEMP_LEVEL_INPUT_DP_H), U16_HI(TEMP_LEVEL_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_TEMPERATYRE), U16_HI(CHARACTERISTIC_UUID_TEMPERATYRE)
};
#endif

#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
//// Humi attribute values
static const u8 my_humiCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(HUMI_LEVEL_INPUT_DP_H), U16_HI(HUMI_LEVEL_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HUMIDITY), U16_HI(CHARACTERISTIC_UUID_HUMIDITY)
};
#endif

#if (DEV_SERVICES & SERVICE_IUS)
//// Temp attribute values
static const u8 my_anaCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(ANA_VALUE_INPUT_DP_H), U16_HI(ANA_VALUE_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_ANALOG), U16_HI(CHARACTERISTIC_UUID_ANALOG)
};
#endif

//// OTA attribute values
#define TELINK_SPP_DATA_OTA1 				0x12,0x2B,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00
static const u8 my_OtaCharVal[19] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(OTA_CMD_OUT_DP_H), U16_HI(OTA_CMD_OUT_DP_H),
	TELINK_SPP_DATA_OTA1,
};

//// RxTx attribute values
static const u8 my_RxTxCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(RxTx_CMD_OUT_DP_H), U16_HI(RxTx_CMD_OUT_DP_H),
	U16_LO(COMMAND_UUID16_CHARACTERISTIC), U16_HI(COMMAND_UUID16_CHARACTERISTIC)
};
// TM : to modify
//static const
RAM attribute_t my_Attributes[] = {
	{ATT_END_H - 1, 0,0,0,0,0},	// total num of attribute
	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_gapServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_devNameCharVal),(u8*)(&my_characterUUID),(u8*)(my_devNameCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,3,(u8*)(&my_devNameUUID),(u8*)&ble_name[2], 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_appearanceCharVal),(u8*)(&my_characterUUID),(u8*)(my_appearanceCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_appearance),(u8*)(&my_appearanceUIID),(u8*)(&my_appearance), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_periConnParamCharVal),(u8*)(&my_characterUUID),(u8*)(my_periConnParamCharVal), 0},
#ifdef CHG_CONN_PARAM
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(my_periConnParameters),(u8*)(&my_periConnParamUUID),(u8*)(&my_periConnParameters), 0, chgConnParameters},
#else
		{0,ATT_PERMISSIONS_READ,2,sizeof(def_periConnParameters),(u8*)(&my_periConnParamUUID),(u8*)(&def_periConnParameters), 0},
#endif
	// 0008 - 000b gatt
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gattServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_serviceChangeCharVal),(u8*)(&my_characterUUID),(u8*)(my_serviceChangeCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(serviceChangeVal),(u8*)(&serviceChangeUUID),(u8*)(&serviceChangeVal), 0},
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(serviceChangeCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&serviceChangeCCC), 0},
#if USE_DEVICE_INFO_CHR_UUID
	// 000c - 0018 Device Information Service
	{13,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_devServiceUUID), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_ModCharVal),(u8*)(&my_characterUUID),(u8*)(my_ModCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_ModelStr),(u8*)(&my_UUID_MODEL_NUMBER),(u8*)(my_ModelStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_SerialCharVal),(u8*)(&my_characterUUID),(u8*)(my_SerialCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_SerialStr),(u8*)(&my_UUID_SERIAL_NUMBER),(u8*)(my_SerialStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_FirmCharVal),(u8*)(&my_characterUUID),(u8*)(my_FirmCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_FirmStr),(u8*)(&my_UUID_FIRMWARE_REV),(u8*)(my_FirmStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_HardCharVal),(u8*)(&my_characterUUID),(u8*)(my_HardCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_HardStr),(u8*)(&my_UUID_HARDWARE_REV),(u8*)(my_HardStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_SoftCharVal),(u8*)(&my_characterUUID),(u8*)(my_SoftCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_SoftStr),(u8*)(&my_UUID_SOFTWARE_REV),(u8*)(my_SoftStr), 0},

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_ManCharVal),(u8*)(&my_characterUUID),(u8*)(my_ManCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_ManStr),(u8*)(&my_UUID_MANUFACTURER_NAME),(u8*)(my_ManStr), 0},
#endif
	////////////////////////////////////// Battery Service /////////////////////////////////////////////////////
	// 0019 - 001C
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_batServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_batCharVal),(u8*)(&my_characterUUID), (u8*)(my_batCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(measured_data.battery_level),(u8*)(&my_batCharUUID),(u8*)(&measured_data.battery_level), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(batteryValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&batteryValueInCCC), 0},	//value
#if (DEV_SERVICES & (SERVICE_THS | SERVICE_PLM))
	////////////////////////////////////// TH Service /////////////////////////////////////////////////////
	// 001D - 0026
	{10,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_envServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_tempCharVal),(u8*)(&my_characterUUID),(u8*)(my_tempCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(measured_data.temp_x01),(u8*)(&my_tempCharUUID),(u8*)(&measured_data.temp_x01), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(tempValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&tempValueInCCC), 0},	//value

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_temp2CharVal),(u8*)(&my_characterUUID),(u8*)(my_temp2CharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(measured_data.temp),(u8*)(&my_temp2CharUUID),(u8*)(&measured_data.temp), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(temp2ValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&temp2ValueInCCC), 0},	//value

		{0,ATT_PERMISSIONS_READ,2,sizeof(my_humiCharVal),(u8*)(&my_characterUUID), (u8*)(my_humiCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(measured_data.humi),(u8*)(&my_humiCharUUID),(u8*)(&measured_data.humi), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(humiValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&humiValueInCCC), 0},	//value
#elif (DEV_SERVICES & SERVICE_18B20)
	////////////////////////////////////// T Service /////////////////////////////////////////////////////
	// 001D - 0026
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_envServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_tempCharVal),(u8*)(&my_characterUUID),(u8*)(my_tempCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(measured_data.temp_x01),(u8*)(&my_tempCharUUID),(u8*)(&measured_data.temp_x01), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(tempValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&tempValueInCCC), 0},	//value
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_temp2CharVal),(u8*)(&my_characterUUID),(u8*)(my_temp2CharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(measured_data.xtemp[0]),(u8*)(&my_temp2CharUUID),(u8*)(&measured_data.xtemp[0]), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(temp2ValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&temp2ValueInCCC), 0},	//value
#elif (DEV_SERVICES & SERVICE_IUS)
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&my_envServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_anaCharVal),(u8*)(&my_characterUUID),(u8*)(my_anaCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(measured_data.voltage),(u8*)(&my_anaCharUUID),(u8*)(&measured_data.voltage), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(anaValueInCCC),(u8*)(&clientCharacterCfgUUID),(u8*)(&anaValueInCCC), 0},	//value
#endif
	////////////////////////////////////// OTA /////////////////////////////////////////////////////
	// 0027 - 002A
	{4,ATT_PERMISSIONS_READ, 2,16,(u8*)(&my_primaryServiceUUID),(u8*)(&my_OtaServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_OtaCharVal),(u8*)(&my_characterUUID),(u8*)(my_OtaCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_RDWR,16,sizeof(my_OtaData),(u8*)(&my_OtaUUID),(&my_OtaData), &otaWritePre, &otaRead},			//value
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_OtaName),(u8*)(&userdesc_UUID),(u8*)(my_OtaName), 0},
	////////////////////////////////////// RxTx ////////////////////////////////////////////////////
	// 002B - 002E RxTx Communication
	{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_RxTx_ServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ, 2,sizeof(my_RxTxCharVal),(u8*)(&my_characterUUID),	(u8*)(my_RxTxCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_RDWR, 2,sizeof(my_RxTx_Data),(u8*)(&my_RxTxUUID), (u8*)&my_RxTx_Data, &RxTxWrite, 0},
		{0,ATT_PERMISSIONS_RDWR, 2,sizeof(RxTxValueInCCC),(u8*)(&clientCharacterCfgUUID), 	(u8*)(&RxTxValueInCCC), 0},	//value
	//Mi 0x95FE
	{2,ATT_PERMISSIONS_READ, 2,2,(u8*)(&my_primaryServiceUUID),(u8*)(&mi_primary_service_uuid), 0},
		{0,ATT_PERMISSIONS_READ, 2,sizeof (my_MiName),(u8*)(&userdesc_UUID),(u8*)(my_MiName), 0},
};

void my_att_init(void) {
#if USE_FLASH_SERIAL_UID
	uint8_t buf[16];
	uint8_t *p = my_SerialStr;
	// Read SoC ID, version
	buf[0] = REG_ADDR8(0x7f);
	buf[1] = REG_ADDR8(0x7e);
	buf[2] = REG_ADDR8(0x7d);
	p = str_bin2hex(p, buf, 3);
	*p++ = '-';
	// Read flash ID
	flash_read_id(buf);
	p = str_bin2hex(p, buf, 3);
	*p++ = '-';
	// Read flash UID
	flash_read_uid(buf);
	memcpy(p, buf, 6);
	//ser_uid_txt(p, &buf[4], 7);
#endif
#if (DEV_SERVICES & SERVICE_PINCODE)
	if (pincode) {
		my_Attributes[RxTx_CMD_OUT_DP_H].perm = ATT_PERMISSIONS_SECURE_CONN_RDWR;
		my_Attributes[OTA_CMD_OUT_DP_H].perm = ATT_PERMISSIONS_SECURE_CONN_RDWR;
	} else {
		my_Attributes[RxTx_CMD_OUT_DP_H].perm = ATT_PERMISSIONS_RDWR;
		my_Attributes[OTA_CMD_OUT_DP_H].perm = ATT_PERMISSIONS_RDWR;
	}
#endif
	bls_att_setAttributeTable ((u8 *)my_Attributes);
}


