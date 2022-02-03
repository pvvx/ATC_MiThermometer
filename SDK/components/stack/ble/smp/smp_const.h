/********************************************************************************************************
 * @file     smp_const.h 
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
/*
 * smp_const.h
 *
 *  Created on: 2018-12-4
 *      Author: Administrator
 */

#ifndef SMP_CONST_H_
#define SMP_CONST_H_



///////////////////////////// SMP concerned ///////////////////////////////////
#define			SMP_OP_PAIRING_REQ					1
#define			SMP_OP_PAIRING_RSP					2
#define			SMP_OP_PAIRING_CONFIRM				3
#define			SMP_OP_PAIRING_RANDOM				4
#define			SMP_OP_PAIRING_FAIL					5
#define			SMP_OP_ENC_INFO						6
#define			SMP_OP_ENC_IDX						7
#define			SMP_OP_ENC_IINFO					8
#define			SMP_OP_ENC_IADR						9
#define			SMP_OP_ENC_SIGN						0x0a
#define			SMP_OP_SEC_REQ						0x0b
#define			SMP_OP_PARING_PUBLIC_KEY			0x0c
#define			SMP_OP_PARING_DHKEY					0x0d
#define			SMP_OP_KEYPRESS_NOTIFICATION		0x0e
#define			SMP_OP_WAIT							0x0f

#define			SMP_OP_ENC_END						0xFF


#define 		SMP_TRANSPORT_SPECIFIC_KEY_START    0xEF
#define 		SMP_TRANSPORT_SPECIFIC_KEY_END      0

#define IO_CAPABLITY_DISPLAY_ONLY					0x00
#define IO_CAPABLITY_DISPLAY_YESNO					0x01
#define IO_CAPABLITY_KEYBOARD_ONLY					0x02
#define IO_CAPABLITY_NO_IN_NO_OUT					0x03
#define IO_CAPABLITY_KEYBOARD_DISPLAY				0x04

#define PASSKEY_TYPE_ENTRY_STARTED					0x00
#define PASSKEY_TYPE_DIGIT_ENTERED					0x01
#define PASSKEY_TYPE_DIGIT_ERASED					0x02
#define PASSKEY_TYPE_CLEARED						0x03
#define PASSKEY_TYPE_ENTRY_COMPLETED				0x04

#define PARING_FAIL_REASON_PASSKEY_ENTRY			0x01
#define PARING_FAIL_REASON_OOB_NOT_AVAILABLE		0x02
#define PARING_FAIL_REASON_AUTH_REQUIRE				0x03
#define PARING_FAIL_REASON_CONFIRM_FAILED			0x04
#define PARING_FAIL_REASON_PARING_NOT_SUPPORTED		0x05
#define PARING_FAIL_REASON_ENCRYPT_KEY_SIZE			0x06
//-- core 4.2
#define PARING_FAIL_REASON_CMD_NOT_SUPPORT			0x07
#define PARING_FAIL_REASON_UNSPECIFIED_REASON		0x08
#define PARING_FAIL_REASON_REPEATED_ATTEMPT			0x09
#define PARING_FAIL_REASON_INVAILD_PARAMETER		0x0A
#define PARING_FAIL_REASON_DHKEY_CHECK_FAIL			0x0B
#define PARING_FAIL_REASON_NUMUERIC_FAILED			0x0C
#define PARING_FAIL_REASON_BREDR_PARING				0x0D
#define PARING_FAIL_REASON_CROSS_TRANSKEY_NOT_ALLOW	0x0E

//teLink define
#define PARING_FAIL_REASON_PARING_TIEMOUT			0x80
#define PARING_FAIL_REASON_CONN_DISCONNECT			0x81
#define PARING_FAIL_REASON_SUPPORT_NC_ONLY			0x82

#define	ENCRYPRION_KEY_SIZE_MAXINUM				16
#define	ENCRYPRION_KEY_SIZE_MINIMUN				7



