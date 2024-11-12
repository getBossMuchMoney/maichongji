/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : f2800137-boot
 *
 *  FILENAME : boot.c
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023年12月29日       V1.0                          Created.
 *
 *============================================================================*/
#define boot_GLOBALS     1
/********************************************************************************
 *include header file                              *
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "boot.h"
#include "Constant.h"
#include "xmodem.h"
#include "shell.h"
#include "driverlib.h"
#include "device.h"
#include "Common.h"
#include "Initial.h"
#include "sciremap.h"
#include "shellprint.h"

/********************************************************************************
 *const define                               *
 ********************************************************************************/

/********************************************************************************
 * Variable declaration                              *
 ********************************************************************************/
uint32_t updateFlag = 0;
uint32_t bootmode = 0;

/********************************************************************************
 * function declaration                              *
 ********************************************************************************/

/*********************************************************************
 Function name:  updateApp
 Description:
 Calls:
 Called By:
 Parameters:     void
 Return:         void
 *********************************************************************/
void updateApp()
{
    int ret = 0;
    if (updateFlag)
    {
        shellPrintf("\r\nstart update app\r\n");
        shellPrintf("\r\nwait for receive file...\r\n");

        ret = xmodemReceive();
        if (ret > 0)
        {
            updateFlag = 0;
            shellPrintf("\r\nupdate success\r\n");
        }
        else
        {
            if (ret != -4)
            {
                shellPrintf("\r\nupdate failed\r\n");
                while (1)
                    ;
            }
            else
            {
                shellPrintf("\r\nThe update operation was not performed\r\n");
            }
        }

    }

}

int update(int argc, char *argv[])
{
    updateFlag = 1;
    updateApp();

    return CMD_SUCCESS;
}

int resetDsp(int argc, char *argv[])
{

    SysCtl_setWatchdogMode(SYSCTL_WD_MODE_RESET);
    SysCtl_enableWatchdog();


    return CMD_SUCCESS;
}

int version(int argc, char *argv[])
{
    versionPrint();

    return CMD_SUCCESS;
}

/*********************************************************************
 Function name:  bootCmdReg
 Description:
 Calls:
 Called By:
 Parameters:     void
 Return:         int
 *********************************************************************/

int bootCmdReg()
{
    int r;
    r = shellCmdRegister("reset", resetDsp, "[none]: reset dsp.");    if (r)return r;
    r = shellCmdRegister("ver", version, "[none]: display version.");    if (r)return r;

    r = shellCmdRegister("update", update, "[none]: update app.");    if (r)return r;

    return 0;
}

/*********************************************************************
 Function name:  bootWait
 Description:
 Calls:
 Called By:
 Parameters:     void
 Return:         void
 *********************************************************************/
void bootWait()
{
    char recbuf[64] = { 0 };
    int len = 0;
    char c = 0;
    uint32_t outtime = BOOT_WAIT_TIME;
    uint32_t curtime = getTimerTickCur();

	if(Get485Addr() == 0)	//地址设置为0表示进入升级模式，下发指令让其他子卡进入升级状态
    {
	    SetStatus485A(STATUS_485_SEND);
	    printEnable();
		waitMs(1000);
    	shellPrintf("stop\r\n");
    	waitMs(100);
	    shellPrintf("stop\r\n");
		waitMs(100);
		shellPrintf("stop\r\n");
		waitMs(100);
		
		bootmode = 1;
	}
	else
	{
	    while (1)
	    {
	        if (getTimerTickCur() - curtime > 1000)
	        {
	            curtime = getTimerTickCur();
	            outtime--;
	//            shellPrintf("\b\b\b%ld s",outtime);
	            if (outtime == 0)
	            {
	                break;
	            }
	        }

	        c = getch();
	        if (c != -1)
	        {
	            recbuf[len] = c;
	            len++;
	        }

	        if (len >= 4)
	        {
	            if (strstr(recbuf, "stop") != 0)
	            {
	                bootmode = 1;
	                break;
	            }
	            else
	            {
	                memset(recbuf, 0, len);
	                len = 0;
	            }
	        }
	    }
	    waitMs(500); //防止后续有多余字符未接收导致影响shell工作
	    while (getch() != -1)
	        ;   //clear fifo

	//    shellPrintf("\r\n\r\n");
	}
}

/*********************************************************************
 Function name:  loadApp
 Description:
 Calls:
 Called By:
 Parameters:     void
 Return:         void
 *********************************************************************/
void loadApp()
{
    unsigned long entryAddress = (unsigned long) APP_Addr;

//    shellPrintf("jump to App...\r\n");
//    waitMs(100);
    ((void (*)(void)) entryAddress)();
}

/*********************************************************************
 Function name:  bootApp
 Description:
 Calls:
 Called By:
 Parameters:     void
 Return:         void
 *********************************************************************/
void bootShell()
{
    if (bootmode)
    {
        shellInit();
        bootCmdReg();
        while (1)
        {
            shellExcu();
        }
    }

}
