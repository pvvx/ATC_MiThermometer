/********************************************************************************************************
 * @file     ble_common.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 18, 2015
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#ifndef BLE_COMMON_H
#define BLE_COMMON_H

#include "blt_config.h"
#include "tl_common.h"
/*********************************************************************
 * CONSTANTS
 */


/**
 *  @brief  Definition for Link Layer Feature Support
 */
#define LL_FEATURE_SIZE                                      8
#define LL_FEATURE_MASK_LL_ENCRYPTION                        (0x00000001)   //core_4.0
#define LL_FEATURE_MASK_CONNECTION_PARA_REQUEST_PROCEDURE  	 (0x00000002)	//core_4.1
#define LL_FEATURE_MASK_EXTENDED_REJECT_INDICATION           (0x00000004)	//core_4.1
#define LL_FEATURE_MASK_SLAVE_INITIATED_FEATURES_EXCHANGE    (0x00000008)	//core_4.1
#define LL_FEATURE_MASK_LE_PING                              (0x00000010)   //core_4.1
#define LL_FEATURE_MASK_LE_DATA_LENGTH_EXTENSION             (0x00000020)	//core_4.2
#define LL_FEATURE_MASK_LL_PRIVACY                           (0x00000040)	//core_4.2
#define LL_FEATURE_MASK_EXTENDED_SCANNER_FILTER_POLICIES     (0x00000080)   //core_4.2
#define LL_FEATURE_MASK_LE_2M_PHY         					 (0x00000100)	//core_5.0
#define LL_FEATURE_MASK_STABLE_MODULATION_INDEX_TX 			 (0x00000200)	//core_5.0
#define LL_FEATURE_MASK_STABLE_MODULATION_INDEX_RX 			 (0x00000400)	//core_5.0
#define LL_FEATURE_MASK_LE_CODED_PHY     					 (0x00000800)	//core_5.0
#define LL_FEATURE_MASK_LE_EXTENDED_ADVERTISING          	 (0x00001000)	//core_5.0
#define LL_FEATURE_MASK_LE_PERIODIC_ADVERTISING     		 (0x00002000)	//core_5.0
#define LL_FEATURE_MASK_CHANNEL_SELECTION_ALGORITHM2         (0x00004000)	//core_5.0
#define LL_FEATURE_MASK_LE_POWER_CLASS_1 					 (0x00008000)	//core_5.0
#define LL_FEATURE_MASK_MIN_USED_OF_USED_CHANNELS   	     (0x00010000)	//core_5.0

/////////////////////////////////////////////////////////////////////////////

#define         VENDOR_ID                       0x0211
#define         VENDOR_ID_HI_B                  U16_HI(VENDOR_ID)
#define         VENDOR_ID_LO_B                  U16_LO(VENDOR_ID)

#define			BLUETOOTH_VER_4_0				6
#define			BLUETOOTH_VER_4_1				7
#define			BLUETOOTH_VER_4_2				8
#define			BLUETOOTH_VER_5_0				9

#ifndef 		BLUETOOTH_VER
#define			BLUETOOTH_VER					BLUETOOTH_VER_5_0
#endif


#if (BLUETOOTH_VER == BLUETOOTH_VER_4_2)
	#define			BLUETOOTH_VER_SUBVER			0x22BB
#elif (BLUETOOTH_VER == BLUETOOTH_VER_5_0)
	#define			BLUETOOTH_VER_SUBVER			0x1C1C
#else
	#define			BLUETOOTH_VER_SUBVER			0x4103
#endif




#if (BLUETOOTH_VER == BLUETOOTH_VER_4_0)
	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define LL_CMD_MAX						   							LL_REJECT_IND

#elif (BLUETOOTH_VER == BLUETOOTH_VER_4_1)
	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				1
	#define	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE			1
	#define	LL_FEATURE_ENABLE_LE_PING									1

	#define LL_CMD_MAX						   							LL_PING_RSP

#elif (BLUETOOTH_VER == BLUETOOTH_VER_4_2)

	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				1
	#define	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE			1
	#define	LL_FEATURE_ENABLE_LE_PING									1
	#define	LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION					BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE

	#define LL_CMD_MAX						  							LL_LENGTH_RSP

#elif (BLUETOOTH_VER == BLUETOOTH_VER_5_0)

	#define LL_FEATURE_ENABLE_LE_ENCRYPTION								1
	#define	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				1
	#define	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE			1
	#define	LL_FEATURE_ENABLE_LE_PING									1
	#define	LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION					BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE

	#define	LL_FEATURE_ENABLE_LE_2M_PHY									LL_FEATURE_SUPPORT_LE_2M_PHY
	#define	LL_FEATURE_ENABLE_LE_CODED_PHY								LL_FEATURE_SUPPORT_LE_CODED_PHY
	#define	LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING					LL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING
	#define	LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING					LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING
	#define	LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2				LL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2

	#define LL_CMD_MAX						   							LL_MIN_USED_CHN_IND
