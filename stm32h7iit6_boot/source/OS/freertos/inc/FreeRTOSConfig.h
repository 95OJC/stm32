/*
 * FreeRTOS Kernel V10.5.1
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#if  defined(__ICCARM__) || defined(__CC_ARM) || defined(__TASKING__) || defined(__GNUC__)
#include <stdint.h>
extern volatile uint32_t SystemCoreClock;
#endif

/*
 设为1则采用抢占式调度器，设为0则采用协作式调度器
 */
#define configUSE_PREEMPTION                          1
/*
 设置MCU内核的工作频率，以HZ为单位
 */
#define configCPU_CLOCK_HZ                            ( SystemCoreClock )
/*
 时钟频率，即系统嘀嗒的速度
 */
#define configTICK_RATE_HZ                            ( ( TickType_t ) 1000 )
/*
 任务最大的优先级
 */
#define configMAX_PRIORITIES                          ( 10 )
/*
任务堆栈的最小大小
 */
#define configMINIMAL_STACK_SIZE                      ( ( unsigned short ) 1024 )
/*
 设置堆空间的大小，只有当程序中采用FreeRTOS提供的内存
 分配算法时才会用到，即heap_x.h
	freertos任务和其函数的开销！！！
 */
#define configTOTAL_HEAP_SIZE                         ( ( size_t ) ( 64 * 1024 ) )
/*
 任务名称最大长度
 */
#define configMAX_TASK_NAME_LEN                       ( 16 )
/*
 将 configUSE_16_BIT_TICKS设为 1后portTickType 将被定义为无符号的16位整形类型，
 configUSE_16_BIT_TICKS 设为0 后portTickType 则被定义为无符号的32位整型。
 */
#define configUSE_16_BIT_TICKS                        0
/*
 这个参数控制那些优先级与idle 任务相同的任务的行为，并且只有当内核被配置
 为抢占式任务调度时才有实际作用。内核对具有同样优先级的任务会采用时间片
 轮转调度算法。当任务的优先级高于idle任务时，各个任务分到的时间片是同样大小的。
 但当任务的优先级与idle任务相同时情况就有些不同了。当configIDLE_SHOULD_YIELD 
 被配置为1时，当任何优先级与idle 任务相同的任务处于就绪态时，idle任务会立刻要求调
 度器进行任务切换。这会使idle任务占用最少的CPU时间，但同时会使得优先级与idle 任务相
 同的任务获得的时间片不是同样大小的。因为idle任务会占用某个任务的部分时间片。
 */
#define configIDLE_SHOULD_YIELD                       1
//互斥信号量
#define configUSE_MUTEXES                             1
//递归信号量
#define configUSE_RECURSIVE_MUTEXES                   1
//二值信号量
#define configUSE_COUNTING_SEMAPHORES                 1
//消息队列注册的数量
#define configQUEUE_REGISTRY_SIZE                     8

#define configUSE_APPLICATION_TASK_TAG                0


/* hook function related definitions */
/*
 设为1则使能idle hook设为0则禁止idle hook
 */
#define configUSE_IDLE_HOOK                           0
/*
 设为1则使能tick hook 设为0则禁止tick hook
 */
#define configUSE_TICK_HOOK                           0

#define configCHECK_FOR_STACK_OVERFLOW                0

#define configUSE_MALLOC_FAILED_HOOK                  0

/* run time and task stats gathering related definitions */
#define configGENERATE_RUN_TIME_STATS                 0
/*
 是否使用TRACE功能，开启TRACE功能后RAM占用量增大许多？
 */
#define configUSE_TRACE_FACILITY                      0//1


/* co-routine definitions */
#define configUSE_CO_ROUTINES                         0
#define configMAX_CO_ROUTINE_PRIORITIES               ( 2 )

/* software timer definitions */
#define configUSE_TIMERS                              1
#define configTIMER_TASK_PRIORITY                     ( 2 )
#define configTIMER_QUEUE_LENGTH                      10
#define configTIMER_TASK_STACK_DEPTH                  ( configMINIMAL_STACK_SIZE * 2 )

/* set to 1 to include the API function, or 0 to exclude the API function */
#define INCLUDE_vTaskPrioritySet                      0//1
#define INCLUDE_uxTaskPriorityGet                     1
#define INCLUDE_vTaskDelete                           0//1
#define INCLUDE_vTaskCleanUpResources                 0
#define INCLUDE_vTaskSuspend                          1
#define INCLUDE_vTaskDelayUntil                       1
#define INCLUDE_vTaskDelay                            1

#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_xTaskGetIdleTaskHandle 	1

/* Cortex-M specific definitions */
#ifdef __NVIC_PRIO_BITS
    /* __NVIC_PRIO_BITS will be specified when CMSIS is being used. */
    #define configPRIO_BITS                           __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS                           4        /* 15 priority levels */
#endif

/* the lowest interrupt priority that can be used in a call to a "set priority" function */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  Do not call
interrupt safe freertos api functions from any interrupt that has a higher
priority than this! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY   2

/* interrupt priorities used by the kernel port layer itself */
#define configKERNEL_INTERRUPT_PRIORITY                ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY           32 //( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )



/* normal assert() semantics without relying on the provision of an assert.h header file */
#define configASSERT( x )                              if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); } 

/* map the FreeRTOS port interrupt handlers to CMSIS standard names */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler



#endif /* FREERTOS_CONFIG_H */
