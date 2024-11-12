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
#define EEpromTask_GLOBALS     1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "SuperTask.h"
#include "LoadTask.h"
#include "Interrupt.h"
#include "CanTask.h"
#include "InterfaceTask.h"
#include "EEpromTask.h"
#include "OsConfig.h"
#include "Initial.h"
#include "eeprommodule.h"


/********************************************************************************
*const define                               *
********************************************************************************/


/********************************************************************************
* Variable declaration                              *
********************************************************************************/
typeEepromCmd EepromCmd;

/********************************************************************************
* function declaration                              *
********************************************************************************/
/********************************************************************************
* function declaration                              *
********************************************************************************/
void EEpromTaskInit(void)
{
    sQInit(&EepromCmdeQueue,EepromCmdBuffer,EepromCmdMaxNum,EepromCmdSize);
}

void EEpromTask(void * pvParameters)
{
    uint32_t event;
    //BaseType_t err;
	//typeEepromCmd EepromCmd;
	
	//sQInit(&EepromCmdeQueue,EepromCmdBuffer,EepromCmdMaxNum,EepromCmdSize);

    //while(pdTRUE)
    {

        //err = xSemaphoreTake( xSemaphoreEEprom, (TickType_t)Tick100mS );
        event = OSEventPend(cPrioEEprom);
        if(event != 0)
        {

			if(OSEventConfirm(event,eEEpromTest))
			{

			}
        }
        else//eEEpromTimer
        {
			if(cQBufNormal == sQDataOut(&EepromCmdeQueue,&EepromCmd.all[0]))
			{
				if(EepromCmd.bit.Cmd == EEPROM_CMD_READ)
				{
					ReadEepromDataLong(EepromCmd.bit.EepromAddr,EepromCmd.bit.pram,EepromCmd.bit.length);
				}
				else if(EepromCmd.bit.Cmd == EEPROM_CMD_WRITE)
				{
					WriteEepromDataLong(EepromCmd.bit.EepromAddr,EepromCmd.bit.pram,EepromCmd.bit.length);
				} 
			}
        }
    }
}





