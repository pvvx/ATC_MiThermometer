/********************************************************************************************************
 * @file     register_8258.h 
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
#pragma once

#include "bsp.h"

/********************************************************************************************
 *****|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|*****
 *****|								Digital  Register Table  						   |*****
 *****|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|*****
 ********************************************************************************************/

/*******************************      i2c registers: 0x00      ******************************/
#define reg_i2c_set				REG_ADDR32(0x00)
#define reg_i2c_speed			REG_ADDR8(0x00)
#define reg_i2c_id				REG_ADDR8(0x01)
enum{
	FLD_I2C_WRITE_READ_BIT  =  BIT(0),
	FLD_I2C_ID              =  BIT_RNG(1,7),
};

#define reg_i2c_status			REG_ADDR8(0x02)
enum{
	FLD_I2C_CMD_BUSY		= 	BIT(0),
	FLD_I2C_BUS_BUSY		= 	BIT(1),
	FLD_I2C_NAK				= 	BIT(2),
};

#define reg_i2c_mode			REG_ADDR8(0x03)
enum{
	FLD_I2C_ADDR_AUTO_ADD 		=	BIT(0),
	FLD_I2C_MASTER_EN			= 	BIT(1),      // 1: master, 0: slave
	FLD_I2C_SLAVE_MAPPING 		=	BIT(2),      // write i2c data to predefined memory address which set by other register
	FLD_I2C_HOLD_MASTER     	=   BIT(3),
};

#define reg_i2c_adr_dat			REG_ADDR16(0x04)
#define reg_i2c_dat_ctrl		REG_ADDR32(0x04)
#define reg_i2c_di_ctrl			REG_ADDR16(0x06)
#define reg_i2c_adr				REG_ADDR8(0x04)
#define reg_i2c_do				REG_ADDR8(0x05)
#define reg_i2c_di				REG_ADDR8(0x06)
#define reg_i2c_ctrl			REG_ADDR8(0x07)
enum{
	FLD_I2C_CMD_ID			= 		BIT(0),
	FLD_I2C_CMD_ADDR		= 		BIT(1),
	FLD_I2C_CMD_DO			= 		BIT(2),
	FLD_I2C_CMD_DI			= 		BIT(3),
	FLD_I2C_CMD_START		= 		BIT(4),
	FLD_I2C_CMD_STOP		= 		BIT(5),
	FLD_I2C_CMD_READ_ID		= 		BIT(6),
	FLD_I2C_CMD_ACK			= 		BIT(7),
};
/*******************************  i2c address map registers: 0xe0      ******************************/
#define reg_i2c_map_hadr		 	 REG_ADDR8(0xe0)

#define reg_i2c_slave_map_addrl      REG_ADDR8(0xe1)
#define reg_i2c_slave_map_addrm      REG_ADDR8(0xe2)
#define reg_i2c_slave_map_addrh      REG_ADDR8(0xe3)

#define reg_i2c_slave_irq_status 	 REG_ADDR8(0xe4)
#define reg_spi_slave_irq_status 	 REG_ADDR8(0xe4)
#define reg_i2c_map_host_status      REG_ADDR8(0xe4)
enum{
	FLD_HOST_CMD_IRQ  = 	BIT(0),  FLD_SLAVE_SPI_IRQ = FLD_HOST_CMD_IRQ,  //both host write & read trigger this status
	FLD_HOST_READ_IRQ = 	BIT(1),                                        //only host read trigger this status
};

#define reg_i2c_map_read0		     REG_ADDR8(0xe5)
#define reg_i2c_map_read1		     REG_ADDR8(0xe6)
#define reg_i2c_map_read2		     REG_ADDR8(0xe7)

/*******************************      spi registers: 0x08      ******************************/

#define reg_spi_data			REG_ADDR8(0x08)
#define reg_spi_ctrl			REG_ADDR8(0x09)
enum{
	FLD_SPI_CS 					= BIT(0),
	FLD_SPI_MASTER_MODE_EN 		= BIT(1),
	FLD_SPI_DATA_OUT_DIS 		= BIT(2),
	FLD_SPI_RD			    	= BIT(3),
	FLD_SPI_ADDR_AUTO_ADD  	 	= BIT(4),
	FLD_SPI_SHARE_MODE      	= BIT(5),
	FLD_SPI_BUSY         		= BIT(6),    // diff from doc,  bit 6 working
};
#define reg_spi_sp				REG_ADDR8(0x0a)
enum{
	FLD_MSPI_CLK = 		BIT_RNG(0,6),
	FLD_SPI_ENABLE =			BIT(7),
};

#define reg_spi_inv_clk			REG_ADDR8(0x0b)   //spi supports four modes
enum {
    FLD_SPI_MODE_WORK_MODE = BIT_RNG(0,1),
};

/*******************************      mspi registers: 0x0c      ******************************/

#define reg_mspi_data		REG_ADDR8(0x0c)
#define reg_mspi_ctrl		REG_ADDR8(0x0d)

enum{
	FLD_MSPI_CS 	= 		BIT(0),
	FLD_MSPI_SDO 	= 		BIT(1),
	FLD_MSPI_CONT 	= 		BIT(2),
	FLD_MSPI_RD 	= 		BIT(3),
	FLD_MSPI_BUSY 	= 		BIT(4),
};

#define reg_mspi_mode      		REG_ADDR8(0x0f)
enum
{
	FLD_MSPI_DUAL_DATA_MODE_EN  = BIT(0),
	FLD_MSPI_DUAL_ADDR_MODE_EN  = BIT(1),
	FLD_MSPI_CLK_DIV     	 	= BIT_RNG(2,7),
};

/*******************************      reset registers: 0x60      ******************************/

#define reg_rst0				REG_ADDR8(0x60)
enum{
	FLD_RST0_SPI = 				BIT(0),
	FLD_RST0_I2C = 				BIT(1),
	FLD_RST0_UART = 			BIT(2),
	FLD_RST0_USB = 				BIT(3),
	FLD_RST0_PWM = 				BIT(4),
	FLD_RST0_QDEC =				BIT(5),
	FLD_RST0_SWIRE = 			BIT(7),
};

#define reg_rst1				REG_ADDR8(0x61)
enum{
	FLD_RST1_ZB = 				BIT(0),
	FLD_RST1_SYS_TIMER = 		BIT(1),
	FLD_RST1_DMA =				BIT(2),
	FLD_RST1_ALGM = 			BIT(3),
	FLD_RST1_AES = 				BIT(4),
	FLD_RST1_ADC = 				BIT(5),
	FLD_RST1_ALG =				BIT(6),
};

#define reg_rst2				REG_ADDR8(0x62)
enum{
	FLD_RST2_AIF =				BIT(0),
	FLD_RST2_AUD =				BIT(1),
	FLD_RST2_DFIFO =			BIT(2),
	FLD_RST2_RISC =				BIT(4),
	FLD_RST2_MCIC = 			BIT(5),
	FLD_RST2_RISC1 =			BIT(6),
	FLD_RST2_MCIC1 = 			BIT(7),
};


#define reg_clk_en0				REG_ADDR8(0x63)
enum{
	FLD_CLK0_SPI_EN = 			BIT(0),
	FLD_CLK0_I2C_EN = 			BIT(1),
	FLD_CLK0_UART_EN = 		BIT(2),
	FLD_CLK0_USB_EN = 			BIT(3),
	FLD_CLK0_PWM_EN = 			BIT(4),
	FLD_CLK0_QDEC_EN = 			BIT(5),
	FLD_CLK0_SWIRE_EN = 		BIT(7),
};

#define reg_clk_en1				REG_ADDR8(0x64)
enum{
	FLD_CLK1_ZB_EN = 			BIT(0),
	FLD_CLK1_SYS_TIMER_EN = 	BIT(1),
	FLD_CLK1_DMA_EN = 			BIT(2),
	FLD_CLK1_ALGM_EN = 			BIT(3),
	FLD_CLK1_AES_EN = 			BIT(4),

};


