/********************************************************************************************************
 * @file     hci_const.h 
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
 * hci_const.h
 *
 *  Created on: 2016-9-20
 *      Author: Administrator
 */

#ifndef HCI_CONST_H_
#define HCI_CONST_H_

/****HCI INFO****/
#define HCI_VERSION 0x09       //Bluetooth Core Specification 5.0
#define HCI_REVISION 0x0002     //Revision of the Current HCI in the BR/EDR Controller
#define HCI_LMP_VERSION 0x09  //Version of the Current LMP or PAL in the Controller, Bluetooth Core Specification 5.0
#define HCI_MANUFACTURER_NAME 		VENDOR_ID //Manufacturer Name of the BR/EDR Controller
#define HCI_LMP_SUBVERSION 0x0001 //Subversion of the Current LMP or PAL in the Controller

#define LMP_FEATURES 0x0000000000000000




/****Events****/
#define HCI_EVT_DISCONNECTION_COMPLETE                               0x05

#define HCI_EVT_REMOTE_NAME_REQ_COMPLETE                             0x07
#define HCI_EVT_ENCRYPTION_CHANGE                                    0x08
#define HCI_EVT_CHANGE_LINK_KEY_COMPLETE							 0x09
#define HCI_EVT_READ_REMOTE_VER_INFO_COMPLETE                        0x0C
#define HCI_EVT_CMD_COMPLETE                                         0x0E
#define HCI_EVT_CMD_STATUS                                           0x0F
#define HCI_EVT_HW_ERROR                                             0x10
#define HCI_EVT_NUM_OF_COMPLETE_PACKETS                              0x13
#define HCI_EVT_DATA_BUF_OVERFLOW                                    0x1A
#define HCI_EVT_ENCRYPTION_KEY_REFRESH                               0x30
#define HCI_EVT_LE_META                                              0x3E
#define HCI_EVT_CERT_VS                                              0xF0

// LE Meta Event Codes
#define HCI_SUB_EVT_LE_CONNECTION_COMPLETE                           0x01	//core_4.0
#define HCI_SUB_EVT_LE_ADVERTISING_REPORT                            0x02
#define HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE                    0x03
#define HCI_SUB_EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE            0x04
#define HCI_SUB_EVT_LE_LONG_TERM_KEY_REQUESTED                       0x05	//core_4.0
#define HCI_SUB_EVT_LE_REMOTE_CONNECTION_PARAM_REQUEST               0x06   //core_4.1
#define HCI_SUB_EVT_LE_DATA_LENGTH_CHANGE                            0x07	//core_4.2
#define HCI_SUB_EVT_LE_READ_LOCAL_P256_KEY_COMPLETE                  0x08
#define HCI_SUB_EVT_LE_GENERATE_DHKEY_COMPLETE				         0x09
#define HCI_SUB_EVT_LE_ENHANCED_CONNECTION_COMPLETE                  0x0A
#define HCI_SUB_EVT_LE_DIRECT_ADVERTISE_REPORT		                 0x0B   //core_4.2

#define HCI_SUB_EVT_LE_PHY_UPDATE_COMPLETE							 0x0C	//core_5.0
#define HCI_SUB_EVT_LE_EXTENDED_ADVERTISING_REPORT                   0x0D   //core_5.0 - LE Extended Advertising Report Event - [5] 7.7.65.13
#define HCI_SUB_EVT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED         0x0E   //core_5.0 - LE Periodic Advertising Sync Established Event - [5] 7.7.65.14
#define HCI_SUB_EVT_LE_PERIODIC_ADVERTISING_REPORT                   0x0F   //core_5.0 - LE Periodic Advertising Report Event- [5] 7.7.65.15
#define HCI_SUB_EVT_LE_PERIODIC_ADVERTISING_SYNC_LOST                0x10   //core_5.0 - LE Periodic Advertising Sync Lost Event - [5] 7.7.65.16
#define HCI_SUB_EVT_LE_SCAN_TIMEOUT                                  0x11   //core_5.0 - LE Scan Timeout Event - [5] 7.7.65.17
#define HCI_SUB_EVT_LE_ADVERTISING_SET_TERMINATED                    0x12   //core_5.0 - LE Advertising Set Terminated Event - [5]7.7.65.18
#define HCI_SUB_EVT_LE_SCAN_REQUEST_RECEIVED                         0x13   //core_5.0 - LE Scan Request Received Event - [5]7.7.65.19
#define HCI_SUB_EVT_LE_LE_CHANNEL_SELECTION_ALGORITHM                0x14   //core_5.0 - LE Channel Selection Algorithm Event - [5]7.7.65.20

