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
#include "device.h"
/********************************************************************************
*const define                               *
********************************************************************************/
#define SetExtLineSyncHigh  {GpioDataRegs.GPASET.bit.GPIO17 = 1;}
#define SetExtLineSyncLow   {GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;}
#define SetLineSyncHigh  {GpioDataRegs.GPBSET.bit.GPIO39 = 1;}
#define SetLineSyncLow   {GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;}

//#define GPIO1_SetLineSyncHigh  {GpioDataRegs.GPASET.bit.GPIO1 = 1;}
//#define GPIO1_SetLineSyncLow  {GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;}
//#define GPIO1_TOGGLE  {GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;}

#define PWMdrive_OFF()    {GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;}
#define PWMdrive_ON()     {GpioDataRegs.GPASET.bit.GPIO12 = 1;}


#define DO1_ON()    {GpioDataRegs.GPASET.bit.GPIO7 = 1;}
#define DO1_OFF()     {GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;}
#define DO1_TOGGLE()     {GpioDataRegs.GPATOGGLE.bit.GPIO7 = 1;}


#define DO2_ON()    {GpioDataRegs.GPBSET.bit.GPIO41 = 1;}
#define DO2_OFF()     {GpioDataRegs.GPBCLEAR.bit.GPIO41 = 1;}


#define LED_OH_High()     {GpioDataRegs.GPBSET.bit.GPIO37 = 1;}
#define LED_OH_LOW()     {GpioDataRegs.GPBCLEAR.bit.GPIO37 = 1;}

#define LED_RUN_High()     {GpioDataRegs.GPBSET.bit.GPIO35 = 1;}
#define LED_RUN_LOW()     {GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;}

#define FAN_RUN()		{GpioDataRegs.GPACLEAR.bit.GPIO13 = 1;}
#define FAN_STOP()		{GpioDataRegs.GPASET.bit.GPIO13 = 1;}




#define Power24Vsignal()       (GpioDataRegs.GPHDAT.bit.GPIO228)
#define HighLevel              1
#define LowLevel               0

//地址拨码开关位宏定义
#define Switch1             (GpioDataRegs.GPHDAT.bit.GPIO239)
#define Switch2             (GpioDataRegs.GPBDAT.bit.GPIO40)
#define Switch3             (GpioDataRegs.GPADAT.bit.GPIO8)
#define Switch4             (GpioDataRegs.GPADAT.bit.GPIO1)

#define  SIN1               (GpioDataRegs.GPHDAT.bit.GPIO245)
#define  SIN2               (GpioDataRegs.GPHDAT.bit.GPIO241)

#define GPIO_TEST_HIGH()    {GpioDataRegs.GPASET.bit.GPIO9 = 1;}
#define GPIO_TEST_LOW()     {GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;}

#define   cControlperiod   1500

#define cPWMperiod         1500
#define cCMPvalue          (cPWMperiod/2)

#define Cmpss1DACHVALS_Pos_N150A    3600
#define Cmpss2DACHVALS_Neg_N150A    3400

#define Cmpss1DACHVALS_Pos_N300A    3600
#define Cmpss2DACHVALS_Neg_N300A    3400

#define Cmpss1DACHVALS_Pos_N450A    3600
#define Cmpss2DACHVALS_Neg_N450A    3400

#define Cmpss1DACHVALS_Pos_N600A    3600
#define Cmpss2DACHVALS_Neg_N600A    3400

#define Cmpss1DACHVALS_Pos_N750A    3600
#define Cmpss2DACHVALS_Neg_N750A    3500

#define cPwmSyncOff      0
#define cPwmSyncOn       1

#define cDeadTime           100     //200 wsy/20240628 100*8.33ns=833ns

#define SwitchPeriod25u     1500    //40KHz UP-DOWN Mode
#define SwitchPeriod12u5    750     //80KHz UP-DOWN Mode
#define SwitchPeriod8u33    500     //120KHz UP-DOWN Mode
#define SwitchPeriod6u25    375     //160KHz UP-DOWN Mode

#define SwitchPeriod25u_UpMode      (1500 * 2 - 1)  //40Khz UP Mode
#define SwitchPeriod12u5_UpMode     (750 * 2 - 1)   //80Khz UP Mode
#define SwitchPeriod8u33_UpMode     (500 * 2 - 1)   //120Khz UP Mode
#define SwitchPeriod6u25_UpMode     (375 * 2 - 1)   //160Khz UP Mode

