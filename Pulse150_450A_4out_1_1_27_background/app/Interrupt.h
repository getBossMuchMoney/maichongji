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
#include "ParallelTask.h"
/********************************************************************************
*const define                               *
********************************************************************************/
#define     cSideApwm    1
#define     cSideBpwm    2

#define     cDigitalPWMperiod   (float)500




#define    cPWMmode   0
#define    cCMPmode   1
#define    cMIXmode	  2

#define  cPWMPERIOD   1500
#define  cCMPPERIOD   (cPWMPERIOD*2)

#define cPWMrunSts  1
#define cPWMwaitSts 0

#define cPositiveCurrent  1
#define cNegtiveCurrent   0

#define cPosCurrentBias   -20
#define cNegCurrentBias   -1
#define cPosCurrentBias2  -20
#define cNegCurrentBias2  -17
#define cSmallCurrentBias   (-1891)
#define cOutPutVoltBias     (-1489)	//1.2/3.3*4095   
#define cBusVolttBias      0
#define cAmbientTemperatureBias  0
#define cSinkTemperatureBias     0
#define cSwitchDIPBias           0

#define cPosCurrentRatio         (1.6117)   //10/((4095/3.3)*(3.3/660)), 10times
#define cNegCurrentRatio         (1.6117)   //10/((4095/3.3)*(3.3/660)), 10times

#define cPosCurrentRatioDiv         (0.62046)   //1/cPosCurrentRatio
#define cNegCurrentRatioDiv         (0.62046)   //1/cNegCurrentRatio

#define cPosCurrentRatio2        (1.6117)   //10/((4095/3.3)*(3.3/660)), 10times
#define cNegCurrentRatio2        (1.6117)   //10/((4095/3.3)*(3.3/660)), 10times
#define cSmallCurrentRatio       (2.442)	//100/40.95 , 100times
#define cOutPutVoltRatio         (2.619)	//100/((4095/3.3)*(1.6/52)), 100times//(3.2185)  //100/31.07 , 100times
#define cBusVoltRatio            (2.442)   //100/40.95 , 100times
#define cAmbientTemperatureRatio  (1.0)
#define cSinkTemperatureRatio     (1.0)
#define cSwitchDIPRatio           (1.0)

#define cSmallCurrModeThreshold		(300)	//30.0A

#define cPosPulseSmallCurrThreshold	(50.0)	//50.0A
#define cNegPulseSmallCurrThreshold	(150.0)	//150.0A

#define cExtSynThreshold		(100)		//us














/********************************************************************************
* Variable declaration                              *
********************************************************************************/
typedef enum PULSE_PHASE
{
    PulsePhase0 = 0,
    PulsePhase1 = 1,
    PulsePhase2,
    PulsePhase3,
    PulsePhase4,
    PulsePhase5,
    PulsePhase6,
    PulsePhase7,
    PulsePhase8,
    PulsePhase9,
    PulsePhase10,
    PulsePhase11,
    PulsePhase12,
    PulsePhase13,
    PulsePhase14,
    PulsePhase15,
    PulsePhase16,
    PulsePhase17,
    PulsePhase18,
    PulsePhase19,
    PulsePhase20,
    PulsePhase21
}PulsePowerPulsePhase;

