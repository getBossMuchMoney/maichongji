
/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : EEpromTask.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/

/********************************************************************************
*include header file                              *
********************************************************************************/
#include "Initial.h"
#include "driverlib.h"
#include "device.h"     // Device Headerfile and Examples Include File
#include "FreeRTOS.h"
#include "semphr.h"

//#include "task.h"
#include "SuperTask.h"
#include "LoadTask.h"
#include "HMITask.h"
#include "OsConfig.h"
#include "InterfaceTask.h"
#include "CanTask.h"
#include "EEpromTask.h"
#include "DebugTask.h"
#include "ParallelTask.h"
#include "Background.h"

/********************************************************************************
*const define                               *
********************************************************************************/


/********************************************************************************
* Variable declaration                              *
********************************************************************************/



/********************************************************************************
* function declaration                              *
********************************************************************************/

/*********************************************************************
Function name:  main
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void main(void)
{

    InitialDSP();
    DataInit();
    OsInit();
    // Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

/*
	if (GetBoardType() == UnitCB)
    // Create the task without using any dynamic memory allocation.
    xTaskCreateStatic(SuperTask,          // Function that implements the task.
                      "supertask",       // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) NULL,       // Parameter passed into the task.
                      tskIDLE_PRIORITY + cPrioSuper, // Priority at which the task is created.
                      SuperTaskStack,         // Array to use as the task's stack.
                      &SuperTaskBuffer );     // Variable to hold the task's data structure.
//	if (GetBoardType() == UnitCB)
    xTaskCreateStatic(ParallelTask,          // Function that implements the task.
                         "paralleltask",       // Text name for the task.
                         STACK_SIZE*2,           // Number of indexes in the xStack array.
                         ( void * ) NULL,       // Parameter passed into the task.
                         tskIDLE_PRIORITY + cPrioParallel, // Priority at which the task is created.
                         ParallelTaskStack,         // Array to use as the task's stack.
                         &ParallelTaskBuffer );     // Variable to hold the task's data structure.
	
	if (GetBoardType() == UnitCB)
    xTaskCreateStatic(LoadTask,         // Function that implements the task.
                      "loadtask",      // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) NULL,      // Parameter passed into the task.
                      tskIDLE_PRIORITY + cPrioLoad, // Priority at which the task is created.
                      LoadTaskStack,        // Array to use as the task's stack.
                      &LoadTaskBuffer );    // Variable to hold the task's data structure.

	if (GetBoardType() == UnitCB)
    xTaskCreateStatic(CanTask,         // Function that implements the task.
                      "cantask",      // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) NULL,      // Parameter passed into the task.
                      tskIDLE_PRIORITY + cPrioCan, // Priority at which the task is created.
                      CanTaskStack,        // Array to use as the task's stack.
                      &CanTaskBuffer );    // Variable to hold the task's data structure.
//	if (GetBoardType() == UnitCB)
    xTaskCreateStatic(EEpromTask,         // Function that implements the task.
                      "eepromtask",      // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) NULL,      // Parameter passed into the task.
                      tskIDLE_PRIORITY + cPrioEEprom, // Priority at which the task is created.
                      EEpromTaskStack,        // Array to use as the task's stack.
                      &EEpromTaskBuffer );    // Variable to hold the task's data structure.
	if (GetBoardType() != UnitCB)
    xTaskCreateStatic(HMITask,         // Function that implements the task.
                      "hmitask",              // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) NULL,      // Parameter passed into the task.
                      tskIDLE_PRIORITY + cPrioHMI, // Priority at which the task is created.
                      HMITaskStack,        // Array to use as the task's stack.
                      &HMITaskBuffer );    // Variable to hold the task's data structure.
	if (GetBoardType() == UnitCB)
    xTaskCreateStatic(InterfaceTask,         // Function that implements the task.
                      "interfacetask",              // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) NULL,      // Parameter passed into the task.
                      tskIDLE_PRIORITY + cPrioInterface, // Priority at which the task is created.
                      InterfaceTaskStack,        // Array to use as the task's stack.
                      &InterfaceTaskBuffer );    // Variable to hold the task's data structure.
	if (GetBoardType() != UnitCB)
    xTaskCreateStatic(DebugTask,         // Function that implements the task.
                      "debugtask",              // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) NULL,      // Parameter passed into the task.
                      tskIDLE_PRIORITY + cPrioDebug, // Priority at which the task is created.
                      DebugTaskStack,        // Array to use as the task's stack.
                      &DebugTaskBuffer );    // Variable to hold the task's data structure.
*/

    //vTaskStartScheduler();

    DEVICE_DELAY_US(1000);
    //Timer2Init();
    BackgroundLoop();
}
