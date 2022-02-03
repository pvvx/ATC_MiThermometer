/********************************************************************************************************
 * @file     rf_drv.h 
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
#ifndef _RF_DRV_H_
#define _RF_DRV_H_

#include "bsp.h"
#include "compiler.h"
#include "gpio_8258.h"
#define RF_CHN_TABLE 		0x8000



/**
 *  @brief  Define RF mode
 */
typedef enum {
	RF_MODE_BLE_2M =    		BIT(0),
	RF_MODE_BLE_1M = 			BIT(1),
    RF_MODE_BLE_1M_NO_PN   =    BIT(2),
	RF_MODE_ZIGBEE_250K    =    BIT(3),
    RF_MODE_LR_S2_500K     =    BIT(4),
    RF_MODE_LR_S8_125K     =    BIT(5),
    RF_MODE_PRIVATE_250K   =    BIT(6),
    RF_MODE_PRIVATE_500K   =    BIT(7),
    RF_MODE_PRIVATE_1M     =    BIT(8),
    RF_MODE_PRIVATE_2M     =    BIT(9),
    RF_MODE_ANT     	   =    BIT(10),
    RF_MODE_BLE_2M_NO_PN   =    BIT(11),
} RF_ModeTypeDef;


/**
 *  @brief  Define RF Tx/Rx/Auto mode
 */

typedef enum {
    RF_MODE_TX = 0,
    RF_MODE_RX = 1,
    RF_MODE_AUTO=2
} RF_StatusTypeDef;




/**
 *  @brief  Define RX pin of RF for RFFE
 */
typedef enum {
	RFFE_RX_PB2 = GPIO_PB2,
    RFFE_RX_PC6 = GPIO_PC6,
    RFFE_RX_PD0 = GPIO_PD0
} RF_LNARxPinDef;


/**
 *  @brief  Define TX pin of RF for RFFE
 */
typedef enum {
	RFFE_TX_PB3 = GPIO_PB3,
    RFFE_TX_PC7 = GPIO_PC7,
    RFFE_TX_PD1 = GPIO_PD1
} RF_PATxPinDef;

/**
 *  @brief  Define power list of RF
 */
