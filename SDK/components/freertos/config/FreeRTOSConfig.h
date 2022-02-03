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


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* 1： 抢占式调度 0：协作式 */
#define configUSE_PREEMPTION                                                      1

/* tc32 硬件不支持这类方法 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION                                   0

/* 将configUSE_TICKLESS_IDLE设置为1以使用 低功耗无滴答模式，或设置为0以使
 * 滴答中断始终保持运行。并非所有FreeRTOS端口都提供低功耗无滴答实现。*/
#define configUSE_TICKLESS_IDLE                                                   1

/* See into vPortSuppressTicksAndSleep source code for explanation */
#define configUSE_TICKLESS_IDLE_SIMPLE_DEBUG                                      0

/* 系统时钟 */
#define configCPU_CLOCK_HZ                                                        (16000000UL)  /*SystemCoreClock*/

/* 滴答中断的频率 */
#define configTICK_RATE_HZ                                                        1000

/* 应用程序任务可用 的优先级数 */
#define configMAX_PRIORITIES                                                      ( 6 )

/* 空闲任务使用的堆栈大小 单位为 *4 byte */
#define configMINIMAL_STACK_SIZE                                                  ( 96 )

/*FreeRTOS堆中可用的RAM(堆)总量 */
#define configTOTAL_HEAP_SIZE                                                     ( 4096*2 )

/* task 名字长度 */
#define configMAX_TASK_NAME_LEN                                                   ( 8 )

/* 时间以'ticks'来衡量 - 这是自RTOS内核启动以来tick中断执行的次数
 * 将configUSE_16_BIT_TICKS定义为1会导致TickType_t被定义（typedef'ed）
 * 为无符号16位类型。将configUSE_16_BIT_TICKS定义为0会导致TickType_t被定义
 * （typedef'ed）为无符号32位类型 */
#define configUSE_16_BIT_TICKS                                                    0

/* 在执行与空闲任务相同优先级的任务过程中，空闲任务是否具有抢占的机会 */
#define configIDLE_SHOULD_YIELD                                                   0

/* 互斥锁使能 */
#define configUSE_MUTEXES                                                         0

/* 递归互斥锁使能 */
#define configUSE_RECURSIVE_MUTEXES                                               0

/* 信号量使能 */
#define configUSE_COUNTING_SEMAPHORES                                             1

/* 已经弃用 */
#define configUSE_ALTERNATIVE_API                                                 0    /* Deprecated! */

/* 定义可以注册的最大队列和信号量*/
#define configQUEUE_REGISTRY_SIZE                                                 2

/* 队列集使能*/
#define configUSE_QUEUE_SETS                                                      0

/* 时间片使能*/
#define configUSE_TIME_SLICING                                                    1

/* newlib使能*/
#define configUSE_NEWLIB_REENTRANT                                                0

#define configENABLE_BACKWARD_COMPATIBILITY                                       1

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                                                       0
#define configUSE_TICK_HOOK                                                       0

/* 堆栈检测*/
#define configCHECK_FOR_STACK_OVERFLOW                                            0

/* 堆内存申请钩子检测*/
#define configUSE_MALLOC_FAILED_HOOK                                              0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS                                             0
#define configUSE_TRACE_FACILITY                                                  0
#define configUSE_STATS_FORMATTING_FUNCTIONS                                      0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                                                     0
#define configMAX_CO_ROUTINE_PRIORITIES                                           ( 2 )

/* Software timer definitions. */
/* 软定时器使能*/
#define configUSE_TIMERS                                                          1

/* 软定时器优先级*/
#define configTIMER_TASK_PRIORITY                                                 ( 2 )

/* 软定时器队列长度*/
#define configTIMER_QUEUE_LENGTH                                                  32

/* 软定时器队列堆栈深度*/
#define configTIMER_TASK_STACK_DEPTH                                              ( 80 )

/* Tickless Idle configuration. */
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP                                     3		//  sleep duration must  greater than 3 ms


/* Define to trap errors during development. */
#define configASSERT( x )                                                         //ASSERT(x)

/* FreeRTOS MPU specific definitions. */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS                    0

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                                                  1
#define INCLUDE_uxTaskPriorityGet                                                 1
#define INCLUDE_vTaskDelete                                                       1
#define INCLUDE_vTaskSuspend                                                      1
#define INCLUDE_xResumeFromISR                                                    1
#define INCLUDE_vTaskDelayUntil                                                   1
#define INCLUDE_vTaskDelay                                                        1
#define INCLUDE_xTaskGetSchedulerState                                            1
#define INCLUDE_xTaskGetCurrentTaskHandle                                         1
#define INCLUDE_uxTaskGetStackHighWaterMark                                       1
#define INCLUDE_xTaskGetIdleTaskHandle                                            1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle                                    1
#define INCLUDE_pcTaskGetTaskName                                                 1
#define INCLUDE_eTaskGetState                                                     1
#define INCLUDE_xEventGroupSetBitFromISR                                          1
#define INCLUDE_xTimerPendFunctionCall                                            1

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
//#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    1

/* Code below should be only used by the compiler, and not the assembler. */
#if !(defined(__ASSEMBLY__) || defined(__ASSEMBLER__))

#endif /* !assembler */

#endif /* FREERTOS_CONFIG_H */
