/********************************************************************************************************
 * @file     att.h 
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

#pragma once
#include "tl_common.h"
#include "stack/ble/blt_config.h"
#include "stack/ble/ble_common.h"
#include "stack/ble/l2cap/l2cap.h"
#include "gatt.h"


#define ATT_MTU_SIZE                        23  //!< Minimum ATT MTU size
#define ATT_MAX_ATTR_HANDLE                 0xFFFF
#define ATT_16BIT_UUID_LEN                  2
#define ATT_128BIT_UUID_LEN                 16
#define L2CAP_RESERVED_LEN                  14
#define OPCODE_SIZE                         1
#define L2CAP_PAYLOAD_OFFSET                (L2CAP_RESERVED_LEN + OPCODE_SIZE)
#define ATT_HANDLE_START                    0x0001
#define ATT_HANDLE_END                      0xFFFF

#define ATT_OP_ERROR_RSP                    0x01 //!< Error Response op code
#define ATT_OP_EXCHANGE_MTU_REQ             0x02 //!< Exchange MTU Request op code
#define ATT_OP_EXCHANGE_MTU_RSP             0x03 //!< Exchange MTU Response op code
#define ATT_OP_FIND_INFO_REQ                0x04 //!< Find Information Request op code
#define ATT_OP_FIND_INFO_RSP                0x05 //!< Find Information Response op code		----Information Data 4 to (ATT_MTU-2)
#define ATT_OP_FIND_BY_TYPE_VALUE_REQ       0x06 //!< Find By Type Vaue Request op code			Attribute Value 0 to (ATT_MTU-7)
#define ATT_OP_FIND_BY_TYPE_VALUE_RSP       0x07 //!< Find By Type Vaue Response op code	----Handles Information List 4 to (ATT_MTU-1)
#define ATT_OP_READ_BY_TYPE_REQ             0x08 //!< Read By Type Request op code
#define ATT_OP_READ_BY_TYPE_RSP             0x09 //!< Read By Type Response op code			----Attribute Data List 2 to (ATT_MTU- 2)
#define ATT_OP_READ_REQ                     0x0a //!< Read Request op code
#define ATT_OP_READ_RSP                     0x0b //!< Read Response op code					@@--Attribute Value 0 to (ATT_MTU-1)
#define ATT_OP_READ_BLOB_REQ                0x0c //!< Read Blob Request op code
#define ATT_OP_READ_BLOB_RSP                0x0d //!< Read Blob Response op code			@@--Part Attribute Value 0 to (ATT_MTU-1)
#define ATT_OP_READ_MULTI_REQ               0x0e //!< Read Multiple Request op code				Set Of Handles 4 to (ATT_MTU-1)
#define ATT_OP_READ_MULTI_RSP               0x0f //!< Read Multiple Response op code		----Set Of Values 0 to (ATT_MTU-1)
#define ATT_OP_READ_BY_GROUP_TYPE_REQ       0x10 //!< Read By Group Type Request op code
#define ATT_OP_READ_BY_GROUP_TYPE_RSP       0x11 //!< Read By Group Type Response op code	----Attribute Data List 4 to (ATT_MTU- 2)
#define ATT_OP_WRITE_REQ                    0x12 //!< Write Request op code						Attribute Value 0 to (ATT_MTU-3)
#define ATT_OP_WRITE_RSP                    0x13 //!< Write Response op code
#define ATT_OP_PREPARE_WRITE_REQ            0x16 //!< Prepare Write Request op code				Part Attribute Value 0 to (ATT_MTU-5)
#define ATT_OP_PREPARE_WRITE_RSP            0x17 //!< Prepare Write Response op code		----Part Attribute Value 0 to (ATT_MTU-5)
#define ATT_OP_EXECUTE_WRITE_REQ            0x18 //!< Execute Write Request op code
#define ATT_OP_EXECUTE_WRITE_RSP            0x19 //!< Execute Write Response op code
#define ATT_OP_HANDLE_VALUE_NOTI            0x1b //!< Handle Value Notification op code			Attribute Value 0 to (ATT_MTU-3)
#define ATT_OP_HANDLE_VALUE_IND             0x1d //!< Handle Value Indication op code			Attribute Value 0 to (ATT_MTU-3)
#define ATT_OP_HANDLE_VALUE_CFM             0x1e //!< Handle Value Confirmation op code
#define ATT_OP_WRITE_CMD                    0x52 //!< ATT Write Command							Attribute Value 0 to (ATT_MTU-3)
#define ATT_OP_SIGNED_WRITE_CMD				0xd2 //!< ATT Signed Write Command					Attribute Value 0 to (ATT_MTU - 15)




/** @defgroup ATT_PERMISSIONS_BITMAPS GAP ATT Attribute Access Permissions Bit Fields
 * @{
 * (See the Core_v5.0(Vol 3/Part C/10.3.1/Table 10.2) for more information)
 */
