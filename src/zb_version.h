/*********************************************************************************************
 * During OTA upgrade, the upgraded device will check the rules of the following three fields.
 * Refer to ZCL OTA specification for details.
 */

#define APP_RELEASE							0x99 // app release 9.9
#define APP_BUILD							0x99 // app build 9.9
#define STACK_RELEASE						0x30 // stack release 3.0
#define STACK_BUILD							0x01 // stack build 01


/*
enum {
	HW_VER_LYWSD03MMC_B14 = 0,
	HW_VER_MHO_C401,		//1
	HW_VER_CGG1,			//2
	HW_VER_LYWSD03MMC_B19,	//3
	HW_VER_LYWSD03MMC_B16,	//4
	HW_VER_LYWSD03MMC_B17,	//5
	HW_VER_CGDK2,			//6
	HW_VER_CGG1_2022,		//7
	HW_VER_MHO_C401_2022,	//8
	HW_VER_MJWSD05MMC,		//9
	HW_VER_LYWSD03MMC_B15,	//10
	HW_VER_MHO_C122,		//11
	// 12,13,14 reserved TH LCD/EPD
	HW_VER_USER = 15,
	HW_VER_TNK,				//16
	HW_VER_TS0201_TZ3000,	//17
	HW_VER_TS0202_TZ3000,	//18
} HW_VERSION_ID;
*/

#ifdef DEVICE_CGG1_ver
#define DEVICE_CGG1_ver		   0 // =2022 - CGG1-M version 2022, or = 0 - CGG1-M version 2020,2021
#endif

#define DEVICE_LYWSD03MMC   10	// LCD display LYWSD03MMC
#define DEVICE_MHO_C401   	 1	// E-Ink display MHO-C401 2020
#define DEVICE_CGDK2 		 6  // LCD display "Qingping Temp & RH Monitor Lite"
#define DEVICE_MHO_C401N   	 8	// E-Ink display MHO-C401 2022
#define DEVICE_MJWSD05MMC	 9  // LCD display MJWSD05MMC
#define DEVICE_MHO_C122   	11	// LCD display MHO_C122
#if DEVICE_CGG1_ver == 0     
#define DEVICE_CGG1 		 2  // E-Ink display Old CGG1-M "Qingping Temp & RH Monitor"
#else
#define DEVICE_CGG1 		 7  // E-Ink display New CGG1-M "Qingping Temp & RH Monitor"
#endif

#ifndef DEVICE_TYPE
#define DEVICE_TYPE DEVICE_LYWSD03MMC
#endif

/* Chip IDs */
#define TLSR_8267							0x00
#define TLSR_8269							0x01
#define TLSR_8258_512K						0x02
#define TLSR_8258_1M						0x03
#define TLSR_8278							0x04
#define TLSR_B91							0x05


#define MANUFACTURER_CODE_TELINK           	0x1141 // Telink ID

#define CHIP_TYPE TLSR_8258_512K

#define	IMAGE_TYPE			((CHIP_TYPE << 8) | DEVICE_TYPE)
#define	FILE_VERSION		((APP_RELEASE << 24) | (APP_BUILD << 16) | (STACK_RELEASE << 8) | STACK_BUILD)
