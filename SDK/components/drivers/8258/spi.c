/********************************************************************************************************
 * @file     spi.c 
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

#include "spi.h"
#include "register.h"
#include "gpio.h"

/**
 * @brief     This function selects a pin port for the SPI interface (master mode)
 * @param[in] PinGrp - the selected pin port
 * @return    none
 *  A3:5b7[0] set 1 as spi input,set 0 not as spi input ;5b7[4] set 1 as i2c input ,set 0 not as i2c input
 *	A4:5b7[1] set 1 as spi input,set 0 not as spi input ;5b7[5] set 1 as i2c input ,set 0 not as i3c input
 *	B6:5b7[2] set 1 as spi input,set 0 not as spi input ;5b7[6] set 1 as i2c input ,set 0 not as i4c input
 *	D7:5b7[3] set 1 as spi input,set 0 not as spi input ;5b7[7] set 1 as i2c input ,set 0 not as i5c input
 *	5b6[7:4]  set spi or i2c output,5b6[5:4] PA spi sel, 5b6[6] PB spi sel,5b6[7] PD spi_sel
 *	spi i2c mux sel:
 *	step1:close gpio func;
 *	step2:set 0x5a8-0x5af sel spii2c func;
 *	step3:set 5b6[7:4] to sel spi or i2c output;
 *	step4:set 5b7[7:0] to sel spi or i2c input;
 *	step5 if use pd[6]as spi slave csn,need to open pd[2] gpio func,because pd[2]have high priority than pd[6]
 */


void spi_master_gpio_set(SPI_GPIO_GroupTypeDef PinGrp)
{
	GPIO_PinTypeDef sclk=GPIO_PA4;
	GPIO_PinTypeDef cs=GPIO_PD6;
	GPIO_PinTypeDef sdo=GPIO_PA2;
	GPIO_PinTypeDef sdi=GPIO_PA3;

	 if (PinGrp == SPI_GPIO_GROUP_A2A3A4D6){

		 sclk = GPIO_PA4;
		 cs   = GPIO_PD6;
		 sdo  = GPIO_PA2;
		 sdi  = GPIO_PA3;

		 reg_pin_i2c_spi_out_en |= (FLD_PIN_PAGROUP_SPI_EN|FLD_PIN_PDGROUP_SPI_EN);
		 reg_pin_i2c_spi_en |= (FLD_PIN_PA3_SPI_EN | FLD_PIN_PA4_SPI_EN);
		 reg_pin_i2c_spi_en &= ~(FLD_PIN_PA3_I2C_EN | FLD_PIN_PA4_I2C_EN);
	 }
	 else if (PinGrp ==SPI_GPIO_GROUP_B6B7D2D7){

		 sclk = GPIO_PD7;
		 cs   = GPIO_PD2;
		 sdo  = GPIO_PB7;
		 sdi  = GPIO_PB6;
		 reg_pin_i2c_spi_out_en |= (FLD_PIN_PBGROUP_SPI_EN|FLD_PIN_PDGROUP_SPI_EN);
		 reg_pin_i2c_spi_en |= (FLD_PIN_PB6_SPI_EN | FLD_PIN_PD7_SPI_EN);
		 reg_pin_i2c_spi_en &= ~(FLD_PIN_PB6_I2C_EN | FLD_PIN_PD7_I2C_EN);
	 }
	 gpio_set_func(sclk ,AS_SPI);
	 gpio_set_func(sdo  ,AS_SPI);
	 gpio_set_func(sdi  ,AS_SPI);
	 gpio_set_input_en(sdi, 1);

	 spi_masterCSpin_select(cs);
}

//SPI related registers definitions

/**
 * @brief     This function selects a GPIO pin as CS of SPI function.
 * @param[in] CSPin - the selected CS pin
 * @return    none
 */


void spi_masterCSpin_select(GPIO_PinTypeDef CSPin)
{
	gpio_set_func(CSPin,AS_GPIO);//enable GPIO function
	gpio_set_input_en(CSPin,0); //disable input function
	gpio_set_output_en(CSPin,1);//enable out put
	gpio_write(CSPin,1);//output high level in idle state
}

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] DivClock - the division factor for SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] Mode - the selected working mode of SPI module
 *            Telink spi supports four standard working modes
 *            register  0x0b set working mode
 *            bit0:CPOL-Clock Polarity  ; bit1:CPHA-Clock Phase
 *            MODE0: CPOL = 0 , CPHA =0;
 *            MODE1: CPOL = 0 , CPHA =1;
 *            MODE2: CPOL = 1 , CPHA =0;
 *            MODE3: CPOL = 1 , CPHA =1;
 * @return    none
 */