#define CBCmode     0
#define T1mode      1









/********************************************************************************
* Variable declaration                              *
********************************************************************************/
Initial_EXT Uint16 PwmSyncStatus;

typedef enum PWM_STATUS
{
	PWMwaitSts = 0,
	PWMrunSts
}PWM_status;	
Initial_EXT PWM_status PWMstatus;

typedef enum SWITCH_FREQUENCY
{
   Fsw40KHz = 0,
   Fsw80KHz = 1,
   Fsw120KHz,
   Fsw160KHz
}SwitchFreqency;
Initial_EXT  SwitchFreqency SwitchFreq;

Initial_EXT int SwitchPeriodReg;





/********************************************************************************
* function declaration                              *
********************************************************************************/
Initial_EXT void Timer0Init( uint32_t ns100);
Initial_EXT void Timer0ReSet(uint32_t period);
Initial_EXT void Timer0Stop();
Initial_EXT void Timer1Init( uint32_t ns100);
Initial_EXT void Timer1ReSet(uint32_t period);
Initial_EXT void Timer1Stop();
//Initial_EXT void Timer2Stop();
//Initial_EXT void Timer2Resume();
//Initial_EXT void Timer2Init( void );
Initial_EXT void Timer2Init( uint32_t ns100);
Initial_EXT void Timer2ReSet(uint32_t period);
Initial_EXT void Timer2Stop();
Initial_EXT uint16_t Timer2IsStart();


Initial_EXT void InitialDSP( void );
Initial_EXT void sInitEPWM(void);
Initial_EXT void sInitADC(void);
Initial_EXT void InitSciA(void);
Initial_EXT void InitSciB(void);
Initial_EXT void InitSciC(void);
Initial_EXT void InitSciToRemote(uint16_t sci);
Initial_EXT void InitSCIAll(void);
Initial_EXT void InitI2C(void);
Initial_EXT void InitCAN(void);
Initial_EXT void InitCMPSS1(void);
Initial_EXT void InitCMPSS2(void);
Initial_EXT	void InitCMPSS3(void);
Initial_EXT	void InitCMPSS4(void);
Initial_EXT	void InitCMPSS(void);
	

Initial_EXT void sInitGpio(void);
Initial_EXT void sInitECAP(void);
Initial_EXT void SideALargePWMON(void);
Initial_EXT void SideALargePWMOFF(void);

Initial_EXT void SideALargePWMON_TZ(void);
Initial_EXT void SideALargePWMON_ISR(void);
Initial_EXT void SideALargePWMOFF_ISR(void);

Initial_EXT void SideALargePWMON_TZ_ISR(void);
Initial_EXT void SideALargePWMOFF_TZ_ISR(void);
Initial_EXT void SideALargePWMOFF_TZ(void);
Initial_EXT void EPWM2A_ZRO_PRD_SET(void);
Initial_EXT void EPWM2A_ZRO_PRD_CLR(void);
Initial_EXT void EPWM2B_ZRO_PRD_SET(void);
Initial_EXT void EPWM2B_ZRO_PRD_CLR(void);
Initial_EXT void EPWM3A_ZRO_PRD_SET(void);
Initial_EXT void EPWM3A_ZRO_PRD_CLR(void);
Initial_EXT void EPWM3B_ZRO_PRD_SET(void);
Initial_EXT void EPWM3B_ZRO_PRD_CLR(void);
Initial_EXT void LoadHwCurrPara(void);
Initial_EXT void LoadAdjustPara(void);
Initial_EXT void DataInit(void);
Initial_EXT PWM_status GetPWMstatus(void);
Initial_EXT void InitEPWM1(void);
Initial_EXT void InitEPWM2(void);
Initial_EXT void InitEPWM3(void);
Initial_EXT void InitEPWM4(void);
Initial_EXT void InitEPWM5(void);
Initial_EXT void InitEPWM6(void);
Initial_EXT void InitEPWM7(void);

Initial_EXT void PwmSyncOff_TZ(void);
Initial_EXT void PwmSyncON_TZ(void);

Initial_EXT void PwmSyncOff_Isr(void);
Initial_EXT void PwmSyncON_Isr(void);


Initial_EXT void SetSideALargePWMstatus(PWM_status status);

#endif