typedef enum {
	 /*VBAT*/
	 RF_POWER_P10p46dBm = 63, //  10.46 dbm
	 RF_POWER_P10p29dBm = 61, //  10.29 dbm
	 RF_POWER_P10p01dBm = 58, //  10.01 dbm
	 RF_POWER_P9p81dBm  = 56, //   9.81 dbm
	 RF_POWER_P9p48dBm  = 53, //   9.48 dbm
	 RF_POWER_P9p24dBm  = 51, //   9.24 dbm
	 RF_POWER_P8p97dBm  = 49, //   8.97 dbm
	 RF_POWER_P8p73dBm  = 47, //   8.73 dbm
	 RF_POWER_P8p44dBm  = 45, //   8.44 dbm
	 RF_POWER_P8p13dBm  = 43, //   8.13 dbm
	 RF_POWER_P7p79dBm  = 41, //   7.79 dbm
	 RF_POWER_P7p41dBm  = 39, //   7.41 dbm
	 RF_POWER_P7p02dBm  = 37, //   7.02 dbm
	 RF_POWER_P6p60dBm  = 35, //   6.60 dbm
	 RF_POWER_P6p14dBm  = 33, //   6.14 dbm
	 RF_POWER_P5p65dBm  = 31, //   5.65 dbm
	 RF_POWER_P5p13dBm  = 29, //   5.13 dbm
	 RF_POWER_P4p57dBm  = 27, //   4.57 dbm
	 RF_POWER_P3p94dBm  = 25, //   3.94 dbm
	 RF_POWER_P3p23dBm  = 23, //   3.23 dbm
	 /*VANT*/
	 RF_POWER_P3p01dBm  = BIT(7) | 63,   //   3.01 dbm
	 RF_POWER_P2p81dBm  = BIT(7) | 61,   //   2.81 dbm
	 RF_POWER_P2p61dBm  = BIT(7) | 59,   //   2.61 dbm
	 RF_POWER_P2p39dBm  = BIT(7) | 57,   //   2.39 dbm
	 RF_POWER_P1p99dBm  = BIT(7) | 54,   //   1.99 dbm
	 RF_POWER_P1p73dBm  = BIT(7) | 52,   //   1.73 dbm
	 RF_POWER_P1p45dBm  = BIT(7) | 50,   //   1.45 dbm
	 RF_POWER_P1p17dBm  = BIT(7) | 48,   //   1.17 dbm
	 RF_POWER_P0p90dBm  = BIT(7) | 46,   //   0.90 dbm
	 RF_POWER_P0p58dBm  = BIT(7) | 44,   //   0.58 dbm
	 RF_POWER_P0p04dBm  = BIT(7) | 41,   //   0.04 dbm
	 RF_POWER_N0p14dBm  = BIT(7) | 40,   //  -0.14 dbm
	 RF_POWER_N0p97dBm  = BIT(7) | 36,   //  -0.97 dbm
	 RF_POWER_N1p42dBm  = BIT(7) | 34,   //  -1.42 dbm
	 RF_POWER_N1p89dBm  = BIT(7) | 32,   //  -1.89 dbm
	 RF_POWER_N2p48dBm  = BIT(7) | 30,   //  -2.48 dbm
	 RF_POWER_N3p03dBm  = BIT(7) | 28,   //  -3.03 dbm
	 RF_POWER_N3p61dBm  = BIT(7) | 26,   //  -3.61 dbm
	 RF_POWER_N4p26dBm  = BIT(7) | 24,   //  -4.26 dbm
	 RF_POWER_N5p03dBm  = BIT(7) | 22,   //  -5.03 dbm
	 RF_POWER_N5p81dBm  = BIT(7) | 20,   //  -5.81 dbm
	 RF_POWER_N6p67dBm  = BIT(7) | 18,   //  -6.67 dbm
	 RF_POWER_N7p65dBm  = BIT(7) | 16,   //  -7.65 dbm
	 RF_POWER_N8p65dBm  = BIT(7) | 14,   //  -8.65 dbm
	 RF_POWER_N9p89dBm  = BIT(7) | 12,   //  -9.89 dbm
	 RF_POWER_N11p4dBm  = BIT(7) | 10,   //  -11.4 dbm
	 RF_POWER_N13p29dBm = BIT(7) | 8,   //  -13.29 dbm
	 RF_POWER_N15p88dBm = BIT(7) | 6,   //  -15.88 dbm
	 RF_POWER_N19p27dBm = BIT(7) | 4,   //  -19.27 dbm
	 RF_POWER_N25p18dBm = BIT(7) | 2,   //  -25.18 dbm

	 RF_POWER_N30dBm    = 0xff,         //  -30 dbm
	 RF_POWER_N50dBm    = BIT(7) | 0,   //  -50 dbm

} RF_PowerTypeDef;

extern const RF_PowerTypeDef rf_power_Level_list[60];

#ifdef		RF_MODE_250K
#define		RF_FAST_MODE_2M		0
#define		RF_FAST_MODE_1M		0
#endif

#ifndef		RF_FAST_MODE_1M
#define		RF_FAST_MODE_1M		1
#endif

#ifndef		RF_FAST_MODE_2M
#define		RF_FAST_MODE_2M		0
#endif

#ifndef		RF_LONG_PACKET_EN
#define		RF_LONG_PACKET_EN	0
#endif

#if			RF_FAST_MODE_2M
	#if			RF_LONG_PACKET_EN
		#define		RF_PACKET_LENGTH_OK(p)		(p[0] == p[12]+13)
		#define		RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x40)
	#else
		#define		RF_PACKET_LENGTH_OK(p)		(p[0] == (p[12]&0x3f)+15)
		#define		RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x40)
	#endif
#elif		RF_FAST_MODE_1M
	#define		RF_BLE_PACKET_LENGTH_OK(p)				( *((unsigned int*)p) == p[5]+13)    			//dma_len must 4 byte aligned
	#define		RF_BLE_PACKET_CRC_OK(p)					((p[*((unsigned int*)p) + 3] & 0x01) == 0x0)

	#if (1) // support RF RX/TX MAX data Length: 251byte
		#define		RF_PACKET_LENGTH_OK(p)				(MAKE_U16(p[1], p[0]) == p[5]+13)
		#define		RF_PACKET_CRC_OK(p)					((p[MAKE_U16(p[1], p[0])+3] & 0x01) == 0x0)
	#else
		#define		RF_PACKET_LENGTH_OK(p)				(p[0] == p[5]+13)
		#define		RF_PACKET_CRC_OK(p)					((p[p[0]+3] & 0x01) == 0x0)
	#endif