#define HCI_SUB_EVT_LE_CONNECTION_ESTABLISH					         0x20   //telink private




//Event mask - last octet
#define HCI_EVT_MASK_NONE                                        	 0x0000000000
#define HCI_EVT_MASK_INQUIRY_COMPLETE                          	  	 0x0000000001
#define HCI_EVT_MASK_INQUIRY_RESULT                              	 0x0000000002
#define HCI_EVT_MASK_CONNECTION_COMPELETE                        	 0x0000000004
#define HCI_EVT_MASK_CONNECTION_REQUEST                          	 0x0000000008
#define HCI_EVT_MASK_DISCONNECTION_COMPLETE                      	 0x0000000010     //
#define HCI_EVT_MASK_AUTHENTICATION_COMPLETE                     	 0x0000000020
#define HCI_EVT_MASK_REMOTE_NAME_REQUEST_COMPLETE                	 0x0000000040
#define HCI_EVT_MASK_ENCRYPTION_CHANGE                           	 0x0000000080
#define HCI_EVT_MASK_CHANGE_CONECTION_LINK_KEY_COMPLETE              0x0000000100
#define HCI_EVT_MASK_MASTER_LINK_KEY_COMPLETE                        0x0000000200
#define HCI_EVT_MASK_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE     	 0x0000000400
#define HCI_EVT_MASK_READ_REMOTE_VERSION_INFORMATION_COMPLETE    	 0x0000000800     //

#define HCI_EVT_MASK_DEFAULT                                     	 HCI_EVT_MASK_DISCONNECTION_COMPLETE


// LE Event mask - last octet
#define HCI_LE_EVT_MASK_NONE                                         0x00000000
#define HCI_LE_EVT_MASK_CONNECTION_COMPLETE                          0x00000001
#define HCI_LE_EVT_MASK_ADVERTISING_REPORT                           0x00000002
#define HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE                   0x00000004
#define HCI_LE_EVT_MASK_READ_REMOTE_FEATURES_COMPLETE                0x00000008
#define HCI_LE_EVT_MASK_LONG_TERM_KEY_REQUEST                        0x00000010
#define HCI_LE_EVT_MASK_REMOTE_CONNECTION_PARAM_REQUEST              0x00000020
#define HCI_LE_EVT_MASK_DATA_LENGTH_CHANGE                        	 0x00000040
#define HCI_LE_EVT_MASK_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE          0x00000080
#define HCI_LE_EVT_MASK_GENERATE_DHKEY_COMPLETE                      0x00000100
#define HCI_LE_EVT_MASK_ENHANCED_CONNECTION_COMPLETE                 0x00000200
#define HCI_LE_EVT_MASK_DIRECT_ADVERTISING_REPORT                    0x00000400
#define HCI_LE_EVT_MASK_PHY_UPDATE_COMPLETE                          0x00000800 // core5.0
#define HCI_LE_EVT_MASK_EXTENDED_ADVERTISING_REPORT                  0x00001000 // core5.0
#define HCI_LE_EVT_MASK_PERIODIC_ADVERTISING_SYNC_ESTABLISHED        0x00002000 // core5.0
#define HCI_LE_EVT_MASK_PERIODIC_ADVERTISING_REPORT                  0x00004000 // core5.0
#define HCI_LE_EVT_MASK_PERIODIC_ADVERTISING_SYNC_LOST               0x00008000 // core5.0
#define HCI_LE_EVT_MASK_EXTENDED_SCAN_TIMEOUT                        0x00010000 // core5.0
#define HCI_LE_EVT_MASK_EXTENDED_ADVERTISING_SET_TERMINATED          0x00020000 // core5.0
#define HCI_LE_EVT_MASK_SCAN_REQUEST_RECEIVED                        0x00040000 // core5.0
#define HCI_LE_EVT_MASK_CHANNEL_SELECTION_ALGORITHM                  0x00080000 // core5.0 - bit19


#define HCI_LE_EVT_MASK_CONNECTION_ESTABLISH                         0x80000000  //telink private


#define HCI_LE_EVT_MASK_DEFAULT                                      HCI_LE_EVT_MASK_NONE





//Link Control Command
//-- OGF --
#define HCI_CMD_LINK_CTRL_OPCODE_OGF								 0x04  //0x01 <<2 = 0x04
//-- OCF --
#define HCI_CMD_INQUIRY                                          	 0x01
#define HCI_CMD_DISCONNECT                                           0x06
#define HCI_CMD_READ_REMOTE_NAME_REQ                                 0x19
#define HCI_CMD_READ_REMOTE_VER_INFO                                 0x1D



