/********************************************************************************************************
 * @file     gpio_8258.h 
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

#include "register.h"
#include "gpio.h"
#include "analog.h"


/**
 *  @brief  Define GPIO types
 */

typedef enum{
		GPIO_GROUPA    = 0x000,
		GPIO_GROUPB    = 0x100,
		GPIO_GROUPC    = 0x200,
		GPIO_GROUPD    = 0x300,
		GPIO_GROUPE    = 0x400,

	    GPIO_PA0 = GPIO_GROUPA | BIT(0),
		GPIO_PA1 = GPIO_GROUPA | BIT(1),
		GPIO_PA2 = GPIO_GROUPA | BIT(2),
		GPIO_PA3 = GPIO_GROUPA | BIT(3),
		GPIO_PA4 = GPIO_GROUPA | BIT(4),
		GPIO_PA5 = GPIO_GROUPA | BIT(5), 	GPIO_DM=GPIO_PA5,
		GPIO_PA6 = GPIO_GROUPA | BIT(6),  GPIO_DP=GPIO_PA6,
		GPIO_PA7 = GPIO_GROUPA | BIT(7), 	GPIO_SWS=GPIO_PA7,
		GPIOA_ALL = GPIO_GROUPA | 0x00ff,

		GPIO_PB0 = GPIO_GROUPB | BIT(0),
		GPIO_PB1 = GPIO_GROUPB | BIT(1),
		GPIO_PB2 = GPIO_GROUPB | BIT(2),
		GPIO_PB3 = GPIO_GROUPB | BIT(3),
		GPIO_PB4 = GPIO_GROUPB | BIT(4),
		GPIO_PB5 = GPIO_GROUPB | BIT(5),
		GPIO_PB6 = GPIO_GROUPB | BIT(6),
		GPIO_PB7 = GPIO_GROUPB | BIT(7),

		GPIO_PC0 = GPIO_GROUPC | BIT(0),
		GPIO_PC1 = GPIO_GROUPC | BIT(1),
		GPIO_PC2 = GPIO_GROUPC | BIT(2),
		GPIO_PC3 = GPIO_GROUPC | BIT(3),
		GPIO_PC4 = GPIO_GROUPC | BIT(4),
		GPIO_PC5 = GPIO_GROUPC | BIT(5),
		GPIO_PC6 = GPIO_GROUPC | BIT(6),
		GPIO_PC7 = GPIO_GROUPC | BIT(7),

		GPIO_PD0 = GPIO_GROUPD | BIT(0),
		GPIO_PD1 = GPIO_GROUPD | BIT(1),
		GPIO_PD2 = GPIO_GROUPD | BIT(2),
		GPIO_PD3 = GPIO_GROUPD | BIT(3),
		GPIO_PD4 = GPIO_GROUPD | BIT(4),
		GPIO_PD5 = GPIO_GROUPD | BIT(5),
		GPIO_PD6 = GPIO_GROUPD | BIT(6),
		GPIO_PD7 = GPIO_GROUPD | BIT(7),

		GPIO_PE0 = GPIO_GROUPE | BIT(0),  GPIO_MSDO=GPIO_PE0,
		GPIO_PE1 = GPIO_GROUPE | BIT(1),  GPIO_MCLK=GPIO_PE1,
		GPIO_PE2 = GPIO_GROUPE | BIT(2),  GPIO_MSCN=GPIO_PE2,
		GPIO_PE3 = GPIO_GROUPE | BIT(3),  GPIO_MSDI=GPIO_PE3,

		GPIO_ALL = 0x500,
}GPIO_PinTypeDef;

/**
 *  @brief  Define GPIO Function types
 */

typedef enum{
	AS_GPIO 	=  0,
	AS_MSPI 	=  1,
	AS_SWIRE	=  2,
	AS_UART		=  3,
	AS_I2C		=  4,
	AS_SPI		=  5,
	AS_I2S		=  6,
	AS_AMIC		=  7,
	AS_DMIC		=  8,
	AS_SDM		=  9,
	AS_USB		= 10,
	AS_ADC		= 11,
	AS_CMP		= 12,
	AS_ATS		= 13,

	AS_PWM0 	= 20,
	AS_PWM1		= 21,
	AS_PWM2 	= 22,
	AS_PWM3		= 23,
	AS_PWM4 	= 24,
	AS_PWM5		= 25,
	AS_PWM0_N	= 26,
	AS_PWM1_N	= 27,
	AS_PWM2_N	= 28,
	AS_PWM3_N	= 29,
	AS_PWM4_N	= 30,
	AS_PWM5_N	= 31,
}GPIO_FuncTypeDef;


typedef enum{
	Level_Low=0,
	Level_High =1,
}GPIO_LevelTypeDef;