#else
#define		RF_PACKET_LENGTH_OK(p)		(p[0] == p[12]+13)
#define		RF_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x10)
#endif

#define		RF_PACKET_1M_LENGTH_OK(p)		(p[0] == p[5]+13)
#define		RF_PACKET_2M_LENGTH_OK(p)		(p[0] == (p[12]&0x3f)+15)


#if (RF_FAST_MODE_2M)
	#define			RF_FAST_MODE			1
	#define			RF_TRX_MODE				0x80
	#define			RF_TRX_OFF				0x44		//f02
#elif (RF_FAST_MODE_1M)
	#define			RF_FAST_MODE			1
	#define			RF_TRX_MODE				0x80
	#define			RF_TRX_OFF				0x45		//f02
#else
	#define			RF_FAST_MODE			0
	#define			RF_TRX_MODE				0xe0
	#define			RF_TRX_OFF				0x45		//f02
#endif



#define    RF_ZIGBEE_PACKET_LENGTH_OK(p)    			(p[0]  == p[4]+9)
#define    RF_ZIGBEE_PACKET_CRC_OK(p)       			((p[p[0]+3] & 0x51) == 0x10)
#define    RF_ZIGBEE_PACKET_RSSI_GET(p)     			(p[p[0]+2])
#define    RF_ZIGBEE_PACKET_TIMESTAMP_GET(p)           	(p[p[0]-4] | (p[p[0]-3]<<8) | (p[p[0]-2]<<16) | (p[p[0]-1]<<24))
#define    RF_ZIGBEE_PACKET_PAYLOAD_LENGTH_GET(p)      	(p[4])
#define    RF_NRF_ESB_PACKET_LENGTH_OK(p)              	(p[0] == (p[4] & 0x3f) + 11)
#define    RF_NRF_ESB_PACKET_CRC_OK(p)                 	((p[p[0]+3] & 0x01) == 0x00)
#define    RF_NRF_ESB_PACKET_RSSI_GET(p)               	(p[p[0]+2])
#define    RF_NRF_SB_PACKET_PAYLOAD_LENGTH_GET(p)      	(p[0] - 10)
#define    RF_NRF_SB_PACKET_CRC_OK(p)                  	((p[p[0]+3] & 0x01) == 0x00)
#define    RF_NRF_SB_PACKET_CRC_GET(p)                 	((p[p[0]-8]<<8) + p[p[0]-7]) //Note: here assume that the MSByte of CRC is received first
#define    RF_NRF_SB_PACKET_RSSI_GET(p)                	(p[p[0]+2])
#define    RF_NRF_ESB_PACKET_TIMESTAMP_GET(p)          (p[p[0]-4] | (p[p[0]-3]<<8) | (p[p[0]-2]<<16) | (p[p[0]-1]<<24))
#define    RF_NRF_SB_PACKET_TIMESTAMP_GET(p)           (p[p[0]-4] | (p[p[0]-3]<<8) | (p[p[0]-2]<<16) | (p[p[0]-1]<<24))




/*******************************            General APIs            **************************/

/**
*	@brief     This function serves to reset RF BaseBand
*	@param[in] none.
*	@return	   none.
*/
static inline void reset_baseband(void)
{
	REG_ADDR8(0x61) = BIT(0);		//reset_baseband
	REG_ADDR8(0x61) = 0;			//release reset signal
}

/**
*	@brief     This function serves to initiate the mode of RF
*	@param[in] rf_mode  -  mode of RF
*	@return	   none.
*/

extern void rf_drv_init (RF_ModeTypeDef rf_mode);

/**
 * @brief   	This function serves to set RF power level index.
 * @param[in]   RF_PowerTypeDef - the RF power types.
 * @return  	none.
 */
extern void rf_set_power_level_index (RF_PowerTypeDef level);

/**
 * @brief   	This function serves to set RF power level index.
 * @param[in]  	level_idx - the RF power index.
 * @return  	none.
 */