//Controller & Baseband Commands
//-- OGF --
#define HCI_CMD_CBC_OPCODE_OGF										 0x0C  //0x03 <<2, controller & baseband control
//-- OCF --
#define HCI_CMD_SET_EVENT_MASK                                       0x01
#define HCI_CMD_RESET                                                0x03
#define HCI_CMD_SET_EVENT_FILTER									 0x05
#define HCI_CMD_WRITE_PIN_TYPE										 0x0A
#define HCI_CMD_CREATE_NEW_UINT_KEY									 0x0B
#define HCI_CMD_DELETE_STORED_LINK_KEY								 0x12
#define	HCI_CMD_WRITE_LOCAL_NAME									 0x13
#define	HCI_CMD_READ_LOCAL_NAME										 0x14
#define	HCI_CMD_WRITE_CONNECTION_ACCEPT_TIMEOUT						 0x16
#define	HCI_CMD_WRITE_PAGE_TIMEOUT									 0x18
#define	HCI_CMD_WRITE_SCAN_ENABLE									 0x1A
#define	HCI_CMD_WRITE_PAGE_SCAN_ACTIVITY							 0x1C
#define	HCI_CMD_WRITE_INQUIRY_SCAN_ACTIVITY							 0x1E
#define	HCI_CMD_WRITE_AUTHENTICATION_ENABLE							 0x20
#define HCI_CMD_WRITE_CLASS_OF_DEVICE			                     0x24
#define HCI_CMD_WRITE_VOICE_SETTING				                     0x26
#define HCI_CMD_WRITE_NUM_BROADCAST_RETRANSMISSIONS			         0x2A
#define HCI_CMD_WRITE_HOLD_MODE_ACTIVITY			         		 0x2C
#define HCI_CMD_READ_TX_POWER_LEVEL                                  0x2D
#define HCI_CMD_SYNCHRONOUS_FLOW_CONTROL_ENABLE						 0x2F
#define HCI_CMD_SET_CONTROLLER_TO_HOST_FLOW_CTRL                     0x31
#define HCI_CMD_HOST_BUF_SIZE                                        0x33
#define HCI_CMD_HOST_NUM_OF_COMPLETE_PACKETS                         0x35
#define HCI_CMD_WRITE_CURRENT_IAC_LAP 								 0x3A
#define HCI_CMD_SET_AFH_HOST_CHN_CLASSIFICATION						 0x3F
#define HCI_CMD_WRITE_INQUIRY_SCAN_TYPE								 0x43
#define HCI_CMD_WRITE_INQUIRY_MODE									 0x45
#define HCI_CMD_WRITE_PAGE_SCAN_TYPE								 0x47


//Informational Parameters
//-- OGF --
#define HCI_CMD_IP_OPCODE_OGF										 0x10  //0x04 <<2, information parameter
//-- OCF --
#define HCI_CMD_READ_LOCAL_VER_INFO                                  0x01
#define HCI_CMD_READ_LOCAL_SUPPORTED_CMDS                            0x02
#define HCI_CMD_READ_LOCAL_SUPPORTED_FEATURES                        0x03
#define HCI_CMD_READ_EXTENDED_LOCAL_SUPPORTED_FEATURES               0x04
#define HCI_CMD_READ_BUFFER_SIZE_COMMAND                             0x05
#define HCI_CMD_READ_BD_ADDR                                         0x09



// Status Parameters
//-- OGF --
#define HCI_CMD_STATUS_PARAM_OPCODE_OGF								 0x14  //0x05 <<2
//-- OCF --
#define HCI_CMD_READ_RSSI                                            0x05



#define	HCI_EVT_CMDSTATUS(n,c,g,s)									((s) | (n<<8) | (c<<16) | (g<<24))
#define	HCI_EVT_CMD_COMPLETE_STATUS(n,c,g,s)						((n<<0) | (c<<8) | (g<<16) | (s<<24))