/**
 *  @brief  Define rising/falling types
 */
typedef enum{
	POL_RISING   = 0,  pol_rising = 0,
	POL_FALLING  = 1,  pol_falling = 1,
}GPIO_PolTypeDef;

/**
 *  @brief  Define pull up or down types
 */
typedef enum {
	PM_PIN_UP_DOWN_FLOAT    = 0,
	PM_PIN_PULLUP_1M     	= 1,
	PM_PIN_PULLDOWN_100K  	= 2,
	PM_PIN_PULLUP_10K 		= 3,
}GPIO_PullTypeDef;


/**
 * @brief      This function servers to initialization all gpio.
 * @param[in]  en  -  if mcu wake up from deep retention mode, it is NOT necessary to reset analog register
 * @return     none.
 */
void gpio_init(int anaRes_init_en);

/**
 * @brief      This function servers to set the GPIO's function.
 * @param[in]  pin - the special pin.
 * @param[in]  func - the function of GPIO.
 * @return     none.
 */
void gpio_set_func(GPIO_PinTypeDef pin, GPIO_FuncTypeDef func);

/**
 * @brief      This function set the output function of a pin.
 * @param[in]  pin - the pin needs to set the output function
 * @param[in]  value - enable or disable the pin's output function(0: disable, 1: enable)
 * @return     none
 */
static inline void gpio_set_output_en(GPIO_PinTypeDef pin, unsigned int value)
{
	unsigned char	bit = pin & 0xff;
	if(!value){
		BM_SET(reg_gpio_oen(pin), bit);
	}else{
		BM_CLR(reg_gpio_oen(pin), bit);
	}
}
void gpio_set_func2(GPIO_PinTypeDef pin, GPIO_FuncTypeDef func);
/**
 * @brief      This function set the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function
 * @param[in]  value - enable or disable the pin's input function(0: disable, 1: enable)
 * @return     none
 */
void gpio_set_input_en(GPIO_PinTypeDef pin, unsigned int value);

/**
 * @brief      This function determines whether the output function of a pin is enabled.
 * @param[in]  pin - the pin needs to determine whether its output function is enabled.
 * @return     1: the pin's output function is enabled ;
 *             0: the pin's output function is disabled
 */
static inline int gpio_is_output_en(GPIO_PinTypeDef pin)
{
	return !BM_IS_SET(reg_gpio_oen(pin), pin & 0xff);
}

/**
 * @brief     This function determines whether the input function of a pin is enabled.
 * @param[in] pin - the pin needs to determine whether its input function is enabled.
 * @return    1: the pin's input function is enabled ;
 *            0: the pin's input function is disabled
 */
static inline int gpio_is_input_en(GPIO_PinTypeDef pin)
{
	return BM_IS_SET(reg_gpio_ie(pin), pin & 0xff);
}

/**
 * @brief     This function set the pin's output level.
 * @param[in] pin - the pin needs to set its output level
 * @param[in] value - value of the output level(1: high 0: low)
 * @return    none
 */
static inline void gpio_write(GPIO_PinTypeDef pin, unsigned int value)
{
	unsigned char	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_out(pin), bit);
	}else{
		BM_CLR(reg_gpio_out(pin), bit);
	}
}

/**
 * @brief     This function read the pin's input/output level.
 * @param[in] pin - the pin needs to read its level
 * @return    the pin's level(1: high 0: low)
 */
static inline unsigned int gpio_read(GPIO_PinTypeDef pin)
{
	return BM_IS_SET(reg_gpio_in(pin), pin & 0xff);
}
/**
 * @brief     This function read a pin's cache from the buffer.
 * @param[in] pin - the pin needs to read.
 * @param[in] p - the buffer from which to read the pin's level.
 * @return    the state of the pin.
 */
static inline unsigned int gpio_read_cache(GPIO_PinTypeDef pin, unsigned char *p)
{
	return p[pin>>8] & (pin & 0xff);
}

/**
 * @brief      This function read all the pins' input level.
 * @param[out] p - the buffer used to store all the pins' input level
 * @return     none
 */
static inline void gpio_read_all(unsigned char *p)
{
	p[0] = REG_ADDR8(0x580);
	p[1] = REG_ADDR8(0x588);
	p[2] = REG_ADDR8(0x590);
	p[3] = REG_ADDR8(0x598);
}

/**
 * @brief     This function set the pin toggle.
 * @param[in] pin - the pin needs to toggle
 * @return    none
 */
static inline void gpio_toggle(GPIO_PinTypeDef pin)
{
	reg_gpio_out(pin) ^= (pin & 0xFF);
}

/**
 * @brief      This function set the pin's driving strength.
 * @param[in]  pin - the pin needs to set the driving strength
 * @param[in]  value - the level of driving strength(1: strong 0: poor)
 * @return     none
 */
