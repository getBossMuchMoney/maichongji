/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : OsConfig.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define OsConfig_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "string.h"
#include "Interrupt.h"
#include "OsConfig.h"
#include "Constant.h"
#include "FreeRTOS.h"
#include "semphr.h"


/********************************************************************************
*const define                               *
********************************************************************************/


/********************************************************************************
* Variable declaration                              *
********************************************************************************/
uint16_t wTimer1TestCnt = 0;

/********************************************************************************
* function declaration                              *
********************************************************************************/
/*********************************************************************
Function name:  vApplicationStackOverflowHook
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    while(1);
}

/*********************************************************************
Function name:  vApplicationGetIdleTaskMemory
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &idleTaskBuffer;
    *ppxIdleTaskStackBuffer = idleTaskStack;
    *pulIdleTaskStackSize = STACK_SIZE;
}
/*********************************************************************
Function name:  TaskTimerCallBack
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void TaskTimerCallBack(void)
{

}

/*********************************************************************
Function name:  TaskTick_ISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
//#if TaskTick_ISR_Enable
#if BACKGROUND_LOOP_Enable
interrupt void TaskTick_ISR( void )
{
    wTimer1TestCnt++;
	//GpioDataRegs.GPASET.bit.GPIO8 = 1;
/*
    TimerTick.SuperTimer++;
	if (TimerTick.SuperTimer >= cTaskTimer3mS )
	{
		 TimerTick.SuperTimer = 0;
		 OSISREventSend(cPrioSuper,eSuperTest);

	}

 	TimerTick.LoadTimer++;
	if (TimerTick.LoadTimer >= cTaskTimer20mS )
	{
		 TimerTick.LoadTimer = 0;
		 OSISREventSend(cPrioLoad,eLoadTest);
	}

	  TimerTick.EEpromTimer++;
	  if (TimerTick.EEpromTimer >= cTaskTimer100mS )
	  {
		   TimerTick.EEpromTimer = 0;
		   OSISREventSend(cPrioEEprom,eEEpromTest);
	  }

	  TimerTick.DebugTimer++;
	  if (TimerTick.DebugTimer >= cTaskTimer50mS )
	  {
		   TimerTick.DebugTimer = 0;
		   OSISREventSend(cPrioDebug,eDebugTest);
	  }

	  TimerTick.CanTimer++;
	  if (TimerTick.CanTimer >= cTaskTimer4mS )
	  {
		   TimerTick.CanTimer = 0;
		   OSISREventSend(cPrioCan,eCanTest);
	  }

	  TimerTick.InterfaceTimer++;
	  if (TimerTick.InterfaceTimer >= cTaskTimer1S )
	  {
		   TimerTick.InterfaceTimer = 0;
		   OSISREventSend(cPrioInterface,eInterfaceTest);
	  }

	  
	  TimerTick.HMITimer++;
	  if (TimerTick.HMITimer >= cTaskTimer500mS )
	  {
		   TimerTick.HMITimer = 0;
		   OSISREventSend(cPrioHMI,eHMITest);
	  }
*/
	 // GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;


}
#endif
/*********************************************************************
Function name:  vApplicationTickHook
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void vApplicationTickHook(void)
{



}
/*********************************************************************
Function name:  OsInit
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void OsInit(void)
{
    uint16_t i;

	//TaskTimer = xTimerCreate("tasktimer",1,pdTRUE,0,TaskTimerCallBack);

    xSemaphoreSuper = xSemaphoreCreateBinaryStatic( &xSemaphoreSuperBuffer );
    xSemaphoreLoad = xSemaphoreCreateBinaryStatic( &xSemaphoreLoadBuffer );
    xSemaphoreInterface = xSemaphoreCreateBinaryStatic( &xSemaphoreInterfaceBuffer );
    xSemaphoreCan = xSemaphoreCreateBinaryStatic( &xSemaphoreCanBuffer );
    xSemaphoreEEprom = xSemaphoreCreateBinaryStatic( &xSemaphoreEEpromBuffer );
    xSemaphoreHMI = xSemaphoreCreateBinaryStatic( &xSemaphoreHMIBuffer );
    xSemaphoreDebug = xSemaphoreCreateBinaryStatic( &xSemaphoreDebugBuffer );
    xSemaphoreParallel = xSemaphoreCreateBinaryStatic( &xSemaphoreParallelBuffer );

    memset(&TimerTick.SuperTimer,0,sizeof(TimerTick));

    for(i = 0; i < cTaskMax; i++)
    {
        OSEvent[i] = 0;
    }

	TimerTick.SuperTimer = 0;
	TimerTick.LoadTimer = 3;
	TimerTick.CanTimer= 1;
	TimerTick.InterfaceTimer = 7;
	TimerTick.HMITimer = 9;
	TimerTick.EEpromTimer = 11;
	TimerTick.DebugTimer = 13;

}
/*********************************************************************
Function name:  OSEventPend
Description:
Calls:
Called By:
Parameters:
Return:
*********************************************************************/
uint32_t  OSEventPend(uint16_t Prio)
{
    uint32_t  event;

    OS_ENTER_CRITICAL();
    event = OSEvent[Prio];
    OSEvent[Prio] &= (uint32_t)0;
    OS_EXIT_CRITICAL();

    return(event);

}
/*********************************************************************
Function name:  OSEventSend
Description:
Calls:
Called By:
Parameters:
Return:
*********************************************************************/
//#pragma CODE_SECTION(OSEventSend,ramFuncSection);
void OSEventSend(uint32_t TaskPrio,uint32_t EventID)
{

    OS_ENTER_CRITICAL();
    OSEvent[TaskPrio] |= ((uint32_t)1<< (uint32_t)EventID);
    //xSemaphoreGive(Sem);
    //portYIELD();
    OS_EXIT_CRITICAL();
}


/*********************************************************************
Function name:  OSEventConfirm
Description:
Calls:
Called By:
Parameters:
Return:
*********************************************************************/
uint32_t OSEventConfirm(uint32_t event,uint32_t EventID)
{
    uint32_t OSTemp;
    OSTemp = event & ((uint32_t)1 << EventID);
    return(OSTemp);
}


/*********************************************************************
Function name:  OSISREventSend
Description:
Calls:
Called By:
Parameters:
Return:
*********************************************************************/
//#pragma CODE_SECTION(OSISREventSend,ramFuncSection);
void OSISREventSend(uint32_t TaskPrio,uint32_t EventID)
{
    //BaseType_t xHigherPriorityTaskWoken1;

    OSEvent[TaskPrio] |= ((uint32_t)1<< (uint32_t)EventID);
    //xSemaphoreGiveFromISR(Sem, &xHigherPriorityTaskWoken1);
    //portYIELD_FROM_ISR( xHigherPriorityTaskWoken1 );

}

/*********************************************************************
Function name:  BackgroundGetTickCount
Description:
Calls:
Called By:
Parameters:     void
Return:         uint32_t
*********************************************************************/
uint32_t BackgroundGetTickCount(void)
{
    uint32_t dwTicks;

    dwTicks = dwBackgroundTickCount;

    return dwTicks;
}
