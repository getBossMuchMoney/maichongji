/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : Version.c
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023Äê12ÔÂ28ÈÕ       V1.0                          Created.
 *
 *============================================================================*/
#define Initial_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include <stdio.h>
#include <string.h>

#include "shellprint.h"
#include "Version.h"


/********************************************************************************
*const define                               *
********************************************************************************/


/********************************************************************************
* Variable declaration                              *
********************************************************************************/
char appVersion[64]={0};


/********************************************************************************
* function declaration                              *
********************************************************************************/
static void l_strupr(char *pstr)
{
    while(*pstr)
    {
        if(*pstr >= 'a' && *pstr <= 'z')
            *pstr = *pstr +'A' -'a';
        pstr++;
    }
}
/*********************************************************************
Function name:  buildTime
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
char* buildTime()
{
    static char bt[64]={0};
    int  nDay,nMonth,nYear,nHour,nMinute;
    char szMon[10];
    const char *pMons[] =
    {"JAN","FEB","MAR",
     "APR","MAY","JUN",
     "JUL","AUG","SEP",
     "OCT","NOV","DEC"};

    sscanf(__DATE__,"%s %d %d",szMon,&nDay,&nYear);
    sscanf(__TIME__,"%d:%d",&nHour,&nMinute);

    l_strupr(szMon);

    for(nMonth = 1; nMonth<= 12; nMonth++)
    {
        if(strcmp(pMons[nMonth-1],szMon) == 0)
        {
            break;
        }
    }
    if(nMonth > 12)
        nMonth = 1;
    sprintf(bt,"%04d%02d%02d.%02d%02d",nYear,nMonth,nDay,nHour,nMinute);

    return bt;
}
/*********************************************************************
Function name:  versionReg
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void versionReg()
{
    snprintf(appVersion,64,"%s-%s-%s",PRJ_VER,BOOT_VER,buildTime());
}
/*********************************************************************
Function name:  buildTime
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void versionPrint()
{
    shellPrintf("%s\r\n",appVersion);
}