//define for TK status
#define TK_ST_REQUEST								BIT(0)
#define TK_ST_UPDATE								BIT(1)
#define TK_ST_CONFIRM_PENDING						BIT(2)
#define TK_ST_NUMERIC_COMPARE						BIT(3)
#define TK_ST_NUMERIC_CHECK_YES					    BIT(4)
#define TK_ST_NUMERIC_CHECK_NO					    BIT(5)
#define TK_ST_NUMERIC_DHKEY_FAIL_PENDING			BIT(6)
#define TK_ST_NUMERIC_DHKEY_SUCC_PENDING			BIT(7)


//define for KEY MASK
#define KEY_MASK_ENC							    BIT(0)
#define KEY_MASK_IDENTITY 							BIT(1)
#define KEY_MASK_SIGN								BIT(2)


typedef enum {
	Smp_None		 = 0,
	Smp_Peripheral   = BIT(0),
	Smp_Central      = BIT(1),
}smp_role_t;




//See the Core_v5.0(Vol 3/Part C/10.2, Page 2067) for more information.
typedef enum {
	LE_Security_Mode_1_Level_1 = BIT(0),  No_Authentication_No_Encryption			= BIT(0),         No_Security   = BIT(0),
	LE_Security_Mode_1_Level_2 = BIT(1),  Unauthenticated_Paring_with_Encryption 	= BIT(1),
	LE_Security_Mode_1_Level_3 = BIT(2),    Authenticated_Paring_with_Encryption 	= BIT(2),
	LE_Security_Mode_1_Level_4 = BIT(3),    Authenticated_LE_Secure_Connection_Paring_with_Encryption = BIT(3),

	LE_Security_Mode_2_Level_1 = BIT(4),  Unauthenticated_Paring_with_Data_Signing 	= BIT(4),
	LE_Security_Mode_2_Level_2 = BIT(5),    Authenticated_Paring_with_Data_Signing  = BIT(5),

	LE_Security_Mode_1 = (LE_Security_Mode_1_Level_1 | LE_Security_Mode_1_Level_2 | LE_Security_Mode_1_Level_3 | LE_Security_Mode_1_Level_4)
}le_security_mode_level_t;


typedef enum {
	Non_Bondable_Mode = 0,
	Bondable_Mode     = 1,
}bonding_mode_t;



//Paring Methods select
//See the Core_v5.0(Vol 3/Part H/2.3) for more information.
typedef enum {
	LE_Legacy_Paring     = 0,   // BLE 4.0/4.2
	LE_Secure_Connection = 1,   // BLE 4.2/5.0/5.1
}paring_methods_t;


typedef enum {
	IO_CAPABILITY_UNKNOWN = 0xff,
	IO_CAPABILITY_DISPLAY_ONLY = 0,
	IO_CAPABILITY_DISPLAY_YES_NO = 1,
	IO_CAPABILITY_KEYBOARD_ONLY = 2,
	IO_CAPABILITY_NO_INPUT_NO_OUTPUT = 3,
	IO_CAPABILITY_KEYBOARD_DISPLAY = 4,
} io_capability_t;





typedef enum {
	JustWorks = 0,
	PK_Init_Dsply_Resp_Input = 1,
	PK_Resp_Dsply_Init_Input = 2,
	PK_BOTH_INPUT = 3,
	OOB_Authentication = 4,
	Numric_Comparison = 5,
} stk_generationMethod_t;


// H: Initiator Capabilities
// V: Responder Capabilities
// See the Core_v5.0(Vol 3/Part H/2.3.5.1) for more information.
static const stk_generationMethod_t gen_method_legacy[5 /*Responder*/][5 /*Initiator*/] = {
	{ JustWorks,      			JustWorks,       		  PK_Resp_Dsply_Init_Input, JustWorks, PK_Resp_Dsply_Init_Input },
	{ JustWorks,      			JustWorks,       		  PK_Resp_Dsply_Init_Input, JustWorks, PK_Resp_Dsply_Init_Input },
	{ PK_Init_Dsply_Resp_Input, PK_Init_Dsply_Resp_Input, PK_BOTH_INPUT,   			JustWorks, PK_Init_Dsply_Resp_Input },
	{ JustWorks,      			JustWorks,       		  JustWorks,      			JustWorks, JustWorks   			    },
	{ PK_Init_Dsply_Resp_Input, PK_Init_Dsply_Resp_Input, PK_Resp_Dsply_Init_Input, JustWorks, PK_Init_Dsply_Resp_Input },
};