void gpio_set_data_strength(GPIO_PinTypeDef pin, unsigned int value);

/**
 * @brief     This function set a pin's pull-up/down resistor.
 * @param[in] gpio - the pin needs to set its pull-up/down resistor
 * @param[in] up_down - the type of the pull-up/down resistor
 * @return    none
 */

void gpio_setup_up_down_resistor(GPIO_PinTypeDef gpio, GPIO_PullTypeDef up_down);

/**
 * @brief      This function servers to set the specified GPIO as high resistor.
 * @param[in]  pin  - select the specified GPIO
 * @return     none.
 */
void gpio_shutdown(GPIO_PinTypeDef pin);

/**
 * @brief     This function set a pin's polarity that trig its IRQ.
 * @param[in] pin - the pin needs to set its edge polarity that trig its IRQ
 * @param[in] falling - value of the edge polarity(1: falling edge 0: rising edge)
 * @return    none
 */
static inline void gpio_set_interrupt_pol(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling)
{
	unsigned char	bit = pin & 0xff;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

/**
 * @brief     This function set a pin's IRQ.
 * @param[in] pin - the pin needs to enable its IRQ
 * @param[in] falling - value of the edge polarity(1: falling edge 0: rising edge)
 * @return    none
 */
static inline void gpio_set_interrupt(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling)
{
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	reg_irq_mask |= FLD_IRQ_GPIO_EN;
	reg_gpio_wakeup_irq |= FLD_GPIO_CORE_INTERRUPT_EN;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

/**
 * @brief     This function enables a pin's IRQ function.
 * @param[in] pin - the pin needs to enables its IRQ function.
 * @param[in] en - 1:enable 0:disable.
 * @return    none
 */
static inline void gpio_en_interrupt(GPIO_PinTypeDef pin, int en)   // reg_irq_mask: FLD_IRQ_GPIO_EN
{
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_wakeup_en(pin), bit);
	}
}

/**
 * @brief     This function set a pin's IRQ.
 * @param[in] pin - the pin needs to enable its IRQ
 * @param[in] falling - value of the edge polarity(1: falling edge 0: rising edge)
 * @return    none
 */
static inline void gpio_set_interrupt_risc0(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling){
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	reg_irq_mask |= FLD_IRQ_GPIO_RISC0_EN;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}


/**
 * @brief     This function enables a pin's IRQ function.
 * @param[in] pin - the pin needs to enables its IRQ function.
 * @param[in] en - 1 enable. 0 disable.
 * @return    none
 */
static inline void gpio_en_interrupt_risc0(GPIO_PinTypeDef pin, int en)  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
{
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc0_en(pin), bit);
	}
}


/**
 * @brief     This function set a pin's IRQ.
 * @param[in] pin - the pin needs to enable its IRQ
 * @param[in] falling - value of the edge polarity(1: falling edge 0: rising edge)
 * @return    none
 */
static inline void gpio_set_interrupt_risc1(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling)
{
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	reg_irq_mask |= FLD_IRQ_GPIO_RISC1_EN;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

/**
 * @brief     This function enables a pin's IRQ function.
 * @param[in] pin - the pin needs to enables its IRQ function.
 * @param[in] en - 1 enable. 0 disable.
 * @return    none
 */
static inline void gpio_en_interrupt_risc1(GPIO_PinTypeDef pin, int en)  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
{
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc1_en(pin), bit);
	}
}

/**
 * @brief      This function enables or disables the internal pull-up resistor
 *             of DP pin of USB interface
 * @param[in]  En - enables or disables the internal pull-up resistor(1: enable 0: disable)
 * @return     none
 */
static inline void usb_dp_pullup_en (int en)
{
	unsigned char dat = analog_read(0x0b);
	if (en) {
		dat = dat | BIT(7);
	}
	else
	{
		dat = dat & 0x7f ;
	}

	analog_write (0x0b, dat);
}

/**
 * @brief      This function serves to power on or down USB module
 * @param[in]  En - 1: power on 0: power down
 * @return     none
 */
static inline void usb_power_on(unsigned char en)
{
	if(en)
	{
		analog_write(0x34,analog_read(0x34)|0x02);
	}
	else
	{
		analog_write(0x34,analog_read(0x34)&0xfd);
	}
}
/**
 * @brief      This function serves to set GPIO MUX function as DP and DM pin of USB
 * @param[in]  none.
 * @return     none.
 */
static inline void usb_set_pin_en(void)
{
	gpio_set_func(GPIO_PA5, AS_USB);
	gpio_set_func(GPIO_PA6, AS_USB);
	usb_dp_pullup_en (1);
}