#define ATT_PERMISSIONS_AUTHOR				 0x10 //Attribute access(Read & Write) requires Authorization
#define ATT_PERMISSIONS_ENCRYPT				 0x20 //Attribute access(Read & Write) requires Encryption
#define ATT_PERMISSIONS_AUTHEN				 0x40 //Attribute access(Read & Write) requires Authentication(MITM protection)
#define ATT_PERMISSIONS_SECURE_CONN			 0x80 //Attribute access(Read & Write) requires Secure_Connection
#define ATT_PERMISSIONS_SECURITY			 (ATT_PERMISSIONS_AUTHOR | ATT_PERMISSIONS_ENCRYPT | ATT_PERMISSIONS_AUTHEN | ATT_PERMISSIONS_SECURE_CONN)

//user can choose permission below
#define ATT_PERMISSIONS_READ                 0x01 //!< Attribute is Readable
#define ATT_PERMISSIONS_WRITE                0x02 //!< Attribute is Writable
#define ATT_PERMISSIONS_RDWR	             (ATT_PERMISSIONS_READ | ATT_PERMISSIONS_WRITE)   //!< Attribute is Readable & Writable


#define ATT_PERMISSIONS_ENCRYPT_READ         (ATT_PERMISSIONS_READ  | ATT_PERMISSIONS_ENCRYPT) 		//!< Read requires Encryption
#define ATT_PERMISSIONS_ENCRYPT_WRITE        (ATT_PERMISSIONS_WRITE | ATT_PERMISSIONS_ENCRYPT) 		//!< Write requires Encryption
#define ATT_PERMISSIONS_ENCRYPT_RDWR         (ATT_PERMISSIONS_RDWR  | ATT_PERMISSIONS_ENCRYPT) 		//!< Read & Write requires Encryption


#define ATT_PERMISSIONS_AUTHEN_READ          (ATT_PERMISSIONS_READ  | ATT_PERMISSIONS_ENCRYPT | ATT_PERMISSIONS_AUTHEN) 		//!< Read requires Authentication
#define ATT_PERMISSIONS_AUTHEN_WRITE         (ATT_PERMISSIONS_WRITE | ATT_PERMISSIONS_ENCRYPT | ATT_PERMISSIONS_AUTHEN) 		//!< Write requires Authentication
#define ATT_PERMISSIONS_AUTHEN_RDWR          (ATT_PERMISSIONS_RDWR  | ATT_PERMISSIONS_ENCRYPT | ATT_PERMISSIONS_AUTHEN) 		//!< Read & Write requires Authentication