#define reg_clk_en2				REG_ADDR8(0x65)
enum{
	FLD_CLK2_AIF_EN = 			BIT(0),
	FLD_CLK2_AUD_EN = 			BIT(1),
	FLD_CLK2_DFIFO_EN = 		BIT(2),
	FLD_CLK2_MC_EN = 			BIT(4),
	FLD_CLK2_MCIC_EN = 			BIT(5),
};


#define reg_clk_sel				REG_ADDR8(0x66)
enum{
	FLD_SCLK_DIV = 				BIT_RNG(0,4),
	FLD_SCLK_SEL =				BIT_RNG(5,6),
	FLD_SCLK_HS_SEL =			BIT(7),
};

#define reg_i2s_step			REG_ADDR8(0x67)
enum{
	FLD_I2S_STEP = 				BIT_RNG(0,6),
	FLD_I2S_CLK_EN =			BIT(7),
};

#define reg_i2s_mod				REG_ADDR8(0x68)


#define reg_dmic_step			REG_ADDR8(0x6c)
enum{
	FLD_DMIC_STEP = 			BIT_RNG(0,6),
	FLD_DMIC_CLK_EN =			BIT(7),
};

#define reg_dmic_mod			REG_ADDR8(0x6d)


#define reg_wakeup_en			REG_ADDR8(0x6e)
enum{
	FLD_WAKEUP_SRC_I2C = 		BIT(0),
	FLD_WAKEUP_SRC_SPI =		BIT(1),
	FLD_WAKEUP_SRC_USB =		BIT(2),
	FLD_WAKEUP_SRC_GPIO =		BIT(3),
	FLD_WAKEUP_SRC_I2C_SYN =	BIT(4),
	FLD_WAKEUP_SRC_GPIO_RM =	BIT(5),
	FLD_WAKEUP_SRC_USB_RESM =	BIT(6),
	FLD_WAKEUP_SRC_RST_SYS =	BIT(7),
};

#define reg_pwdn_ctrl			REG_ADDR8(0x6f)
enum
{
	FLD_PWDN_CTRL_REBOOT = BIT(5),
	FLD_PWDN_CTRL_SLEEP  = BIT(7),
};


#define reg_mcu_wakeup_mask		REG_ADDR32(0x78)

/*******************************      7816 registers: 0x7b      ******************************/

#define reg_7816_clk_div		REG_ADDR8(0x7b)

/*******************************      uart registers: 0x90      ******************************/

#define reg_uart_data_buf0		REG_ADDR8(0x90)
#define reg_uart_data_buf1		REG_ADDR8(0x91)
#define reg_uart_data_buf2		REG_ADDR8(0x92)
#define reg_uart_data_buf3		REG_ADDR8(0x93)

#define reg_uart_data_buf(i)    REG_ADDR8(0x90 + (i))  //i = 0~3
#define reg_uart_clk_div		REG_ADDR16(0x94)
enum{
	FLD_UART_CLK_DIV = 			BIT_RNG(0,14),
	FLD_UART_CLK_DIV_EN = 		BIT(15)
};

#define reg_uart_ctrl0			REG_ADDR8(0x96)
enum{
	FLD_UART_BPWC = 			BIT_RNG(0,3),
	FLD_UART_RX_DMA_EN = 		BIT(4),
	FLD_UART_TX_DMA_EN =		BIT(5),
	FLD_UART_RX_IRQ_EN = 		BIT(6),
	FLD_UART_TX_IRQ_EN =		BIT(7),
};

#define reg_uart_ctrl1         		REG_ADDR8(0x97)
enum {
    FLD_UART_CTRL1_CTS_SELECT	   = BIT(0),
    FLD_UART_CTRL1_CTS_EN 		   = BIT(1),
    FLD_UART_CTRL1_PARITY_EN 	   = BIT(2),
    FLD_UART_CTRL1_PARITY_POLARITY = BIT(3),       //1:odd parity   0:even parity
    FLD_UART_CTRL1_STOP_BIT 	   = BIT_RNG(4,5),
    FLD_UART_CTRL1_TTL 			   = BIT(6),
    FLD_UART_CTRL1_LOOPBACK 	   = BIT(7),
};

#define reg_uart_ctrl2			REG_ADDR16(0x98)
enum {
    FLD_UART_CTRL2_RTS_TRIG_LVL   	 = BIT_RNG(0,3),
    FLD_UART_CTRL2_RTS_PARITY 		 = BIT(4),
    FLD_UART_CTRL2_RTS_MANUAL_VAL 	 = BIT(5),
    FLD_UART_CTRL2_RTS_MANUAL_EN 	 = BIT(6),
    FLD_UART_CTRL2_RTS_EN 			 = BIT(7),
	FLD_UART_CTRL3_RX_IRQ_TRIG_LEVEL = BIT_RNG(8,11),
	FLD_UART_CTRL3_TX_IRQ_TRIG_LEVEL = BIT_RNG(12,15),
};


#define reg_uart_ctrl3        	REG_ADDR8(0x99)
enum {
	FLD_UART_RX_IRQ_TRIG_LEV = BIT_RNG(0,3),
	FLD_UART_TX_IRQ_TRIG_LEV = BIT_RNG(4,7),
};

#define reg_uart_rx_timeout0	REG_ADDR8(0x9a)
enum{
	FLD_UART_TIMEOUT_BW		 = 	BIT_RNG(0,7),
};

#define reg_uart_rx_timeout1    REG_ADDR8(0x9b)
enum{
	FLD_UART_TIMEOUT_MUL	 = 	BIT_RNG(0,1),
	FLD_UART_P7816_EN	 	 =  BIT(5),
	FLD_UART_MASK_TXDONE_IRQ =  BIT(6),
	FLD_UART_MASK_ERR_IRQ 	 =  BIT(7),
};


#define reg_uart_buf_cnt       REG_ADDR8(0x9c)

enum{
	FLD_UART_RX_BUF_CNT		=  BIT_RNG(0,3),
	FLD_UART_TX_BUF_CNT		=  BIT_RNG(4,7),
};

#define reg_uart_status0       REG_ADDR8(0x9d)
enum{
	FLD_UART_RBCNT 	     	=  BIT_RNG(0,2),
	FLD_UART_IRQ_FLAG    	=  BIT(3),
	FLD_UART_WBCNT 	     	=  BIT_RNG(4,6),
	FLD_UART_CLEAR_RX_FLAG 	=  BIT(6),
	FLD_UART_RX_ERR_FLAG 	=  BIT(7),
};

#define reg_uart_status1       REG_ADDR8(0x9e)
enum{
	FLD_UART_TX_DONE   	  =  BIT(0),
	FLD_UART_TX_BUF_IRQ   =  BIT(1),
	FLD_UART_RX_DONE   	  =  BIT(2),
	FLD_UART_RX_BUF_IRQ   =  BIT(3),
};


#define reg_uart_state       REG_ADDR8(0x9f)
enum{
	FLD_UART_TSTATE_I 	     =  BIT_RNG(0,2),
	FLD_UART_RSTATE_I	     =  BIT_RNG(4,7),
};


/*******************************      swire registers: 0xb0      ******************************/

#define reg_swire_data			REG_ADDR8(0xb0)
#define reg_swire_ctrl1			REG_ADDR8(0xb1)
enum{
	FLD_SWIRE_WR  = 			BIT(0),
	FLD_SWIRE_RD  = 			BIT(1),
	FLD_SWIRE_CMD =				BIT(2),
	FLD_SWIRE_USB_DET =			BIT(6),
	FLD_SWIRE_USB_EN =			BIT(7),
};

#define reg_swire_clk_div		REG_ADDR8(0xb2)

enum
{
	FLD_SWIRE_CLK_DIV = BIT_RNG(0,6),
};

#define reg_swire_id      		REG_ADDR8(0xb3)

enum
{
	FLD_SWIRE_ID_SLAVE_ID      = BIT_RNG(0,6),
	FLD_SWIRE_ID_SLAVE_FIFO_EN = BIT(7),
};

/*******************************      analog control registers: 0xb8      ******************************/

