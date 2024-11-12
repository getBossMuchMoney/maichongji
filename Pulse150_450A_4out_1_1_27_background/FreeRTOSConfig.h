/*
 * Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
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
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

//--------------------------------------------------------------------------------------------------
// Application specific definitions.
//
// These definitions should be adjusted for your particular hardware and
// application requirements.
//
// THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
// FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
//
// See http://www.freertos.org/a00110.html.
//--------------------------------------------------------------------------------------------------

#define configUSE_PREEMPTION                1
#define configUSE_IDLE_HOOK                 0
#define configUSE_TICK_HOOK                 1//0
#define configCPU_CLOCK_HZ                  ( ( unsigned long ) 120000000 )
#define configTICK_RATE_HZ                  ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                ( 16 )
#define configMINIMAL_STACK_SIZE            ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE               ( ( size_t ) ( 10 * 1024 ) )
#define configMAX_TASK_NAME_LEN             ( 16 )
#define configUSE_TRACE_FACILITY            0
#define configUSE_16_BIT_TICKS              0
#define configIDLE_SHOULD_YIELD             0
#define configCHECK_FOR_STACK_OVERFLOW      1
#define configSUPPORT_STATIC_ALLOCATION     1
#define configSUPPORT_DYNAMIC_ALLOCATION    0

#define configGENERATE_RUN_TIME_STATS       0
// Set the following definitions to 1 to include the API function, or zero
// to exclude the API function.

#define INCLUDE_vTaskPrioritySet            0
#define INCLUDE_uxTaskPriorityGet           0
#define INCLUDE_vTaskDelete                 0
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                0
#define INCLUDE_vTaskDelayUntil             0
#define INCLUDE_vTaskDelay                  1


#if ( configGENERATE_RUN_TIME_STATS == 1 )
#define INCLUDE_xTaskGetIdleTaskHandle      1
extern void configRunTimerTick( void );
extern uint32_t GetRunTimerTick(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()     configRunTimerTick()
#define portGET_RUN_TIME_COUNTER_VALUE()             GetRunTimerTick()
#endif /* configGENERATE_RUN_TIME_STATS */

#endif /* FREERTOS_CONFIG_H */
