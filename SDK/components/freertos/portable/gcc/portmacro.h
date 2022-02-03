/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers/8258/bsp.h"

extern void vPortYield(void);
extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);
extern void vportSUPPRESS_TICKS_AND_SLEEP(uint32_t xExpectedIdleTime);

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	unsigned long
#define portBASE_TYPE	portLONG

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;


#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
	not need to be guarded with a critical section. */
	#define portTICK_TYPE_IS_ATOMIC 1
#endif

/*-----------------------------------------------------------*/
/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			4
/*-----------------------------------------------------------*/

#define portDISABLE_INTERRUPTS()				( *((volatile char*) (0x800643)) = 0)//write_reg8(0x800643,0x00)
#define portENABLE_INTERRUPTS()					{ *((volatile char*) (0x800643)) = 1; asm("tnop"); asm("tnop"); asm("tnop"); asm("tnop"); asm("tnop"); asm("tnop"); asm("tnop"); asm("tnop");}//write_reg8(0x800643,0x01)

#define portENTER_CRITICAL()					vPortEnterCritical()
#define portEXIT_CRITICAL()						vPortExitCritical()
#define portYIELD()								vPortYield()			// asm("tloadr r1, =taskLR"); asm("tmov r0, r14"); asm("tstorer r0, [r1]"); 

/* Tickless idle/low power functionality. */
#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime )	vportSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime)

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) 	void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) 		void vFunction( void *pvParameters )

#define portNOP()  asm("tnop")

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