#define reg_ana_ctrl32			REG_ADDR32(0xb8)
#define reg_ana_addr_data		REG_ADDR16(0xb8)
#define reg_ana_addr			REG_ADDR8(0xb8)
#define reg_ana_data			REG_ADDR8(0xb9)
#define reg_ana_ctrl			REG_ADDR8(0xba)

enum{
	FLD_ANA_BUSY  = 			BIT(0),
	FLD_ANA_RSV	=				BIT(4),
	FLD_ANA_RW  = 				BIT(5),
	FLD_ANA_START  = 			BIT(6),
	FLD_ANA_CYC  = 				BIT(7),
};


/*******************************      usb registers: 0x100      ******************************/

#define reg_ctrl_ep_ptr			REG_ADDR8(0x100)
#define reg_ctrl_ep_dat			REG_ADDR8(0x101)
#define reg_ctrl_ep_ctrl		REG_ADDR8(0x102)

enum{
	FLD_EP_DAT_ACK  = 			BIT(0),
	FLD_EP_DAT_STALL =			BIT(1),
	FLD_EP_STA_ACK = 			BIT(2),
	FLD_EP_STA_STALL = 			BIT(3),
};

#define reg_ctrl_ep_irq_sta		REG_ADDR8(0x103)
enum{
	FLD_CTRL_EP_IRQ_TRANS  = 	BIT_RNG(0,3),
	FLD_CTRL_EP_IRQ_SETUP  =	BIT(4),
	FLD_CTRL_EP_IRQ_DATA  =		BIT(5),
	FLD_CTRL_EP_IRQ_STA  = 		BIT(6),
	FLD_CTRL_EP_IRQ_INTF  = 	BIT(7),
};

#define reg_ctrl_ep_irq_mode	REG_ADDR8(0x104)
enum{
	FLD_CTRL_EP_AUTO_ADDR = 	BIT(0),
	FLD_CTRL_EP_AUTO_CFG =		BIT(1),
	FLD_CTRL_EP_AUTO_INTF =		BIT(2),
	FLD_CTRL_EP_AUTO_STA =		BIT(3),
	FLD_CTRL_EP_AUTO_SYN =		BIT(4),
	FLD_CTRL_EP_AUTO_DESC =		BIT(5),
	FLD_CTRL_EP_AUTO_FEAT =		BIT(6),
	FLD_CTRL_EP_AUTO_STD =		BIT(7),
};

#define reg_usb_ctrl			REG_ADDR8(0x105)
enum{
	FLD_USB_CTRL_AUTO_CLK = 	BIT(0),
	FLD_USB_CTRL_LOW_SPD = 		BIT(1),
	FLD_USB_CTRL_LOW_JITT =		BIT(2),
	FLD_USB_CTRL_TST_MODE = 	BIT(3),
};

#define reg_usb_cyc_cali		REG_ADDR16(0x106)
#define reg_usb_mdev			REG_ADDR8(0x10a)
#define reg_usb_host_conn		REG_ADDR8(0x10b)
enum{
	FLD_USB_MDEV_SELF_PWR = 	BIT(0),
	FLD_USB_MDEV_SUSP_STA = 	BIT(1),
};

#define reg_usb_sups_cyc_cali	REG_ADDR8(0x10c)
#define reg_usb_intf_alt		REG_ADDR8(0x10d)

#define reg_usb_ep8123_ptr		REG_ADDR32(0x110)
#define reg_usb_ep8_ptr			REG_ADDR8(0x110)
#define reg_usb_ep1_ptr			REG_ADDR8(0x111)
#define reg_usb_ep2_ptr			REG_ADDR8(0x112)
#define reg_usb_ep3_ptr			REG_ADDR8(0x113)
#define reg_usb_ep4567_ptr		REG_ADDR32(0x114)
#define reg_usb_ep4_ptr			REG_ADDR8(0x114)
#define reg_usb_ep5_ptr			REG_ADDR8(0x115)
#define reg_usb_ep6_ptr			REG_ADDR8(0x116)
#define reg_usb_ep7_ptr			REG_ADDR8(0x117)
#define reg_usb_ep_ptr(i)		REG_ADDR8(0x110+((i) & 0x07))

#define reg_usb_ep8123_dat		REG_ADDR32(0x118)
#define reg_usb_ep8_dat			REG_ADDR8(0x118)
#define reg_usb_ep1_dat			REG_ADDR8(0x119)
#define reg_usb_ep2_dat			REG_ADDR8(0x11a)
#define reg_usb_ep3_dat			REG_ADDR8(0x11b)
#define reg_usb_ep4567_dat		REG_ADDR32(0x11c)
#define reg_usb_ep4_dat			REG_ADDR8(0x11c)
#define reg_usb_ep5_dat			REG_ADDR8(0x11d)
#define reg_usb_ep6_dat			REG_ADDR8(0x11e)
#define reg_usb_ep7_dat			REG_ADDR8(0x11f)
#define reg_usb_ep_dat(i)		REG_ADDR8(0x118+((i) & 0x07))
#define reg_usb_mic_dat0		REG_ADDR16(0x1800)
#define reg_usb_mic_dat1		REG_ADDR16(0x1802)

#define reg_usb_ep8_ctrl		REG_ADDR8(0x120)
#define reg_usb_ep1_ctrl		REG_ADDR8(0x121)
#define reg_usb_ep2_ctrl		REG_ADDR8(0x122)
#define reg_usb_ep3_ctrl		REG_ADDR8(0x123)
#define reg_usb_ep4_ctrl		REG_ADDR8(0x124)
#define reg_usb_ep5_ctrl		REG_ADDR8(0x125)
#define reg_usb_ep6_ctrl		REG_ADDR8(0x126)
#define reg_usb_ep7_ctrl		REG_ADDR8(0x127)
#define reg_usb_ep_ctrl(i)		REG_ADDR8(0x120+((i) & 0x07))

enum{
	FLD_USB_EP_BUSY = 			BIT(0),
	FLD_USB_EP_STALL =			BIT(1),
	FLD_USB_EP_DAT0 =			BIT(2),
	FLD_USB_EP_DAT1 =			BIT(3),
	FLD_USB_EP_MONO =			BIT(6),
	FLD_USB_EP_EOF_ISO =		BIT(7),
};

#define reg_usb_ep8123_buf_addr	REG_ADDR32(0x128)
#define reg_usb_ep8_buf_addr	REG_ADDR8(0x128)
#define reg_usb_ep1_buf_addr	REG_ADDR8(0x129)
#define reg_usb_ep2_buf_addr	REG_ADDR8(0x12a)
#define reg_usb_ep3_buf_addr	REG_ADDR8(0x12b)
#define reg_usb_ep4567_buf_addr	REG_ADDR32(0x12c)
#define reg_usb_ep4_buf_addr	REG_ADDR8(0x12c)
#define reg_usb_ep5_buf_addr	REG_ADDR8(0x12d)
#define reg_usb_ep6_buf_addr	REG_ADDR8(0x12e)
#define reg_usb_ep7_buf_addr	REG_ADDR8(0x12f)
#define reg_usb_ep_buf_addr(i)	REG_ADDR8(0x128+((i) & 0x07))

#define reg_usb_ram_ctrl		REG_ADDR8(0x130)
enum{
	FLD_USB_CEN_PWR_DN =		BIT(0),
	FLD_USB_CLK_PWR_DN =		BIT(1),
	FLD_USB_WEN_PWR_DN =		BIT(3),
	FLD_USB_CEN_FUNC =			BIT(4),
};