#else


#endif


#ifndef		 LL_FEATURE_ENABLE_LE_ENCRYPTION
#define		 LL_FEATURE_ENABLE_LE_ENCRYPTION							0
#endif

#ifndef		 LL_FEATURE_ENABLE_CONNECTION_PARA_REQUEST_PROCEDURE
#define		 LL_FEATURE_ENABLE_CONNECTION_PARA_REQUEST_PROCEDURE		0
#endif

#ifndef		 LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION
#define		 LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION				0
#endif

#ifndef		 LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE
#define		 LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE		0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_PING
#define		 LL_FEATURE_ENABLE_LE_PING									0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION
#define		 LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION					0
#endif

#ifndef		 LL_FEATURE_ENABLE_LL_PRIVACY
#define		 LL_FEATURE_ENABLE_LL_PRIVACY								0
#endif

#ifndef		 LL_FEATURE_ENABLE_EXTENDED_SCANNER_FILTER_POLICIES
#define		 LL_FEATURE_ENABLE_EXTENDED_SCANNER_FILTER_POLICIES			0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_2M_PHY
#define		 LL_FEATURE_ENABLE_LE_2M_PHY								0
#endif

#ifndef		 LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_TX
#define		 LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_TX				0
#endif

#ifndef		 LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_RX
#define		 LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_RX				0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_CODED_PHY
#define		 LL_FEATURE_ENABLE_LE_CODED_PHY								0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING
#define		 LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING					0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING
#define		 LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING					0
#endif

#ifndef		 LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2
#define		 LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2				0
#endif

#ifndef		 LL_FEATURE_ENABLE_LE_POWER_CLASS_1
#define		 LL_FEATURE_ENABLE_LE_POWER_CLASS_1							0
#endif

#ifndef		 LL_FEATURE_ENABLE_MIN_USED_OF_USED_CHANNELS
#define		 LL_FEATURE_ENABLE_MIN_USED_OF_USED_CHANNELS				0
#endif




//BIT<0:31>
#if 1

// feature below is conFiged by application layer
// LL_FEATURE_ENABLE_LE_2M_PHY
// LL_FEATURE_ENABLE_LE_CODED_PHY
// LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING
// LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2

#define LL_FEATURE_MASK_BASE0											(	LL_FEATURE_ENABLE_LE_ENCRYPTION 					<<0		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_CONNECTION_PARA_REQUEST_PROCEDURE <<1		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION 	   	<<2		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE <<3		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_PING 						   	<<4		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION 		   	<<5		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LL_PRIVACY 					   	<<6		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_EXTENDED_SCANNER_FILTER_POLICIES  <<7		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_TX 	   	<<9		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_RX  	   	<<10	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING 		   	<<13	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_POWER_CLASS_1 				   	<<15	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_MIN_USED_OF_USED_CHANNELS 		<<16 	)
#else
#define LL_FEATURE_MASK_0												(	LL_FEATURE_ENABLE_LE_ENCRYPTION 					<<0		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_CONNECTION_PARA_REQUEST_PROCEDURE <<1		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_EXTENDED_REJECT_INDICATION 	   	<<2		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_SLAVE_INITIATED_FEATURES_EXCHANGE <<3		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_PING 						   	<<4		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_DATA_LENGTH_EXTENSION 		   	<<5		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LL_PRIVACY 					   	<<6		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_EXTENDED_SCANNER_FILTER_POLICIES  <<7		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_2M_PHY 						<<8		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_TX 	   	<<9		|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_STABLE_MODULATION_INDEX_RX  	   	<<10	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_CODED_PHY 					   	<<11	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_EXTENDED_ADVERTISING  		   	<<12	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_PERIODIC_ADVERTISING 		   	<<13	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2 	   	<<14	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_LE_POWER_CLASS_1 				   	<<15	|  \
		   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	LL_FEATURE_ENABLE_MIN_USED_OF_USED_CHANNELS 		<<16 	)

#endif


extern u32 LL_FEATURE_MASK_0;

//BIT<32:63>
#define LL_FEATURE_MASK_1												0