typedef struct STEP_PARAMETER
{
    long Vsum;
    long Vsum_temp;
    long Isum;
    long Isum_temp;
	long IPossum_temp;
	long INegsum_temp;
	long IPossum;
	long INegsum;
    float Iavg;
    float Vavg;
	Uint32 timecountLimit;
    Uint32 timecount; 
    Uint32 timecountset;
    float  reference;
    float  Vload; 
	long  VloadPosSum;
	long  VloadNegSum;
	long  VloadPosSum_temp;
	long  VloadNegSum_temp;
    float  Vinput; 
    float  Ismallsample;
    float  feedback; 
	float  feedback_0;
	float  feedback_1;
    float  IlargesamplePos;
    float  IlargesampleNeg;
    float  error_0;  
    float  error_1;
    float  error_2;
	float  errorUplimit;  
    float  errorDwlimit;
	float  cmpss;
    float        B1;
    float        B2;
    float        A0;
    float        A1;
    float        A2;
    float        yn; // yn = B1 * yn_1 + B2 * yn_2 + A0 * error_0 + A1 * error_1 + A2 *error_2;
    float        yn_1;
    float       yn_2;
    float       gain;
	float      dutyConst;
    float   duty;  // duty = (yn + Vload)/Vinput
    float   dutySumTemp;  // duty = (yn + Vload)/Vinput
    float   dutySum; 
    float   dutyAvg;  // duty = (yn + Vload)/Vinput
    float   cali;
	float referenceStep;
	float reference_1;
	float reference_2;
	float DeltaReference;
	float referenceBase;
	float CMPSSVALUEPos;
	float CMPSSVALUENeg;
}stStepParameter;

typedef struct SIDE_X_CONTROL
{
    float Vavg_Max;
    float Vavg_Min;
    float Iavg_Max;
    float Iavg_Min;
	float Vavg;
	float VavgPos;
    float VavgNeg;
	float VavgPosDC;
    float VavgNegDC;
	
    float Iavg;
    float IavgPos;
    float IavgNeg;	
	float IavgPosDC;
    float IavgNegDC;
	float  IPosNominal;
	float  INegNominal;
    Uint32 PulseControlCnt;
	Uint32 SumOfPeriod;
    PulsePowerPulsePhase PulsePhase;
    float smalllevel;
    float Ismallsample;
    float IlargesamplePos;
    float IlargesampleNeg;
	long Isum_temp;
	long Vsum_temp;
	long VsumPos_temp;
	long VsumNeg_temp;
	long VsumPos;
	long VsumNeg;

	long VLoadsumPos_temp;
	long VLoadsumNeg_temp;
	long VLoadsumPos;
	long VLoadsumNeg;

	long VloadPosSum_temp2;
	long VloadNegSum_temp2;

	long IPossum_temp2;
	long INegsum_temp2;
	
	long Vsum;
	long Isum;
	long IsumPos_temp;
	long IsumNeg_temp;
	long IsumPos;
	long IsumNeg;
	long timecountset;
	long timecountset_temp;
	long timecountsetPos_temp;
	long timecountsetNeg_temp;
	long timecountsetPos;
	long timecountsetNeg;

    long IloadPhasesum_temp2[21];
    long timecountsetPhase_temp2[21];
    long IloadPhasesum[21];
    long timecountsetPhase[21];
    float IavgPhase[21];

    PulsePowerPulsePhase PhaseTimeSetNoZero;

    stStepParameter step[22];
}stSideXControl;

typedef struct OVER_CHECK
{
    float Overlevel;
    float Overback;
    int16 Overflag;
    int16 Enable;
    float compvalue;
    int16 Overcount;
    int16 Overbackcount;
	int16 Overcountset;
    int16 Overbackcountset;
}stOverCheck;

Interrup_EXT stOverCheck PosI,NegI,LoadV;
Interrup_EXT stOverCheck LoadVoltPosAvg;
Interrup_EXT stOverCheck LoadVoltNegAvg;
Interrup_EXT stOverCheck LoadCurrPosAvg;
Interrup_EXT stOverCheck LoadCurrNegAvg;
Interrup_EXT stOverCheck Power24Vcheck;




typedef enum CONTROL_TYPE
{
   InvalidLoop = 0,
   OpenLoop = 1,
   CloseLoop
}ControlType;
ParallelTask_EXT ControlType ControlMode;

typedef struct ADJUST_FACTOR
{
    float ratio;
    float bias;
}sAdjustFactor;


