/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : Common.c
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023Äê12ÔÂ28ÈÕ       V1.0                          Created.
 *
 *============================================================================*/
#define Common_GLOBALS     1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include <stdint.h>
#include "Constant.h"
#include "Common.h"
#include "Initial.h"
#include "device.h"


/********************************************************************************
*const define                               *
********************************************************************************/

/********************************************************************************
* Variable declaration                              *
********************************************************************************/
//Uint16 SwitchDIPAddr123;
Uint16 SwitchDIPAddr1;
Uint16 SwitchDIPAddr2;
Uint16 SwitchDIPAddr3;
Uint16 SwitchDIPAddr4;
Uint16 SwitchDIPAddr;
float SwitchDIP;

/********************************************************************************
* function declaration                              *
********************************************************************************/

/*********************************************************************
Function name:  CheckDIPSwitchState
Description:
Calls:
Called By:
Parameters:     void
Return:         uint16_t
*********************************************************************/
void AddressCheck(void)
{
    /*Uint16 switchtem[100];
//    Uint16 avg,min=4096,max=0;
    Uint32 sum = 0;
    int count = 0,adccount = ADC_A1_ISR_Count;
    while(1)
    {
        if(adccount != ADC_A1_ISR_Count)
        {
            while(ADC_isBusy(ADCC_BASE) == true);
            if(count >= 100)
                break;
            switchtem[count] = AdccResultRegs.ADCRESULT4;
            sum += switchtem[count];
//            if(min > switchtem[count])
//                min = switchtem[count];
//            if(max < switchtem[count])
//                max = switchtem[count];
            adccount = ADC_A1_ISR_Count;
            count++;
        }
    }
//    avg = sum/count;

    SwitchDIP = (float)sum/count;

    if(SwitchDIP <= 1950)
    {
        SwitchDIPAddr123 = 0x7;
    }
    else if(SwitchDIP <= 2150)
    {
        SwitchDIPAddr123 = 0x6;
    }
    else if(SwitchDIP <= 2370)
    {
        SwitchDIPAddr123 = 0x5;
    }
    else if(SwitchDIP <= 2600)
    {
        SwitchDIPAddr123 = 0x4;
    }
    else if(SwitchDIP <= 2850)
    {
        SwitchDIPAddr123 = 0x3;
    }
    else if(SwitchDIP <= 3300)
    {
        SwitchDIPAddr123 = 0x2;
    }
    else if(SwitchDIP <= 3800)
    {
        SwitchDIPAddr123 = 0x1;
    }
    else
    {
        SwitchDIPAddr123 = 0x0;
    }*/

    if(Switch4 == 0)
    {
        SwitchDIPAddr4 = 0x0;
    }
    else
    {
        SwitchDIPAddr4 = 0x8;
    }

    if(Switch3 == 0)
    {
        SwitchDIPAddr3 = 0x0;
    }
    else
    {
        SwitchDIPAddr3 = 0x4;
    }

    if(Switch2 == 0)
    {
        SwitchDIPAddr2 = 0x0;
    }
    else
    {
        SwitchDIPAddr2 = 0x2;
    }

    if(Switch1 == 0)
    {
        SwitchDIPAddr1 = 0x0;
    }
    else
    {
        SwitchDIPAddr1 = 0x1;
    }

    //SwitchDIPAddr = SwitchDIPAddr123 + SwitchDIPAddr4;
    SwitchDIPAddr = SwitchDIPAddr1 + SwitchDIPAddr2 + SwitchDIPAddr3 + SwitchDIPAddr4;
}
/*********************************************************************
Function name:  Get485Addr
Description:
Calls:
Called By:
Parameters:     void
Return:         uint16_t
*********************************************************************/
uint16_t Get485Addr(void)
{
    return SwitchDIPAddr;
}

/*********************************************************************
Function name:  SetStatus485A
Description:
Calls:
Called By:
Parameters:     int
Return:         void
*********************************************************************/
void SetStatus485A(int status)
{
    if (status == STATUS_485_SEND)
    {
        GpioDataRegs.GPHSET.bit.GPIO242 = 1;
        Rs485Flag[0] = 1;
    }
    else
    {
        GpioDataRegs.GPHCLEAR.bit.GPIO242 = 1;
        Rs485Flag[0] = 0;
    }
}

/*********************************************************************
Function name:  SetStatus485B
Description:
Calls:
Called By:
Parameters:     int
Return:         void
*********************************************************************/
void SetStatus485B(int status)
{
    if (status == STATUS_485_SEND)
    {
        GpioDataRegs.GPASET.bit.GPIO16 = 1;
    }
    else
    {
        GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;
    }
}

/*********************************************************************
Function name:  SetStatus485B
Description:
Calls:
Called By:
Parameters:     int
Return:         void
*********************************************************************/
void SetStatus485C(int status)
{
    if (status == STATUS_485_SEND)
    {
        GpioDataRegs.GPASET.bit.GPIO20 = 1;
    }
    else
    {
        GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;
    }
}