#define ATT_PERMISSIONS_SECURE_CONN_READ	 (ATT_PERMISSIONS_READ  | ATT_PERMISSIONS_SECURE_CONN | ATT_PERMISSIONS_ENCRYPT | ATT_PERMISSIONS_AUTHEN)   //!< Read requires Secure_Connection
#define ATT_PERMISSIONS_SECURE_CONN_WRITE    (ATT_PERMISSIONS_WRITE | ATT_PERMISSIONS_SECURE_CONN | ATT_PERMISSIONS_ENCRYPT | ATT_PERMISSIONS_AUTHEN)  //!< Write requires Secure_Connection
#define ATT_PERMISSIONS_SECURE_CONN_RDWR	 (ATT_PERMISSIONS_RDWR  | ATT_PERMISSIONS_SECURE_CONN | ATT_PERMISSIONS_ENCRYPT | ATT_PERMISSIONS_AUTHEN)   //!< Read & Write requires Secure_Connection


#define ATT_PERMISSIONS_AUTHOR_READ          (ATT_PERMISSIONS_READ | ATT_PERMISSIONS_AUTHOR) 		//!< Read requires Authorization
#define ATT_PERMISSIONS_AUTHOR_WRITE         (ATT_PERMISSIONS_WRITE | ATT_PERMISSIONS_AUTHEN) 		//!< Write requires Authorization
#define ATT_PERMISSIONS_AUTHOR_RDWR          (ATT_PERMISSIONS_RDWR | ATT_PERMISSIONS_AUTHOR) 		//!< Read & Write requires Authorization


/** @} End GAP_ATT_PERMISSIONS_BITMAPS */



//See the Core_v5.0(Vol 3/Part F/3.4.1.1, "Error Response") for more information.
typedef enum {

	ATT_SUCCESS = 0,

    ATT_ERR_INVALID_HANDLE,                              //!< The attribute handle given was not valid on this server
    ATT_ERR_READ_NOT_PERMITTED,                          //!< The attribute cannot be read
    ATT_ERR_WRITE_NOT_PERMITTED,                         //!< The attribute cannot be written
    ATT_ERR_INVALID_PDU,                                 //!< The attribute PDU was invalid
    ATT_ERR_INSUFFICIENT_AUTH,                           //!< The attribute requires authentication before it can be read or written
    ATT_ERR_REQ_NOT_SUPPORTED,                           //!< Attribute server does not support the request received from the client
    ATT_ERR_INVALID_OFFSET,                              //!< Offset specified was past the end of the attribute
    ATT_ERR_INSUFFICIENT_AUTHOR,                         //!< The attribute requires authorization before it can be read or written
    ATT_ERR_PREPARE_QUEUE_FULL,                          //!< Too many prepare writes have been queued
    ATT_ERR_ATTR_NOT_FOUND,                              //!< No attribute found within the given attribute handle range
    ATT_ERR_ATTR_NOT_LONG,                               //!< The attribute cannot be read or written using the Read Blob Request
    ATT_ERR_INSUFFICIENT_KEY_SIZE,                       //!< The Encryption Key Size used for encrypting this link is insufficient
    ATT_ERR_INVALID_ATTR_VALUE_LEN,                      //!< The attribute value length is invalid for the operation
    ATT_ERR_UNLIKELY_ERR,                                //!< The attribute request that was requested has encountered an error that was unlikely, and therefore could not be completed as requested
    ATT_ERR_INSUFFICIENT_ENCRYPT,                        //!< The attribute requires encryption before it can be read or written
    ATT_ERR_UNSUPPORTED_GRP_TYPE,                        //!< The attribute type is not a supported grouping attribute as defined by a higher layer specification
    ATT_ERR_INSUFFICIENT_RESOURCES,                      //!< Insufficient Resources to complete the request



}att_err_t;




//TELINK MTU no longer than 256, so 1 byte is enough
typedef struct{
	u8 init_MTU;
	u8 effective_MTU;
	u8 Data_pending_time;    //10ms unit
	u8 Data_permission_check;
}att_para_t;
extern att_para_t bltAtt;




typedef struct
{
  u8 len;                    //!< Length of UUID
  u8 uuid[16];               //!< UUID
} uuid_t;

/**
 * Error Response 
 */
typedef struct
{
    u8 reqOpcodeInErr; //!< The request that generated this error response
    u8 errCode;   //!< The reason why the request has generated an error response
    u16 attHandleInErr;   //!< The attribute handle that generated this error response
} errorRsp_t;