#define LL_FEATURE_BYTE_0												U32_BYTE0(LL_FEATURE_MASK_0)
#define LL_FEATURE_BYTE_1												U32_BYTE1(LL_FEATURE_MASK_0)
#define LL_FEATURE_BYTE_2												U32_BYTE2(LL_FEATURE_MASK_0)
#define LL_FEATURE_BYTE_3												U32_BYTE3(LL_FEATURE_MASK_0)
#define LL_FEATURE_BYTE_4												U32_BYTE0(LL_FEATURE_MASK_1)
#define LL_FEATURE_BYTE_5												U32_BYTE1(LL_FEATURE_MASK_1)
#define LL_FEATURE_BYTE_6												U32_BYTE2(LL_FEATURE_MASK_1)
#define LL_FEATURE_BYTE_7												U32_BYTE3(LL_FEATURE_MASK_1)





















#define 		ADV_INTERVAL_3_125MS                        5
#define 		ADV_INTERVAL_3_75MS                         6
#define 		ADV_INTERVAL_10MS                           16
#define 		ADV_INTERVAL_15MS                           24
#define 		ADV_INTERVAL_20MS                           32
#define 		ADV_INTERVAL_25MS                           40
#define 		ADV_INTERVAL_30MS                           48
#define 		ADV_INTERVAL_35MS                           56
#define 		ADV_INTERVAL_40MS                           64
#define 		ADV_INTERVAL_45MS                           72
#define 		ADV_INTERVAL_50MS                           80
#define 		ADV_INTERVAL_55MS                           88
#define 		ADV_INTERVAL_60MS                           96
#define 		ADV_INTERVAL_70MS                           112
#define 		ADV_INTERVAL_80MS                           128
#define 		ADV_INTERVAL_90MS							144
#define 		ADV_INTERVAL_100MS                          160
#define 		ADV_INTERVAL_105MS                          168
#define 		ADV_INTERVAL_200MS                          320
#define 		ADV_INTERVAL_205MS                          328
#define 		ADV_INTERVAL_300MS                          480
#define 		ADV_INTERVAL_305MS                          488
#define 		ADV_INTERVAL_400MS                          640
#define 		ADV_INTERVAL_405MS                          648
#define 		ADV_INTERVAL_500MS                          800
#define 		ADV_INTERVAL_505MS                          808

#define 		ADV_INTERVAL_1S                          	1600
#define 		ADV_INTERVAL_1S5                          	2400
#define 		ADV_INTERVAL_2S                          	3200
#define 		ADV_INTERVAL_1_28_S                         0x0800
#define 		ADV_INTERVAL_10_24S                         16384



#define 		SCAN_INTERVAL_10MS                           16
#define 		SCAN_INTERVAL_30MS                           48
#define 		SCAN_INTERVAL_50MS                           80
#define 		SCAN_INTERVAL_60MS                           96
#define 		SCAN_INTERVAL_90MS                           144
#define 		SCAN_INTERVAL_100MS                          160
#define 		SCAN_INTERVAL_200MS                          320
#define 		SCAN_INTERVAL_300MS                          480


#define 		CONN_INTERVAL_7P5MS                          6
#define 		CONN_INTERVAL_10MS                           8
#define 		CONN_INTERVAL_15MS                           12
#define 		CONN_INTERVAL_18P75MS                        15
#define 		CONN_INTERVAL_20MS                           16
#define 		CONN_INTERVAL_30MS                           24
#define 		CONN_INTERVAL_38P75MS                        31
#define 		CONN_INTERVAL_40MS                           32
#define 		CONN_INTERVAL_48P75MS                        39
#define 		CONN_INTERVAL_50MS                           40
#define 		CONN_INTERVAL_100MS                          80


#define 		CONN_TIMEOUT_500MS							 50
#define 		CONN_TIMEOUT_1S							 	 100
#define 		CONN_TIMEOUT_4S							 	 400
#define 		CONN_TIMEOUT_10S							 1000
#define 		CONN_TIMEOUT_20S							 2000


/*********************************************************************
 * ENUMS
 */