//Interrup_EXT sAdjustFactor PosCurrAdjustFactor;
//Interrup_EXT sAdjustFactor NegCurrAdjustFactor;
//Interrup_EXT sAdjustFactor PosVoltAdjustFactor;
//Interrup_EXT sAdjustFactor NegVoltAdjustFactor;
//Interrup_EXT sAdjustFactor PosSmallCurrAdjustFactor;
//Interrup_EXT sAdjustFactor NegSmallCurrAdjustFactor;
//Interrup_EXT sAdjustFactor PosSmallVoltAdjustFactor;
//Interrup_EXT sAdjustFactor NegSmallVoltAdjustFactor;
//Interrup_EXT sAdjustFactor PosCmpCurrAdjustFactor;
//Interrup_EXT sAdjustFactor NegCmpCurrAdjustFactor;
//Interrup_EXT sAdjustFactor PosCmpVoltAdjustFactor;
//Interrup_EXT sAdjustFactor NegCmpVoltAdjustFactor;
//Interrup_EXT sAdjustFactor PosCmpSmallCurrAdjustFactor;
//Interrup_EXT sAdjustFactor NegCmpSmallCurrAdjustFactor;
//Interrup_EXT sAdjustFactor PosCmpSmallVoltAdjustFactor;
//Interrup_EXT sAdjustFactor NegCmpSmallVoltAdjustFactor;
//Interrup_EXT sAdjustFactor PosCmpDispSmallCurrAdjustFactor;
//Interrup_EXT sAdjustFactor NegCmpDispSmallCurrAdjustFactor;
//Interrup_EXT sAdjustFactor PosCmpDispCurrAdjustFactor;
//Interrup_EXT sAdjustFactor NegCmpDispCurrAdjustFactor;
//Interrup_EXT sAdjustFactor PosPulseCurrAdjustFactor;
//Interrup_EXT sAdjustFactor NegPulseCurrAdjustFactor;
//Interrup_EXT sAdjustFactor PosPulseSmallCurrAdjustFactor;
//Interrup_EXT sAdjustFactor NegPulseSmallCurrAdjustFactor;

//定义数组，10段校正参数
Interrup_EXT sAdjustFactor PosCurrAdjustFactorArray[10];
Interrup_EXT sAdjustFactor NegCurrAdjustFactorArray[10];
Interrup_EXT sAdjustFactor PosCmpCurrAdjustFactorArray[10];
Interrup_EXT sAdjustFactor NegCmpCurrAdjustFactorArray[10];

Interrup_EXT sAdjustFactor PosVoltAdjustFactorArray[10];
Interrup_EXT sAdjustFactor NegVoltAdjustFactorArray[10];
Interrup_EXT sAdjustFactor PosCmpVoltAdjustFactorArray[10];
Interrup_EXT sAdjustFactor NegCmpVoltAdjustFactorArray[10];

Interrup_EXT sAdjustFactor PosCmpDispCurrAdjustFactorArray[10];
Interrup_EXT sAdjustFactor NegCmpDispCurrAdjustFactorArray[10];

Interrup_EXT float AdjustFactorSectionDivide[10];


Interrup_EXT int MixModeEnable;
Interrup_EXT int MixModeFlag;
Interrup_EXT int MixModeFlagBefore;

Interrup_EXT int CMPmodePWMCountMode;   //0:UP-DOWN Count Mode  1:UP Count Mode

Interrup_EXT Uint16 HwOCCountSet;
Interrup_EXT Uint16 HwOCCount;

Interrup_EXT Uint16 HwOCRecoveryTimeSet;
Interrup_EXT Uint16 HwOCRecoveryTimeCount;

Interrup_EXT Uint16 HwOCCheckEnable;
Interrup_EXT Uint32 HwOCFaultTime;
Interrup_EXT Uint32 HwOCFaultTimeCnt;
Interrup_EXT Uint16 HwOCFaultRecoverySet;
Interrup_EXT Uint16 HwOCFaultRecoverySetCnt;
Interrup_EXT Uint16 HwOCFaultRecoveryFlag;

Interrup_EXT float CMPmodeSlopePos;
Interrup_EXT float CMPmodeSlopeNeg;
Interrup_EXT float CMPmodeReferenceTemp;
Interrup_EXT Uint16 CMPmodeSlopeEnable;