/**
 * Exchange MTU Request 
 */
typedef struct
{
    u16 clientRxMTU; //!< Attribute client receive MTU size 
} exchangeMTUReq_t;

/**
 * Exchange MTU Response 
 */
typedef struct
{
    u16 serverRxMTU; //!< Attribute server receive MTU size
} exchangeMTURsp_t;

/**
 * Find Information Request 
 */
typedef struct
{
    u16 startingHandle;       //!< First requested handle number
    u16 endingHandle;         //!< Last requested handle number
} findInformationReq_t;

/**
 * Handle(s) and 16-bit Bluetooth UUID(s)
 */
typedef struct
{
    u16 handle;                //!< Handle
    u8 uuid[ATT_16BIT_UUID_LEN]; //!< 16 bit UUID
} handleBtUUID_t;

/**
 * Handle(s) and 128-bit UUID(s)
 */
typedef struct
{
    u16 handle;             //!< Handle
    u8 uuid[ATT_128BIT_UUID_LEN]; //!< 128 bit UUID
} handleUUID_t;

/**
 * Find Information Response 
 */
typedef struct
{
    u8 format;       //!< Format of information
    u8 infoNum;      //!< information num
    u8 info[1];      //!< information
} findInformationRsp_t;

/**
 * Find By Type Value Request 
 */
typedef struct
{
  u16  startingHandle;          //!< First requested handle number 
  u16  endingHandle;            //!< Last requested handle number
  u16  uuid;                    //!< UUID to find
  u8   len;                     //!< Length of value
  u8   value[1];   //!< Attribute value to find
} findByTypeValueReq_t;

/**
 * Handles Infomation list element
 */
typedef struct
{
  u8 handle;         //!< Found attribute handle
  u8 groupEndHandle; //!< Group end handle
} handleInfo_t;

/**
 * Find By Type Value Response 
 */
typedef struct
{
  u8 handleInfoNum;                                       //!< Number of handles information below
  handleInfo_t handleInfo[1] ; //!< A list of 1 or more Handle Informations
} findByTypeValueRsp_t;

/**
 * Read By Type Request 
 */
typedef struct
{
  u16 startingHandle; //!< First requested handle number
  u16 endingHandle;   //!< Last requested handle number
  uuid_t attrType;    //!< 2 or 16 octet UUID
} readByTypeReq_t;

/**
 * Read By Type Response 
 */
typedef struct
{
  u8 numData;                  //!< Number of attribute data list item
  u8 len;                      //!< The size of each attribute handle-value pair
  u8 data[1];     //!< Attribute Data List
} readByTypeRsp_t;

/**
 * Read Request 
 */
typedef struct
{
  u16 handle;               //!< The handle of the attribute to be read
} readReq_t;

/**
 * Read Response 
 */
typedef struct
{
  u8 len;                   //!< Length of value
  u8 attrValue[1];          //!< Value of the attribute with the handle given
} readRsp_t;

/**
 * Read Blob Req 
 */
typedef struct
{
  u16 handle; //!< The handle of the attribute to be read
  u16 offset; //!< The offset of the first octet to be read
} readBlobReq_t;

/**
 * Read Blob Response 
 */
typedef struct
{
  u8 len;      //!< Length of value
  u8 attrValue[1]; //!< Part of the value of the attribute with the handle given
} readBlobRsp_t;

/**
 * Read Multiple Request
 */
typedef struct
{
  u8  numHandles; //!< Number of attribute handles
  u16 handle[1];  //!< A set of two or more attribute handles
} readMultipleReq_t;

/**
 * Read Multiple Response 
 */
typedef struct
{
  u8 len;       //!< Length of values
  u8 values[1]; //!< A set of two or more values
} readMultiRsp_t;

/**
 * Read By Group Type Request 
 */