extern void rf_set_power_level_index_zgb (int level_idx);
/**
 * @brief   	This function serves to set RF's channel.
 * @param[in]   chn - RF channel.
 * @param[in]   set - the value to set.
 * @return  	none.
 */
extern void rf_set_channel (signed char chn, unsigned short set);


/**
*	@brief	 This function is to update TP(two point),this value will affect
*			 RF performance
*	@param[in]	tp0  	    Tp value for lower frequency
*							If you set a value outside the range, you will be set to fail.
*	@param[in]	tp1  	    Tp value for higher frequency
*							If you set a value outside the range, you will be set to fail.
*	@return	 	0 :set success;
*              -1 :set failed
*/

extern void rf_update_tp_value (unsigned char tp0, unsigned char tp1);

/**
*	@brief	  	this function is to Set byte length for access_code.
*
*	@param[in]	len  	Optional range: 3~5
*										Note: The effect for 3-byte access_code is not good.
*
*	@return	 	none
*/
extern void rf_set_acc_code_len(unsigned char len);

/**
*	@brief	  	this function performs to get access_code length.
*
*	@param[in]	len  	Optional range: 3~5
*										Note: The effect for 3-byte access_code is not good.
*
*	@return	 	access_byte_num[2:0]
*/
static inline unsigned char rf_acc_len_get(unsigned char len)
{
    return (read_reg8(0x405) & 0x07);
}
/**
 * @brief   This function serves to set RF access command.
 * @param[in]  acc - the command.
 * @return  none.
 */
static inline void rf_access_code_comm (unsigned int acc)
{
	write_reg32 (0x800408, acc);
	//notice: This state will be reset after reset baseband
	write_reg8 (0x800405, read_reg8(0x405)|0x80);//setting acess code needs to writ 0x405 access code trigger bit 1 to enable in long range mode,,and the mode of  BLE1M and BLE2M need not.
}

/**
 * @brief   This function serves to set RF access command.
 * @param[in]   acc - the command.
 * @return  none.
 */
static inline void rf_longrange_access_code_comm (unsigned int acc)
{
	write_reg32 (0x800408, acc);
	write_reg8 (0x800405, read_reg8(0x405)|0x80);//setting acess code needs to writ 0x405 access code trigger bit to enable in long range mode,,and the mode of  BLE1M and BLE2M need not.
}

/**
*	@brief	  	this function is to set access code.
*	@param[in]	pipe  	index number for access_code channel.
*	@param[in]	addr    the access code address.
*	@return	 	none
*/
extern void rf_set_acc_code_pipe(unsigned char pipe, const unsigned char *addr);

/**
*	@brief	  	this function is to get access code.
*	@param[in]	pipe  	Set index number for access_code channel.
*	@param[in]	addr  	the access of the code address.
*	@return	 	none
*/
extern void rf_get_acc_code_pipe(unsigned char pipe, unsigned char *addr);

/**
*	@brief		this function is to enable/disable each access_code channel for
*				RF Rx terminal.
*	@param[in]	pipe  	Bit0~bit5 correspond to channel 0~5, respectively.
*						0£ºDisable 1£ºEnable
*						If ¡°enable¡± is set as 0x3f (i.e. 00111111),
*						all access_code channels (0~5) are enabled.
*	@return	 	none
*/
static inline void rf_rx_acc_code_enable(unsigned char pipe)
{
    write_reg8(0x407, (read_reg8(0x407)&0xc0) | pipe); //rx_access_code_chn_en
}

/**
*	@brief		this function is to select access_code channel for RF Rx terminal.
*	@param[in]	pipe  	Bit0~bit5 correspond to channel 0~5, respectively.
*						0£ºDisable 1£ºEnable
*						If ¡°enable¡± is set as 0x3f (i.e. 00111111),
*						all access_code channels (0~5) are enabled.
*	@return	 	none
*/
static inline void rf_tx_acc_code_select(unsigned char pipe)
{
    write_reg8(0xf15, (read_reg8(0xf15)&0xf8) | pipe); //Tx_Channel_man[2:0]
}


/**
 * @brief   This function serves to reset RF Tx/Rx mode.
 * @param   none.
 * @return  none.
 */

