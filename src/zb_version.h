/*********************************************************************************************
 * During OTA upgrade, the upgraded device will check the rules of the following three fields.
 * Refer to ZCL OTA specification for details.
 */

#define APP_RELEASE							0x99 // app release 9.9
#define APP_BUILD							0x99 // app build 9.9
#define STACK_RELEASE						0x30 // stack release 3.0
#define STACK_BUILD							0x01 // stack build 01


/* Zigbee Board ID:
//#define BOARD_LYWSD03MMC_B14		0 // number used for BLE firmware
#define BOARD_MHO_C401				1
#define BOARD_CGG1					2
//#define BOARD_LYWSD03MMC_B19		3 // number used for BLE firmware
//#define BOARD_LYWSD03MMC_DEVBIS	3 // ver https://github.com/devbis/z03mmc
//#define BOARD_LYWSD03MMC_B16		4 // number used for BLE firmware
//#define BOARD_WATERMETER			4 // ver https://github.com/slacky1965/watermeter_zed 
//#define BOARD_LYWSD03MMC_B17		5 // number used for BLE firmware
#define BOARD_CGDK2					6
#define BOARD_CGG1N					7 // 2022
#define BOARD_MHO_C401N				8 // 2022
#define BOARD_MJWSD05MMC			9
//#define BOARD_LYWSD03MMC_B15		10 // number used for BLE firmware
#define BOARD_LYWSD03MMC			10
#define BOARD_MHO_C122				11
// 13..15 - number used for BLE firmware - https://github.com/pvvx/ATC_MiThermometer
#define BOARD_TB03F					16 // DIY TB-03F-Kit
#define BOARD_TS0201_TZ3000			17
#define BOARD_TNKS					18 // Water tank controller (not yet published at the moment)
#define BOARD_THB2					19
#define BOARD_BTH01					20
#define BOARD_TH05					21
#define DEVICE_TH03Z   				22 // ZigBee TH03Z
#define BOARD_THB1					23 // https://github.com/pvvx/THB2
#define BOARD_TH05D					24 // TH05_V1.3 https://github.com/pvvx/THB2
#define BOARD_TH05F					25 // TH05Y_V1.2 https://github.com/pvvx/THB2
#define BOARD_THB3					26 // https://github.com/pvvx/THB2
#define BOARD_ZTH01   				27 // ZigBee ZTH01
#define BOARD_ZTH02   				28 // ZigBee ZTH02
*/

#ifndef DEVICE_CGG1_ver
#define DEVICE_CGG1_ver		   0 // =2022 - CGG1-M version 2022, or = 0 - CGG1-M version 2020,2021
#endif

#define DEVICE_LYWSD03MMC   10	// LCD display LYWSD03MMC
#define DEVICE_MHO_C401   	 1	// E-Ink display MHO-C401 2020
#define DEVICE_CGDK2 		 6  // LCD display "Qingping Temp & RH Monitor Lite"
#define DEVICE_MHO_C401N   	 8	// E-Ink display MHO-C401 2022
#define DEVICE_MJWSD05MMC	 9  // LCD display MJWSD05MMC
#define DEVICE_MHO_C122   	11	// LCD display MHO_C122
#define DEVICE_TB03F		16  // DIY TB-03F-Kit + INA226
#define DEVICE_TS0201   	17	// ZigBee TS0201, analog: IH-K009
#define DEVICE_TNK01		18  // DIY, PB-03F module, Water tank controller
#define DEVICE_TH03Z   		22	// ZigBee TH03Z
#define DEVICE_ZTH01   		27	// ZigBee ZTH01
#define DEVICE_ZTH02   		28	// ZigBee ZTH02
#define DEVICE_PLM1 		29  // Tuya BLE Plant monitor ECF-SGS01-A rev1.3 (BT3L Tuya module)
#define DEVICE_ZTH03 		30  // Tuya TH03 Zigbee LCD
#define DEVICE_LKTMZL02		31  // Tuya LKTMZL02 Zigbee LCD 2xAAA


#if 0 // not used in this code!
#define DEVICE_03MMC_DEVBIS	 3  // https://github.com/devbis/z03mmc
#define DEVICE_WATERMETER	 4	// https://github.com/slacky1965/watermeter_zed
#define DEVICE_TNK 			 16
#define DEVICE_TS0201_TZ3000 17 // https://github.com/pvvx/Zigbee2BLE
#endif

#if DEVICE_CGG1_ver == 0     
#define DEVICE_CGG1 		 2  // E-Ink display Old CGG1-M "Qingping Temp & RH Monitor"
#else
#define DEVICE_CGG1 		 7  // E-Ink display New CGG1-M "Qingping Temp & RH Monitor"
#endif

#ifndef DEVICE_TYPE
#define DEVICE_TYPE DEVICE_LYWSD03MMC
#endif

/* Chip IDs */
#define CHIP_TYPE_8267							0x00
#define CHIP_TYPE_8269							0x01
#define CHIP_TYPE_8258							0x02
#define CHIP_TYPE_8258_1M						0x03
#define CHIP_TYPE_8278							0x04
#define CHIP_TYPE_B91							0x05


#define MANUFACTURER_CODE_TELINK           	0x1141 // Telink ID

#define CHIP_TYPE CHIP_TYPE_8258

#define	IMAGE_TYPE			((CHIP_TYPE << 8) | DEVICE_TYPE)
#define	FILE_VERSION		((APP_RELEASE << 24) | (APP_BUILD << 16) | (STACK_RELEASE << 8) | STACK_BUILD)