#define reg_usb_iso_mode		REG_ADDR8(0x138)
#define reg_usb_irq				REG_ADDR8(0x139)
#define reg_usb_mask			REG_ADDR8(0x13a)
enum{
	FLD_USB_EDP8_IRQ 		= 	BIT(0),
	FLD_USB_EDP1_IRQ 		= 	BIT(1),
	FLD_USB_EDP2_IRQ 		= 	BIT(2),
	FLD_USB_EDP3_IRQ 		= 	BIT(3),
	FLD_USB_EDP4_IRQ 		= 	BIT(4),
	FLD_USB_EDP5_IRQ 		= 	BIT(5),
	FLD_USB_EDP6_IRQ 		= 	BIT(6),
	FLD_USB_EDP7_IRQ 		= 	BIT(7),
};
#define reg_usb_ep8_send_max	REG_ADDR8(0x13b)
#define reg_usb_ep8_send_thre	REG_ADDR8(0x13c)
#define reg_usb_ep8_fifo_mode	REG_ADDR8(0x13d)
#define reg_usb_ep_max_size		REG_ADDR8(0x13e)

enum{
	FLD_USB_ENP8_FIFO_MODE =	BIT(0),
	FLD_USB_ENP8_FULL_FLAG =	BIT(1),
};

#define reg_rf_acc_len			REG_ADDR8(0x405)
enum{
	FLD_RF_ACC_LEN	    = 		BIT_RNG(0,2),
	FLD_RF_LR_MAN_EN	=       BIT(3),   //long range manual enable
	FLD_RF_LR_TX_SEL	=   	BIT(4),
	FLD_RF_BLE_LR		=   	BIT(5),
	FLD_RF_LR_ACC_TRIG	=   	BIT(7),
};


#define reg_rf_timestamp		REG_ADDR32(0x450)


/*******************************      aes registers: 0x540      ******************************/

#define reg_aes_ctrl            REG_ADDR8(0x540)

enum {
    FLD_AES_CTRL_CODEC_TRIG = BIT(0),
    FLD_AES_CTRL_DATA_FEED = BIT(1),
    FLD_AES_CTRL_CODEC_FINISHED = BIT(2),
};

#define reg_aes_data            REG_ADDR32(0x548)
#define reg_aes_key(v)     		REG_ADDR8(0x550+v)


/*******************************      audio registers: 0x560      ******************************/

#define reg_audio_ctrl     REG_ADDR8(0x560)
enum{
	AUDIO_OUTPUT_OFF		   = 0,
	FLD_AUDIO_MONO_MODE        = BIT(0),
	FLD_AUDIO_I2S_PLAYER_EN    = BIT(1),
	FLD_AUDIO_SDM_PLAYER_EN    = BIT(2),
	FLD_AUDIO_ISO_PLAYER_EN	   = BIT(3),
	FLD_AUDIO_I2S_RECORDER_EN  = BIT(4),
	FLD_AUDIO_I2S_INTERFACE_EN = BIT(5),
	FLD_AUDIO_GRP_EN           = BIT(6),
	FLD_AUDIO_HPF_EN           = BIT(7),
};

#define	reg_pwm_ctrl	  REG_ADDR8(0x563)
enum{
	FLD_PWM_MULTIPLY2			= BIT(0),
	FLD_PWM_ENABLE				= BIT(1),
	FLD_LINER_INTERPOLATE_EN	= BIT(2),
	FLD_LEFT_SHAPING_EN 		= BIT(5),
	FLD_RIGTH_SHAPING_EN 		= BIT(6)
};

#define reg_ascl_tune     REG_ADDR32(0x564)

#define reg_pn1_left      REG_ADDR8(0x568)
enum{
	PN1_LEFT_CHN_BITS 	= BIT_RNG(0,4),
	PN2_LEFT_CHN_EN	  	= BIT(5),
	PN1_LEFT_CHN_EN   	= BIT(6),
};

#define reg_pn2_left      REG_ADDR8(0x569)
enum{
	PN2_LEFT_CHN_BITS 	= BIT_RNG(0,4),
	PN2_RIGHT_CHN_EN	= BIT(5),
	PN1_RIGHT_CHN_EN	= BIT(6),
};

#define reg_pn1_right      REG_ADDR8(0x56a)
enum{
	PN1_RIGHT_CHN_BITS 	= BIT_RNG(0,4),
	CLK2A_AUDIO_CLK_EN	= BIT(5),
	EXCHANGE_SDM_DATA_EN= BIT(6),
};

#define reg_pn2_right      REG_ADDR8(0x56b)
enum{
	PN2_RIGHT_CHN_BITS 		= BIT_RNG(0,4),
	SDM_LEFT_CHN_CONST_EN	= BIT(5),
	SDM_RIGHT_CHN_CONST_EN	= BIT(6),
};

/*******************************      gpio registers: 0x580      ******************************/

#define reg_gpio_pa_in			REG_ADDR8(0x580)
#define reg_gpio_pa_ie			REG_ADDR8(0x581)
#define reg_gpio_pa_oen			REG_ADDR8(0x582)
#define reg_gpio_pa_out			REG_ADDR8(0x583)
#define reg_gpio_pa_pol			REG_ADDR8(0x584)
#define reg_gpio_pa_ds			REG_ADDR8(0x585)
#define reg_gpio_pa_gpio		REG_ADDR8(0x586)
#define reg_gpio_pa_irq_en		REG_ADDR8(0x587)

#define reg_gpio_pb_in			REG_ADDR8(0x588)
#define areg_gpio_pb_ie			0xbd
#define reg_gpio_pb_oen			REG_ADDR8(0x58a)
#define reg_gpio_pb_out			REG_ADDR8(0x58b)
#define reg_gpio_pb_pol			REG_ADDR8(0x58c)
#define areg_gpio_pb_ds			0xbf
#define reg_gpio_pb_gpio		REG_ADDR8(0x58e)
#define reg_gpio_pb_irq_en		REG_ADDR8(0x58f)

#define reg_gpio_pc_in			REG_ADDR8(0x590)
#define areg_gpio_pc_ie			0xc0
#define reg_gpio_pc_oen			REG_ADDR8(0x592)
#define reg_gpio_pc_out			REG_ADDR8(0x593)
#define reg_gpio_pc_pol			REG_ADDR8(0x594)
#define areg_gpio_pc_ds			0xc2
#define reg_gpio_pc_gpio		REG_ADDR8(0x596)
#define reg_gpio_pc_irq_en		REG_ADDR8(0x597)

#define reg_gpio_pd_in			REG_ADDR8(0x598)
#define reg_gpio_pd_ie			REG_ADDR8(0x599)
#define reg_gpio_pd_oen			REG_ADDR8(0x59a)
#define reg_gpio_pd_out			REG_ADDR8(0x59b)
#define reg_gpio_pd_pol			REG_ADDR8(0x59c)
#define reg_gpio_pd_ds			REG_ADDR8(0x59d)
#define reg_gpio_pd_gpio		REG_ADDR8(0x59e)
#define reg_gpio_pd_irq_en		REG_ADDR8(0x59f)

#define reg_gpio_pe_in			REG_ADDR8(0x5a0)
#define reg_gpio_pe_ie			REG_ADDR8(0x5a1)
#define reg_gpio_pe_oen			REG_ADDR8(0x5a2)
#define reg_gpio_pe_out			REG_ADDR8(0x5a3)
#define reg_gpio_pe_pol			REG_ADDR8(0x5a4)
#define reg_gpio_pe_ds			REG_ADDR8(0x5a5)
#define reg_gpio_pe_gpio		REG_ADDR8(0x5a6)
#define reg_gpio_pe_irq_en		REG_ADDR8(0x5a7)

#define reg_gpio_pa_setting1	REG_ADDR32(0x580)
#define reg_gpio_pa_setting2	REG_ADDR32(0x584)
#define reg_gpio_pd_setting1	REG_ADDR32(0x598)
#define reg_gpio_pd_setting2	REG_ADDR32(0x59c)

#define reg_gpio_in(i)				REG_ADDR8(0x580+((i>>8)<<3))
#define reg_gpio_ie(i)				REG_ADDR8(0x581+((i>>8)<<3))
#define reg_gpio_oen(i)				REG_ADDR8(0x582+((i>>8)<<3))
#define reg_gpio_out(i)				REG_ADDR8(0x583+((i>>8)<<3))
#define reg_gpio_pol(i)				REG_ADDR8(0x584+((i>>8)<<3))
#define reg_gpio_ds(i)				REG_ADDR8(0x585+((i>>8)<<3))


