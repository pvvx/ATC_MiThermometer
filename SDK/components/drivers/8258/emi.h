/********************************************************************************************************
 * @file     emi.h
 *
 * @brief    This is the header file for TLSR8258
 *
 * @author	 Driver Group
 * @date     May 8, 2018
 *
 * @par      Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 * @par      History:
 * 			 1.initial release(DEC. 26 2018)
 *
 * @version  A001
 *
 *******************************************************************************************************/
#ifndef EMI_H_
#define EMI_H_

#include "rf_drv.h"


/**
 * @brief   This function serves to set singletone power.
 * @param   level - the power level.
 * @return  none.
 */
void rf_set_power_level_index_singletone (RF_PowerTypeDef level);

/**
 * @brief   This function serves to set singletone power and channel.
 * @param   power_level - the power level.
 * @param   rf_chn - the channel.
 * @return  none.
 */
void rf_emi_single_tone(RF_PowerTypeDef power_level,signed char rf_chn);


/**
 * @brief   This function serves to close RF
 * @param   none.
 * @return  none.
 */
void rf_emi_stop(void);

/**
 * @brief   This function serves to set rx mode and channel.
 * @param   mode - mode of RF
 * @param   rf_chn - the rx channel.
 * @return  none.
 */
void rf_emi_rx(RF_ModeTypeDef mode,signed char rf_chn);
/**
 * @brief   This function serves is receiving service program
 * @param   none.
 * @return  none.
 */
void rf_emi_rx_loop(void);

/**
 * @brief   This function serves to get the number of packets received
 * @param   none.
 * @return  the number of packets received.
 */
unsigned int rf_emi_get_rxpkt_cnt(void);

/**
 * @brief   This function serves to get the RSSI of packets received
 * @param   none.
 * @return  the RSSI of packets received.
 */
char rf_emi_get_rssi_avg(void);

/**
 * @brief   This function serves to get the address of the received packets
 * @param   none.
 * @return  the address of the received packets
 */
unsigned char *rf_emi_get_rxpkt(void);

/**
 * @brief   This function serves to continue to send CD mode.
 * @param   none.
 * @return  none.
 */
void rf_continue_mode_run(void);




/**
 * @brief   This function serves to init the CD mode.
 * @param   rf_mode - mode of RF.
 * @param   power_level - power level of RF.
 * @param   rf_chn - channel of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_continue_setup(RF_ModeTypeDef rf_mode,RF_PowerTypeDef power_level,signed char rf_chn,unsigned char pkt_type);




/**
 * @brief   This function serves to init the burst mode.
 * @param   rf_mode - mode of RF.
 * @param   power_level - power level of RF.
 * @param   rf_chn - channel of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_burst_setup(RF_ModeTypeDef rf_mode,RF_PowerTypeDef power_level,signed char rf_chn,unsigned char pkt_type);

/**
 * @brief   This function serves to init the burst mode with PA ramp up/down.
 * @param   rf_mode - mode of RF.
 * @param   power_level - power level of RF.
 * @param   rf_chn - channel of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_brust_setup_ramp(RF_ModeTypeDef rf_mode,RF_PowerTypeDef power_level,signed char rf_chn,unsigned char pkt_type);




/**
 * @brief   This function serves to send packets in the burst mode with PA ramp up/down.
 * @param   rf_mode - mode of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_burst_loop_ramp(RF_ModeTypeDef rf_mode,unsigned char pkt_type);

/**
 * @brief   This function serves to send packets in the burst mode
 * @param   rf_mode - mode of RF.
 * @param 	pkt_type - The type of data sent
 * 					   0:random data
 * 					   1:0xf0
 * 					   2:0x55
 * @return  none.
 */
void rf_emi_tx_burst_loop(RF_ModeTypeDef rf_mode,unsigned char pkt_type);

/**
 * @brief   This function serves to set the channel in singletone mode.
 * @param   chn - channel of RF.
 * @return  none.
 */
void rf_set_channel_singletone (signed char chn);



#endif /* EMI_H_ */
