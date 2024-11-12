/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : OsConfig.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _OsConfig_H_
#define _OsConfig_H_

#ifdef OsConfig_GLOBALS
#define OsConfig_EXT
#else
#define OsConfig_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "FreeRTOS.h"
#include "semphr.h"


/********************************************************************************
*const define                               *
********************************************************************************/
#define OS_ENTER_CRITICAL()     __asm(" setc INTM")
#define OS_EXIT_CRITICAL()      __asm(" clrc INTM")

#define Background_RATE_HZ      1000    //1000Hz

#define cTaskMax  16

#define cPrioSuper        9
#define cPrioParallel     8
#define cPrioCan          7
#define cPrioLoad         6
#define cPrioInterface    5
#define cPrioHMI          4
#define cPrioEEprom       3
#define cPrioDebug        2

#define BACKGROUND_LOOP_Enable  1
#define TaskTick_ISR_Enable  0
#define STACK_SIZE  256U


#define cTaskTimer1mS        1
#define cTaskTimer2mS        2
#define cTaskTimer3mS        3
#define cTaskTimer4mS        4
#define cTaskTimer5mS        5
#define cTaskTimer10mS       10
#define cTaskTimer20mS       20
#define cTaskTimer50mS       50
#define cTaskTimer100mS      100
#define cTaskTimer500mS      500
#define cTaskTimer1S         1000
#define cTaskTimer2S         2000

#define Tick1mS      (1/portTICK_PERIOD_MS)
#define Tick4mS      (4/portTICK_PERIOD_MS)
#define Tick5mS      (5/portTICK_PERIOD_MS)
#define Tick10mS      (10/portTICK_PERIOD_MS)
#define Tick20mS      (20/portTICK_PERIOD_MS)
#define Tick50mS      (50/portTICK_PERIOD_MS)
#define Tick100mS      (100/portTICK_PERIOD_MS)
#define Tick500mS      (500/portTICK_PERIOD_MS)
#define Tick1S      (1000/portTICK_PERIOD_MS)
#define Tick2S      (2000/portTICK_PERIOD_MS)


//Super task
#define eSuperTimer                    0
#define eSuperFault                    1
#define eSuperToDCMode                 2
#define eSuperToPulseMode              3
#define eSuperShutDown                 4
#define eSuperTurnOn                   5
#define eSuperTurnOff                  6



#define eSuperTest                     15

//Parallel task
#define eParallelTimer                    0

#define eParallelTest                    15


//Load task
#define eLoadTimer                    0

#define eLoadTest                    15

//Can task
#define eCanTimer					0
#define eCanPowerOn					1
#define eCanPowerOff				2
#define eCanTest					15

//EEprom task
#define eEEpromTimer                     0

#define eEEpromTest                     15

//HMI task
#define eHMITimer                     0

#define eHMITest                     15

//Interface task
#define eInterfaceTimer               0

#define eInterfaceTest               15

//Debug task
#define eDebugTimer                  0

#define eDebugTest                  15



/********************************************************************************
* Variable declaration                              *
********************************************************************************/
typedef struct
{
    uint16_t SuperTimer;
    uint16_t LoadTimer;
    uint16_t DebugTimer;
    uint16_t HMITimer;
    uint16_t EEpromTimer;
    uint16_t CanTimer;
    uint16_t InterfaceTimer;
    uint16_t ParallelTimer;


}TickPeriod;

OsConfig_EXT TickPeriod TimerTick;

OsConfig_EXT SemaphoreHandle_t xSemaphoreSuper;
OsConfig_EXT SemaphoreHandle_t xSemaphoreLoad;
OsConfig_EXT SemaphoreHandle_t xSemaphoreInterface;
OsConfig_EXT SemaphoreHandle_t xSemaphoreHMI;
OsConfig_EXT SemaphoreHandle_t xSemaphoreCan;
OsConfig_EXT SemaphoreHandle_t xSemaphoreEEprom;
OsConfig_EXT SemaphoreHandle_t xSemaphoreDebug;
OsConfig_EXT SemaphoreHandle_t xSemaphoreParallel;

OsConfig_EXT StaticTask_t SuperTaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(SuperTaskStack,2)
#endif
OsConfig_EXT StackType_t  SuperTaskStack[STACK_SIZE];
OsConfig_EXT StaticSemaphore_t xSemaphoreSuperBuffer;

OsConfig_EXT StaticTask_t LoadTaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(LoadTaskStack,2)
#endif
OsConfig_EXT StackType_t  LoadTaskStack[STACK_SIZE];
OsConfig_EXT StaticSemaphore_t xSemaphoreLoadBuffer;

OsConfig_EXT StaticTask_t InterfaceTaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(InterfaceTaskStack,2)
#endif
OsConfig_EXT StackType_t  InterfaceTaskStack[STACK_SIZE];
OsConfig_EXT StaticSemaphore_t xSemaphoreInterfaceBuffer;

OsConfig_EXT StaticTask_t HMITaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(HMITaskStack,2)
#endif
OsConfig_EXT StackType_t  HMITaskStack[STACK_SIZE];
OsConfig_EXT StaticSemaphore_t xSemaphoreHMIBuffer;

OsConfig_EXT StaticTask_t CanTaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(CanTaskStack,2)
#endif
OsConfig_EXT StackType_t  CanTaskStack[STACK_SIZE];
OsConfig_EXT StaticSemaphore_t xSemaphoreCanBuffer;

OsConfig_EXT StaticTask_t EEpromTaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(EEpromTaskStack,2)
#endif
OsConfig_EXT StackType_t  EEpromTaskStack[STACK_SIZE];
OsConfig_EXT StaticSemaphore_t xSemaphoreEEpromBuffer;

OsConfig_EXT StaticTask_t DebugTaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(DebugTaskStack,2)
#endif
OsConfig_EXT StackType_t  DebugTaskStack[STACK_SIZE];
OsConfig_EXT StaticSemaphore_t xSemaphoreDebugBuffer;

OsConfig_EXT StaticTask_t ParallelTaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(ParallelTaskStack,2)
#endif
OsConfig_EXT StackType_t  ParallelTaskStack[STACK_SIZE*2];
OsConfig_EXT StaticSemaphore_t xSemaphoreParallelBuffer;

OsConfig_EXT StaticTask_t idleTaskBuffer;
#ifdef OsConfig_GLOBALS
#pragma DATA_ALIGN(idleTaskStack,2)
#endif
OsConfig_EXT StackType_t  idleTaskStack[STACK_SIZE];

OsConfig_EXT uint32_t OSEvent[cTaskMax];

OsConfig_EXT uint32_t TaskTimer;

OsConfig_EXT uint32_t dwBackgroundTickCount;
OsConfig_EXT uint16_t wBackgroundPwmFreqDivCnt;
OsConfig_EXT uint16_t wBackgroundPwmFreqDivFlag;

/********************************************************************************
* function declaration                              *
********************************************************************************/
OsConfig_EXT interrupt void TaskTick_ISR( void );
OsConfig_EXT void TaskTimerCallBack(void);

OsConfig_EXT void OsInit(void);
OsConfig_EXT uint32_t  OSEventPend(uint16_t Prio);
OsConfig_EXT void OSEventSend(uint32_t TaskPrio,uint32_t EventID);
OsConfig_EXT uint32_t OSEventConfirm(uint32_t event,uint32_t EventID);
OsConfig_EXT void OSISREventSend(uint32_t TaskPrio,uint32_t EventID);

OsConfig_EXT uint32_t BackgroundGetTickCount(void);

#endif /* APP_TASKTICK_H_ */
