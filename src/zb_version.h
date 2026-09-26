/*********************************************************************************************
 * During OTA upgrade, the upgraded device will check the rules of the following three fields.
 * Refer to ZCL OTA specification for details.
 */

#define APP_RELEASE							0x99 // app release 9.9
#define APP_BUILD							0x99 // app build 9.9
#define STACK_RELEASE						0x30 // stack release 3.0
#define STACK_BUILD							0x01 // stack build 01


#ifndef DEVICE_CGG1_ver
#define DEVICE_CGG1_ver		   0 // =2022 - CGG1-M version 2022, or = 0 - CGG1-M version 2020,2021
#endif

#define DEVICE_LYWSD03MMC   10	// LCD display LYWSD03MMC
#define DEVICE_MHO_C401   	 1	// E-Ink display MHO-C401 2020
#define DEVICE_CGDK2 		 6  // LCD display "Qingping Temp & RH Monitor Lite"
#define DEVICE_MHO_C401N   	 8	// E-Ink display MHO-C401 2022
#define DEVICE_MJWSD05MMC	 9  // LCD (ch) display MJWSD05MMC
#define DEVICE_MHO_C122   	11	// LCD display MHO_C122
#define DEVICE_MJWSD05MMC_EN 12 // LCD (en) display MJWSD05MMC
#define DEVICE_MJWSD06MMC   13  // LCD display MJWSD06MMC
#define DEVICE_TB03F		16  // DIY TB-03F-Kit + INA226
#define DEVICE_TS0201   	17	// ZigBee TS0201, analog: IH-K009
#define DEVICE_TNK01		18  // DIY, PB-03F module, Water tank controller
#define DEVICE_TH03Z   		22	// ZigBee TH03Z
#define DEVICE_ZTH01   		27	// ZigBee ZTH01
#define DEVICE_ZTH02   		28	// ZigBee ZTH02
#define DEVICE_PLM1 		29  // Tuya BLE Plant monitor ECF-SGS01-A rev1.3 (BT3L Tuya module)
#define DEVICE_ZTH03 		30  // Tuya TH03 Zigbee LCD
#define DEVICE_LKTMZL02		31  // Tuya LKTMZL02 Zigbee LCD 2xAAA
#define DEVICE_ZTH05Z		33  // Tuya ZTH05ZTUv12 Zigbee LCD, AHT30, CR2032
#define DEVICE_CB3S			35  // development is not completed! TS0041_TZ3000_fa9mlvja, Tuya ZigBee "Smart Button"
#define DEVICE_HS09			36  // development is not completed! TS0201_TZ3000_1twfmkcc: Tuya ZigBee "Smart Humidity Sensor"
#define DEVICE_ZYZTH02		37  // Tuya ZY-ZTH02 Zigbee, 2 x AAA, SHT30/CHT832x
#define DEVICE_ZYZTH01		38  // Tuya ZY-ZTH02Pro Zigbee LCD, 2 x AAA, SHT30/CHT832x
#define DEVICE_ZG_227Z		39  // Tuya ZG-227Z, CR2450, AHT20
#define DEVICE_ZG303Z		44  // ZG-303Z Plant monitor, 2xAAA, AHT20
#define DEVICE_ZBEACON_TH01	45  // Tuya ZBEACON-TH01, 2xAAA , SHT4X
#define DEVICE_ZB_MC		46  // ZigBee-MC, 2xAAA, CHT8305
#define DEVICE_ZBEACON2TH01	47  // Tuya ZBEACON-TH01 v2.0, 2xAAA , SHT4X/G40
#define DEVICE_RSH_HS03	    48  // Tuya ZigBee Temperature/Humidity Sensor "TY0201_TZ3000_bjawzodf", PCB designator "RSH-HS03-V2.0-241018"
#define DEVICE_LYWSD02MMC	49  // EInk display, Clock
#define DEVICE_ZG204ZL		50  // HOBEIAN ZG-204ZL PIR + LUX sensor
#define DEVICE_ZG204ZV		51  // HOBEIAN-ZG-204ZV T&H + LUX + Radar Sensor
#define DEVICE_TS0201_WING	52  // TS0201_TZ3000_dnpd6ayp, Tuya Zigbee "Temp & Humidity Sensor", Wing TS0201 2xAAA
//#define DEVICE_ZG223Z			53 // ZigBee only, HOBEIAN ZG223Z, Zigbee "Raindrop Detection Senser" + Lx, CR123A
//#define DEVICE_DIY_SCD41		54 // reserved: BLE, DIY module TB03F + SCD41, Li(ion, pol,...) battery >= 800 mAh
//#define DEVICE_ZG204ZV1		55 // ZigBee only, HOBEIAN-ZG-204ZV LUX + Radar Sensor XBR818
//#define DEVICE_ZG204ZV2_TH	56 // ZigBee only, HOBEIAN-ZG-204ZV v2 T&H + LUX + Radar Sensor XBR818 (HOBEIAN GPIO)
//#define DEVICE_ZG204ZV2		57 // ZigBee only, HOBEIAN-ZG-204ZV v2 LUX + Radar Sensor XBR818
#define DEVICE_ZBEACON2TH01_SHTC3 58 // ZBEACON-TH01 v2.0 PCB variant: SHTC3, SDA/SCL swapped, no TX pad (TS0201_TZ3000_rdhukkmi)
//#define DEVICE_NEXT			59 // ?


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