void spi_master_init(unsigned char DivClock, SPI_ModeTypeDef Mode)
{
	reg_clk_en0 |= FLD_CLK0_SPI_EN;//enable spi clock
	reg_spi_sp = 0;            //clear register(0x0a) value
	reg_spi_sp|= DivClock;     //0x0a: bit0~bit6 set spi clock ; spi clock=system clock/((DivClock+1)*2)
	reg_spi_sp|=FLD_SPI_ENABLE;//0x0a: bit7 enables spi function mode
	reg_spi_ctrl|= FLD_SPI_MASTER_MODE_EN; //0x09: bit1 enables master mode
	reg_spi_inv_clk	&= (~FLD_SPI_MODE_WORK_MODE); // clear spi working mode
	reg_spi_inv_clk |= Mode;// select SPI mode,surpport four modes
}


/**
 * @brief      This function serves to write a bulk of data to the SPI slave
 *             device specified by the CS pin
 * @param[in]  Cmd - pointer to the command bytes needed written into the
 *             slave device first before the writing operation of actual data
 * @param[in]  CmdLen - length in byte of the command bytes
 * @param[in]  Data - pointer to the data need to write
 * @param[in]  DataLen - length in byte of the data need to write
 * @param[in]  CSPin - the CS pin specifing the slave device
 * @return     none
 */
void spi_write(unsigned char *Cmd, int CmdLen, unsigned char *Data, int DataLen, GPIO_PinTypeDef CSPin)
{
    int i = 0;
    gpio_write(CSPin,0);//CS level is low

    reg_spi_ctrl &= ~FLD_SPI_DATA_OUT_DIS; //0x09- bit2 enables spi data output
    reg_spi_ctrl &= ~FLD_SPI_RD; //enable write,0x09-bit3 : 0 for read ,1 for write

    /***write Cmd***/
    for (i = 0; i < CmdLen; i++) {

    	reg_spi_data = Cmd[i];
        while(reg_spi_ctrl& FLD_SPI_BUSY); //wait writing finished
    }

    /***write Data***/
    for (i = 0; i < DataLen; i++) {
    	reg_spi_data = Data[i];
        while(reg_spi_ctrl & FLD_SPI_BUSY); //wait writing finished
    }
    /***pull up CS***/
    gpio_write(CSPin,1);//CS level is high
}

/**
 * @brief      This function serves to read a bulk of data from the SPI slave
 *             device specified by the CS pin
 * @param[in]  Cmd - pointer to the command bytes needed written into the
 *             slave device first before the reading operation of actual data
 * @param[in]  CmdLen - length in byte of the command bytes
 * @param[out] Data - pointer to the buffer that will cache the reading out data
 * @param[in]  DataLen - length in byte of the data need to read
 * @param[in]  CSPin - the CS pin specifing the slave device
 * @return     none
 */
void spi_read(unsigned char *Cmd, int CmdLen, unsigned char *Data, int DataLen, GPIO_PinTypeDef CSPin)
{
    int i = 0;
    unsigned char temp = 0;
    gpio_write(CSPin,0); //CS level is low
    reg_spi_ctrl &= ~FLD_SPI_DATA_OUT_DIS; ////0x09- bit2 enables spi data output

    /***write cmd***/
    for (i = 0; i < CmdLen; i++) {
    	reg_spi_data = Cmd[i];
        while(reg_spi_ctrl& FLD_SPI_BUSY );//wait writing finished
    }

    reg_spi_ctrl |=FLD_SPI_DATA_OUT_DIS;

    /***when the read_bit was set 1,you can read 0x800008 to take eight clock cycle***/
    reg_spi_ctrl |= FLD_SPI_RD; //enable read,0x09-bit3 : 0 for read ,1 for write
    temp = reg_spi_data; //first byte isn't useful data,only take 8 clock cycle
    while(reg_spi_ctrl &FLD_SPI_BUSY ); //wait reading finished

    /***read data***/
    for (i = 0; i < DataLen; i++) {
        Data[i] = reg_spi_data; //take 8 clock cycles
        while(reg_spi_ctrl & FLD_SPI_BUSY ); //wait reading finished
    }
    //pull up CS
    gpio_write(CSPin,1);//CS level is high
}

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] DivClock - the division factor for SPI module
 *            SPI clock = System clock / ((DivClock+1)*2)
 * @param[in] Mode - the selected working mode of SPI module
 *            Telink spi supports four standard working modes
 *            register  0x0b set working mode
 *            bit0:CPOL-Clock Polarity  ; bit1:CPHA-Clock Phase
 *            MODE0: CPOL = 0 , CPHA =0;
 *            MODE1: CPOL = 0 , CPHA =1;
 *            MODE2: CPOL = 1 , CPHA =0;
 *            MODE3: CPOL = 1 , CPHA =1;
 * @return    none
 */
