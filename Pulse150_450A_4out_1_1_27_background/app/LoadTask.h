/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : LoadTask.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _LoadTask_H_
#define _LoadTask_H_
#ifdef LoadTask_GLOBALS
#define LoadTask_EXT
#else
#define LoadTask_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "Interrupt.h"
/********************************************************************************
*const define                               *
********************************************************************************/
#define cLoadLedOff			0
#define cLoadLedBlinkSlow	1
#define cLoadLedBlinkFast	2
#define cLoadLedOn			3

#define cLoadTimer300MS    15
#define cLoadTimer1S       50
#define cLoadTimer2S       100
#define cLoadTimer5S       250
#define cLoadTimer10S      500

#define cBusVolt1msCnt      40
#define cBusVolt10msCnt     400
#define cBusVolt100msCnt    4000
#define cBusVolt1sCnt       40000

#define cFanRunTemp		38
#define cFanLevelBack	35


#define  cVBusAdjustRatio    1.0
#define  FILTER(ik,iVar,lMean)  (((((long)(iVar)<<16)-(lMean))>>((ik)+1))+(lMean))
#define TEMP_10K_LENGTH  64

typedef enum NTC_STATUS
{
    TempInvalid = 0,
	TempChkDisable = 1,
    NTCFault = 2,
    TempOverHigh,
    TempOverLow,
    TempNormal
}NTC_Status;







/********************************************************************************
* Variable declaration                              *
********************************************************************************/
typedef struct Temperature
{
	float value;
	float OverLow;
	float OverHigh;
	int  TempChkEnable;
	int  OverLowEnable;
	int  OverHighEnable;
	int  OverLowcntset;
	int  OverHighcntset;
	int  levelback;
	int  FaultCount;
	int  OverLowCount;
	int  OverHighCount;
	int  BackCount;
	int  NTCcntset;
	int  backcntset;
	float  NTChighlevel;
	float  NTClowlevel;
	NTC_Status Status;

}stTemperature;

LoadTask_EXT stTemperature AmbientTemp;
LoadTask_EXT stTemperature SinkTemp;


typedef struct BUS_CHECK
{
	float highlevel;
	float highback;
	float lowlevel;
	float lowback;
	int16 highflag;
	int16 lowflag;
	int16 highEnable;
	int16 lowEnable;
	float compvalue;
	int16 highcount;
	int16 highcountset;
	int16 highbackcount;
	int16 highbackcountset;
	int16 lowcount;
	int16 lowcountset;
	int16 lowbackcount;
	int16 lowbackcountset;
	int16 highturnoff;
	int16 lowturnoff;

}stBusCheck;

LoadTask_EXT stBusCheck BusCheck;       //¹ÊÕÏ
LoadTask_EXT stBusCheck BusCheckWarn;   //¸æ¾¯

typedef struct AVERAGE_VALUE
{
	float Sum;
	float Sumtemp;
	float AverageValue;
	float AdjustRatio;
	Uint32 CountTemp;	
	Uint32 Count;
}stAverageValue;
LoadTask_EXT stAverageValue  VBus;

LoadTask_EXT int16 LoadCurrHighCnt;
LoadTask_EXT int16 LoadVoltHighCnt;
LoadTask_EXT int16 LoadVoltAvg;
LoadTask_EXT int16 LoadVoltAvgPos;
LoadTask_EXT int16 LoadVoltAvgNeg;
LoadTask_EXT int16 LoadHighLimitVolt;
LoadTask_EXT int16 LoadCurrAvg;
LoadTask_EXT int16 LoadCurrAvgPos;
LoadTask_EXT int16 LoadCurrAvgNeg;
LoadTask_EXT int16 LoadHighLimitCurr;


LoadTask_EXT Uint16 LED_OH_Cnt;

LoadTask_EXT float AmbientDegreeC;
LoadTask_EXT float SinkDegreeC;

LoadTask_EXT int32_t AmbientTempSum;
LoadTask_EXT int16 AmbientTempADC;
LoadTask_EXT int32_t SinkTempSum;
LoadTask_EXT int16 SinkTempADC;

LoadTask_EXT float VavgPosRatio;
LoadTask_EXT float VavgNegRatio;
LoadTask_EXT float VavgPosBias;
LoadTask_EXT float VavgNegBias;
LoadTask_EXT float IavgPosRatio;
LoadTask_EXT float IavgNegRatio;
LoadTask_EXT float IavgPosBias;
LoadTask_EXT float IavgNegBias;

LoadTask_EXT float VavgPosDCRatio;
LoadTask_EXT float VavgNegDCRatio;
LoadTask_EXT float VavgPosDCBias;
LoadTask_EXT float VavgNegDCBias;
LoadTask_EXT float IavgPosDCRatio;
LoadTask_EXT float IavgNegDCRatio;
LoadTask_EXT float IavgPosDCBias;
LoadTask_EXT float IavgNegDCBias;

LoadTask_EXT float IavgPosFilter;
LoadTask_EXT float IavgNegFilter;
LoadTask_EXT float VavgPosFilter;
LoadTask_EXT float VavgNegFilter;

LoadTask_EXT float IavgPosDCFilter;
LoadTask_EXT float IavgNegDCFilter;
LoadTask_EXT float VavgPosDCFilter;
LoadTask_EXT float VavgNegDCFilter;