typedef enum {
    BLE_SUCCESS = 0,

//// HCI Status, See the Core_v5.0(Vol 2/Part D/1.3 "list of Error Codes") for more information)
    HCI_ERR_UNKNOWN_HCI_CMD                                        = 0x01,
    HCI_ERR_UNKNOWN_CONN_ID                                        = 0x02,
    HCI_ERR_HW_FAILURE                                             = 0x03,
    HCI_ERR_PAGE_TIMEOUT                                           = 0x04,
    HCI_ERR_AUTH_FAILURE                                           = 0x05,
    HCI_ERR_PIN_KEY_MISSING                                        = 0x06,
    HCI_ERR_MEM_CAP_EXCEEDED                                       = 0x07,
    HCI_ERR_CONN_TIMEOUT                                           = 0x08,
    HCI_ERR_CONN_LIMIT_EXCEEDED                                    = 0x09,
    HCI_ERR_SYNCH_CONN_LIMIT_EXCEEDED                              = 0x0A,
    HCI_ERR_ACL_CONN_ALREADY_EXISTS                                = 0x0B,
    HCI_ERR_CMD_DISALLOWED                                         = 0x0C,
    HCI_ERR_CONN_REJ_LIMITED_RESOURCES                             = 0x0D,
    HCI_ERR_CONN_REJECTED_SECURITY_REASONS                         = 0x0E,
    HCI_ERR_CONN_REJECTED_UNACCEPTABLE_BDADDR                      = 0x0F,
    HCI_ERR_CONN_ACCEPT_TIMEOUT_EXCEEDED                           = 0x10,
    HCI_ERR_UNSUPPORTED_FEATURE_PARAM_VALUE                        = 0x11,
    HCI_ERR_INVALID_HCI_CMD_PARAMS                                 = 0x12,
    HCI_ERR_REMOTE_USER_TERM_CONN                                  = 0x13,
    HCI_ERR_REMOTE_DEVICE_TERM_CONN_LOW_RESOURCES                  = 0x14,
    HCI_ERR_REMOTE_DEVICE_TERM_CONN_POWER_OFF                      = 0x15,
    HCI_ERR_CONN_TERM_BY_LOCAL_HOST                                = 0x16,
    HCI_ERR_REPEATED_ATTEMPTS                                      = 0x17,
    HCI_ERR_PAIRING_NOT_ALLOWED                                    = 0x18,
    HCI_ERR_UNKNOWN_LMP_PDU                                        = 0x19,
    HCI_ERR_UNSUPPORTED_REMOTE_FEATURE                             = 0x1A,
    HCI_ERR_SCO_OFFSET_REJ                                         = 0x1B,
    HCI_ERR_SCO_INTERVAL_REJ                                       = 0x1C,
    HCI_ERR_SCO_AIR_MODE_REJ                                       = 0x1D,
    HCI_ERR_INVALID_LMP_PARAMS                                     = 0x1E,
    HCI_ERR_UNSPECIFIED_ERROR                                      = 0x1F,
    HCI_ERR_UNSUPPORTED_LMP_PARAM_VAL                              = 0x20,
    HCI_ERR_ROLE_CHANGE_NOT_ALLOWED                                = 0x21,
    HCI_ERR_LMP_LL_RESP_TIMEOUT                                    = 0x22,
    HCI_ERR_LMP_ERR_TRANSACTION_COLLISION                          = 0x23,
    HCI_ERR_LMP_PDU_NOT_ALLOWED                                    = 0x24,
    HCI_ERR_ENCRYPT_MODE_NOT_ACCEPTABLE                            = 0x25,
    HCI_ERR_LINK_KEY_CAN_NOT_BE_CHANGED                            = 0x26,
    HCI_ERR_REQ_QOS_NOT_SUPPORTED                                  = 0x27,
    HCI_ERR_INSTANT_PASSED                                         = 0x28,
    HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED                    = 0x29,
    HCI_ERR_DIFFERENT_TRANSACTION_COLLISION                        = 0x2A,
    HCI_ERR_RESERVED1                                              = 0x2B,
    HCI_ERR_QOS_UNACCEPTABLE_PARAM                                 = 0x2C,
    HCI_ERR_QOS_REJ                                                = 0x2D,
    HCI_ERR_CHAN_ASSESSMENT_NOT_SUPPORTED                          = 0x2E,
    HCI_ERR_INSUFFICIENT_SECURITY                                  = 0x2F,
    HCI_ERR_PARAM_OUT_OF_MANDATORY_RANGE                           = 0x30,
    HCI_ERR_RESERVED2                                              = 0x31,
    HCI_ERR_ROLE_SWITCH_PENDING                                    = 0x32,
    HCI_ERR_RESERVED3                                              = 0x33,
    HCI_ERR_RESERVED_SLOT_VIOLATION                                = 0x34,
    HCI_ERR_ROLE_SWITCH_FAILED                                     = 0x35,
    HCI_ERR_EXTENDED_INQUIRY_RESP_TOO_LARGE                        = 0x36,
    HCI_ERR_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST                   = 0x37,
    HCI_ERR_HOST_BUSY_PAIRING                                      = 0x38,
    HCI_ERR_CONN_REJ_NO_SUITABLE_CHAN_FOUND                        = 0x39,
    HCI_ERR_CONTROLLER_BUSY                                        = 0x3A,
    HCI_ERR_UNACCEPTABLE_CONN_INTERVAL                             = 0x3B,
    HCI_ERR_ADVERTISING_TIMEOUT                                    = 0x3C,
    HCI_ERR_CONN_TERM_MIC_FAILURE                                  = 0x3D,
    HCI_ERR_CONN_FAILED_TO_ESTABLISH                               = 0x3E,
    HCI_ERR_MAC_CONN_FAILED                                        = 0x3F,
    HCI_ERR_COARSE_CLOCK_ADJUSTMENT_REJECT						   = 0x40,
    HCI_ERR_TYPE0_SUBMAP_NOT_DEFINED							   = 0x41,
    HCI_ERR_UNKNOWN_ADV_INDENTIFIER								   = 0x42,
    HCI_ERR_LIMIT_REACHED										   = 0x43,
    HCI_ERR_OP_CANCELLED_BY_HOST								   = 0x44,
    




///////////////////////// TELINK define status /////////////////////////////

    //LL status
	LL_ERR_CONNECTION_NOT_ESTABLISH 							   = 0x80,
	LL_ERR_TX_FIFO_NOT_ENOUGH,
	LL_ERR_ENCRYPTION_BUSY,
	LL_ERR_CURRENT_STATE_NOT_SUPPORTED_THIS_CMD,

	//L2CAP status
    L2CAP_ERR_INVALID_PARAMETER 								   = 0x90,

    //SMP status
	SMP_ERR_INVALID_PARAMETER 									   = 0xA0,
	SMP_ERR_PAIRING_BUSY,

	//GATT status
	GATT_ERR_INVALID_PARAMETER 									   = 0xB0,
	GATT_ERR_PREVIOUS_INDICATE_DATA_HAS_NOT_CONFIRMED,
	GATT_ERR_SERVICE_DISCOVERY_TIEMOUT,
	GATT_ERR_NOTIFY_INDICATION_NOT_PERMITTED,
	GATT_ERR_DATA_PENDING_DUE_TO_SERVICE_DISCOVERY_BUSY,
	GATT_ERR_DATA_LENGTH_EXCEED_MTU_SIZE,

	//GAP status
	GAP_ERR_INVALID_PARAMETER 								   	   = 0xC0,


} ble_sts_t;






