
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
#include "device.h"     // Device Headerfile and Examples Include File
#include <stdio.h>
#include <string.h>
#include "xmodem.h"
#include "flashmodule.h"
#include "Constant.h"
#include "boot.h"
/*****lll*****/ 

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
    bootWait();
    bootShell();
    loadApp();
}