static inline void rf_set_tx_rx_off(void)
{
	write_reg8 (0x800f16, 0x29);
	write_reg8 (0x800428, RF_TRX_MODE);	// rx disable
	write_reg8 (0x800f02, RF_TRX_OFF);	// reset tx/rx state machine
}
/**
 * @brief   This function serves to turn off RF auto mode.
 * @param   none.
 * @return  none.
 */
static inline void rf_set_tx_rx_off_auto_mode(void)
{
	write_reg8 (0xf00, 0x80);
}
/**
*	@brief	  	This function serves to judge RF Tx/Rx state.
*	@param[in]	rf_status - Tx/Rx status.
*	@param[in]	rf_channel - RF channel.
*	@return	 	failed -1,else success.
*/
extern int rf_set_trx_state(RF_StatusTypeDef rf_status, signed char rf_channel);

/**
*	@brief	  	This function serves to get RF status.
*	@param[in]	none.
*	@return	 	RF Rx/Tx status.
*/
extern RF_StatusTypeDef rf_get_trx_state(void);

/**
 * @brief   This function serves to set RF Tx mode.
 * @param   none.
 * @return  none.
 */
static inline void rf_set_txmode (void)
{
	write_reg8 (0x800f02, RF_TRX_OFF | BIT(4));	//TX enable
}

/**
 * @brief   This function serves to settle adjust for RF Tx.
 * @param   txstl_us - adjust TX settle time.
 * @return  none
 */
static inline void 	tx_settle_adjust(unsigned short txstl_us)
{
	REG_ADDR16(0xf04) = txstl_us;
}

/**
 * @brief   This function serves to set pipe for RF Tx.
 * @param   pipe - RF Optional range .
 * @return  none
 */
static inline void rf_set_tx_pipe (unsigned char pipe)
{
	write_reg8 (0x800f15, 0xf0 | pipe);
}
/**
*	@brief	  	This function serves to set RF Tx packet.
*	@param[in]	rf_txaddr - the address RF to send packet.
*	@return	 	none.
*/
extern void rf_tx_pkt(unsigned char * addr);
/**
 * @brief   This function serves to send packet for RF.
 * @param   addr - the address need to be send.
 * @return  none.
 */
extern void	rf_tx_pkt_auto (void* addr);

/**
*	@brief	  	This function serves to determine whether sending a packet of data is finished
*	@param[in]	none.
*	@return	 	Yes: 1, NO: 0.
*/
static inline unsigned char rf_tx_finish(void)
{
    return ((read_reg8(0xf20) & BIT(1))==0x02);
}

/**
*	@brief	  	This function serves to clear the Tx finish flag bit.
*				After all packet data are sent, corresponding Tx finish flag bit
*				will be set as 1.By reading this flag bit, it can check whether
*				packet transmission is finished. After the check, it¡¯s needed to
*				manually clear this flag bit so as to avoid misjudgment.
*   @param      none
*	@return	 	none
*/
static inline void rf_tx_finish_clear_flag(void)
{
    write_reg8(0xf20, read_reg8(0xf20) | 0x02);
}



/**
*	@brief	  	This function serves to determine whether sending a packet of rf_tx_acc_code_select is finished
*	@param[in]	none.
*	@return	 	Yes: 1, NO: 0.
*/

static inline unsigned char rf_is_rx_finish(void)
{
    return (unsigned char)((read_reg8(0xf20) & BIT(0))==0x01);
}

/**
*	@brief	  	This function is to set rx buffer
*
*	@param[out]	RF_RxAddr  	Pointer for Rx buffer in RAM(Generally it¡¯s starting
*							address of an array.Should be 4-byte aligned)
*	@param[in]	size   		Rx buffer size (It¡¯s an integral multiple of 16)
*	@param[in]	PingpongEn 	Enable/Disable Ping-Pong buffer 1£ºEnable 0£ºDisable
*							Note:
*							When ¡°PingpongEn¡± is set as 0, received RF data will
*							be stored in RAM pointed by ¡° RF_RxAddr¡±.
*							When ¡°PingpongEn¡± is set as 1, received RF data will
*							be stored in buffer0 and buffer1 successively.
*							The RAM size reserved for received RF data should be
*							double of ¡°Size¡±.
*
*	@return	 	none
*/

extern void  rf_rx_buffer_set(unsigned char *  RF_RxAddr, int size, unsigned char  PingpongEn);

