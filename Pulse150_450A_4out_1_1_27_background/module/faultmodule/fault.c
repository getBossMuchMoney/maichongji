/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : fault.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define fault_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "Interrupt.h"
#include "f280013x_pie_defines.h"
//#include "Initial.h"
#include "SuperTask.h"
#include "LoadTask.h"
#include "CanTask.h"
#include "InterfaceTask.h"
#include "ParallelTask.h"
#include "OsConfig.h"
#include "fault.h"




/********************************************************************************
*const define                               *
********************************************************************************/


/********************************************************************************
* Variable declaration                              *
********************************************************************************/



/********************************************************************************
* function declaration                              *
********************************************************************************/
/*******************************************************************
//Falut code & warning code information
*******************************************************************/
void sSetFaultCode(Uint16 wFaultCodeTemp)
{
    OS_ENTER_CRITICAL();

	if(wFaultCode == 0)
	{
		wFaultCode = wFaultCodeTemp;		
	}		
    OS_EXIT_CRITICAL();

}

void sClrFaultCode(void)				
{
    OS_ENTER_CRITICAL();
	wFaultCode = 0;					
	OS_EXIT_CRITICAL();
}

void sSetISRFaultCode(Uint16 wFaultCodeTemp)
{
	if(wFaultCode == 0)
	{
		wFaultCode = wFaultCodeTemp;
	}
}

void sClrISRFaultCode(void)				
{
	wFaultCode = 0;	
}

							
Uint16 sGetFaultCode(void)
{
	return(wFaultCode);
}


void sSetWarningCode(Uint32 dwWarningCodeBit)	
{
    OS_ENTER_CRITICAL();
	wWarningCode |= dwWarningCodeBit;
	OS_EXIT_CRITICAL();
}

void sClrWarningCode(Uint32 dwWarningCodeBit)	
{
    OS_ENTER_CRITICAL();
	wWarningCode &= ~dwWarningCodeBit;
	OS_EXIT_CRITICAL();
}

Uint32 swGetWarningCode(void)					
{
	return(wWarningCode);	
}


void sSetISRWarningCode(Uint32 dwWarningCodeBit)	
{
	wWarningCode |= dwWarningCodeBit;
}

void sClrISRWarningCode(Uint32 dwWarningCodeBit)	
{
	wWarningCode &= ~dwWarningCodeBit;
}

Uint32 swISRGetWarningCode(void)					
{
	return(wWarningCode);
}