#define reg_gpio_func(i)			REG_ADDR8(0x586+((i>>8)<<3))

#define reg_gpio_irq_wakeup_en(i)	REG_ADDR8(0x587+((i>>8)<<3))  // reg_irq_mask: FLD_IRQ_GPIO_EN

#define reg_gpio_irq_risc0_en(i)  	REG_ADDR8(0x5b8 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
#define reg_gpio_irq_risc1_en(i)  	REG_ADDR8(0x5c0 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
#define reg_gpio_irq_risc2_en(i)  	REG_ADDR8(0x5c8 + (i >> 8))

#define reg_mux_func_a1      		REG_ADDR8(0x5a8)
#define reg_mux_func_a2      		REG_ADDR8(0x5a9)
#define reg_mux_func_b1     		REG_ADDR8(0x5aa)
#define reg_mux_func_b2     		REG_ADDR8(0x5ab)
#define reg_mux_func_c1     		REG_ADDR8(0x5ac)
#define reg_mux_func_c2     		REG_ADDR8(0x5ad)
#define reg_mux_func_d1     		REG_ADDR8(0x5ae)
#define reg_mux_func_d2     		REG_ADDR8(0x5af)

#define reg_gpio_wakeup_irq  		REG_ADDR8(0x5b5)
enum{
    FLD_GPIO_CORE_WAKEUP_EN  	= BIT(2),
    FLD_GPIO_CORE_INTERRUPT_EN 	= BIT(3),
};

#define reg_pin_i2c_spi_out_en	    REG_ADDR8(0x5b6)
enum{
	FLD_PIN_PAGROUP_SPI_EN =	BIT_RNG(4,5),
	FLD_PIN_PBGROUP_SPI_EN =	BIT(6),
	FLD_PIN_PDGROUP_SPI_EN =	BIT(7),
};


#define reg_pin_i2c_spi_en			REG_ADDR8(0x5b7)  //poweron default  0xff
enum{
	FLD_PIN_PA3_SPI_EN =	BIT(0),
	FLD_PIN_PA4_SPI_EN =	BIT(1),
	FLD_PIN_PB6_SPI_EN =	BIT(2),
	FLD_PIN_PD7_SPI_EN =	BIT(3),
	FLD_PIN_PA3_I2C_EN =	BIT(4),
	FLD_PIN_PA4_I2C_EN =	BIT(5),
	FLD_PIN_PB6_I2C_EN =	BIT(6),
	FLD_PIN_PD7_I2C_EN =	BIT(7),
};


/*******************************      timer registers: 0x620      ******************************/

#define reg_tmr_ctrl			REG_ADDR32(0x620)
#define reg_tmr_ctrl16			REG_ADDR16(0x620)
#define reg_tmr_ctrl8			REG_ADDR8(0x620)
enum{
	FLD_TMR0_EN =				BIT(0),
	FLD_TMR0_MODE =				BIT_RNG(1,2),
	FLD_TMR1_EN = 				BIT(3),
	FLD_TMR1_MODE =				BIT_RNG(4,5),
	FLD_TMR2_EN =				BIT(6),
	FLD_TMR2_MODE = 			BIT_RNG(7,8),
	FLD_TMR_WD_CAPT = 			BIT_RNG(9,22),
	FLD_TMR_WD_EN =				BIT(23),
	FLD_TMR0_STA =				BIT(24),
	FLD_TMR1_STA =				BIT(25),
	FLD_TMR2_STA =				BIT(26),
	FLD_CLR_WD =				BIT(27),
};

#define reg_tmr_sta				REG_ADDR8(0x623)
enum{
	FLD_TMR_STA_TMR0 =			BIT(0),
	FLD_TMR_STA_TMR1 =			BIT(1),
	FLD_TMR_STA_TMR2 =			BIT(2),
	FLD_TMR_STA_WD =			BIT(3),
};

#define reg_tmr0_capt			REG_ADDR32(0x624)
#define reg_tmr1_capt			REG_ADDR32(0x628)
#define reg_tmr2_capt			REG_ADDR32(0x62c)
#define reg_tmr_capt(i)			REG_ADDR32(0x624 + ((i) << 2))
#define reg_tmr0_tick			REG_ADDR32(0x630)
#define reg_tmr1_tick			REG_ADDR32(0x634)
#define reg_tmr2_tick			REG_ADDR32(0x638)
#define reg_tmr_tick(i)			REG_ADDR32(0x630 + ((i) << 2))

/*******************************      irq registers: 0x640      ******************************/

#define reg_irq_mask			REG_ADDR32(0x640)
#define reg_irq_pri				REG_ADDR32(0x644)
#define reg_irq_src				REG_ADDR32(0x648)
#define reg_irq_src3			REG_ADDR8(0x64a)
enum{
	FLD_IRQ_TMR0_EN =			BIT(0),
	FLD_IRQ_TMR1_EN =			BIT(1),
	FLD_IRQ_TMR2_EN =			BIT(2),
	FLD_IRQ_USB_PWDN_EN =		BIT(3),
	FLD_IRQ_DMA_EN =			BIT(4),
	FLD_IRQ_DAM_FIFO_EN =		BIT(5),
	FLD_IRQ_UART_EN 	=		BIT(6),
	FLD_IRQ_MIX_CMD_EN =		BIT(7), FLD_IRQ_HOST_CMD_EN  =	BIT(7),//< MIX = I2C/QDEC/SPI

	FLD_IRQ_EP0_SETUP_EN =		BIT(8),
	FLD_IRQ_EP0_DAT_EN =		BIT(9),
	FLD_IRQ_EP0_STA_EN =		BIT(10),
	FLD_IRQ_SET_INTF_EN =		BIT(11),
	FLD_IRQ_EP_DATA_EN =		BIT(12),  FLD_IRQ_IRQ4_EN = BIT(12),
	FLD_IRQ_ZB_RT_EN =			BIT(13),
	FLD_IRQ_SW_PWM_EN =			BIT(14),  //irq_software | irq_pwm
    //	RSVD 		=			BIT(15),

	FLD_IRQ_USB_250US_EN =		BIT(16),
	FLD_IRQ_USB_RST_EN =		BIT(17),
	FLD_IRQ_GPIO_EN =			BIT(18),
	FLD_IRQ_PM_EN =				BIT(19),
	FLD_IRQ_SYSTEM_TIMER =		BIT(20),
	FLD_IRQ_GPIO_RISC0_EN =		BIT(21),
	FLD_IRQ_GPIO_RISC1_EN =		BIT(22),
	//	RSVD		  	  = 	BIT(23),

	FLD_IRQ_EN =				BIT_RNG(24,31),
	FLD_IRQ_ALL           =     -1
};
#define reg_irq_en				REG_ADDR8(0x643)




/*******************************      system timer registers: 0x740      ******************************/

#define reg_system_tick			REG_ADDR32(0x740)
#define reg_system_tick_irq		REG_ADDR32(0x744)
#define reg_system_wakeup_tick	REG_ADDR32(0x748)
#define reg_system_tick_mode	REG_ADDR8(0x74c)
#define reg_system_tick_ctrl	REG_ADDR8(0x74f)

enum {
	FLD_SYSTEM_TICK_START	=		BIT(0),
	FLD_SYSTEM_TICK_STOP	=		BIT(1),
	FLD_SYSTEM_TICK_RUNNING	=		BIT(1),

	FLD_SYSTEM_TICK_IRQ_EN  = 		BIT(1),
};



/*******************************      pwm registers: 0x780      ******************************/

#define reg_pwm_enable			REG_ADDR8(0x780)
#define reg_pwm0_enable			REG_ADDR8(0x781)
#define reg_pwm_clk				REG_ADDR8(0x782)

#define reg_pwm0_mode			REG_ADDR8(0x783)


#define reg_pwm_invert			REG_ADDR8(0x784)
#define reg_pwm_n_invert		REG_ADDR8(0x785)
#define reg_pwm_pol				REG_ADDR8(0x786)

