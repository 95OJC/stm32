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
 ��Ϊ1�������ռʽ����������Ϊ0�����Э��ʽ������
 */
#define configUSE_PREEMPTION                          1
/*
 ����MCU�ں˵Ĺ���Ƶ�ʣ���HZΪ��λ
 */
#define configCPU_CLOCK_HZ                            ( SystemCoreClock )
/*
 ʱ��Ƶ�ʣ���ϵͳ��વ��ٶ�
 */
#define configTICK_RATE_HZ                            ( ( TickType_t ) 1000 )
/*
 �����������ȼ�
 */
#define configMAX_PRIORITIES                          ( 10 )
/*
�����ջ����С��С
 */
#define configMINIMAL_STACK_SIZE                      ( ( unsigned short ) 1024 )
/*
 ���öѿռ�Ĵ�С��ֻ�е������в���FreeRTOS�ṩ���ڴ�
 �����㷨ʱ�Ż��õ�����heap_x.h
	freertos������亯���Ŀ���������
 */
#define configTOTAL_HEAP_SIZE                         ( ( size_t ) ( 64 * 1024 ) )
/*
 ����������󳤶�
 */
#define configMAX_TASK_NAME_LEN                       ( 16 )
/*
 �� configUSE_16_BIT_TICKS��Ϊ 1��portTickType ��������Ϊ�޷��ŵ�16λ�������ͣ�
 configUSE_16_BIT_TICKS ��Ϊ0 ��portTickType �򱻶���Ϊ�޷��ŵ�32λ���͡�
 */
#define configUSE_16_BIT_TICKS                        0
/*
 �������������Щ���ȼ���idle ������ͬ���������Ϊ������ֻ�е��ں˱�����
 Ϊ��ռʽ�������ʱ����ʵ�����á��ں˶Ծ���ͬ�����ȼ�����������ʱ��Ƭ
 ��ת�����㷨������������ȼ�����idle����ʱ����������ֵ���ʱ��Ƭ��ͬ����С�ġ�
 ������������ȼ���idle������ͬʱ�������Щ��ͬ�ˡ���configIDLE_SHOULD_YIELD 
 ������Ϊ1ʱ�����κ����ȼ���idle ������ͬ�������ھ���̬ʱ��idle���������Ҫ���
 �������������л������ʹidle����ռ�����ٵ�CPUʱ�䣬��ͬʱ��ʹ�����ȼ���idle ������
 ͬ�������õ�ʱ��Ƭ����ͬ����С�ġ���Ϊidle�����ռ��ĳ������Ĳ���ʱ��Ƭ��
 */
#define configIDLE_SHOULD_YIELD                       1
//�����ź���
#define configUSE_MUTEXES                             1
//�ݹ��ź���
#define configUSE_RECURSIVE_MUTEXES                   1
//��ֵ�ź���
#define configUSE_COUNTING_SEMAPHORES                 1
//��Ϣ����ע�������
#define configQUEUE_REGISTRY_SIZE                     8

#define configUSE_APPLICATION_TASK_TAG                0


/* hook function related definitions */
/*
 ��Ϊ1��ʹ��idle hook��Ϊ0���ֹidle hook
 */
#define configUSE_IDLE_HOOK                           0
/*
 ��Ϊ1��ʹ��tick hook ��Ϊ0���ֹtick hook
 */
#define configUSE_TICK_HOOK                           0

#define configCHECK_FOR_STACK_OVERFLOW                0

#define configUSE_MALLOC_FAILED_HOOK                  0

/* run time and task stats gathering related definitions */
#define configGENERATE_RUN_TIME_STATS                 0
/*
 �Ƿ�ʹ��TRACE���ܣ�����TRACE���ܺ�RAMռ����������ࣿ
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
