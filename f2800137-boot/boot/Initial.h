/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : Initial.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _Initial_H_
#define _Initial_H_
#ifdef Initial_GLOBALS
#define Initial_EXT
#else
#define Initial_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "stdint.h"
/********************************************************************************
*const define                               *
********************************************************************************/
#define TaskTick_ISR_Enable  1



#define PWMdrive_OFF()    {GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;}
#define PWMdrive_ON()     {GpioDataRegs.GPASET.bit.GPIO12 = 1;}
#define Switch1             (GpioDataRegs.GPHDAT.bit.GPIO239)
#define Switch2             (GpioDataRegs.GPBDAT.bit.GPIO40)
#define Switch3             (GpioDataRegs.GPADAT.bit.GPIO8)
#define Switch4             (GpioDataRegs.GPADAT.bit.GPIO1)
#define cPWMperiod         1500
#define cCMPvalue          (cPWMperiod/2)
/********************************************************************************
* Variable declaration                              *
********************************************************************************/



/********************************************************************************
* function declaration                              *
********************************************************************************/
#if TaskTick_ISR_Enable
Initial_EXT uint32_t getTimerTickCur( void );
Initial_EXT void waitMs( uint32_t ms );
#endif
Initial_EXT void InitialDSP( void );
Initial_EXT void sInitEPWM(void);
Initial_EXT void sInitADC(void);
Initial_EXT void InitSCI(void);
Initial_EXT void InitI2C(void);
Initial_EXT void InitCAN(void);
Initial_EXT void InitCanInt();
Initial_EXT void InitCMPSS(void);
Initial_EXT void sInitGpio(void);


Initial_EXT void InitEPWM1(void);
Initial_EXT void InitEPWM2(void);
Initial_EXT void InitEPWM3(void);
#endif