#define reg_pwm_cycle(i)		REG_ADDR32(0x794 + (i << 2))   //<15:0>: TCMP 0~5  <31:16>: TMAX 0~5
#define reg_pwm_cmp(i)			REG_ADDR16(0x794 + (i << 2))   //TCMP 0~5
#define reg_pwm_max(i)			REG_ADDR16(0x796 + (i << 2))   //TMAX 0~5
enum{
	FLD_PWM_CMP  = 				BIT_RNG(0,15),
	FLD_PWM_MAX  = 				BIT_RNG(16,31),
};

#define reg_pwm0_pulse_num		REG_ADDR16(0x7ac)

#define reg_pwm_irq_mask		REG_ADDR8(0x7b0)
#define reg_pwm_irq_sta			REG_ADDR8(0x7b1)
enum{
	FLD_IRQ_PWM0_PNUM =					BIT(0),
	FLD_IRQ_PWM0_IR_DMA_FIFO_DONE  =	BIT(1),
	FLD_IRQ_PWM0_FRAME =				BIT(2),
	FLD_IRQ_PWM1_FRAME =				BIT(3),
	FLD_IRQ_PWM2_FRAME =				BIT(4),
	FLD_IRQ_PWM3_FRAME =				BIT(5),
	FLD_IRQ_PWM4_FRAME 	=				BIT(6),
	FLD_IRQ_PWM5_FRAME =				BIT(7),
};



#define reg_pwm0_fifo_mode_irq_mask		REG_ADDR8(0x7b2)

enum{
	FLD_PWM0_IRQ_IR_FIFO_EN  	 = BIT(0),
};

#define reg_pwm0_fifo_mode_irq_sta		REG_ADDR8(0x7b3)

enum{
	FLD_PWM0_IRQ_IR_FIFO_CNT 	 = BIT(0),
};



#define reg_pwm_tcmp0_shadow		REG_ADDR16(0x7c4)   //<15:0>: TCMP 0~5  <31:16>: TMAX 0~5
#define reg_pwm_tmax0_shadow		REG_ADDR16(0x7c6)   //TCMP 0~5

#define reg_pwm_ir_fifo_dat(i)			REG_ADDR16(0x7c8+i*2)
#define reg_pwm_ir_fifo_irq_trig_level	REG_ADDR8(0x7cc)

#define reg_pwm_ir_fifo_data_status		REG_ADDR8(0x7cd)
enum{
	FLD_PWM0_IR_FIFO_DATA_NUM 	=		BIT_RNG(0,3),
	FLD_PWM0_IR_FIFO_EMPTY 		=		BIT(4),
	FLD_PWM0_IR_FIFO_FULL 		=		BIT(5),
};

#define reg_pwm_ir_clr_fifo_data		REG_ADDR8(0x7ce)

enum{
	FLD_PWM0_IR_FIFO_CLR_DATA 	=		BIT(0),
};

/*******************************      fifo registers: 0x800      ******************************/

#define reg_fifo0_data			REG_ADDR32(0x800)
#define reg_fifo1_data			REG_ADDR32(0x900)
#define reg_fifo2_data			REG_ADDR32(0xa00)

/*******************************      dfifo registers: 0xb00      ******************************/

#define reg_dfifo0_addr			REG_ADDR16(0xb00)
#define reg_dfifo0_size			REG_ADDR8(0xb02)
#define reg_dfifo0_addHi		REG_ADDR8(0xb03)  //default 0x04, no need set

#define reg_dfifo1_addr			REG_ADDR16(0xb04)
#define reg_dfifo1_size			REG_ADDR8(0xb06)
#define reg_dfifo1_addHi		REG_ADDR8(0xb07)  //default 0x04, no need set

//misc channel only use dfifo2
#define reg_dfifo2_addr			REG_ADDR16(0xb08)
#define reg_dfifo2_size			REG_ADDR8(0xb0a)
#define reg_dfifo2_addHi		REG_ADDR8(0xb0b)  //default 0x04, no need set

#define reg_dfifo_audio_addr		reg_dfifo0_addr
#define reg_dfifo_audio_size		reg_dfifo0_size

#define reg_dfifo_misc_chn_addr		reg_dfifo2_addr
#define reg_dfifo_misc_chn_size		reg_dfifo2_size


#define reg_dfifo0_l_level		REG_ADDR8(0xb0c)  //dfifo0  low int threshold(wptr - rptr)
#define reg_dfifo0_h_level		REG_ADDR8(0xb0d)  //dfifo0 high int threshold(wptr - rptr)
#define reg_dfifo1_h_level		REG_ADDR8(0xb0e)  //dfifo1 high int threshold(wptr - rptr)
#define reg_dfifo2_h_level		REG_ADDR8(0xb0f)  //dfifo2 high int threshold(wptr - rptr)


#define	reg_dfifo_mode			REG_ADDR8(0xb10)
enum{
	FLD_AUD_DFIFO0_IN 		= BIT(0),
	FLD_AUD_DFIFO1_IN 		= BIT(1),
	FLD_AUD_DFIFO2_IN 		= BIT(2),
	FLD_AUD_DFIFO0_OUT 		= BIT(3),
	FLD_AUD_DFIFO0_L_INT	= BIT(4),
	FLD_AUD_DFIFO0_H_INT	= BIT(5),
	FLD_AUD_DFIFO1_H_INT	= BIT(6),
	FLD_AUD_DFIFO2_H_INT	= BIT(7),
};

#define	reg_dfifo_ain			REG_ADDR8(0xb11)
enum{
	FLD_AUD_DMIC0_DATA_IN_RISING_EDGE = BIT(0),
	FLD_AUD_DMIC1_DATA_IN_RISING_EDGE = BIT(1),
	FLD_AUD_INPUT_SELECT     		  = BIT_RNG(2,3),
	FLD_AUD_INPUT_MONO_MODE	   		  = BIT(4),
	FLD_AUD_DECIMATION_FILTER_BYPASS  = BIT(5),
	FLD_AUD_DMIC_RISING_EDGE_BYPASS   = BIT(6),
	FLD_AUD_DMIC_FALLING_EDGE_BYPASS  = BIT(7),

};

enum{  //core_b11<0> <1>  audio dmic_n  rising/falling edge
	AUDIO_DMIC_DATA_IN_RISING_EDGE 		= 1,
	AUDIO_DMIC_DATA_IN_FALLING_EDGE 	= 0,
};

enum{  //core_b11<3:2>  audio input select
	AUDIO_INPUT_USB  = 0x00,
	AUDIO_INPUT_I2S  = 0x01,
	AUDIO_INPUT_AMIC = 0x02,
	AUDIO_INPUT_DMIC = 0x03,
};



#define reg_dfifo_dec_ratio		REG_ADDR8(0xb12)

#define reg_dfifo_irq_status	REG_ADDR8(0xb13)
enum{
	FLD_AUD_DFIFO0_L_IRQ	= BIT(4),
	FLD_AUD_DFIFO0_H_IRQ	= BIT(5),
	FLD_AUD_DFIFO1_L_IRQ	= BIT(6),
	FLD_AUD_DFIFO1_H_IRQ	= BIT(7),
};
#define reg_dfifo0_rptr			REG_ADDR16(0xb14)
#define reg_dfifo0_wptr			REG_ADDR16(0xb16)

#define reg_dfifo1_rptr			REG_ADDR16(0xb18)
#define reg_dfifo1_wptr			REG_ADDR16(0xb1a)

#define reg_dfifo2_rptr			REG_ADDR16(0xb1c)
#define reg_dfifo2_wptr			REG_ADDR16(0xb1e)


#define reg_audio_wptr			reg_dfifo0_wptr

#define reg_dfifo0_num			REG_ADDR16(0xb20)
#define reg_dfifo1_num			REG_ADDR16(0xb24)
#define reg_dfifo2_num			REG_ADDR16(0xb28)

#define reg_dfifo0_manual		REG_ADDR8(0xb2c)
enum{
	FLD_DFIFO_MANUAL_MODE_EN	= BIT(0),
};

#define reg_dfifo0_man_dat		REG_ADDR32(0xb30)

