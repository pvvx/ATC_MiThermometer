/********************************************************************************************************
 * @file     MassStorageClassCommon.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     Sep. 30, 2010
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

/* Includes: */
#include <application/usbstd/stdDescriptors.h>
#include "../common/types.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#pragma pack(1)

#define MS_CBW_SIGNATURE                               0x43425355UL
#define MS_CSW_SIGNATURE                               0x53425355UL
#define MS_COMMAND_DIR_DATA_OUT                        (0<<7)
#define MS_COMMAND_DIR_DATA_IN                         (1<<7)

#define MS_SCSI_CMD_INQUIRY                               0x12
#define MS_SCSI_CMD_REQUEST_SENSE                         0x03
#define MS_SCSI_CMD_TEST_UNIT_READY                       0x00
#define MS_SCSI_CMD_READ_CAPACITY_10                      0x25
#define MS_SCSI_CMD_SEND_DIAGNOSTIC                       0x1D
#define MS_SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL          0x1E
#define MS_SCSI_CMD_WRITE_10                              0x2A
#define MS_SCSI_CMD_READ_10                               0x28
#define MS_SCSI_CMD_WRITE_6                               0x0A
#define MS_SCSI_CMD_READ_6                                0x08
#define MS_SCSI_CMD_VERIFY_10                             0x2F
#define MS_SCSI_CMD_MODE_SENSE_6                          0x1A
#define MS_SCSI_CMD_MODE_SENSE_10                         0x5A

#define MS_SCSI_SENSE_KEY_GOOD                            0x00
#define MS_SCSI_SENSE_KEY_RECOVERED_ERROR                 0x01
#define MS_SCSI_SENSE_KEY_NOT_READY                       0x02
#define MS_SCSI_SENSE_KEY_MEDIUM_ERROR                    0x03
#define MS_SCSI_SENSE_KEY_HARDWARE_ERROR                  0x04
#define MS_SCSI_SENSE_KEY_ILLEGAL_REQUEST                 0x05

#define MS_SCSI_SENSE_KEY_UNIT_ATTENTION                  0x06
#define MS_SCSI_SENSE_KEY_DATA_PROTECT                    0x07
#define MS_SCSI_SENSE_KEY_BLANK_CHECK                     0x08
#define MS_SCSI_SENSE_KEY_VENDOR_SPECIFIC                 0x09
#define MS_SCSI_SENSE_KEY_COPY_ABORTED                    0x0A
#define MS_SCSI_SENSE_KEY_ABORTED_COMMAND                 0x0B
#define MS_SCSI_SENSE_KEY_VOLUME_OVERFLOW                 0x0D
#define MS_SCSI_SENSE_KEY_MISCOMPARE                      0x0E

#define MS_SCSI_ASENSE_NO_ADDITIONAL_INFORMATION          0x00
#define MS_SCSI_ASENSE_LOGICAL_UNIT_NOT_READY             0x04
#define MS_SCSI_ASENSE_INVALID_FIELD_IN_CDB               0x24

#define MS_SCSI_ASENSE_NOT_READY_TO_READY_CHANGE          0x28
#define MS_SCSI_ASENSE_WRITE_PROTECTED                    0x27
#define MS_SCSI_ASENSE_FORMAT_ERROR                       0x31
#define MS_SCSI_ASENSE_INVALID_COMMAND                    0x20
#define MS_SCSI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE 0x21
#define MS_SCSI_ASENSE_MEDIUM_NOT_PRESENT                 0x3A

#define MS_SCSI_ASENSEQ_NO_QUALIFIER                      0x00
#define MS_SCSI_ASENSEQ_FORMAT_COMMAND_FAILED             0x01
#define MS_SCSI_ASENSEQ_INITIALIZING_COMMAND_REQUIRED     0x02
#define MS_SCSI_ASENSEQ_OPERATION_IN_PROGRESS             0x07

enum MS_Descriptor_ClassSubclassProtocol_t
{
	MS_CSCP_MassStorageClass = 0x08,
	MS_CSCP_SCSITransparentSubclass = 0x06,
	MS_CSCP_BulkOnlyTransportProtocol = 0x50,
};

enum MS_ClassRequests_t
{
	MS_REQ_GetMaxLUN = 0xFE,
	MS_REQ_MassStorageReset,
};

enum MS_CommandStatusCodes_t
{
	MS_MS_SCSI_COMMAND_Pass,
	MS_MS_SCSI_COMMAND_Fail,
	MS_MS_SCSI_COMMAND_PhaseError,
};


typedef struct
{
	u32 Signature;
	u32 Tag;
	u32 DataTransferLength;
	u8  Flags;
	u8  LUN;
	u8  SCSICommandLength;
	u8  SCSICommandData[16];
}  MS_CommandBlockWrapper_t;

typedef struct
{
	u32 Signature;
	u32 Tag;
	u32 DataTransferResidue;
	u8  Status;
}  MS_CommandStatusWrapper_t;

typedef struct
{
	u8  ResponseCode;
	u8  SegmentNumber;
	u8 SenseKey            : 4;
	u8 Reserved            : 1;
	u8 ILI                 : 1;
	u8 EOM                 : 1;
	u8 FileMark            : 1;
	u8  Information[4];
	u8  AdditionalLength;
	u8  CmdSpecificInformation[4];
	u8  AdditionalSenseCode;
	u8  AdditionalSenseQualifier;
	u8  FieldReplaceableUnitCode;
	u8  SenseKeySpecific[3];
} MS_SCSI_Request_Sense_Response_t;


typedef struct
{
	u8 DeviceType          : 5;
	u8 PeripheralQualifier : 3;
	u8 Reserved            : 7;
	u8 Removable           : 1;
	u8  Version;
	u8 ResponseDataFormat  : 4;
	u8 Reserved2           : 1;
	u8 NormACA             : 1;
	u8 TrmTsk              : 1;
	u8 AERC                : 1;
	u8  AdditionalLength;
	u8  Reserved3[2];
	u8 SoftReset           : 1;
	u8 CmdQue              : 1;
	u8 Reserved4           : 1;
	u8 Linked              : 1;
	u8 Sync                : 1;
	u8 WideBus16Bit        : 1;
	u8 WideBus32Bit        : 1;
	u8 RelAddr             : 1;
	u8 VendorID[8];
	u8 ProductID[16];
	u8 RevisionID[4];
} MS_SCSI_Inquiry_Response_t;

#pragma pack()

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
