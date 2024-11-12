/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : DebugTask.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define DebugTask_GLOBALS   1
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
#include "DebugTask.h"
#include "OsConfig.h"
#include "Initial.h"
#include "Constant.h"
#include "shell.h"
#include "shellprint.h"
#include "Common.h"
#include "Version.h"
#include "eeprommodule.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SyncProc.h"

#ifdef SHELL_ENABLE

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
Function name:  memDump
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdMemDump(int argc, char*argv[])
{
    uint32_t addr,len;
    uint32_t i,j;
    uint16_t *p=NULL;

    int ret = shellArgGet(argc,argv,&addr,&len);
    if(ret != 2)
        return CMD_ERRARG;

    p = (uint16_t*)addr;

    for(i=0;i<(len+7)/8;i++)
    {
        shellPrintf("0x%08lX:",(uint32_t)p);
        for(j=0;((j<8) && (i*8+j<len));(j++,p++))
        {
            shellPrintf("0x%04X ",*p);
        }
        shellPrintf("\r\n");
    }

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  memRead16
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdMemRead16(int argc, char*argv[])
{
    uint32_t addr;

    int ret = shellArgGet(argc,argv,&addr);
    if(ret != 1)
        return CMD_ERRARG;

    shellPrintf("mem(0x%lx)= 0x%x\n",addr,*(uint16_t*)addr);

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  memRead32
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdMemRead32(int argc, char*argv[])
{
    uint32_t addr;

    int ret = shellArgGet(argc,argv,&addr);
    if(ret != 1)
        return CMD_ERRARG;

    shellPrintf("mem(0x%08lX)= 0x%lX\n",addr,*(uint32_t*)addr);

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  memWrite16
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdMemWrite16(int argc, char*argv[])
{
    uint32_t addr,val;

    int ret = shellArgGet(argc,argv,&addr,&val);
    if(ret != 2)
        return CMD_ERRARG;

    *(uint16_t*)addr = val&0xffff;

    shellPrintf("mem(0x%08lX)= 0x%X\n",addr,*(uint16_t*)addr);

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  memWrite32
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdMemWrite32(int argc, char*argv[])
{
    uint32_t addr,val;

    int ret = shellArgGet(argc,argv,&addr,&val);
    if(ret != 2)
        return CMD_ERRARG;

    *(uint32_t*)addr = val;

    shellPrintf("mem(0x%08lX)= 0x%lX\n",addr,*(uint32_t*)addr);


    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  resetDsp
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdResetDsp(int argc, char*argv[])
{
    SysCtl_setWatchdogMode(SYSCTL_WD_MODE_RESET);
    SysCtl_enableWatchdog();

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  version
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdVersion(int argc, char*argv[])
{
    versionPrint();

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  gpioSet
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdGpioSet(int argc, char*argv[])
{
    uint32_t pin,val;

    int ret = shellArgGet(argc,argv,&pin,&val);
    if(ret != 2)
        return CMD_ERRARG;

    GPIO_writePin(pin, val);

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  gpioRead
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdGpioRead(int argc, char*argv[])
{
    uint32_t pin,val;

    int ret = shellArgGet(argc,argv,&pin);
    if(ret != 1)
        return CMD_ERRARG;

    val = GPIO_readPin(pin);
    shellPrintf("Pin[%ld]:%ld\n", pin, val);

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  cmdEepromWrite
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdEepromWrite(int argc, char*argv[])
{
    uint32_t addr,len,type;
    uint16_t i;
    uint16_t *pbuf = NULL;

    int ret = shellArgGet(argc,argv,&addr,&len,&type);
    if(ret != 3)
        return CMD_ERRARG;

    pbuf = malloc(len);
    if(!pbuf)
    {
        shellPrintf("malloc failed\n");
        return CMD_FAILURE;
    }

    if(type == 0)
    {
        memset(pbuf,0,len);
    }
    else if(type == 1)
    {
        memset(pbuf,0xff,len);
    }
    else
    {
        for(i=0; i<len; i++)
            pbuf[i] = (i<<8)+i;
    }

    WriteEepromDataLong(addr,pbuf,len);

    free(pbuf);

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  cmdEepromRead
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdEepromRead(int argc, char*argv[])
{
    uint32_t addr,len;
    uint16_t i,j;
    uint16_t *pbuf = NULL;

    int ret = shellArgGet(argc,argv,&addr,&len);
    if(ret != 2)
        return CMD_ERRARG;

    pbuf = malloc(len);
    if(!pbuf)
    {
        shellPrintf("malloc failed\n");
        return CMD_FAILURE;
    }

    ReadEepromDataLong(addr,pbuf,len);


    for(i=0;i<(len+7)/8;i++)
    {
        shellPrintf("0x%04lX:",addr);
        for(j=0;((j<8) && (i*8+j<len));(j++,addr++))
        {
            shellPrintf("0x%04X ",*(pbuf+i*8+j));
        }
        shellPrintf("\r\n");
    }
    free(pbuf);

    return CMD_SUCCESS;
}


/*********************************************************************
Function name:  cmdEepromWriteByte
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdEepromWriteByte(int argc, char*argv[])
{
    uint32_t addr,dat;
    uint16_t dtem;

    int ret = shellArgGet(argc,argv,&addr,&dat);
    if(ret != 2)
        return CMD_ERRARG;

    dtem = dat;
    I2CWrite(EEPROM_SLAVE_ADDR,addr,&dtem,1);

    return CMD_SUCCESS;
}

/*********************************************************************
Function name:  cmdEepromRead
Description:
Calls:
Called By:
Parameters:     int,char*
Return:         int
*********************************************************************/
int cmdEepromReadByte(int argc, char*argv[])
{
    uint32_t addr;
    uint16_t dat;

    int ret = shellArgGet(argc,argv,&addr);
    if(ret != 1)
        return CMD_ERRARG;


    I2CRead(EEPROM_SLAVE_ADDR,addr,&dat,1);

    shellPrintf("addr[0x%lx]:0x%x\n",addr,dat);

    return CMD_SUCCESS;
}



int DebugCmdReg()
{
    int r;
    r=shellCmdRegister("reset",cmdResetDsp, "[none]: reset dsp.");if(r) return r;
    r=shellCmdRegister("ver",cmdVersion, "[none]: display version.");if(r) return r;

    r=shellCmdRegister("md",cmdMemDump, "[addr]: memory dump.");if(r) return r;
    r=shellCmdRegister("mr16",cmdMemRead16, "[addr]: read memory.");if(r) return r;
    r=shellCmdRegister("mr32",cmdMemRead32, "[addr]: read memory.");if(r) return r;
    r=shellCmdRegister("mw16",cmdMemWrite16, "[addr][val]: write memory.");if(r) return r;
    r=shellCmdRegister("mw32",cmdMemWrite32, "[addr][val]: write memory.");if(r) return r;

    r=shellCmdRegister("ios",cmdGpioSet, "[pin][val]: set out pin status.");if(r) return r;
    r=shellCmdRegister("ior",cmdGpioRead, "[pin]: read pin status.");if(r) return r;

    r=shellCmdRegister("eepw",cmdEepromWrite, "[addr][len][type]: write eeprom.");if(r) return r;
    r=shellCmdRegister("eepr",cmdEepromRead, "[addr][len]: read eeprom.");if(r) return r;
    r=shellCmdRegister("eepwb",cmdEepromWriteByte, "[addr][data]: write eeprom.");if(r) return r;
    r=shellCmdRegister("eeprb",cmdEepromReadByte, "[addr]: read eeprom.");if(r) return r;

    return 0;
}

#endif

void DebugTaskInit(void)
{
    //add init code
}

#if 0
void DebugTask(void * pvParameters)
{
    uint32_t event;
    BaseType_t err;
	
#ifdef SHELL_ENABLE
    shellInit();
    DebugCmdReg();
#endif
    while(pdTRUE)
    {

        err = xSemaphoreTake( xSemaphoreDebug, (TickType_t)Tick10mS );
//        event = OSEventPend(cPrioDebugTask);
        if(err == pdTRUE)
        {

            if(OSEventConfirm(event,eDebugTest))
            {

            }



        }
        else//eDebugTimer
        {
        #ifdef SHELL_ENABLE
            shellExcu();
		#endif
        }
    }

}
#else
void DebugTask(void * pvParameters)
{
	uint32_t event;
    //BaseType_t err;
	
	//while(pdTRUE)
    {

        //err = xSemaphoreTake( xSemaphoreDebug, (TickType_t)Tick1mS );
        //event = OSEventPend(cPrioDebugTask);
        event = OSEventPend(cPrioDebug);
        if(event != 0)
        {

            if(OSEventConfirm(event,eDebugTest))
            {

            }
        }
		else
		{
			if(GpioDataRegs.GPHDAT.bit.GPIO230 == 1)
				SynLineIdleCnt++;
			else
				SynLineIdleCnt = 0;
		}
	}
}


#endif



