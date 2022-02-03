/********************************************************************************************************
 * @file     dma.h 
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

#ifndef DMA_H_
#define DMA_H_

#include "compiler.h"


#define DMA_RFRX_LEN_HW_INFO				0	// 826x: 8
#define DMA_RFRX_OFFSET_HEADER				4	// 826x: 12
#define DMA_RFRX_OFFSET_RFLEN				5   // 826x: 13
#define DMA_RFRX_OFFSET_DATA				6	// 826x: 14


#define DMA_RFRX_OFFSET_CRC24(p)			(p[DMA_RFRX_OFFSET_RFLEN]+6)  //data len:3
#define DMA_RFRX_OFFSET_TIME_STAMP(p)		(p[DMA_RFRX_OFFSET_RFLEN]+9)  //data len:4
#define DMA_RFRX_OFFSET_FREQ_OFFSET(p)		(p[DMA_RFRX_OFFSET_RFLEN]+13) //data len:2
#define DMA_RFRX_OFFSET_RSSI(p)				(p[DMA_RFRX_OFFSET_RFLEN]+15) //data len:1, signed



#define	RFDMA_RX_DISABLE		(reg_dma_chn_en &= (~FLD_DMA_CHN_RF_RX))
#define	RFDMA_RX_ENABLE			(reg_dma_chn_en |= (FLD_DMA_CHN_RF_RX))

#define	RFDMA_TX_DISABLE		(reg_dma_chn_en &= (~FLD_DMA_CHN_RF_TX))
#define	RFDMA_TX_ENABLE			(reg_dma_chn_en |= (FLD_DMA_CHN_RF_TX))


typedef enum{
	DMA0_UART_RX,
	DMA1_UART_TX,
	DMA2_RF_RX,
	DMA3_RF_TX,
	DMA4_AES_DECO,
	DMA5_AES_CODE,
	DMA6,
	DMA7_PWM,
}DMA_chn_Typdef; 

/**
 * @brief     This function resets the DMA module.
 * @param[in] none
 * @return    none
 */
static inline void dma_reset(void)
{
	reg_rst1 |= FLD_RST1_DMA;
	reg_rst1 &= (~FLD_RST1_DMA);
}
/**
 * @brief     This function performs to enable DMA.
 * @param[in] msk - variable to config.
 * @return    none.
 */
static inline void dma_irq_enable(unsigned int msk)
{
	reg_dma_chn_irq_msk |= msk;
}

/**
 * @brief     This function performs to enable DMA.
 * @param[in] msk - variable to config.
 * @return    none.
 */
static inline void dma_irq_disable(unsigned int msk)
{
	reg_dma_chn_irq_msk &= ~msk;
}

/**
 * @brief     This function performs to enable DMA chnnel.
 * @param[in] en - en: 1 enable. 0 disable.
 * @return    none.
 */

static inline void dma_chn_enable(unsigned char chn, unsigned int en)
{
	if(en){
		reg_dma_chn_en |= chn;
	}
	else{
		reg_dma_chn_en &= ~chn;
	}
}

/**
 * @brief     This function performs to enable DMA interrupt.
 * @param[in] chn - variable to config the DMA interrupt channel.
 * @param[in] en - en: 1 enable. 0 disable.
 * @return    none.
 */

static inline void dma_chn_irq_enable(unsigned char chn, unsigned int en)
{
	reg_dma_irq_status = chn;

	if(en){
		reg_dma_chn_en |= chn;
		reg_dma_chn_irq_msk |= chn;
	}
	else{
		reg_dma_chn_en &= ~chn;
		reg_dma_chn_irq_msk &= ~chn;
	}
}

/**
 * @brief      Clear IRQ status of uart.
 * @param[in]  irq_src - select tx or rx irq.
 * @return     none
 */

static inline void dma_chn_irq_status_clr(unsigned char irq_status)
{
	reg_dma_irq_status = irq_status;
}


/**
 * @brief      Get IRQ status of uart.
 * @param[in]  irq_src - select tx or rx irq.
 * @return     none
 */

static inline unsigned char dma_chn_irq_status_get(void)
{
    return reg_dma_irq_status;
}

/**
 * @brief      This function serves to set the size of dma buffer
 * @param[in]  size - select tx or rx irq. caution: max size = 2048
 * @return     none
 */
static inline void dma_set_buff_size(DMA_chn_Typdef chn,unsigned int size)
{
	reg_dma_size(chn) = (unsigned char)(size/16);
}

#endif /* DMA_H_ */