/**
*	@brief	  	This function serves to  reconfigure the buffer
*	@param[out]	RF_RxAddr  	Pointer for Rx buffer in RAM(Generally it¡¯s starting
*							address of an array.Should be 4-byte aligned)
*	@return	 	Yes: 1, NO: 0.
*/

static inline void rf_rx_buffer_reconfig(unsigned short RF_RxAddr)
{
	reg_dma2_addr = (unsigned short)(RF_RxAddr);
}

/**
*	@brief	  	This function serves to determine whether a packet of data received is right
*	@param[in]	none.
*	@return	 	Yes: 1, NO: 0.
*/
static inline unsigned char rf_is_rx_right(void)
{
    return ((REG_ADDR8(0x44f)&0x0f)==0);
}

/**
*	@brief	  	This function serves to clear the Rx finish flag bit.
*				After all packet data are resive, corresponding Rx finish flag bit
*				will be set as 1.By reading this flag bit, it can check whether
*				packet transmission is finished. After the check, it¡¯s needed to
*				manually clear this flag bit so as to avoid misjudgment.
*   @param      none
*	@return	 	none
*/
static inline void rf_rx_finish_clear_flag(void)
{
    write_reg8(0xf20, read_reg8(0xf20) | 0x01);
}

/**
 * @brief   This function serves to set RF Tx mode.
 * @param   none.
 * @return  none.
 */
static inline void rf_set_rxmode (void)
{
    write_reg8 (0x800428, RF_TRX_MODE | BIT(0));	//rx enable
    write_reg8 (0x800f02, RF_TRX_OFF | BIT(5));	// RX enable
}

/**
*	@brief	  	This function serves to start Tx of ble_mode.
*	@param[in]	addr   Tx packet address in RAM. Should be 4-byte aligned.
*	@param[in]	tick  Tick value of system timer. It determines when to
*						  	  start ble mode and send packet.
*	@return	 	none
*/
extern void rf_start_btx (void* addr, unsigned int tick);

/**
*	@brief	  	This function serves to start Rx of auto mode. In this mode,
*				RF module stays in Rx status until a packet is received or it fails to receive packet when timeout expires.
*				Timeout duration is set by the parameter "tick".
*				The address to store received data is set by the function ¡°addr¡±.
*	@param[in]	addr - The address to store received data.
*	@param[in]	tick - Unit is us. It indicates timeout duration in Rx status.Max value: 0xffffff (16777215)
*	@return	 	none
*/
extern void rf_start_brx  (void* addr, unsigned int tick);

/**
*	@brief	  	This function serves to start Tx.
*	@param[in]	addr   Tx packet address in RAM. Should be 4-byte aligned.
*	@param[in]	tick  Tick value of system timer.
*	@return	 	none
*/

extern void rf_start_stx  (void* addr, unsigned int tick);

/**
*	@brief	  	This function serves to start Rx.
*	@param[in]	tick  Tick value of system timer.
*	@return	 	none
*/
extern void rf_start_srx  (unsigned int tick);


/**
*	@brief	  	This function serves to start stx2rx mode of auto_mode.
*				In this mode, a packet is sent first,RF module waits for 10us,
*				stays in Rx status until data is received or timeout expires,
*				then exits this mode.Timeout duration is set by the parameter
*				¡°timeout_us¡±.The address to store received data is set by the
*				function ¡°RF_RxBufferSet¡±.
*
*	@param[in]	addr  Tx packet address in RAM. Should be 4-byte aligned.
*	@param[in]	tick   	Tick value of system timer. It determines when
*								to start StxToRx mode and send packet.
*	@param[in]	timeout_us  Unit is us. It indicates timeout duration in
*							 	Rx status.Max value: 0xfff (4095)
*
*	@return	 	none
*/


extern void rf_start_stx2rx  (void* addr, unsigned int tick);