#define	reg_alc_sft				REG_ADDR8(0xb34)

#define	reg_audio_dec_mode		REG_ADDR8(0xb35)
enum{
	FLD_AUD_LNR_VALID_SEL	= BIT(0),
	FLD_AUD_CIC_MODE  		= BIT(3)
};


#define	reg_adc_mul				REG_ADDR8(0xb36)
#define	reg_adc_bias			REG_ADDR8(0xb37)

#define	reg_aud_alc_hpf_lpf_ctrl	 REG_ADDR8(0xb40)
enum {
	FLD_AUD_IN_HPF_SFT		=	BIT_RNG(0,3),
	FLD_AUD_IN_HPF_BYPASS	=	BIT(4),
	FLD_AUD_IN_ALC_BYPASS	=	BIT(5),
	FLD_AUD_IN_LPF_BYPASS   =   BIT(6),
	FLD_DOUBLE_DOWN_SAMPLING_ON =   BIT(7)
};



#define reg_aud_alc_vol_l_chn		REG_ADDR8(0xb41)  //default 0x00, will be 0x20 after ana_34 set to 0x80
#define reg_aud_alc_vol_r_chn		REG_ADDR8(0xb42)  //default 0x00, will be 0x20 after ana_34 set to 0x80
enum{
	FLD_AUD_ALC_MIN_VOLUME_IN_DIGITAL_MODE 	= BIT_RNG(0,5),
	FLD_AUD_ALC_MIN_PGA_IN_ANALOG_MODE	    = BIT_RNG(0,6),
	//alc digital mode left/right channel regulate mode select: 1 for auto regulate; 0 for manual regulate
	FLD_AUD_ALC_DIGITAL_MODE_AUTO_REGULATE_EN	= BIT(7),
};


#define reg_alc_vol_h				REG_ADDR8(0xb43)
enum{
	FLD_AUD_ALC_MAX_VOLUME_IN_DIGITAL_MODE 	= BIT_RNG(0,5),
	FLD_AUD_ALC_MAX_PGA_IN_ANALOG_MODE	    = BIT_RNG(0,6),
};


#define reg_alc_vol_th_h			REG_ADDR16(0xb44)
#define reg_alc_vol_th_l			REG_ADDR16(0xb46)
#define reg_alc_vol_thn				REG_ADDR16(0xb48)
#define reg_alc_vad_thn				REG_ADDR16(0xb4a)

#define reg_alc_vol_step			REG_ADDR8(0xb4c)

#define reg_alc_vol_l				REG_ADDR8(0xb4d)
#define reg_alc_vol_r				REG_ADDR8(0xb4e)

#define reg_alc_peak_tick			REG_ADDR16(0xb50)

#define reg_alc_dec_tick			REG_ADDR8(0xb52)
#define reg_alc_noi_tick			REG_ADDR8(0xb53)


#define reg_aud_alc_cfg				REG_ADDR8(0xb54)   //default 0x00, will be 0x02 after ana_34 set to 0x80
enum{
	FLD_AUD_ALC_ANALOG_MODE_EN 		= BIT(0),    //alc mode select:   1 for analog mode;  0 for digital mode
	FLD_AUD_ALC_NOISE_EN 			= BIT(1),
};

#define reg_alc_coef_iir			REG_ADDR8(0xb55)
#define reg_alc_dat_mask			REG_ADDR8(0xb56)
#define reg_alc_inc_spd				REG_ADDR8(0xb57)
#define reg_alc_inc_max				REG_ADDR8(0xb58)
#define reg_alc_dec_spd				REG_ADDR8(0xb59)
#define reg_alc_dec_max				REG_ADDR8(0xb5a)
#define reg_alc_noi_spd				REG_ADDR8(0xb5b)
#define reg_alc_noi_max				REG_ADDR8(0xb5c)

#define reg_pga_gain_init			REG_ADDR8(0xb5d)
#define reg_pga_gain_l				REG_ADDR8(0xb5e)   //used to check current left  channel gain in analog mode auto regulate
#define reg_pga_gain_r				REG_ADDR8(0xb5f)   //used to check current right channel gain in analog mode auto regulate
#define reg_pga_man_speed			REG_ADDR8(0xb60)

#define reg_pga_man_target_l		REG_ADDR8(0xb61)
#define reg_pga_value_l				REG_ADDR8(0xb62)
#define reg_pga_fix_value			REG_ADDR8(0xb63)

enum {
	FLD_PGA_POST_AMPLIFIER_GAIN		=	BIT_RNG(0,5),
	FLD_PGA_PRE_AMPLIFIER_GAIN   =   BIT(6),
	FLD_PGA_GAIN_FIX_EN =   BIT(7)
};

#define reg_pga_r_l					REG_ADDR8(0xb64)
#define reg_pga_man_target_r		REG_ADDR8(0xb65)
#define reg_pga_value_r				REG_ADDR8(0xb66)

/*******************************      dma registers: 0xc00      ******************************/

//uart rx
#define reg_dma0_addr			REG_ADDR16(0xc00)
#define reg_dma0_size			REG_ADDR8(0xc02)
#define reg_dma0_mode			REG_ADDR8(0xc03)
enum{
	FLD_DMA_WR_MEM =			BIT(0),
	FLD_DMA_PINGPONG_EN =		BIT(1),
	FLD_DMA_FIFO_EN =			BIT(2),
	FLD_DMA_AUTO_MODE =			BIT(3),
	FLD_DMA_READ_MODE =			BIT(4),
	FLD_DMA_BYTE_MODE =			BIT(5)
};


//uart tx
#define reg_dma1_addr			REG_ADDR16(0xc04)
#define reg_dma1_size			REG_ADDR8(0xc06)
#define reg_dma1_mode			REG_ADDR8(0xc07)

//rf rx dma
#define reg_dma2_addr			REG_ADDR16(0xc08)
#define reg_dma2_size			REG_ADDR8(0xc0a)
#define reg_dma2_mode			REG_ADDR8(0xc0b)

//rf tx dma
#define reg_dma3_addr			REG_ADDR16(0xc0c)
#define reg_dma3_size			REG_ADDR8(0xc0e)
#define reg_dma3_mode			REG_ADDR8(0xc0f)

#define reg_dma4_addr			REG_ADDR16(0xc10)
#define reg_dma4_size			REG_ADDR8(0xc12)
#define reg_dma4_mode			REG_ADDR8(0xc13)

#define reg_dma5_addr			REG_ADDR16(0xc14)
#define reg_dma5_size			REG_ADDR8(0xc16)
#define reg_dma5_mode			REG_ADDR8(0xc17)

//pwm tx dma
#define reg_dma7_addr			REG_ADDR16(0xc18)
#define reg_dma7_size			REG_ADDR8(0xc1a)
#define reg_dma7_mode			REG_ADDR8(0xc1b)


#define reg_dma_t_addr			REG_ADDR16(0xc1c)
#define reg_dma_t_size			REG_ADDR8(0xc1e)

#define reg_dma_size(v)			REG_ADDR8(0xc02+4*v)

//  The default channel assignment
#define reg_dma_uart_rx_addr	reg_dma0_addr
#define reg_dma_uart_rx_size	reg_dma0_size
#define reg_dma_uart_rx_mode	reg_dma0_mode

#define reg_dma_uart_tx_addr	reg_dma1_addr
#define reg_dma_uart_tx_size	reg_dma1_size
#define reg_dma_uart_tx_mode	reg_dma1_mode

#define reg_dma_rf_rx_addr		reg_dma2_addr
#define reg_dma_rf_rx_size		reg_dma2_size
#define reg_dma_rf_rx_mode		reg_dma2_mode

#define reg_dma_rf_tx_addr		reg_dma3_addr
#define reg_dma_rf_tx_size		reg_dma3_size
#define reg_dma_rf_tx_mode		reg_dma3_mode

#define reg_dma_pwm_addr		reg_dma7_addr
#define reg_dma_pwm_size		reg_dma7_size
#define reg_dma_pwm_mode		reg_dma7_mode