void spi_slave_init(unsigned char DivClock, SPI_ModeTypeDef Mode)
{
	reg_clk_en0 |= FLD_CLK0_SPI_EN;//enable spi clock
	reg_spi_sp = 0;            //clear register(0x0a) value
	reg_spi_sp|= DivClock;     //0x0a: bit0~bit6 set spi clock ; spi clock=system clock/((DivClock+1)*2)
	reg_spi_sp|=FLD_SPI_ENABLE;//0x0a: bit7 enables spi function mode
	reg_spi_ctrl&= (~FLD_SPI_MASTER_MODE_EN ); //disable master mode
	reg_spi_inv_clk &= (~FLD_SPI_MODE_WORK_MODE);
	reg_spi_inv_clk|= Mode; //select SPI mode,surpport four modes
}
/**
 * @brief     This function selects a pin port for the SPI interface (slave mode)
 * @param[in] PinGrp - the selected pin port
 * @return    none
 *  A3:5b7[0] set 1 as spi input,set 0 not as spi input ;5b7[4] set 1 as i2c input ,set 0 not as i2c input
 *	A4:5b7[1] set 1 as spi input,set 0 not as spi input ;5b7[5] set 1 as i2c input ,set 0 not as i3c input
 *	B6:5b7[2] set 1 as spi input,set 0 not as spi input ;5b7[6] set 1 as i2c input ,set 0 not as i4c input
 *	D7:5b7[3] set 1 as spi input,set 0 not as spi input ;5b7[7] set 1 as i2c input ,set 0 not as i5c input
 *	5b6[7:4]  set spi or i2c output,5b6[5:4] PA spi sel, 5b6[6] PB spi sel,5b6[7] PD spi_sel
 *	spi i2c mux sel:
 *	step1:close gpio func;
 *	step2:set 0x5a8-0x5af sel spii2c func;
 *	step3:set 5b6[7:4] to sel spi or i2c output;
 *	step4:set 5b7[7:0] to sel spi or i2c input;
 *	step5 if use pd[6]as spi slave csn,need to open pd[2] gpio func,because pd[2]have high priority than pd[6]
 */

void spi_slave_gpio_set(SPI_GPIO_GroupTypeDef PinGrp)
{
	GPIO_PinTypeDef sclk=GPIO_PA4;
	GPIO_PinTypeDef cs=GPIO_PD6;
	GPIO_PinTypeDef sdo=GPIO_PA2;
	GPIO_PinTypeDef sdi=GPIO_PA3;

    if (PinGrp == SPI_GPIO_GROUP_A2A3A4D6)
    {
		 sclk = GPIO_PA4;
		 cs   = GPIO_PD6;
		 sdo  = GPIO_PA2;
		 sdi  = GPIO_PA3;

		 reg_pin_i2c_spi_out_en |= (FLD_PIN_PAGROUP_SPI_EN|FLD_PIN_PDGROUP_SPI_EN);
		 reg_pin_i2c_spi_en |= (FLD_PIN_PA3_SPI_EN | FLD_PIN_PA4_SPI_EN);
		 reg_pin_i2c_spi_en &= ~(FLD_PIN_PA3_I2C_EN | FLD_PIN_PA4_I2C_EN);
    }
    else if (PinGrp ==SPI_GPIO_GROUP_B6B7D2D7)
    {
		 sclk = GPIO_PD7;
		 cs   = GPIO_PD2;
		 sdo  = GPIO_PB7;
		 sdi  = GPIO_PB6;
		 reg_pin_i2c_spi_out_en |= (FLD_PIN_PBGROUP_SPI_EN|FLD_PIN_PDGROUP_SPI_EN);
		 reg_pin_i2c_spi_en |= (FLD_PIN_PB6_SPI_EN | FLD_PIN_PD7_SPI_EN);
		 reg_pin_i2c_spi_en &= ~(FLD_PIN_PB6_I2C_EN | FLD_PIN_PD7_I2C_EN);
    }
	gpio_set_func(sclk ,AS_SPI);
	gpio_set_func(cs   ,AS_SPI);
	gpio_set_func(sdo  ,AS_SPI);
	gpio_set_func(sdi  ,AS_SPI);
	gpio_set_input_en(sclk, 1);
	gpio_set_input_en(cs, 1);
	gpio_set_input_en(sdi, 1);
}


/**
 * @brief 		This function use to set the share mode of SPI.
 * @param[in] 	none
 * @return 		none
 */
void spi_set_share_mode_en(void)
{
	reg_spi_ctrl |= FLD_SPI_SHARE_MODE; //0x09:bit[5] enable share mode
}
