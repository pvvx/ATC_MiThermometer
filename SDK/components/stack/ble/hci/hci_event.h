/********************************************************************************************************
 * @file     hci_event.h 
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
 * hci_event.h
 *
 *  Created on: 2016-9-21
 *      Author: Administrator
 */

#ifndef HCI_EVENT_H_
#define HCI_EVENT_H_

#include <stack/ble/hci/hci.h>
#include <stack/ble/ble_common.h>

/**
 *  @brief  Definition for general HCI event packet
 */
typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         parameters[1];
} hci_event_t;



typedef struct {
	u8         status;
	u16        connHandle;
	u8         reason;
} hci_disconnectionCompleteEvt_t;



typedef struct {
	u8         numHciCmds;
	u8         opCode_OCF;
	u8		   opCode_OGF;
	u8         returnParas[1];
} hci_cmdCompleteEvt_t;


typedef struct {
	u8         status;
	u8         numHciCmds;
	u8         opCode_OCF;
	u8		   opCode_OGF;
} hci_cmdStatusEvt_t;



typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         subEventCode;
	u8         parameters[1];
} hci_le_metaEvt_t;


typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u8         role;
	u8         peerAddrType;
	u8         peerAddr[BLE_ADDR_LEN];
	u16        connInterval;
	u16        slaveLatency;
	u16        supervisionTimeout;
	u8         masterClkAccuracy;
} hci_le_connectionCompleteEvt_t;


typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u16        connInterval;
	u16        connLatency;
	u16        supervisionTimeout;
} hci_le_connectionUpdateCompleteEvt_t;


typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u8		   feature[LL_FEATURE_SIZE];
} hci_le_readRemoteFeaturesCompleteEvt_t;


typedef struct {
	u8         subEventCode;
	u16        connHandle;  //no aligned, can not be used as pointer
	u16  	   maxTxOct;
	u16		   maxTxtime;
	u16  	   maxRxOct;
	u16		   maxRxtime;
} hci_le_dataLengthChangeEvt_t;


/**
 *  @brief  Definition for HCI LE Read Local P-256 Public Key Complete event
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u8         localP256Key[64];
} hci_le_readLocalP256KeyCompleteEvt_t;

/**
 *  @brief  Definition for HCI LE generate DHKey Complete event
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u8         DHKey[32];
} hci_le_generateDHKeyCompleteEvt_t;


/**
 *  @brief  Definition for HCI long term key request event
 */
typedef struct {
	u8         subEventCode;
	u16        connHandle;
	u8         random[8];
	u16        ediv;
} hci_le_longTermKeyRequestEvt_t;


/**
 *  @brief  Definition for HCI Encryption Change event
 */
typedef struct {
	u8         status;
	u16        connHandle;
	u8         encryption_enable;
} hci_le_encryptEnableEvt_t;

/**
 *  @brief  Definition for HCI Encryption Key Refresh Complete event
 */
typedef struct {
	u8         status;
	u16        connHandle;
} hci_le_encryptKeyRefreshEvt_t;







/**
 *  @brief  Definition for HCI LE PHY Update Complete event
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u8 		   tx_phy;
	u8		   rx_phy;
} hci_le_phyUpdateCompleteEvt_t;


/**
 *  @brief  Definition for HCI LE Extended Advertising Report event
 */
typedef struct {
	//TODO
} hci_le_extAdvReportEvt_t;


/**
 *  @brief  Definition for HCI LE Periodic Advertising Sync Established Event
 */
typedef struct {
	//TODO
} hci_le_PeriodicAdvSyncEstablishedEvt_t;


/**
 *  @brief  Definition for HCI LE Periodic Advertising Report Event
 */
typedef struct {
	//TODO
} hci_le_periodicAdvReportEvt_t;


/**
 *  @brief  Definition for HCI LE Periodic Advertising Sync Lost Event
 */
typedef struct {
	//TODO
} hci_le_periodicAdvSyncLostEvt_t;


/**
 *  @brief  Definition for HCI LE Scan Timeout Event
 */
typedef struct {
	//TODO
} hci_le_scanTimeoutEvt_t;


/**
 *  @brief  Definition for HCI LE Advertising Set Terminated Event
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u8         advHandle;
	u16		   connHandle;
	u8		   num_compExtAdvEvt;
} hci_le_advSetTerminatedEvt_t;


/**
 *  @brief  Definition for HCI LE Scan Request Received Event
 */
typedef struct {
	//TODO
} hci_le_scanReqRcvdEvt_t;


/**
 *  @brief  Definition for HCI LE Channel Selection Algorithm Event
 */
typedef struct {
	u8         subEventCode;
	u16        connHandle;
	u8		   channel_selection_algotihm;
} hci_le_chnSelectAlgorithmEvt_t;









void hci_disconnectionComplete_evt(u8 status, u16 connHandle, u8 reason);
int  hci_cmdComplete_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 paraLen, u8 *para, u8 *result);
void hci_cmdStatus_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 status, u8 *result);


void hci_le_connectionComplete_evt(u8 status, u16 connHandle, u8 role, u8 peerAddrType, u8 *peerAddr,
                                   u16 connInterval, u16 slaveLatency, u16 supervisionTimeout, u8 masterClkAccuracy);
void hci_le_connectionUpdateComplete_evt(u8 status, u16 connHandle, u16 connInterval,
        									u16 connLatency, u16 supervisionTimeout);
void hci_le_readRemoteFeaturesComplete_evt(u8 status, u16 connHandle, u8 * feature);

void hci_le_chennel_selection_algorithm_evt(u16 connhandle, u8 channel_selection_alg);
void hci_le_phyUpdateComplete_evt(u16 connhandle,u8 status, u8 new_phy);
void hci_le_data_len_update_evt(u16 connhandle,u16 effTxOctets, u16 effRxOctets, u16 maxtxtime, u16 maxrxtime);



int hci_le_longTermKeyRequest_evt(u16 connHandle, u8* random, u16 ediv, u8* result);
int hci_le_readLocalP256KeyComplete_evt(u8* localP256Key, u8* result);
int hci_le_generateDHKeyComplete_evt(u8* DHkey,  u8* result);
int hci_le_encryptChange_evt(u16 connhandle,  u8 encrypt_en);
int hci_le_encryptKeyRefresh_evt(u16 connhandle);

int hci_remoteNateReqComplete_evt (u8* bd_addr);
#endif /* HCI_EVENT_H_ */