// l2cap pb flag type
#define L2CAP_FRIST_PKT_H2C              0x00
#define L2CAP_CONTINUING_PKT             0x01
#define L2CAP_FIRST_PKT_C2H              0x02





#define BLE_ADDR_PUBLIC                  0
#define BLE_ADDR_RANDOM                  1
#define BLE_ADDR_INVALID                 0xff
#define BLE_ADDR_LEN                     6


//Definition for BLE Common Address Type
/*
 *
 *				  |--public  ..................................................... BLE_DEVICE_ADDRESS_PUBLIC
 *                |
 * Address Type --|		      |-- random static  ................................. BLE_DEVICE_ADDRESS_RANDOM_STATIC
 *           	  |           |
 *    			  |--random --|
 * 			   				  |				       |-- non_resolvable private  ... BLE_DEVICE_ADDRESS_NON_RESOLVABLE_PRIVATE
 * 			 				  |-- random private --|
 *           					                   |-- resolvable private  ....... BLE_DEVICE_ADDRESS_RESOLVABLE_PRIVATE
 *
 */

#define	BLE_DEVICE_ADDRESS_PUBLIC							1
#define BLE_DEVICE_ADDRESS_RANDOM_STATIC					2
#define BLE_DEVICE_ADDRESS_NON_RESOLVABLE_PRIVATE			3
#define BLE_DEVICE_ADDRESS_RESOLVABLE_PRIVATE				4



#define IS_PUBLIC_ADDR(Type, Addr)  					( (Type)==BLE_ADDR_PUBLIC) )
#define IS_RANDOM_STATIC_ADDR(Type, Addr)  				( (Type)==BLE_ADDR_RANDOM && (Addr[5] & 0xC0) == 0xC0 )
#define IS_NON_RESOLVABLE_PRIVATE_ADDR(Type, Addr)  	( (Type)==BLE_ADDR_RANDOM && (Addr[5] & 0xC0) == 0x00 )
#define IS_RESOLVABLE_PRIVATE_ADDR(Type, Addr)  		( (Type)==BLE_ADDR_RANDOM && (Addr[5] & 0xC0) == 0x40 )