/**
*	@brief	  	This function serves to start srx2tx mode of auto_mode.
*				In this mode,RF module stays in Rx status until a packet is
*				received or it fails to receive packetwhen timeout expires.
*				If a packet is received within the timeout duration, RF module
*				will wait for 10us,send a packet, and then exit this mode.
*				If it fails to receive packet when timeout expires, RF module
*				will directly exit this mode.Timeout duration is set by the
*				parameter "timeout_us".	The address to store received data is set
*				by the function ¡°RF_RxBufferSet¡±.
*
*	@param[in]	addr 	Tx packet address in RAM. Should be 4-byte aligned.
*	@param[in]	tick   Tick value of system timer. It determines when to
*								start SrxToTx mode.
*	@param[in]	timeout_us  Unit is us. It indicates timeout duration in Rx status.
*								Max value: 0xffffff (16777215)
*
*	@return	 	none
*/

extern void rf_start_srx2tx  (void* addr, unsigned int tick);


/*******************************            Private APIs            **************************/

/**
 * @brief   This function serves to set the ble channel.
 * @param   chn_num - channel numbers.
 * @return  none.
 */
void rf_set_ble_channel (signed char chn_num);

/**
 * @brief     This function performs to enable RF Tx.
 * @param[in] none.
 * @return    none.
 */
static inline void rf_ble_tx_on ()
{
	write_reg8  (0x800f02, 0x45 | BIT(4));	// TX enable
}

/**
 * @brief     This function performs to done RF Tx.
 * @param[in] none.
 * @return    none.
 */
static inline void rf_ble_tx_done ()
{
	write_reg8  (0x800f02, 0x45);
}

/**
 * @brief   This function serves to reset function for RF.
 * @param   none
 * @return  none
 */
static inline void reset_sn_nesn(void)
{
	REG_ADDR8(0xf01) =  0x01;
}

/**
 * @brief   This function serves to reset the RF sn.
 * @param   none.
 * @return  none.
 */
static inline void rf_reset_sn (void)
{
	write_reg8  (0x800f01, 0x3f);
	write_reg8  (0x800f01, 0x00);
}

/**
 * @brief   This function serves to set pipe for RF Tx.
 * @param   p - RF Optional range .
 * @return  none
 */
static inline void rf_set_ble_crc (unsigned char *p)
{
	write_reg32 (0x800424, p[0] | (p[1]<<8) | (p[2]<<16));
}

/**
 * @brief   This function serves to set CRC value for RF.
 * @param[in]  crc - CRC value.
 * @return  none.
 */
static inline void rf_set_ble_crc_value (unsigned int crc)
{
	write_reg32 (0x800424, crc);
}

/**
 * @brief   This function serves to set CRC advantage.
 * @param   none.
 * @return  none.
 */
static inline void rf_set_ble_crc_adv ()
{
	write_reg32 (0x800424, 0x555555);
}

/**
 * @brief   This function serves to set RF access code.
 * @param[in]   p - the address to access.
 * @return  none
 */
static inline void rf_set_ble_access_code (unsigned char *p)
{
	write_reg32 (0x800408, p[3] | (p[2]<<8) | (p[1]<<16) | (p[0]<<24));
}

/**
 * @brief   This function serves to set RF access code value.
 * @param[in]   ac - the address value.
 * @return  none
 */
static inline void rf_set_ble_access_code_value (unsigned int ac)
{
	write_reg32 (0x800408, ac);
}

/**
 * @brief   This function serves to set RF access code advantage.
 * @param   none.
 * @return  none.
 */
static inline void rf_set_ble_access_code_adv (void)
{
	write_reg32 (0x800408, 0xd6be898e);
}

/**
 * @brief   This function serves to set RF access code 6bit to 32bit.
 * @param[in]  code - the access code.
 * @return  the value of the access code.
 */
static inline unsigned int rf_access_code_16to32 (unsigned short code)
{
	unsigned int r = 0;
	for (int i=0; i<16; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}
	return r;
}

/**
 * @brief   This function serves to set RF access code 6bit to 32bit.
 * @param[in]   code - the access code.
 * @return  the value of access code.
 */
static inline unsigned short rf_access_code_32to16 (unsigned int code)
{
	unsigned short r = 0;
	for (int i=0; i<16; i++) {
		r = r << 1;

		r |= (code & BIT(i*2)) ? 1 : 0;

	}
	return r;
}


/**
 * @brief   This function serves to judge the statue of  RF receive.
 * @param   none.
 * @return  none.
 */
