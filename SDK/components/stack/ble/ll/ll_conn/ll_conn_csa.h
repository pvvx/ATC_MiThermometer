/********************************************************************************************************
 * @file     ll_conn_csa.h
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
 * @date     July. 4, 2019
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

#ifndef LL_CONN_CSA_H_
#define LL_CONN_CSA_H_



/******************************* Macro & Enumeration variables for Stack, user can not use!!!!  ********************************/

//See the Core_v5.0(Vol 6/Part B/4.5.8, "Data Channel Index Selection") for more information.
typedef enum {
	CHANNAL_SELECTION_ALGORITHM_1      	=	0x00,
	CHANNAL_SELECTION_ALGORITHM_2      	=	0x01,
} channel_algorithm_t;



typedef u8 (*ll_chn_index_calc_callback_t)(u8*, u16, u16);
extern ll_chn_index_calc_callback_t	ll_chn_index_calc_cb;






/********************************  Macro & Enumeration variables for User *****************************************************/













/********************************  User Interface  ****************************************************************************/
void blc_ll_initChannelSelectionAlgorithm_2_feature(void);















/*******************************  Stack Interface, user can not use!!! *******************************************************/
 u8 blc_calc_remapping_table(u8 chm[5]);

 u8 blc_ll_channel_index_calc_csa2(u8 chm[5], u16 event_cntr, u16 channel_id);




#endif /* LL_CONN_CSA_H_ */