typedef enum{
	OWN_ADDRESS_PUBLIC = 0,
	OWN_ADDRESS_RANDOM = 1,
	OWN_ADDRESS_RESOLVE_PRIVATE_PUBLIC = 2,
	OWN_ADDRESS_RESOLVE_PRIVATE_RANDOM = 3,
}own_addr_type_t;








typedef struct {
	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;
}rf_adv_head_t;



typedef struct {
    u8 llid   :2;
    u8 nesn   :1;
    u8 sn     :1;
    u8 md     :1;
    u8 rfu1   :3;
}rf_data_head_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)
}rf_packet_head_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)
}rf_packet_auto_reply_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number

	rf_adv_head_t  header;
	u8  rf_len;				//LEN(6)_RFU(2)

	u8	advA[6];			//address
	u8	data[31];			//0-31 byte
}rf_packet_adv_t;



typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	rf_adv_head_t  header;	//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)

	u8	scanA[6];			//
	u8	advA[6];			//
}rf_packet_scan_req_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	rf_adv_head_t  header;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)

	u8	advA[6];			//address
	u8	data[31];			//0-31 byte
}rf_packet_scan_rsp_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number

	u8 type   :4;
	u8 rfu1   :1;
	u8 chan_sel:1;
	u8 txAddr :1;
	u8 rxAddr :1;

	u8  rf_len;				//LEN(6)_RFU(2)
	u8	initA[6];			//scanA
	u8	advA[6];			//
	u8	accessCode[4];		// access code
	u8	crcinit[3];
	u8	winSize;
	u16	winOffset;
	u16 interval;
	u16 latency;
	u16 timeout;
	u8	chm[5];
	u8	hop;				//sca(3)_hop(5)
}rf_packet_connect_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	rf_adv_head_t  header;				//RA(1)_TA(1)_RFU(2)_TYPE(4): connect request PDU
	u8  rf_len;				//LEN(6)_RFU(2)
	u8	scanA[6];			//
	u8	advA[6];			//
	u8	aa[4];				// access code
	u8	crcinit[3];
	u8	wsize;
	u16	woffset;
	u16 interval;
	u16 latency;
	u16 timeout;
	u8	chm[5];
	u8	hop;				//sca(3)_hop(5)
}rf_packet_ll_init_t;

typedef struct {
	u8	type;
	u8  rf_len;
	u8 	opcode;
	u8 	winSize;
	u16 winOffset;
	u16 interval;
	u16 latency;
	u16 timeout;
	u16 instant;
} rf_packet_ll_updateConnPara_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4): connect request PDU
	u8  rf_len;				//LEN(6)_RFU(2)

	u8	scanA[6];			//
	u8	advA[6];			//
	u8	aa[4];				// access code
	u8	crcinit[3];
	u8	wsize;
	u16	woffset;
	u16 interval;
	u16 latency;
	u16 timeout;
	u8	chm[5];
	u8	hop;				//sca(3)_hop(5)
}rf_packet_relay_t;


typedef struct {
	u16 intervalMin;      // Minimum advertising interval for non-directed advertising, time = N * 0.625ms
	u16 intervalMax;      // Maximum advertising interval for non-directed advertising, time = N * 0.625ms
	u8  advType;          // Advertising
	u8  ownAddrType;
	u8  peerAddrType;
	u8  peerAddr[BLE_ADDR_LEN];
	u8  advChannelMap;
	u8  advFilterPolicy;
} adv_para_t;

typedef struct {
	u16 connHandle;
	u16 connIntervalMin;
	u16 connIntervalMax;
	u16 connLatency;
	u16 supervisionTimeout;
	u16 minCELen;
	u16 maxCELen;
} conn_para_t;



/*
LLID(2) - NESN(1) - SN(1) - MD(1) - RFU(3) - Length(5) - RFU(3)
*/


typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)
	u16	l2capLen;
	u16	chanId;
}rf_packet_l2cap_head_t;


typedef struct{
	rf_data_head_t	header;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 data[1];
}rf_packet_l2cap_t;


typedef struct{
	rf_data_head_t	header;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  handle0;
	u8  handle1;
	u8	dat[20];
}rf_packet_att_t;


typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 data[1];
}rf_packet_l2cap_req_t;


typedef struct{
	u32 dma_len;
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  code;
	u8  id;
	u16 dataLen;
	u16  result;
}rf_pkt_l2cap_sig_connParaUpRsp_t;


typedef struct{
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 data[1];
}rf_pkt_l2cap_req_t;


typedef struct{
	u8	llid;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8	id;
	u16 data_len;
	u16 min_interval;
	u16 max_interval;
	u16 latency;
	u16 timeout;
}rf_packet_l2cap_connParaUpReq_t;


