/********************************************************************************************************
 * @file     timer.h
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

#ifndef TIMER_H_
#define TIMER_H_
#include "compiler.h"
#include "register.h"
#include "analog.h"
#include "gpio.h"

/**
 * @brief   Type of Timer
 */
typedef enum{
	TIMER0		=0,
	TIMER1		=1,
	TIMER2		=2,
}TIMER_TypeDef;

/**
 * @brief   Mode of Timer
 */
typedef enum{
	TIMER_MODE_SYSCLK		=0,
	TIMER_MODE_GPIO_TRIGGER	=1,
	TIMER_MODE_GPIO_WIDTH	=2,
	TIMER_MODE_TICK			=3,
}TIMER_ModeTypeDef;

#define	    sys_tick_per_us   				16
#define		CLOCK_SYS_CLOCK_1250US			(1250 * sys_tick_per_us)

/**
 * @brief   system Timer : 16Mhz, Constant
 */
enum{
	CLOCK_16M_SYS_TIMER_CLK_1S  =  16*1000*1000,
	CLOCK_16M_SYS_TIMER_CLK_1MS = 16*1000,
	CLOCK_16M_SYS_TIMER_CLK_1US = 16,
};

/**
 * @brief     This function performs to gets system timer0 address.
 * @param[in] none.
 * @return    timer0 address.
 */

static inline unsigned long clock_time(void)
{
	return reg_system_tick;
}




/**
 * @brief     This function performs to set sleep us.
 * @param[in] us - microseconds need to delay.
 * @return    none
 */

extern void sleep_us(unsigned long us);

#define ClockTime			clock_time
#define WaitUs				sleep_us
#define WaitMs(t)			sleep_us((t)*1000)
#define sleep_ms(t)			sleep_us((t)*1000)

/**
 * @brief     This function performs to calculation exceed us of the timer.
 * @param[in] ref - Variable of reference timer address.
 * @param[in] span_us - Variable of span us.
 * @return    the exceed.
 */

static inline unsigned int clock_time_exceed(unsigned int ref, unsigned int us)
{
	return ((unsigned int)(clock_time() - ref) > us * 16);
}

/**
 * @brief     initiate GPIO for gpio trigger and gpio width mode of timer0.
 * @param[in] pin - select pin for timer0.
 * @param[in] pol - select polarity for gpio trigger and gpio width
 * @return    none
 */
extern void timer0_gpio_init(GPIO_PinTypeDef pin, GPIO_PolTypeDef pol);

/**
 * @brief     initiate GPIO for gpio trigger and gpio width mode of timer1.
 * @param[in] pin - select pin for timer1.
 * @param[in] pol - select polarity for gpio trigger and gpio width
 * @return    none
 */
extern void timer1_gpio_init(GPIO_PinTypeDef pin, GPIO_PolTypeDef pol);

/**
 * @brief     initiate GPIO for gpio trigger and gpio width mode of timer2.
 * @param[in] pin - select pin for timer2.
 * @param[in] pol - select polarity for gpio trigger and gpio width
 * @return    none
 */
extern void timer2_gpio_init(GPIO_PinTypeDef pin,GPIO_PolTypeDef pol);

/**
 * @brief     set mode, initial tick and capture of timer0.
 * @param[in] mode - select mode for timer0.
 * @param[in] init_tick - initial tick.
 * @param[in] cap_tick  - tick of capture.
 * @return    none
 */
extern void timer0_set_mode(TIMER_ModeTypeDef mode,unsigned int init_tick, unsigned int cap_tick);

/**
 * @brief     set mode, initial tick and capture of timer1.
 * @param[in] mode - select mode for timer1.
 * @param[in] init_tick - initial tick.
 * @param[in] cap_tick  - tick of capture.
 * @return    none
 */
extern void timer1_set_mode(TIMER_ModeTypeDef mode,unsigned int init_tick, unsigned int cap_tick);

/**
 * @brief     set mode, initial tick and capture of timer2.
 * @param[in] mode - select mode for timer2.
 * @param[in] init_tick - initial tick.
 * @param[in] cap_tick  - tick of capture.
 * @return    none
 */
extern void timer2_set_mode(TIMER_ModeTypeDef mode,unsigned int init_tick, unsigned int cap_tick);

/**
 * @brief     the specifed timer start working.
 * @param[in] type - select the timer to start.
 * @return    none
 */
extern void timer_start(TIMER_TypeDef type);

/**
 * @brief     the specifed timer stop working.
 * @param[in] type - select the timer to stop.
 * @return    none
 */
extern void timer_stop(TIMER_TypeDef type);



#endif /* TIMER_H_ */