static inline unsigned char is_rf_receiving_pkt(void)
{
	//if the value of [5:4] of the reg_0x448 is 0b10 or 0b11, it means that the RF is in the receiving packet phase.(confirmed by junwen)
	return (((read_reg8(0x448)>>5)& 1) == 1);
}
/**
 * @brief   This function serves to set the ble channel.
 * @param[in]   chn_num - channel numbers.
 * @return  none.
 */
extern void rf_set_channel (signed char chn, unsigned short set);//general
/**
 * @brief   This function serves to set The channel .
 * @param[in]   RF_PowerTypeDef - the RF power types.
 * @return  none.
 */
extern void rf_set_channel_500k(signed short chn, unsigned short set);
/**
*	@brief		this function is to set shock burst for RF.
*	@param[in]	len - length of shockburst.
*	@return	 	none.
*/
static inline void rf_nordic_shockburst(int len)
{
    write_reg8(0x404, read_reg8(0x404)|0x03); //select shockburst header mode
    write_reg8(0x406, len);
}


/**
 * @brief      This function
 * @param[in]  size - the expected size of dma2
 * @param[in]  PingpongEn - the flag of g_RFRxPingpongEn
 * @return     none
 */
extern void  rf_rx_cfg(int size, unsigned char  PingpongEn);

/**
*	@brief	  	This function serves to get CRC
*   @param[in]  *input  		- the content of payload
*   @param[in]	len 			- the length of payload
*   @param[in]  init_val 		- the initial value of CRC
*	@return	 	init_val 		- CRC
*/
extern unsigned short crc16_ccitt_cal(unsigned char *input, unsigned int len, unsigned short init_val);

/**
*	@brief	  	This function serves to simulate 100k Tx by 500k Tx
*   @param[in]  *preamble  		- the content of preamble
*   @param[in]	preamble_len 	- the length of preamble
*   @param[in]  *acc_code 		- the content of access code
*   @param[in]  acc_len			- the length of access code
*   @param[in]  *payload		- the content of payload
*   @param[in]	pld_len			- the length of payload
*   @param[in]	*tx_buf			- the data need to be sent
*   @param[in]	crc_init		- the initial value of CRC
*	@return	 	none
*/

extern void rf_tx_500k_simulate_100k(unsigned char *preamble, unsigned char preamble_len,
                              unsigned char *acc_code, unsigned char acc_len,
                              unsigned char *payload, unsigned char pld_len,
                              unsigned char *tx_buf, unsigned short crc_init);

/**
*	@brief	  	This function is to start energy detect of the current channel for zigbee mode
*				Before using it.
*   @param      none
*	@return	 	none
*/

extern void rf_154_start_ed(void);

/**
*	@brief	  	This function is to stop energy detect and get energy detect value of
*				the current channel for zigbee mode.
*   @param      none
*	@return	 	rf_ed:0x00~0xff
*
*/

extern unsigned char rf_stop_ed_154(void);

/**
*	@brief	  	This function serves to set pin for RFFE of RF
*   @param[in]     tx_pin - select pin to send
*   @param[in]      rx_pin - select pin to receive
*	@return	 	none
*
*/
extern void rf_rffe_set_pin(RF_PATxPinDef tx_pin, RF_LNARxPinDef rx_pin);

/**
 * @brief      This function process the received packet in 1mbps shockburst mode only for hanshow for the
 *             compatiblity with third-party chips. The process includes data-whitening
               transformation and crc check.
 * @param[in]  rx_buf - the rf rx buffer containing the received packet(dma length+payload+3 byte crc)
 * @param[in]  len - the expected rx length of shockburst mode, containing payload and 3byte crc
 * @return     the status of the processing procesure. 1: the received packet is correct, 0: the received packet is incorrect
 */

unsigned char rx_packet_process_1mbps(unsigned char *rx_buf, unsigned int len);
/**
 * @brief      This function process the tx packet in 1mbps shockburst mode only for hanshow for the
 *             compatiblity with third-party chips. The process includes data-whitening
               transformation and crc padding.
 * @param[in]  tx_buf - the rf tx buffer containing the tx packet(dma length+payload+3 byte crc)
 * @param[in]  len - the expected tx length of shockburst mode, containing payload and 3byte crc
 * @return     none
 */
void tx_packet_process_1mbps(unsigned char *tx_buf, unsigned char *payload, unsigned int len);

#endif