typedef struct{
	u8	llid;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8	id;
	u16 data_len;
	u16 result;
}rf_packet_l2cap_connParaUpRsp_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u8	data;
}rf_packet_l2cap_cust_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 	errOpcode;
	u16 errHandle;
	u8  errReason;
}rf_packet_att_errRsp_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)

	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8	startingHandle;
	u8	startingHandle1;
	u8	endingHandle;
	u8	endingHandle1;
	u8	attType[2];				//
}rf_packet_att_readByType_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)

	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8	startingHandle;
	u8	startingHandle1;
	u8	endingHandle;
	u8	endingHandle1;
	u8	attType[2];
	u8  attValue[2];
}rf_packet_att_findByTypeReq_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)

	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u16 	data[1];
}rf_packet_att_findByTypeRsp_t;


typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 handle;
	u8 handle1;
}rf_packet_att_read_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 handle;
	u8 handle1;
	u8 offset0;
	u8 offset1;
}rf_packet_att_readBlob_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 	value[22];
}rf_packet_att_readRsp_t;


typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  datalen;
	u8  data[1];			// character_handle / property / value_handle / value
}rf_pkt_att_readByTypeRsp_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  datalen;
	u8  data[1];			// character_handle / property / value_handle / value
}rf_packet_att_readByTypeRsp_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  datalen;
	u16  data[3];
}rf_packet_att_readByGroupTypeRsp_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  format;
	u8  data[1];			// character_handle / property / value_handle / value
}rf_packet_att_findInfoReq_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 flags;
}rf_packet_att_executeWriteReq_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 handle;
	u8 handle1;
	u8 value;
}rf_packet_att_write_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 handle[2];
	u8 data;
}rf_packet_att_notification_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number

	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)

	u16	l2cap;				//0x17
	u16	chanid;				//0x04,

	u8	att;				//0x12 for master; 0x1b for slave
	u8	hl;					// assigned by master
	u8	hh;					//
	u8	sno;

	u8	ctype;
	u8	cmd[18];				//byte
}rf_packet_ll_write_data_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 mtu[2];
}rf_packet_att_mtu_t;

typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 mtu[2];
}rf_packet_att_mtu_exchange_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
}rf_packet_att_writeRsp_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)
	u8 	opcode;
	u8	data[8];
}rf_packet_feature_rsp_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)
	u8 	opcode;
	u8  mainVer;
	u16 vendor;
	u16 subVer;
}rf_packet_version_ind_t;

typedef struct {
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8 	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)
	u8 	opcode;
	u8 	winSize;
	u16 winOffset;
	u16 interval;
	u16 latency;
	u16 timeout;
	u16 instant;
}rf_packet_connect_upd_req_t;

typedef struct {
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8 	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)
	u8 	opcode;
	u8 	chm[5];
	u16 instant;
} rf_packet_chm_upd_req_t;


typedef struct {
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RA(1)_TA(1)_RFU(2)_TYPE(4)
	u8  rf_len;				//LEN(6)_RFU(2)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  id;
	u16 siglen;
    u16 min_interval;
    u16 max_interval;
    u16 latency;
    u16 timeout;
}rf_packet_connParUpReq_t;

typedef struct {
	u8  valid;
	u8 	winSize;
	u16 winOffset;
	u16 interval;
	u16 latency;
	u16 timeout;
	u16 instant;
}connect_upd_data_t;

typedef struct {
	u8  valid;
	u8 	chm[5];
	u16 instant;
}connect_chm_upd_data_t;


typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)

	u8	opcode;				//
	u8	reason;				//
}rf_packet_ll_terminate_t;


typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)

	u8	opcode;				//
	u8	dat[1];				//
}rf_packet_ll_control_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number

	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2cap;
	u16	chanid;

	u8	att;
	u8	hl;					// assigned by master
	u8	hh;					//

	u8	dat[20];

}rf_packet_att_data_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number

	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	flag;

	u32	src_id;

	u8	att;				//0x12 for master; 0x1b for slave
	u8	hl;					// assigned by master
	u8	hh;					//
	u8	sno;

	u16	nid;				// network ID
	u16	group;

	u32	dst_id;

	u8	cmd[11];			//byte
							// 10 xx xx xx xx xx xx		=> light on
							// 11 xx xx xx xx xx xx		=> light off
							// 12 rr gg bb ww uu vv		=> set

	//u32	mic[4];			//optional
}rf_packet_ll_rc_data_t;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number

	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)

	u16	l2capLen;			// can be src_id
	u16	chanId;

	u8	att;				//0x12 for master; 0x1b for slave
	u8	hl;					// assigned by master
	u8	hh;					//
	u8	sno;

	u16	mic;				// network ID
	u16	group;

	u8	dst_id[4];

	u8	cmd[11];			//byte
							// 10 xx xx xx xx xx xx		=> light on
							// 11 xx xx xx xx xx xx		=> light off
							// 12 rr gg bb ww uu vv		=> set

	//u32	mic[4];			//optional
}rf_packet_mesh_data_phone_t;