#if SECURE_CONNECTION_ENABLE
static const stk_generationMethod_t gen_method_sc[5 /*Responder*/][5 /*Initiator*/] = {
	{ JustWorks,      			JustWorks,       		  PK_Resp_Dsply_Init_Input, JustWorks, PK_Resp_Dsply_Init_Input },
	{ JustWorks,      			Numric_Comparison,        PK_Resp_Dsply_Init_Input, JustWorks, Numric_Comparison },
	{ PK_Init_Dsply_Resp_Input, PK_Init_Dsply_Resp_Input, PK_BOTH_INPUT,   			JustWorks, PK_Init_Dsply_Resp_Input },
	{ JustWorks,      			JustWorks,       		  JustWorks,      			JustWorks, JustWorks   			    },
	{ PK_Init_Dsply_Resp_Input, Numric_Comparison, 		  PK_Resp_Dsply_Init_Input, JustWorks, Numric_Comparison },
};
#endif




//Local Device Paring Status
//See the Core_v5.0(Vol 3/Part C/10.3.1/Table 10.2) for more information.
typedef enum {
	No_Bonded_No_LTK = 0,    //No_LTK
	Unauthenticated_LTK,
	Authenticated_LTK_Legacy_Paring,
	Authenticated_LTK_Secure_Connection,
} paring_sts_t;










typedef union {
	struct{
		u8 bondingFlag : 2;
		u8 MITM : 1;
		u8 SC	: 1;
		u8 keyPress: 1;
		u8 rsvd: 3;
	};
	u8 authType;
}smp_authReq_t;

typedef union{
	struct {
		u8 encKey : 1;
		u8 idKey : 1;
		u8 sign  : 1;
		u8 linkKey : 4;
	};
	u8 keyIni;
}smp_keyDistribution_t;

typedef struct{
	u8 code;  //req = 0x01; rsp = 0x02;
	u8 ioCapablity;
	u8 oobDataFlag;
	smp_authReq_t authReq;
	u8 maxEncrySize;

	smp_keyDistribution_t initKeyDistribution;
	smp_keyDistribution_t rspKeyDistribution;
}smp_paring_req_rsp_t;



typedef struct{
	u8  code; //0x04
	u16 randomValue[16];
}smp_paring_random_t;

typedef struct{
	u8  code; //0x03
	u16 confirmValue[16];
}smp_paring_confirm_t;

typedef struct{
	u8 code; // 0x05
	u8 reason;
}smp_paring_failed_t;

typedef struct{
	u8 code;//0x0b

	smp_authReq_t authReq;
}smp_secure_req_t;

typedef struct{
	u8  code;//0xa
	u8 	signalKey[16];
}smp_signal_info_t;

typedef struct{
	u8 code;//0x9
	u8 addrType;
	u8 bdAddr[6];
}smp_id_addr_info_t;

typedef struct{
	u8 code;//0x8
	u8 idResolveKey[16];
}smp_id_info_t;

typedef struct{
	u8  code;//0x7
	u16 edivPtr[2];
	u8 	masterRand[8];
}smp_master_id_t;

typedef struct{
	u8  code;//0x6
	u8 	LTK[16];
}smp_enc_info_t;

// -------add core 4.2 ------
typedef struct{
	u8  code;//0xc
	u8  publicKeyX[32];
	u8  publicKeyY[32];
}smp_paring_public_key_t;

typedef struct{
	u8  code;//0xd
	u8  DHKeyCheck[16];
}smp_DHkey_check_t;

typedef struct{
	u8  code;//0xe
	u8  notifyType;
}smp_key_notify_t;

typedef struct{
	u8 sc_sk_dhk_own[32];  //  own  private key[32]
	u8 sc_pk_own[64];      //  own  public  key[64]
	u8 sc_pk_peer[64];     // peer  public  key[64]
}smp_sc_key_t;




typedef struct {
	/* data */
	u8 csrk[16];
	u32 signCounter;
} smp_secSigInfo_t;











#endif /* SMP_CONST_H_ */