#define reg_dma_chn_en			REG_ADDR8(0xc20)
#define reg_dma_chn_irq_msk		REG_ADDR8(0xc21)
#define reg_dma_tx_rdy0			REG_ADDR8(0xc24)
#define reg_dma_tx_rdy1			REG_ADDR8(0xc25)
#define reg_dma_rx_rdy0			REG_ADDR8(0xc26)
#define reg_dma_rx_rdy1			REG_ADDR8(0xc27)

#define reg_dma_irq_status		reg_dma_rx_rdy0

enum{
	FLD_DMA_CHN0 =	BIT(0),		FLD_DMA_CHN_UART_RX =	BIT(0),
	FLD_DMA_CHN1 =	BIT(1),		FLD_DMA_CHN_UART_TX =	BIT(1),
	FLD_DMA_CHN2 =	BIT(2),		FLD_DMA_CHN_RF_RX =		BIT(2),
	FLD_DMA_CHN3 =	BIT(3),		FLD_DMA_CHN_RF_TX =		BIT(3),
	FLD_DMA_CHN4 =	BIT(4),		FLD_DMA_CHN_AES_DECO =  BIT(4),
	FLD_DMA_CHN5 =	BIT(5),     FLD_DMA_CHN_AES_CODE =  BIT(5),
	FLD_DMA_CHN7 =	BIT(7),		FLD_DMA_CHN_PWM  	 =	BIT(7),
};

typedef enum {
    FLD_DMA_IRQ_UART_RX  = BIT(0),
    FLD_DMA_IRQ_UART_TX  = BIT(1),
    FLD_DMA_IRQ_RF_RX    = BIT(2),
    FLD_DMA_IRQ_RF_TX    = BIT(3),
    FLD_DMA_IRQ_AES_DECO = BIT(4),
    FLD_DMA_IRQ_AES_CODE = BIT(5),
    FLD_DMA_IRQ_PWM		 = BIT(7),
    FLD_DMA_IRQ_ALL      = 0xff,
} IRQ_DMAIrqTypeDef;


#define reg_dma_rx_rptr			REG_ADDR8(0xc28)
#define reg_dma_rx_wptr			REG_ADDR8(0xc29)

#define reg_dma_tx_rptr			REG_ADDR8(0xc2a)
#define reg_dma_tx_wptr			REG_ADDR8(0xc2b)
#define reg_dma_tx_fifo			REG_ADDR16(0xc2c)
enum{
	FLD_DMA_RPTR_CLR =			BIT(4),
	FLD_DMA_RPTR_NEXT =			BIT(5),
	FLD_DMA_RPTR_SET =			BIT(6),
};

#define reg_dma0_addrHi			REG_ADDR8(0xc40)
#define reg_dma1_addrHi			REG_ADDR8(0xc41)
#define reg_dma2_addrHi			REG_ADDR8(0xc42)
#define reg_dma3_addrHi			REG_ADDR8(0xc43)
#define reg_dma4_addrHi			REG_ADDR8(0xc44)
#define reg_dma5_addrHi			REG_ADDR8(0xc45)
#define reg_dma_ta_addrHi		REG_ADDR8(0xc46)
#define reg_dma_a3_addrHi		REG_ADDR8(0xc47)
#define reg_dma7_addrHi			REG_ADDR8(0xc48)

/*******************************      linklayer registers: 0xf00      ******************************/

#define reg_rf_ll_ctrl_0		REG_ADDR8(0xf02)

#define reg_rf_ll_ctrl_1		REG_ADDR8(0xf03)
enum{
	FLD_RF_FSM_TIMEOUT_EN 		=   BIT(0),
	FLD_RF_RX_FIRST_TIMEOUT_EN	=   BIT(1),
	FLD_RF_RX_TIMEOUT_EN		=   BIT(2),
	FLD_RF_CRC_2_EN 			=   BIT(3),

	//BLE mode
	FLD_RF_BRX_SN_INIT	 		=   BIT(4),
	FLD_RF_BRX_NESN_INIT	 	=   BIT(5),
	FLD_RF_BTX_SN_INIT	 		=   BIT(6),
	FLD_RF_BTX_NESN_INIT	 	=   BIT(7),
};

#define  	FSM_TIMEOUT_ENABLE 	 	( reg_rf_ll_ctrl_1 |= FLD_RF_FSM_TIMEOUT_EN )
#define  	FSM_TIMEOUT_DISABLE		( reg_rf_ll_ctrl_1 &= (~FLD_RF_FSM_TIMEOUT_EN) )

#define reg_rf_rx_timeout		REG_ADDR16(0xf0a)

#define reg_rf_ll_ctrl_2		REG_ADDR8(0xf15)

#define reg_rf_ll_ctrl_3		REG_ADDR8(0xf16)
enum{
	FLD_RF_TX_EN_DLY_EN = 		BIT(0),
	FLD_RF_PLL_RST_EN	=       BIT(1),
	FLD_RF_CMD_SCHEDULE_EN	=   BIT(2),
	FLD_RF_R_TX_EN_DLY =		BIT_RNG(4,7),
};


#define reg_rf_irq_mask			REG_ADDR16(0xf1c)
#define reg_rf_irq_status		REG_ADDR16(0xf20)
#define reg_rf_fsm_timeout		REG_ADDR32(0xf2c)

#define		CLEAR_ALL_RFIRQ_STATUS   ( reg_rf_irq_status = 0xffff )

enum{
	FLD_RF_IRQ_RX = 			BIT(0),
	FLD_RF_IRQ_TX =				BIT(1),
	FLD_RF_IRQ_RX_TIMEOUT =		BIT(2),
	FLD_RF_IRQ_RX_CRC_2 =		BIT(4),
	FLD_RF_IRQ_CMD_DONE  =		BIT(5),
	FLD_RF_IRQ_FSM_TIMEOUT  =	BIT(6),
	FLD_RF_IRQ_RETRY_HIT =		BIT(7),
	FLD_RF_IRQ_TX_DS =          BIT(8),
    FLD_RF_IRQ_RX_DR =          BIT(9),
	FLD_RF_IRQ_FIRST_TIMEOUT =	BIT(10),
	FLD_RF_IRQ_INVALID_PID =    BIT(11),
	FLD_RF_IRQ_STX_TIMEOUT =    BIT(12),
	FLD_RF_IRQ_ALL =            0X1FFF,
};


/********************************************************************************************
 *****|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|*****
 *****|								Aanlog  Register Table  						   |*****
 *****|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|*****
 ********************************************************************************************/

/*******************************      analog registers(3v3): 0x00      ***************************/

#define areg_06_pll_bg			        0x06
enum{
	FLD_PLL_BG_POWER_DOWN = BIT(4),      //    	1: Power down;     0: Power up
};

#define areg_0e_pa0_pa3_pull			0x0E
#define areg_0f_pa4_pa7_pull			0x0F
#define areg_10_pb0_pb3_pull			0x10
#define areg_11_pb4_pb7_pull			0x11
#define areg_12_pc0_pc3_pull			0x12
#define areg_13_pc4_pc7_pull			0x13
#define areg_14_pd0_pd3_pull			0x14
#define areg_15_pd4_pd7_pull			0x15

/*******************************      analog registers(1v8): 0x80      ***************************/
#define areg_clk_setting				0x82
enum{
	FLD_DCCC_DOUBLER_POWER_DOWN	  = BIT(3),
	FLD_CLK_48M_TO_RX_EN 		  = BIT(4),
	FLD_CLK_48M_TO_DIG_EN 		  = BIT(5),
	FLD_CLK_24M_TO_SAR_EN 		  = BIT(6),
	FLD_CLK_48M_TO_CAL_DIG_MAN_EN = BIT(7),
};

#define areg_xo_setting				0x8a
enum{
	FLD_XO_CDAC_ANA	  	= BIT_RNG(0,5),
	FLD_XO_MODE_ANA 	= BIT(6),
	FLD_XO_CAP_OFF_ANA  = BIT(7),
};

#if defined(__cplusplus)
}
#endif