typedef enum
{
    Module_P100A_N300A = 0,
    Module_P150A_N450A = 1,
    Module_P200A_N600A = 2,
    Module_P150A_N750A = 3,
    Module_P50A_N150A
}eMODULE_TYPE;
Interrup_EXT eMODULE_TYPE Moduletype;   //0:100A/-300A  1:150A/-450A  2:200A/-600A  3:150A/-750A  4:50A/-150A

Interrup_EXT Uint16 Cmpss1DACHVALS_Pos;
Interrup_EXT Uint16 Cmpss2DACHVALS_Neg;

Interrup_EXT int AdjustMode;

typedef struct ADJUST_VALUE
{
	float x;
	float y;
}sAdjustValue;
Interrup_EXT sAdjustValue AdjustValue[8];
Interrup_EXT sAdjustValue AdjustValue2[8];


Interrup_EXT Uint16 PosCurrentResultRegArray[10];
Interrup_EXT Uint16 NegCurrentResultRegArray[10];
Interrup_EXT float PosCurrentResultRegAvg;
Interrup_EXT float NegCurrentResultRegAvg;
Interrup_EXT float PosCurrentArrayAvg;
Interrup_EXT float NegCurrentArrayAvg;

Interrup_EXT Uint16 OutPutVoltResultRegArray[10];
Interrup_EXT float OutPutVoltResultRegAvg;
Interrup_EXT float OutPutVoltArrayAvg;


Interrup_EXT stOverCheck PosI,NegI,LoadV;


Interrup_EXT float OutPutVoltSample;
Interrup_EXT float OutPutVoltBias; 
Interrup_EXT float OutPutVolt;
Interrup_EXT float OutPutVoltRatio;


Interrup_EXT float AmbientTemperatureSample;
Interrup_EXT float AmbientTemperatureBias; 
Interrup_EXT float AmbientTemperature;
Interrup_EXT float AmbientTemperatureRatio;

Interrup_EXT float SinkTemperatureSample;
Interrup_EXT float SinkTemperatureBias; 
Interrup_EXT float SinkTemperature;
Interrup_EXT float SinkTemperatureRatio;

Interrup_EXT float PosCurrentSample;
Interrup_EXT float PosCurrentBias; 
Interrup_EXT float PosCurrent;
Interrup_EXT float PosCurrentRatio;

Interrup_EXT float PosCurrentSample2;
Interrup_EXT float PosCurrentBias2; 
Interrup_EXT float PosCurrent2;
Interrup_EXT float PosCurrentRatio2;

Interrup_EXT float SmallCurrentSample;
Interrup_EXT float SmallCurrentBias; 
Interrup_EXT float SmallCurrent;
Interrup_EXT float SmallCurrentRatio;



Interrup_EXT float NegCurrentSample;
Interrup_EXT float NegCurrentBias; 
Interrup_EXT float NegCurrent;
Interrup_EXT float NegCurrentRatio;

Interrup_EXT float NegCurrentSample2;
Interrup_EXT float NegCurrentBias2; 
Interrup_EXT float NegCurrent2;
Interrup_EXT float NegCurrentRatio2;


Interrup_EXT float BusVoltSample;
Interrup_EXT float BusVolttBias; 
Interrup_EXT float BusVolt;
Interrup_EXT float BusVoltSum;
Interrup_EXT float BusVoltSumTemp;
Interrup_EXT int   BusVoltCnt;
Interrup_EXT float BusVoltAvg;

Interrup_EXT float BusVoltRatio;

Interrup_EXT float SwitchDIPSample;
Interrup_EXT float SwitchDIPBias; 
Interrup_EXT float SwitchDIP;
Interrup_EXT float SwitchDIPRatio;


Interrup_EXT stSideXControl SideAControl;

Interrup_EXT int SidePWMjudge;

Interrup_EXT float pwm1A,pwm2A,pwm3A,pwm4A,pwm5A,pwm6A,pwm7A,pwm8A;