LoadTask_EXT float IavgPosFilterDisPlay;
LoadTask_EXT float IavgNegFilterDisPlay;
LoadTask_EXT float IavgPosDCFilterDisPlay;
LoadTask_EXT float IavgNegDCFilterDisPlay;

LoadTask_EXT float PulsePosAverageValueSet;
LoadTask_EXT float PulseNegAverageValueSet;

LoadTask_EXT float IavgPosDCRMSLoopErr;
LoadTask_EXT float IavgNegDCRMSLoopErr;
LoadTask_EXT float IavgPosDCRMSLoopInt;
LoadTask_EXT float IavgNegDCRMSLoopInt;
LoadTask_EXT float IavgPosDCRMSLoopOut;
LoadTask_EXT float IavgNegDCRMSLoopOut;
LoadTask_EXT Uint16 DCModeRMSLoopFreqCnt;

LoadTask_EXT float IavgPosRMSLoopErr;
LoadTask_EXT float IavgNegRMSLoopErr;
LoadTask_EXT float IavgPosRMSLoopInt;
LoadTask_EXT float IavgNegRMSLoopInt;
LoadTask_EXT float IavgPosRMSLoopOut;
LoadTask_EXT float IavgNegRMSLoopOut;
LoadTask_EXT Uint16 PulseModeRMSLoopFreqCnt;

LoadTask_EXT float IavgPhaseFilter[21];

/********************************************************************************
* function declaration                              *
********************************************************************************/
LoadTask_EXT void LoadTaskInit(void);
LoadTask_EXT void LoadTask(void * pvParameters);
LoadTask_EXT void blueLedToggle(void);
LoadTask_EXT void	sTempChk(stTemperature* temper);
LoadTask_EXT NTC_Status GetAmbientTempStatus(void);
LoadTask_EXT void AmbientTempChk(void);
LoadTask_EXT void LoadParameterInit(void);
LoadTask_EXT void LoadParameterUpdate(void);
LoadTask_EXT NTC_Status GetSinkTempStatus(void);
LoadTask_EXT void SinkTempChk(void);
LoadTask_EXT void sBusRangeCheck(stBusCheck* input);
LoadTask_EXT int16 GetBusHighFlag(void);
LoadTask_EXT int16 GetBusLowFlag(void);
LoadTask_EXT void BusCheckAction(void);
LoadTask_EXT void LoadAverageCal(stSideXControl* SideX);
LoadTask_EXT float GetLoadVoltAverage(void);
LoadTask_EXT float GetLoadCurrentAverage(void);
LoadTask_EXT float AverageValueCal(stAverageValue* output);

LoadTask_EXT int16_t   sbOverLevelChk(int16 wCompareData, int16 wHighLever, int16 bFilter, int16 *pbCounter);
LoadTask_EXT int16_t   sbUnderLevelChk(int16 wCompareData, int16 wLowLever, int16 bFilter, int16 *pbCounter);
LoadTask_EXT int16_t sbLoadPosVoltHighTimeChk(int16 bHighFilter);
LoadTask_EXT int16_t sbLoadNegVoltHighTimeChk(int16 bHighFilter);
LoadTask_EXT int16_t sbLoadPosCurrHighTimeChk(int16 bHighFilter);
LoadTask_EXT int16_t sbLoadNegCurrHighTimeChk(int16 bHighFilter);
LoadTask_EXT void SetLoadVoltAvg(int16 Avg);
LoadTask_EXT void SetLoadVoltPosAvg(int16 Avg);
LoadTask_EXT void SetLoadVoltNegAvg(int16 Avg);
LoadTask_EXT void SetLoadCurrAvg(int16 Avg);
LoadTask_EXT void SetLoadCurrPosAvg(int16 Avg);
LoadTask_EXT void SetLoadCurrNegAvg(int16 Avg);


LoadTask_EXT void	LoadAvgOverAction(void);
LoadTask_EXT int GetLoadAvgVoltPosOverFlag(void);
LoadTask_EXT int GetLoadAvgVoltNegOverFlag(void);
LoadTask_EXT int GetLoadAvgCurrPosOverFlag(void);
LoadTask_EXT int GetLoadAvgCurrNegOverFlag(void);
LoadTask_EXT Uint16 GetHwExtLoadOC(void);
LoadTask_EXT void ClrHwExtLoadOC(void);

LoadTask_EXT void FAULT_OT_LED_OH(void);
LoadTask_EXT void RUN_LED_OH(void);

LoadTask_EXT void AmbientTempCal(void);
LoadTask_EXT void SinkTempCal(void);

LoadTask_EXT uint16_t GetEndValue(const uint16_t * ptbl, uint16_t tblsize, uint16_t dat);

LoadTask_EXT void DisplayVoltAndCurrFilter(void);
LoadTask_EXT void CalculationPulsePosAndNegAverageValue(void);
LoadTask_EXT void DCModeRMSLoop(void);
LoadTask_EXT void PulseModeRMSLoop(void);

LoadTask_EXT float sliding_average_filter1(float value1);
LoadTask_EXT float sliding_average_filter2(float value1);
LoadTask_EXT float sliding_average_filter3(float value1);
LoadTask_EXT float sliding_average_filter4(float value1);

#endif
