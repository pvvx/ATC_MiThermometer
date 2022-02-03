/********************************************************************************************************
 * @file     s7816.c 
 *
 * @brief    This is the source file for TLSR8258
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
#include "s7816.h"
#include "uart.h"
#include "irq.h"
#include "dma.h"

/**
 * @brief      	This function is used to initiate 7816 module of MCU
 * @param[in]  	Div	-set the divider of clock of 7816 module:
 * 				7816clk = sysclk/(0x7b[6:4]*2),	0x7b[7]:enable 7816clk
 * 				7816clk:  0x40-2Mhz   0x20-4Mhz
 * 				baudrate: 0x40-16194  0x20-32388
 * @return     	none
 */
void s7816_set_clk(unsigned char Div)
{
	//---Set 7816 CLK divider
    //caution:7816 clk module only controlled by the highest bit of 7816 clk mode
	reg_7816_clk_div&=0x0f;
	reg_7816_clk_div|=(unsigned char)Div;
}

/**
 * @brief      	This function is used to set address and size of buffer 7816 module of MCU
 * @param[in]  	*RecvAddr		-set the address of buffer to receive data
 * @param[in]  	RecvBufLen		-set the length of buffer to receive data
 * @return     	none
 */
void s7816_set_rx_buf(unsigned short *RecvAddr, unsigned short RecvBufLen){
    unsigned char bufLen;
    unsigned int addr;
	//Set DMA begin address and the size of buffer needed

    addr = (unsigned int) RecvAddr;
    bufLen = RecvBufLen / 16;

    reg_dma0_addr = addr; 				//set receive buffer address(L,M)
    reg_dma0_addrHi = ((addr>>16)&0xff);//set receive buffer address(H)
    reg_dma0_size = bufLen; 			//set receive buffer size

    reg_dma0_mode = FLD_DMA_WR_MEM;
}

/**
 * @brief      	This function is used to initiate 7816 module of MCU
 * @param[in]  	Pin_7816_TRX	-select the I/O 	pin of 7816 module
 * @param[in]	Pin_7816_RST	-select the RST 	pin of 7816 module
 * @param[in]	Pin_7816_VCC	-select the VCC 	pin of 7816 module
 * @return     	none
 */
void s7816_set_pin(S7816_TRx_PinDef Pin_7816_TRX)
{
	//---enable UART 7816 CLK(GPIOA_GP1)
	write_reg8(0x586,read_reg8(0x586)&(~(BIT(1))));				//disable relative GPIO
	unsigned char tmp = read_reg8(0x5a8)| BIT(2);
	write_reg8(0x5a8,tmp&(~BIT(3)));	//enable 7816CLK
	//---enable UART 7816 TRX(Optional)
	switch(Pin_7816_TRX)
	{
		case S7816_TRX_D0://take care the details of every pin when using this pin
			gpio_setup_up_down_resistor(GPIO_PD0, PM_PIN_PULLUP_10K);
			gpio_set_func(GPIO_PD0,AS_UART);
			gpio_set_input_en(GPIO_PD0, 1);
			break;
		case S7816_TRX_D3:
			gpio_setup_up_down_resistor(GPIO_PD3, PM_PIN_PULLUP_10K);
			gpio_set_func(GPIO_PD3,AS_UART);
			gpio_set_input_en(GPIO_PD3, 1);
			break;
		case S7816_TRX_D7:
			gpio_setup_up_down_resistor(GPIO_PD7, PM_PIN_PULLUP_10K);
			gpio_set_func(GPIO_PD7,AS_UART);
			gpio_set_input_en(GPIO_PD7, 1);
			break;
		default:
			break;
	}
}

/**
 * @brief      	This function is used to send data to ID card,after succeeding in getting ATR
 * @param[in]  	*TransAddr	- data is waitting to send
 * @return     	none
 */
unsigned char s7816_dma_send(unsigned char *TransAddr)
{
    unsigned int addr;

	if(reg_uart_status1 & FLD_UART_TX_DONE) {//waiting for finishing an operation of sending

		//send DMA and buffer details
		addr = (unsigned int) TransAddr;

		reg_dma1_addr = addr; //set receive buffer address
		reg_dma1_addrHi = ((addr>>16)&0xff);
		reg_dma_tx_rdy0	 = FLD_DMA_CHN1;

		return 1;
	}
	return 0;
}

/**
 * @brief      	This function is used to transform half duplex mode of 7816
 * @param[in]  	mode	- half_duplex_mode_TX/RX is transformed by setting 0x9b[5]
 * 						0x9b[5]=1:half_duplex_mode_RX;0x9b[5]=0:half_duplex_mode_TX
 * @return     	none
 */
void s7816_set_half_duplex(S7816_Half_Duplex_ModeDef mode)
{
	//---------------------------------------------------------
	if(mode==S7816_RX){
		reg_uart_rx_timeout1|=FLD_UART_P7816_EN;
	}
	else if(mode==S7816_TX)
	{
		reg_uart_rx_timeout1&=~FLD_UART_P7816_EN;
	}
}