Interrup_EXT int SideALargePWMstatus;

//Interrup_EXT volatile int ADC_A1_ISR_Count;

Interrup_EXT int PosOrNegCurrentA;
Interrup_EXT float SideAduty;

//Interrup_EXT int DACHVALSTest;

Interrup_EXT int OverLoadCnt;
Interrup_EXT int OverLoadBackCnt;

Interrup_EXT int PulsePhaseNum;

Interrup_EXT int32 LoadCurrentCnt;
Interrup_EXT float OverLoadLevel;
Interrup_EXT int OverLoadCntSet;
Interrup_EXT int OverLoadBackCntSet;
Interrup_EXT int OverLoadCheckEnable;


Interrup_EXT int ModuleSyncPeriod;
Interrup_EXT int SmallCurrModeThreshold;  //目前实际是混合模式门槛电流值

Interrup_EXT Uint32 TimerPeriod_100ns;
Interrup_EXT int32 CapIntDelay;
Interrup_EXT Uint32 CapIntDelayCnt;
Interrup_EXT int32 CanBusDelay;
Interrup_EXT int32 TimerProcDelay;
Interrup_EXT Uint32 Epwm5Count;
Interrup_EXT Uint16 UnitCommTimeout[4];
Interrup_EXT Uint16 CommTimeoutProhibitSendSyncFlag;
Interrup_EXT Uint16 CommTimeoutToRemoteFlag[4];


Interrup_EXT Uint16 PosPWMdutyOpenLoop;
Interrup_EXT Uint16 NegPWMdutyOpenLoop;
Interrup_EXT Uint16 PosCMPDACOpenLoop;
Interrup_EXT Uint16 NegCMPDACOpenLoop;
Interrup_EXT Uint16 PwmOpenLoopEnable;
Interrup_EXT Uint16 CmpOpenLoopEnable;
Interrup_EXT uint16_t AdjustRatio1;
Interrup_EXT uint16_t AdjustRatio2;

/********************************************************************************
* function declaration                              *
********************************************************************************/

Interrup_EXT interrupt void epwm1ISR(void);
Interrup_EXT interrupt void ADC_A1_ISR(void);
Interrup_EXT interrupt void PWMsync_CAP1_ISR(void);
Interrup_EXT interrupt void Linesync_CAP2_ISR(void);
Interrup_EXT interrupt void EPWM2_TZ1_ISR(void);

Interrup_EXT void PhaseControlSideA(void);
Interrup_EXT void sPulseControl(int SideXpwm,float VloadSample,float VinSample_large,float VinSample_small,stSideXControl* SideX,PulsePowerPulsePhase phase);
Interrup_EXT void sOverProtect(stOverCheck* input);
Interrup_EXT int GetPosIoverFlag(void);
Interrup_EXT int GetNegIoverFlag(void);
Interrup_EXT int GetLoadVoverFlag(void);

Interrup_EXT void	OverCurrentActionSW(void);
Interrup_EXT void	OverVoltageActionSW(void);
Interrup_EXT void sDC_Control(int SideXpwm,float VloadSample,float VinSample_large,float VinSample_small,stSideXControl* SideX,PulsePowerPulsePhase phase);

Interrup_EXT void sOpenLoopControl(int SideXpwm,int PosOrNeg,float dutycycle);
Interrup_EXT void PWMsyn(stXparallel* input);

Interrup_EXT ControlType	GetControlMode(void);

Interrup_EXT void	sLevelProtect(stOverCheck* input);
Interrup_EXT void Power24Vdetect(void);
Interrup_EXT int16 GetPower24Vunderflag(void);

Interrup_EXT __interrupt void canISR(void);
//Interrup_EXT void sOverLoad(void);

Interrup_EXT interrupt void Timer0Isr();
Interrup_EXT interrupt void Timer1Isr();
Interrup_EXT interrupt void Timer2Isr();

Interrup_EXT interrupt void epwm4ISR(void);
Interrup_EXT interrupt void epwm5ISR(void);

#endif