typedef struct
{
  u16 startingHandle; //!< First requested handle number (must be first field)
  u16 endingHandle;   //!< Last requested handle number
  uuid_t attrType;     //!< 2 or 16 octet UUID
} readByGroupTypeReq_t;

/**
 * Read By Group Type Response 
 */
typedef struct
{
  u8 grpNum;                  //!< The number of attributes in this group
  u8 len;                      //!< Length of each attribute handle
  u8 data[1];                 //!< Attribute Data
} readByGroupTypeRsp_t;

/**
 * Write Request 
 */
typedef struct
{
  u16 handle;                            //!< The handle of the attribute to be written (must be first field)
  u8 len;                                //!< Length of value
  u8 value[1];            //!< The value to be written to the attribute
} writeReq_t;

/**
 * Write Command 
 */
typedef struct
{
  u16 handle;                         //!< The handle of the attribute to be written (must be first field)
  u8 len;                             //!< Length of value
  u8 value[1];         //!< The value to be written to the attribute
  u8 sig;                             //!< the sig flag
} writeCmd_t;

/**
 * Prepare Write Request 
 */
typedef struct
{
  u16 handle;                 //!< Handle of the attribute to be written (must be first field)
  u16 offset;                 //!< Offset of the first octet to be written
  u8 len;                     //!< Length of value
  u8 value[1]; //!< Part of the value of the attribute to be written
} prepareWriteReq_t;

/**
 * Prepare Write Response 
 */
typedef struct
{
  u16 handle;                       //!< The handle of the attribute to be written
  u16 offset;                       //!< The offset of the first octet to be written
  u8 len;                           //!< Length of value
  u8 value[1];              //!< The value of the attribute to be written
} prepareWriteRsp_t;

/**
 * Execute Write Request 
 */
typedef struct
{
  u8 flags;   //!< 0x00 - cancel all prepared writes 0x01 - immediately write all pending prepared values
} executeWriteReq_t;

/**
 * Handle Value Notification 
 */
typedef struct
{
  u16 handle;               //!< The handle of the attribute
  u8 len;                   //!< Length of value
  u8 value[1];              //!< The current value of the attribute
} handleValueNoti_t;

/**
 * Handle Value Indication 
 */
typedef struct
{
  u16 handle;               //!< The handle of the attribute
  u8 len;                   //!< Length of value
  u8 value[1];              //!< The current value of the attribute
} handleValueInd_t;

typedef union attOpCode{
    struct{
        u8 method:6;
        u8 cmdFlag:1;
        u8 authSigFlag:1;
    }bitField;
    u8 byte;
}attOpCode_t;




/////////////////////////////
// gatt.h
/////////////////////////////
/** @addtogroup GATT_Characteristic_Property GATT characteristic properties
 * @{
 */
#define CHAR_PROP_BROADCAST              0x01 //!< permit broadcasts of the Characteristic Value
#define CHAR_PROP_READ                   0x02 //!< permit reads of the Characteristic Value
#define CHAR_PROP_WRITE_WITHOUT_RSP      0x04 //!< Permit writes of the Characteristic Value without response
#define CHAR_PROP_WRITE                  0x08 //!< Permit writes of the Characteristic Value with response
#define CHAR_PROP_NOTIFY                 0x10 //!< Permit notifications of a Characteristic Value without acknowledgement
#define CHAR_PROP_INDICATE               0x20 //!< Permit indications of a Characteristic Value with acknowledgement
#define CHAR_PROP_AUTHEN                 0x40 //!< permit signed writes to the Characteristic Value
#define CHAR_PROP_EXTENDED               0x80 //!< additional characteristic properties are defined
/** @} end of group GATT_Characteristic_Property */


/** @addtogroup GATT_CCCC_Bits Client CharacteristicConfiguration bits
 * @{
 */
#define CLIENT_CHAR_CFG_NOTI              0x0001 //!< permit broadcasts of the Characteristic Value
#define CLIENT_CHAR_CFG_IND               0x0002 //!< permit reads of the Characteristic Value
/** @} end of group GATT_CCCC_Bits */