typedef struct{
    u8 sno[3];
    u8 src[2];
    u8 dst[2];
    u8 op_para[13];
 //   u8 ttl;
 //   u8 hop;
}rf_packet_mesh_nwk_t;


typedef struct{
	u32 dma_len;            //29

	u8	rf_len;				//28
	u8  type;				//LEN(5)_RFU(3)

	u8	src_id[4];			// src_id

	u8	ttl;				// time to live
	u8	hl;					// assigned by master
	u8	hh;					//
	u8	sno;

	u16	mic;				// network ID
	u16	group;

	u8	dst_id[4];

	u8	cmd[11];			//byte
							// 10 xx xx xx xx xx xx		=> light on
							// 11 xx xx xx xx xx xx		=> light off
							// 12 rr gg bb ww uu vv		=> set

	//u32	mic[4];			//optional
}rf_packet_mesh_data_t1;

typedef struct{
	u32 dma_len;            //29

	u8	rf_len;				//28
	u8  type;				//LEN(5)_RFU(3)

	u16	l2cap;				// l2cap length
	u16	chan;				// channel ID

	u8	ttl;				// time to live
	u8	hl;					// assigned by master
	u8	hh;					//
	rf_packet_mesh_nwk_t	c;

}rf_packet_mesh_phone_t;

typedef struct{
	u32 dma_len;            //29

	u8	rf_len;				//28
	u8  type;				//LEN(5)_RFU(3)

	u16	l2cap;				// l2cap length
	u16	chan;				// channel ID

	u8	ttl;				// time to live
	u8	hl;					// assigned by master
	u8	hh;					//
	rf_packet_mesh_nwk_t	c;
	u8 rsv[6];

}rf_packet_mesh_data_t;

typedef struct{
	u32 dma_len;            //39

	u8	rf_len;				//38
	u8  type;				//LEN(5)_RFU(3)

	u16	l2cap;				// l2cap length
	u16	chan;				// channel ID

	u8	ttl;				// time to live
	u8	hl;					// assigned by master
	u8	hh;					//
	u8	dat[30];

}rf_packet_mesh_status_t;

typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  datalen;
	u8  data[1];			// character_handle / property / value_handle / value
}att_readByTypeRsp_t;

typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  format;
	u8  data[1];			// character_handle / property / value_handle / value
}att_findInfoRsp_t;

typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 	value[22];
}att_readRsp_t;

typedef struct{
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8	handle;
	u8	hh;
	u8 	value[1];
}att_notify_t;

typedef struct {
	u8 type;
	u8 address[BLE_ADDR_LEN];
} addr_t;

typedef struct {
	u8 address[BLE_ADDR_LEN];
} public_addr_t;


////////////////////////////////////////////////////////////////////////////







typedef struct {
	u8	num;
	u8	property;
	u16	handle;
	u16	uuid;
	u16 ref;
} att_db_uuid16_t;			//8-byte


typedef struct {
	u8	num;
	u8	property;
	u16	handle;
	u8	uuid[16];
} att_db_uuid128_t;			//20-byte

//-------------   event --------------------------------
typedef struct {
	u8	status;
	u8	handle;
	u8	hh;
	u8	reason;
} event_disconnection_t;			//20-byte

typedef struct {
	u8	subcode;
	u8	nreport;
	u8	event_type;
	u8	adr_type;
	u8	mac[6];
	u8	len;
	u8	data[1];
} event_adv_report_t;			//20-byte

typedef struct {
	u8	subcode;
	u8	status;
	u16	handle;
	u8	role;
	u8	peer_adr_type;
	u8	mac[6];
	u16	interval;
	u16	latency;
	u16	timeout;
	u8	accuracy;
} event_connection_complete_t;			//20-byte

typedef struct {
	u8	subcode;
	u8	status;
	u16	handle;
	u16	interval;
	u16	latency;
	u16	timeout;
} event_connection_update_t;			//20-byte

typedef struct {
	u8	status;
	u16	handle;
	u8  enc_enable;
} event_enc_change_t;

typedef struct {
	u8	status;
	u16	handle;
} event_enc_refresh_t;


#endif
