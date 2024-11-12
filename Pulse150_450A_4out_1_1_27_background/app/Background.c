/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : Background.c
 *  PURPOSE  :
 *  AUTHOR   : wushiyuan
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2024-09-23       V1.0                          Created.
 *
 *============================================================================*/
#define Background_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "Initial.h"
#include "Interrupt.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "OsConfig.h"

#include "Constant.h"
#include "Common.h"
#include "canmodule.h"
#include "sciframe.h"

#include "Background.h"
#include "SuperTask.h"
#include "ParallelTask.h"
#include "CanTask.h"
#include "LoadTask.h"
#include "InterfaceTask.h"
#include "EEpromTask.h"
#include "HMITask.h"
#include "DebugTask.h"

/********************************************************************************
*const define                               *
********************************************************************************/


/********************************************************************************
* Variable declaration                              *
********************************************************************************/
uint16_t wSuperTaskTestCnt = 0;
uint16_t wParallelTaskTestCnt = 0;
uint16_t wCanTaskTestCnt = 0;
uint16_t wLoadTaskTestCnt = 0;
uint16_t wInterfaceTaskTestCnt = 0;
uint16_t wEEpromTaskTestCnt = 0;
uint16_t wHMITaskTestCnt = 0;
uint16_t wDebugTaskTestCnt = 0;

/*********************************************************************
Function name:  BackgroundLoop
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void BackgroundLoop(void)
{
    //background loop initialize
    wBackgroundTimeBaseCnt = 0;
    dwBackgroundTickCount = 0;

    wBackgroundPwmFreqDivCnt = 0;
    wBackgroundPwmFreqDivFlag = 0;

    //super task initialize
    if(GetBoardType() == UnitCB)
    {
        SuperTaskInit();
    }

    //Parallel task initialize
    //if(GetBoardType() == UnitCB)
    {
        ParallelTaskInit();
    }

    //Load task initialize
    if(GetBoardType() == UnitCB)
    {
        LoadTaskInit();
    }

    //can task initialize
    if(GetBoardType() == UnitCB)
    {
        CanTaskInit();
    }

    //EEprom task initialize
    //if(GetBoardType() == UnitCB)
    {
        EEpromTaskInit();
    }

    //HMI task initialize
    if(GetBoardType() != UnitCB)
    {
        HMITaskInit();
    }

    //Interface task initialize
    if(GetBoardType() == UnitCB)
    {
        InterfaceTaskInit();
    }

    //Debug task initialize
    if (GetBoardType() != UnitCB)
    {
        DebugTaskInit();
    }

    while(1)
    {
        //wait for 1ms time base
        //while(CPUTimer_getTimerOverflowStatus(CPUTIMER2_BASE) == false){};
        //CPUTimer_clearOverflowFlag(CPUTIMER2_BASE);
        //CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);
        if(wBackgroundPwmFreqDivFlag == 0)
        {
            //do nothing
        }
        else
        {
            wBackgroundPwmFreqDivFlag = 0;

            //DO1_ON();
            //GPIO_TEST_HIGH();
            dwBackgroundTickCount++;

            //divide it into 100 time slices
            wBackgroundTimeBaseCnt++;
            if(wBackgroundTimeBaseCnt >= 100) //0-99 100ms
            {
                wBackgroundTimeBaseCnt = 0;
            }

            //
            //super task
            //
            if(wBackgroundTimeBaseCnt % 10 == 1) //10ms
            {
                if(GetBoardType() == UnitCB)
                {
                    //GPIO_TEST_HIGH();
                    SuperTask(NULL);
                    wSuperTaskTestCnt++;
                    //GPIO_TEST_LOW();
                }
            }

            //
            //Parallel task
            //
            if(wBackgroundTimeBaseCnt % 4 == 1) //4ms
            {
                //if(GetBoardType() == UnitCB)
                {
                    //DO1_ON();
                    //GPIO_TEST_HIGH();
                    ParallelTask(NULL);
                    wParallelTaskTestCnt++;
                    //GPIO_TEST_LOW();
                    //DO1_OFF();
                }
            }

            //
            //Can task
            //
    //        if(wBackgroundTimeBaseCnt % 4 == 2) //4ms
            {
                if(GetBoardType() == UnitCB)
                {
                    //GPIO_TEST_HIGH();
                    CanTask(NULL);
                    wCanTaskTestCnt++;
                    //GPIO_TEST_LOW();
                }
            }

            //
            //Load task
            //
            if(wBackgroundTimeBaseCnt % 20 == 2) //20ms
            {
                if(GetBoardType() == UnitCB)
                {
                    //GPIO_TEST_HIGH();
                    LoadTask(NULL);
                    wLoadTaskTestCnt++;
                    //GPIO_TEST_LOW();
                }
            }

            //
            //Interface task
            //
            if(wBackgroundTimeBaseCnt % 4 == 3) //4ms
            {
                if(GetBoardType() == UnitCB)
                {
                    //GPIO_TEST_HIGH();
                    InterfaceTask(NULL);
                    wInterfaceTaskTestCnt++;
                    //GPIO_TEST_LOW();
                }
            }

            //
            //EEprom task
            //
            if(wBackgroundTimeBaseCnt % 100 == 99) //100ms
            {
                //if(GetBoardType() == UnitCB)
                {
                    //DO1_ON();
                    //GPIO_TEST_HIGH();
                    EEpromTask(NULL);
                    wEEpromTaskTestCnt++;
                    //GPIO_TEST_LOW();
                    //DO1_OFF();
                }
            }

            //
            //HMI task
            //
            if(wBackgroundTimeBaseCnt % 4 == 0) //4ms
            {
                if(GetBoardType() != UnitCB)
                {
                    //DO1_ON();
                    HMITask(NULL);
                    wHMITaskTestCnt++;
                    //DO1_OFF();
                }
            }

            //
            //Debug task
            //
            //if(wBackgroundTimeBaseCnt % 10 == 3) //1ms
            if (GetBoardType() != UnitCB)
            {
                //DO1_ON();
                DebugTask(NULL);
                wDebugTaskTestCnt++;
                //DO1_OFF();
            }

            //DO1_OFF();
            //GPIO_TEST_LOW();
        }
    }
}