/** @addtogroup GATT_Property_length GATT characteristic property length
 * @{
 */
#define CHAR_PROP_SIZE                   1
/** @} end of group GATT_Property_length */

/** @addtogroup GATT_Char_Cfg_Bit_length GATT characteristic configuration Bits length
 * @{
 */
#define CHAR_CFG_BITS_SIZE               2
/** @} end of group GATT_Char_Cfg_Bit_length */



typedef int (*att_mtuSizeExchange_callback_t)(u16, u16);
typedef int (*att_handleValueConfirm_callback_t)(void);
typedef int (*att_readwrite_callback_t)(void* p);

typedef struct attribute
{
  u16  attNum;
  u8   perm;
  u8   uuidLen;
  u32  attrLen;    //4 bytes aligned
  u8* uuid;
  u8* pAttrValue;
  att_readwrite_callback_t w;
  att_readwrite_callback_t r;
} attribute_t;






extern u16	blt_indicate_handle;




/******************************* User Interface  ************************************/
ble_sts_t  bls_att_pushNotifyData (u16 attHandle, u8 *p, int len);
ble_sts_t	 bls_att_pushIndicateData (u16 attHandle, u8 *p, int len);
void		 bls_att_setAttributeTable (u8 *p);



//mtu size
ble_sts_t  blc_att_setRxMtuSize(u16 mtu_size);

ble_sts_t	 blc_att_requestMtuSizeExchange (u16 connHandle, u16 mtu_size);
ble_sts_t	 blc_att_responseMtuSizeExchange (u16 connHandle, u16 mtu_size);


		// 0x04: ATT_OP_FIND_INFO_REQ
void 	att_req_find_info(u8 *dat, u16 start_attHandle, u16 end_attHandle);
		// 0x06: ATT_OP_FIND_BY_TYPE_VALUE_REQ
void 	att_req_find_by_type (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, u8* attr_value, int len);
		// 0x08: ATT_OP_READ_BY_TYPE_REQ
void 	att_req_read_by_type (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, int uuid_len);
		// 0x0a: ATT_OP_READ_REQ
void 	att_req_read (u8 *dat, u16 attHandle);
		// 0x0c: ATT_OP_READ_BLOB_REQ
void 	att_req_read_blob (u8 *dat, u16 attHandle, u16 offset);
		// 0x10: ATT_OP_READ_BY_GROUP_TYPE_REQ
void 	att_req_read_by_group_type (u8 *dat, u16 start_attHandle, u16 end_attHandle, u8 *uuid, int uuid_len);
		// 0x12: ATT_OP_WRITE_REQ
void 	att_req_write (u8 *dat, u16 attHandle, u8 *buf, int len);
		// 0x52: ATT_OP_WRITE_CMD
void 	att_req_write_cmd (u8 *dat, u16 attHandle, u8 *buf, int len);




ble_sts_t 	bls_att_setDeviceName(u8* pName,u8 len);  //only module/mesh/hci use


int 		att_register_idle_func (void *p);
int 		l2cap_att_client_handler (u16 conn, u8 *p);




ble_sts_t	blc_att_requestMtuSizeExchange (u16 connHandle, u16 mtu_size);
ble_sts_t	blc_att_responseMtuSizeExchange (u16 connHandle, u16 mtu_size);






/************************* Stack Interface, user can not use!!! ***************************/
extern u32 att_service_discover_tick;

u8 * l2cap_att_handler(u16 connHandle, u8 * p);

static inline u16  blc_att_getEffectiveMtuSize(u16 connHandle)
{
	return bltAtt.effective_MTU;
}

static inline void  blt_att_setEffectiveMtuSize(u16 connHandle, u8 effective_mtu)
{
	bltAtt.effective_MTU = effective_mtu;
}

static inline void  blt_att_resetEffectiveMtuSize(u16 connHandle)
{
	bltAtt.effective_MTU = ATT_MTU_SIZE;
}

void blt_att_procHoldAttributeCommand(void);