// LE Controller Commands
//-- OGF --
#define HCI_CMD_LE_OPCODE_OGF										 0x20  //0x08 <<2 = 0x20
//-- OCF --
#define HCI_CMD_LE_SET_EVENT_MASK                                    0x01
#define HCI_CMD_LE_READ_BUF_SIZE                                     0x02
#define HCI_CMD_LE_READ_LOCAL_SUPPORTED_FEATURES                     0x03
#define HCI_CMD_LE_SET_RANDOM_ADDR                                   0x05
#define HCI_CMD_LE_SET_ADVERTISE_PARAMETERS                          0x06
#define HCI_CMD_LE_READ_ADVERTISING_CHANNEL_TX_POWER                 0x07
#define HCI_CMD_LE_SET_ADVERTISE_DATA                                0x08
#define HCI_CMD_LE_SET_SCAN_RSP_DATA                                 0x09
#define HCI_CMD_LE_SET_ADVERTISE_ENABLE                              0x0A
#define HCI_CMD_LE_SET_SCAN_PARAMETERS                               0x0B
#define HCI_CMD_LE_SET_SCAN_ENABLE                                   0x0C
#define HCI_CMD_LE_CREATE_CONNECTION                                 0x0D
#define HCI_CMD_LE_CREATE_CONNECTION_CANCEL                          0x0E
#define HCI_CMD_LE_READ_WHITE_LIST_SIZE                              0x0F
#define HCI_CMD_LE_CLEAR_WHITE_LIST                                  0x10
#define HCI_CMD_LE_ADD_DEVICE_TO_WHITE_LIST                          0x11
#define HCI_CMD_LE_REMOVE_DEVICE_FROM_WL                             0x12
#define HCI_CMD_LE_CONNECTION_UPDATE                                 0x13
#define HCI_CMD_LE_SET_HOST_CHANNEL_CLASSIFICATION                   0x14
#define HCI_CMD_LE_READ_CHANNEL_MAP                                  0x15
#define HCI_CMD_LE_READ_REMOTE_USED_FEATURES                         0x16
#define HCI_CMD_LE_ENCRYPT                                           0x17
#define HCI_CMD_LE_RANDOM                                            0x18
#define HCI_CMD_LE_START_ENCRYPTION                                  0x19
#define HCI_CMD_LE_LONG_TERM_KEY_REQUESTED_REPLY                     0x1A
#define HCI_CMD_LE_LONG_TERM_KEY_REQUESTED_NEGATIVE_REPLY            0x1B
#define HCI_CMD_LE_READ_SUPPORTED_STATES                             0x1C
#define HCI_CMD_LE_RECEIVER_TEST                                     0x1D
#define HCI_CMD_LE_TRANSMITTER_TEST                                  0x1E
#define HCI_CMD_LE_TEST_END                                          0x1F
//core_4.0 end
//core_4.2 begin
#define HCI_CMD_LE_REMOTE_CONNECTION_PARAM_REQ_REPLY             	 0x20
#define HCI_CMD_LE_REMOTE_CONNECTION_PARAM_REQ_NEGATIVE_REPLY    	 0x21
//core_4.1 end
//core_4.2 begin
#define HCI_CMD_LE_SET_DATA_LENGTH			                         0x22
#define HCI_CMD_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH                0x23
#define HCI_CMD_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH               0x24
#define HCI_CMD_LE_READ_LOCAL_P256_PUBLIC_KEY                        0x25
#define HCI_CMD_LE_GENERATE_DHKEY                         	 		 0x26
#define HCI_CMD_LE_ADD_DEVICE_TO_RESOLVING_LIST                      0x27
#define HCI_CMD_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST                 0x28
#define HCI_CMD_LE_CLEAR_RESOLVING_LIST                              0x29
#define HCI_CMD_LE_READ_RESOLVING_LIST_SIZE                     	 0x2A
#define HCI_CMD_LE_READ_PEER_RESOLVABLE_ADDRESS            			 0x2B
#define HCI_CMD_LE_READ_LOCAL_RESOLVABLE_ADDRESS                     0x2C
#define HCI_CMD_LE_SET_ADDRESS_RESOLUTION_ENABLE                     0x2D
#define HCI_CMD_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT            0x2E
#define HCI_CMD_LE_READ_MAX_DATA_LENGTH                              0x2F
//core_4.2 end
//core_5.0 begin
#define HCI_CMD_LE_READ_PHY                       					 0x30//LE Read PHY Command - [5] 7.8.47
#define HCI_CMD_LE_SET_DEFAULT_PHY              					 0x31//LE Set Default PHY Command - [5] 7.8.48
#define HCI_CMD_LE_SET_PHY                      					 0x32//LE Set PHY Command - [5] 7.8.49
#define HCI_CMD_LE_ENHANCED_RECEIVER_TEST                			 0x33//LE Enhanced Receiver Test Command - [5] 7.8.50
#define HCI_CMD_LE_ENHANCED_TRANSMITTER_TEST               			 0x34//LE Enhanced Transmitter Test Command - [5] 7.8.51
#define HCI_CMD_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS         		 0x35//LE Set Advertising Set Random Address Command - [5] 7.8.52
#define HCI_CMD_LE_SET_EXTENDED_ADVERTISING_PARAMETERS            	 0x36//LE Set Extended Advertising Parameters Command - [5] 7.8.53
#define HCI_CMD_LE_SET_EXTENDED_ADVERTISING_DATA             		 0x37//LE Set Extended Advertising Data Command - [5] 7.8.54
#define HCI_CMD_LE_SET_EXTENDED_SCAN_RESPONSE_DATA       		     0x38//LE Set Extended Scan Response Data Command - [5] 7.8.55
#define HCI_CMD_LE_SET_EXTENDED_ADVERTISING_ENABLE           		 0x39//LE Set Extended Advertising Enable Command - [5] 7.8.56
#define HCI_CMD_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH          	 0x3A//LE Read Maximum Advertising Data Length Command - [5] 7.8.57
#define HCI_CMD_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS         0x3B//LE Read Number of Supported Advertising Sets Command - [5] 7.8.58
#define HCI_CMD_LE_REMOVE_ADVERTISING_SET               			 0x3C//LE Remove Advertising Set Command - [5] 7.8.59
#define HCI_CMD_LE_CLEAR_ADVERTISING_SETS               			 0x3D//LE Clear Advertising Sets Command - [5] 7.8.60
#define HCI_CMD_LE_SET_PERIODIC_ADVERTISING_PARAMETERS           	 0x3E//LE Set Periodic Advertising Parameters Command - [5] 7.8.61
#define HCI_CMD_LE_SET_PERIODIC_ADVERTISING_DATA             		 0x3F//LE Set Periodic Advertising Data Command - [5] 7.8.62
#define HCI_CMD_LE_SET_PERIODIC_ADVERTISING_ENABLE           		 0x40//LE Set Periodic Advertising Enable Command - [5] 7.8.63
#define HCI_CMD_LE_SET_EXTENDED_SCAN_PARAMETERS           			 0x41//LE Set Extended Scan Parameters Command - [5] 7.8.64
#define HCI_CMD_LE_SET_EXTENDED_SCAN_ENABLE          				 0x42//LE Set Extended Scan Enable Command - [5] 7.8.65
#define HCI_CMD_LE_EXTENDED_CREATE_CONNECTION              			 0x43//LE Extended Create Connection Command - [5] 7.8.66
#define HCI_CMD_LE_PERIODIC_ADVERTISING_CREATE_SYNC          		 0x44//LE Periodic Advertising Create Sync Command- [5] 7.8.67
#define HCI_CMD_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL   		 0x45//LE Periodic Advertising Create Sync Cancel Command - [5] 7.8.68
#define HCI_CMD_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC            	 0x46//LE Periodic Advertising Terminate Sync Command - [5] 7.8.69
#define HCI_CMD_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST      		 0x47//LE Add Device To Periodic Advertiser List Command - [5] 7.8.70
#define HCI_CMD_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST    	 0x48//LE Remove Device From Periodic Advertiser List Command - [5] 7.8.71
#define HCI_CMD_LE_CLEAR_PERIODIC_ADVERTISER_LIST           		 0x49//LE Clear Periodic Advertiser List Command - [5] 7.8.72
#define HCI_CMD_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE         		 0x4A//LE Read Periodic Advertiser List Size Command - [5] 7.8.73
#define HCI_CMD_LE_READ_TRANSMIT_POWER            					 0x4B//LE Read Transmit Power Command - [5] 7.8.74
#define HCI_CMD_LE_READ_RF_PATH_COMPENSATION      					 0x4C//LE Read RF Path Compensation Command - [5] 7.8.75
#define HCI_CMD_LE_WRITE_RF_PATH_COMPENSATION      					 0x4D//LE Write RF Path Compensation Command - [5] 7.8.76
#define HCI_CMD_LE_SET_PRIVACY_MODE             					 0x4E//LE Set Privacy Mode Command - [5] 7.8.77
//core_5.0 end


#define HCI_CMD_LINK_POLICY_OPCODE_OGF								 0x08 //0x02<<2 = 0x08
#define HCI_CMD_TEST_OPCODE_OGF										 0x18 //0x06<<2 = 0x18

// Vendor specific Commands
//-- OGF --
#define HCI_CMD_VENDOR_OPCODE_OGF									 0xFC  //0x3f <<2 = 0xFC
//-- OCF --
#define HCI_TELINK_READ_REG											 0x01
#define HCI_TELINK_WRTIE_REG										 0x02
#define HCI_TELINK_SET_TX_PWR										 0x03

#define HCI_TELINK_SET_RXTX_DATA_LEN								 0x40
#endif /* HCI_CONST_H_ */
