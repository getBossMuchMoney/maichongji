/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : Interrupt.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _Interrupt_H_
#define _Interrupt_H_
#ifdef Interrupt_GLOBALS
#define Interrup_EXT
#else
#define Interrup_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "device.h"
//#include "ParallelTask.h"
/********************************************************************************
*const define                               *
********************************************************************************/
#define     cSideApwm    1
#define     cSideBpwm    2

#define     cDigitalPWMperiod   (float)1500.0


#define    cPWMmode   0
#define    cCMPmode   1

#define  cPWMPERIOD   1500
#define  cCMPPERIOD   (cPWMPERIOD*2)

#define cPWMrunSts  1
#define cPWMwaitSts 0

#define cPositiveCurrent  1
#define cNegtiveCurrent   0

#define cPosCurrentBias   10
#define cNegCurrentBias   10
#define cPosCurrentBias2  10
#define cNegCurrentBias2  10
#define cSmallCurrentBias   (-1891)
#define cOutPutVoltBias     (-2530)   
#define cBusVolttBias      0
#define cAmbientTemperatureBias  0
#define cSinkTemperatureBias     0
#define cSwitchDIPBias           0

#define cPosCurrentRatio         (1.221)   //10/((4095/3.3)*(3.3/500)), 10times
#define cNegCurrentRatio         (1.221)   //10/((4095/3.3)*(3.3/500)), 10times
#define cPosCurrentRatio2        (1.221)   //10/((4095/3.3)*(3.3/500)), 10times
#define cNegCurrentRatio2        (1.221)   //10/((4095/3.3)*(3.3/500)), 10times
#define cSmallCurrentRatio       (2.442)	//100/40.95 , 100times
#define cOutPutVoltRatio         (3.2185)  //100/31.07 , 100times
#define cBusVoltRatio            (2.442)   //100/40.95 , 100times
#define cAmbientTemperatureRatio  (1.0)
#define cSinkTemperatureRatio     (1.0)
#define cSwitchDIPRatio           (1.0)



















/********************************************************************************
* Variable declaration                              *
********************************************************************************/
Interrup_EXT int ADC_A1_ISR_Count;



/********************************************************************************
* function declaration                              *
********************************************************************************/

Interrup_EXT interrupt void epwm1ISR(void);
Interrup_EXT interrupt void ADC_A1_ISR(void);
Interrup_EXT interrupt void PWMsync_CAP1_ISR(void);
Interrup_EXT interrupt void Linesync_CAP2_ISR(void);
Interrup_EXT interrupt void EPWM2_TZ1_ISR(void);

Interrup_EXT __interrupt void canISR(void);

#endif
