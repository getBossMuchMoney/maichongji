/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : Interrupt.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define Interrupt_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "Interrupt.h"
#include "Constant.h"
#include "f280013x_pie_defines.h"
#include "Initial.h"
#include "SuperTask.h"
#include "LoadTask.h"
#include "CanTask.h"
#include "InterfaceTask.h"
#include "ParallelTask.h"
#include "OsConfig.h"
#include "squeue.h"
#include "canmodule.h"
#include "fault.h"
#include "math.h"
#include "Common.h"
#include "SyncProc.h"
/********************************************************************************
*const define                               *
********************************************************************************/
//#define INTERRUPT_NESTING_ENABLE

/********************************************************************************
* Variable declaration                              *
********************************************************************************/
volatile uint32_t txCanMsgCount = 0;
volatile uint32_t rxCanMsgCount = 0;
uint16_t timer0RunFlag = 0;
uint16_t timer1RunFlag = 0;

//Miller/20240929/for sync begin
//uint16_t u16_Epwm1Flag = 0;
uint16_t u16_PulseResetFlag = 0;
//Miller/20240929/for sync end
uint16_t PWMorCMPSwitchPre = cPWMmode;
uint16_t PWMorCMPSwitch = cPWMmode;
/*********************************************************************
Function name:  epwm1ISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
#pragma CODE_SECTION(epwm1ISR,".TI.ramfunc");
interrupt void epwm1ISR(void)
{
	int temp;
	//int temp2;
	int PWMorCMPTemp;

//	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
#ifdef INTERRUPT_NESTING_ENABLE
	IER |= (M_INT1 | M_INT4 | M_INT13);
	IER &= (M_INT1 | M_INT4 | M_INT13);
	PieCtrlRegs.PIEACK.all = 0xFFFF;      // Enable PIE interrupts
	
	asm(" nop");
	EINT;

    wBackgroundPwmFreqDivCnt++;
    if(wBackgroundPwmFreqDivCnt >= 40) //1ms
    {
        wBackgroundPwmFreqDivCnt = 0;
        wBackgroundPwmFreqDivFlag = 1;
    }

    if(GetBoardType() == UnitCB)
    {
#else
    wBackgroundPwmFreqDivCnt++;
    if(wBackgroundPwmFreqDivCnt >= 40) //1ms
    {
        wBackgroundPwmFreqDivCnt = 0;
        wBackgroundPwmFreqDivFlag = 1;
    }

	if((sSyncTimerReach() == 0) && (GetBoardType() == UnitCB))
	{
#endif
	
	PosCurrentResultRegArray[1] = AdcaResultRegs.ADCRESULT7;
	PosCurrentResultRegArray[2] = AdcaResultRegs.ADCRESULT8;
	PosCurrentResultRegArray[3] = AdcaResultRegs.ADCRESULT9;
	PosCurrentResultRegArray[4] = AdcaResultRegs.ADCRESULT10;
	PosCurrentResultRegArray[5] = AdcaResultRegs.ADCRESULT11;
	PosCurrentResultRegArray[6] = AdcaResultRegs.ADCRESULT12;
	PosCurrentResultRegArray[7] = AdcaResultRegs.ADCRESULT13;
    PosCurrentResultRegArray[8] = AdcaResultRegs.ADCRESULT14;
    PosCurrentResultRegArray[9] = AdcaResultRegs.ADCRESULT15;

    NegCurrentResultRegArray[1] = AdccResultRegs.ADCRESULT7;
    NegCurrentResultRegArray[2] = AdccResultRegs.ADCRESULT8;
    NegCurrentResultRegArray[3] = AdccResultRegs.ADCRESULT9;
    NegCurrentResultRegArray[4] = AdccResultRegs.ADCRESULT10;
    NegCurrentResultRegArray[5] = AdccResultRegs.ADCRESULT11;
    NegCurrentResultRegArray[6] = AdccResultRegs.ADCRESULT12;
    NegCurrentResultRegArray[7] = AdccResultRegs.ADCRESULT13;
    NegCurrentResultRegArray[8] = AdccResultRegs.ADCRESULT14;
    NegCurrentResultRegArray[9] = AdccResultRegs.ADCRESULT15;

    OutPutVoltResultRegArray[1] = AdccResultRegs.ADCRESULT4;
    OutPutVoltResultRegArray[2] = AdccResultRegs.ADCRESULT5;
    OutPutVoltResultRegArray[3] = AdccResultRegs.ADCRESULT6;
    OutPutVoltResultRegArray[4] = AdcaResultRegs.ADCRESULT5;
    OutPutVoltResultRegArray[5] = AdcaResultRegs.ADCRESULT6;
	
	ExtSynSignalSetting();


	 if(GetBoardType() == UnitCB) 
	 {
	 	UnitPWMSyncSignalLoss();
	 }
  
//	DACHVALSTest++;
//	// Cmpss1Regs.DACHVALS.bit.DACVAL = DACHVALSTest;
//	 if(DACHVALSTest >= 3000)
//	 {
//	     DACHVALSTest = 0;
//	 }

  
//	ADC_A1_ISR_Count++;
//	if(ADC_A1_ISR_Count >= 100)
//	{
//		ADC_A1_ISR_Count = 0;
//
//		//OSISREventSend(cPrioSuper,eSuperTest);
//
//	}
	
	while(ADC_isBusy(ADCA_BASE) == pdTRUE);

		//---------------------read adc sampling result----------------------------//

	//PosCurrentSample = ((float)AdcaResultRegs.ADCRESULT0  + PosCurrentBias); //CMP1_HP4;
	//PosCurrent = PosCurrentSample * PosCurrentRatio;
	PosCurrentResultRegArray[0] = AdcaResultRegs.ADCRESULT0;
	PosCurrentResultRegAvg = (float)(PosCurrentResultRegArray[0] + PosCurrentResultRegArray[1] + PosCurrentResultRegArray[2] + \
	                                 PosCurrentResultRegArray[3] + PosCurrentResultRegArray[4] + PosCurrentResultRegArray[5] + \
	                                 PosCurrentResultRegArray[6] + PosCurrentResultRegArray[7] + PosCurrentResultRegArray[8] + \
	                                 PosCurrentResultRegArray[9]) * 0.1f;
	PosCurrentArrayAvg = (PosCurrentResultRegAvg + PosCurrentBias)  * PosCurrentRatio;
	PosCurrent = PosCurrentArrayAvg;

	//NegCurrentSample = ((float)AdccResultRegs.ADCRESULT0  + NegCurrentBias);
	//NegCurrent = NegCurrentSample * NegCurrentRatio;
	NegCurrentResultRegArray[0] = AdccResultRegs.ADCRESULT0;
	NegCurrentResultRegAvg = (float)(NegCurrentResultRegArray[0] + NegCurrentResultRegArray[1] + NegCurrentResultRegArray[2] + \
	                                 NegCurrentResultRegArray[3] + NegCurrentResultRegArray[4] + NegCurrentResultRegArray[5] + \
	                                 NegCurrentResultRegArray[6] + NegCurrentResultRegArray[7] + NegCurrentResultRegArray[8] + \
	                                 NegCurrentResultRegArray[9]) * 0.1f;
	NegCurrentArrayAvg = (NegCurrentResultRegAvg + NegCurrentBias)  * NegCurrentRatio;
	NegCurrent = NegCurrentArrayAvg;

	PosCurrentSample2 = ((float)AdcaResultRegs.ADCRESULT1  + PosCurrentBias2); //CMP1_HP3
	PosCurrent2 = PosCurrentSample2 * PosCurrentRatio2;

	NegCurrentSample2 = ((float)AdccResultRegs.ADCRESULT1  + NegCurrentBias2); //CMP2_HP2
	NegCurrent2 = NegCurrentSample2 * NegCurrentRatio2;

	SmallCurrentSample = ((float)AdcaResultRegs.ADCRESULT2  + SmallCurrentBias); //
	SmallCurrent = SmallCurrentSample * SmallCurrentRatio;
	
	//OutPutVoltSample = ((float)AdccResultRegs.ADCRESULT2  + OutPutVoltBias);
	//OutPutVolt = OutPutVoltSample * OutPutVoltRatio;
	OutPutVoltResultRegArray[0] = AdccResultRegs.ADCRESULT2;
	OutPutVoltResultRegAvg = (float)(OutPutVoltResultRegArray[0] + OutPutVoltResultRegArray[1] + OutPutVoltResultRegArray[2] + \
	                                 OutPutVoltResultRegArray[3] + OutPutVoltResultRegArray[4] + OutPutVoltResultRegArray[5]) * 0.1666667f;
	OutPutVoltArrayAvg = (OutPutVoltResultRegAvg + OutPutVoltBias) * OutPutVoltRatio;
	OutPutVolt = OutPutVoltArrayAvg;

	BusVoltSample = ((float)AdcaResultRegs.ADCRESULT3  + BusVolttBias); 
	if(BusVoltSample <= 0.0)
	{
		BusVoltSample = 1.0;
	}
	
	BusVolt = BusVoltSample * BusVoltRatio;

	AmbientTemperatureSample = ((float)AdccResultRegs.ADCRESULT3  + AmbientTemperatureBias); 
	AmbientTemperature = AmbientTemperatureSample * AmbientTemperatureRatio;

	SinkTemperatureSample = ((float)AdcaResultRegs.ADCRESULT4  + SinkTemperatureBias); 
	SinkTemperature = SinkTemperatureSample * SinkTemperatureRatio;

	//SwitchDIPSample = ((float)AdccResultRegs.ADCRESULT4  + SwitchDIPBias); //SwitchDIP
	//SwitchDIP = SwitchDIPSample * SwitchDIPRatio;
	

	BusVoltSumTemp += BusVolt;
	BusVoltCnt++;
	if(BusVoltCnt >= 8)
	{
		BusVoltCnt = 0;
		BusVoltAvg = BusVoltSumTemp*0.125;
		BusVoltSumTemp = 0;
	
	}

	VBus.CountTemp++;
	VBus.Sumtemp += BusVolt;
    if(VBus.CountTemp >= cBusVolt100msCnt)  
    {
		VBus.Sum = VBus.Sumtemp;
		VBus.Count = VBus.CountTemp;
		VBus.Sumtemp = 0;
		VBus.CountTemp = 0;
		
    }
	
	if(GetWaveForm() == DC)
	{
		LoadCurrentCnt++;
		if(LoadCurrentCnt >= 40000)//1S
		{
		    LoadCurrentCnt = 0;
			SideAControl.step[PulsePhase0].VloadPosSum = SideAControl.step[PulsePhase0].VloadPosSum_temp;						
			SideAControl.step[PulsePhase0].IPossum = SideAControl.step[PulsePhase0].IPossum_temp;
			SideAControl.step[PulsePhase0].IPossum_temp = 0.0;
			SideAControl.step[PulsePhase0].VloadPosSum_temp = 0.0;
			
			SideAControl.step[PulsePhase0].VloadNegSum = SideAControl.step[PulsePhase0].VloadNegSum_temp;						
			SideAControl.step[PulsePhase0].INegsum = SideAControl.step[PulsePhase0].INegsum_temp;
			SideAControl.step[PulsePhase0].INegsum_temp = 0.0;
			SideAControl.step[PulsePhase0].VloadNegSum_temp = 0.0;

		}
			
	
	}
	else
	{
			SideAControl.step[PulsePhase0].IPossum_temp = 0.0;
			SideAControl.step[PulsePhase0].INegsum_temp = 0.0;			
			SideAControl.step[PulsePhase0].VloadPosSum_temp = 0.0;
			SideAControl.step[PulsePhase0].VloadNegSum_temp = 0.0;

			SideAControl.step[PulsePhase0].IPossum = 0.0;
			SideAControl.step[PulsePhase0].INegsum = 0.0;			
			SideAControl.step[PulsePhase0].VloadPosSum = 0.0;
			SideAControl.step[PulsePhase0].VloadNegSum = 0.0;
	}

	//OverCurrentActionSW();
	//OverVoltageActionSW();

	//sOverLoad();

	
	SideAControl.Ismallsample = SmallCurrent;

    if(MixModeEnable)
    {
//        if(GetWaveForm() == DC)
//        {
//            PWMorCMPTemp = PWMorCMP;
//        }
//        else
//        {
            if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= SmallCurrModeThreshold)
            {
                PWMorCMPTemp = cPWMmode;
            }
            else
            {
                PWMorCMPTemp = PWMorCMP;
            }
//        }
    }
    else
    {
        PWMorCMPTemp = PWMorCMP;
    }

	if(AdjustMode)
	{
//		if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= SmallCurrModeThreshold)
//		{
//			SideAControl.IlargesamplePos = fabsf(SmallCurrent*0.1);
//			SideAControl.IlargesampleNeg = fabsf(SmallCurrent*0.1);
//		}

	    SideAControl.IlargesamplePos = PosCurrent;
	    SideAControl.IlargesampleNeg = NegCurrent;
	    /*if(PWMorCMPTemp == cPWMmode)
	    {
	        SideAControl.IlargesamplePos = PosCurrent;
	        SideAControl.IlargesampleNeg = NegCurrent;
	    }
	    else
	    {
            SideAControl.IlargesamplePos = PosCurrentArrayAvg;
            SideAControl.IlargesampleNeg = NegCurrentArrayAvg;
	    }*/
	}
	else
	{
		if(PWMorCMPTemp == cPWMmode)
		{
//			if(GetWaveForm() == DC)
//			{
//				if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= SmallCurrModeThreshold)
//				{
//					SideAControl.IlargesamplePos = fabsf(SmallCurrent*0.1)*PosSmallCurrAdjustFactor.ratio + PosSmallCurrAdjustFactor.bias;
//					SideAControl.IlargesampleNeg = fabsf(SmallCurrent*0.1)*NegSmallCurrAdjustFactor.ratio + NegSmallCurrAdjustFactor.bias;
//				}
//				else
//				{
//					SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactor.ratio + PosCurrAdjustFactor.bias;
//					SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactor.ratio + NegCurrAdjustFactor.bias;
//				}
//			}

            if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= AdjustFactorSectionDivide[1])
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[0].ratio + PosCurrAdjustFactorArray[0].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[0].ratio + NegCurrAdjustFactorArray[0].bias;
            }
            else if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= AdjustFactorSectionDivide[2])
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[1].ratio + PosCurrAdjustFactorArray[1].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[1].ratio + NegCurrAdjustFactorArray[1].bias;
            }
            //else if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= AdjustFactorSectionDivide[3])
            else
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[2].ratio + PosCurrAdjustFactorArray[2].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[2].ratio + NegCurrAdjustFactorArray[2].bias;
            }
            /*else if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= AdjustFactorSectionDivide[4])
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[3].ratio + PosCurrAdjustFactorArray[3].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[3].ratio + NegCurrAdjustFactorArray[3].bias;
            }
            else if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= AdjustFactorSectionDivide[5])
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[4].ratio + PosCurrAdjustFactorArray[4].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[4].ratio + NegCurrAdjustFactorArray[4].bias;
            }
            else if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= AdjustFactorSectionDivide[6])
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[5].ratio + PosCurrAdjustFactorArray[5].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[5].ratio + NegCurrAdjustFactorArray[5].bias;
            }
            else if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= AdjustFactorSectionDivide[7])
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[6].ratio + PosCurrAdjustFactorArray[6].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[6].ratio + NegCurrAdjustFactorArray[6].bias;
            }
            else if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= AdjustFactorSectionDivide[8])
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[7].ratio + PosCurrAdjustFactorArray[7].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[7].ratio + NegCurrAdjustFactorArray[7].bias;
            }
            else
            {
                SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactorArray[8].ratio + PosCurrAdjustFactorArray[8].bias;
                SideAControl.IlargesampleNeg = NegCurrent*NegCurrAdjustFactorArray[8].ratio + NegCurrAdjustFactorArray[8].bias;
            }*/

//			else
//			{
//				if(SideAControl.step[SideAControl.PulsePhase].reference > SmallCurrModeThreshold)
//					SideAControl.IlargesamplePos = PosCurrent*PosCurrAdjustFactor.ratio + PosCurrAdjustFactor.bias;
//				else
//					SideAControl.IlargesamplePos = PosCurrent*PosSmallCurrAdjustFactor.ratio + PosSmallCurrAdjustFactor.bias;
//				SideAControl.IlargesampleNeg = NegCurrent;
//			}
		}
		else
		{
//			if(fabsf(SideAControl.step[SideAControl.PulsePhase].reference) <= SmallCurrModeThreshold)
//			{
//				SideAControl.IlargesamplePos = fabsf(SmallCurrent*0.1);
//				SideAControl.IlargesampleNeg = fabsf(SmallCurrent*0.1);
//			}
//			else
			{
				SideAControl.IlargesamplePos = PosCurrent;
				SideAControl.IlargesampleNeg = NegCurrent;
				//SideAControl.IlargesamplePos = PosCurrentArrayAvg;
				//SideAControl.IlargesampleNeg = NegCurrentArrayAvg;
			}
		}
	}

//	if(testflag)
//	{
//		if(DACHVALSTest == 400)
//		{
//			waveRecord(ePosCurrWave,1,PosCurrent);
//			waveRecord(eNegCurrWave,1,NegCurrent);
//		}
//		else
//		{
//			waveRecord(ePosCurrWave,0,PosCurrent);
//			waveRecord(eNegCurrWave,0,NegCurrent);
//		}
//	}
	if(GetPWMstatus() == PWMrunSts)
	{
		
		if(GetControlMode() == CloseLoop)
		{
			if(GetWaveForm() == DC)
			{
				sDC_Control(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase0);
			}
			else if(GetWaveForm() == Pulse)
			{
			    /*for(temp = 1; temp <= 20; temp++)
			    {
			        //20段里找最先不是0时间的段
			        if(SideAControl.step[temp].timecountset != 0)
			        {
			            SideAControl.PhaseTimeSetNoZero = (PulsePowerPulsePhase)temp;
			            break;
			        }
			        //如果20段时间全部为零，则清除显示值，并封波
			        else if(temp == 20)
			        {
			            SideAControl.IsumPos = 0;
			            SideAControl.IsumNeg = 0;
			            SideAControl.VLoadsumPos = 0;
			            SideAControl.VLoadsumNeg = 0;
			            SideAControl.timecountset = 0;
			            SideAControl.timecountsetPos = 0;
			            SideAControl.timecountsetNeg = 0;

                        for(temp2 = 1; temp2 <= 20; temp2++)
                        {
                            SideAControl.IloadPhasesum[temp2] = 0;
                            SideAControl.timecountsetPhase[temp2] = 0;
                        }

                        SideALargePWMOFF_ISR();
                        break;
			        }
			        else
			        {
			            //do nothing
			        }
			    }*/

			    //Miller/20240929/for sync begin
                //u16_Epwm1Flag = 1;
				PhaseControlSideA();
				//u16_Epwm1Flag = 0;

                //Miller/20240929/for sync begin
                //if(u16_PulseResetFlag == 1)
                //{
                    //u16_PulseResetFlag = 0;
                    //StepTimecountClr();
                //}
                //Miller/20240929/for sync end
			}
			else
			{
			
			}
		
		}
		else if(GetControlMode() == OpenLoop)
		{
			sOpenLoopControl(cSideApwm,PosOrNegCurrentA,SideAduty);
		}
		else
		{
		
		}
	}
	else
	{
			for (temp = 0; temp < 22; temp++)
			{
				SideAControl.step[temp].yn_1 = 0.0;			
				SideAControl.step[temp].feedback_1 = 0.0;
			}
			Cmpss1Regs.DACHVALS.bit.DACVAL = Cmpss1DACHVALS_Pos;
			CmpssLite2Regs.DACHVALS.bit.DACVAL = Cmpss2DACHVALS_Neg;

		OverLoadCnt = 0;
		OverLoadBackCnt = 0;
		 WarnRegs.TempWarn.bit.OverLoad = 0;
		sClrISRWarningCode(cWHwOverLoad);

		SideALargePWMOFF_ISR();

		MixModeFlag = 0;
		MixModeFlagBefore = 0;

		HwOCCount = 0;
		HwOCRecoveryTimeCount = 0;

        CMPmodeReferenceTemp = 0.0f;
	}

	//硬件过流故障处理
	HwOCFaultTimeCnt++;
	if(HwOCFaultTimeCnt >= HwOCFaultTime)
	{
	    HwOCFaultTimeCnt = 0;
	    HwOCFaultRecoverySetCnt = 0;
	}
	if(HwOCFaultRecoverySetCnt >= HwOCFaultRecoverySet)
	{
	    HwOCFaultRecoveryFlag = 1;
	}

	if(HwOCCheckEnable == 1)
    {
        if((EPwm2Regs.TZFLG.bit.OST == 1) || (EPwm3Regs.TZFLG.bit.OST == 1))
        {
            HwOCRecoveryTimeCount = 0;

            HwOCCount++;
            if(HwOCCount >= HwOCCountSet)
            {
                HwOCCount = HwOCCountSet;

                SideALargePWMOFF_ISR();
                SetSideALargePWMstatus(PWMwaitSts);
                FaultRegs.HWfaultLoad.bit.HwExtLoadOC = 1;
                sSetISRFaultCode(cFHwExtLoadOC);
                OSISREventSend(cPrioSuper,eSuperFault);

                HwOCFaultRecoverySetCnt++;
            }

            // Clear the OST flag
            EALLOW;
            EPwm2Regs.TZCLR.bit.OST = 1;
            EPwm3Regs.TZCLR.bit.OST = 1;
            EDIS;
        }

        HwOCRecoveryTimeCount++;
        if(HwOCRecoveryTimeCount >= HwOCRecoveryTimeSet)
        {
            HwOCRecoveryTimeCount = 0;
            HwOCCount = 0;  //reset to 0
        }
    }
    else
    {
        // Clear the OST flag
        EALLOW;
        EPwm2Regs.TZCLR.bit.OST = 1;
        EPwm3Regs.TZCLR.bit.OST = 1;
        EDIS;

        HwOCCount = 0;
        HwOCRecoveryTimeCount = 0;
        HwOCFaultTimeCnt = 0;
        HwOCFaultRecoverySetCnt = 0;
    }

//	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
#ifdef INTERRUPT_NESTING_ENABLE
	DINT;
	}
#else
	}
#endif
   // EPwm1Regs.ETCLR.bit.INT = 1;
   // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
       // Clear INT flag for this timer
    //
//    EPWM_clearEventTriggerInterruptFlag(EPWM1_BASE);
	EPwm1Regs.ETCLR.bit.INT = 1;
    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
//	GpioDataRegs.GPASET.bit.GPIO9 = 1;
}
/*********************************************************************
Function name:  adcA1ISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
interrupt void ADC_A1_ISR(void)
{






	 ExtSynSignalSetting();

	
	 if(GetBoardType() == UnitCB) 
	 {
	 	UnitPWMSyncSignalLoss();
	 }
  
//	DACHVALSTest++;
//	// Cmpss1Regs.DACHVALS.bit.DACVAL = DACHVALSTest;
//	 if(DACHVALSTest >= 3000)
//	 {
//	     DACHVALSTest = 0;
//	 }

  
//	ADC_A1_ISR_Count++;
//	if(ADC_A1_ISR_Count >= 100)
//	{
//		ADC_A1_ISR_Count = 0;
//
//		//OSISREventSend(cPrioSuper,eSuperTest);
//
//	}
	


  
   


		//---------------------read adc sampling result----------------------------//

	PosCurrentSample = ((float)AdcaResultRegs.ADCRESULT0  + PosCurrentBias); //CMP1_HP4;
	PosCurrent = PosCurrentSample * PosCurrentRatio;

	NegCurrentSample = ((float)AdccResultRegs.ADCRESULT0  + NegCurrentBias); 
	NegCurrent = NegCurrentSample * NegCurrentRatio;

	PosCurrentSample2 = ((float)AdcaResultRegs.ADCRESULT1  + PosCurrentBias2); //CMP1_HP3
	PosCurrent2 = PosCurrentSample2 * PosCurrentRatio2;

	NegCurrentSample2 = ((float)AdccResultRegs.ADCRESULT1  + NegCurrentBias2); //CMP2_HP2
	NegCurrent2 = NegCurrentSample2 * NegCurrentRatio2;

	SmallCurrentSample = ((float)AdcaResultRegs.ADCRESULT2  + SmallCurrentBias); //
	SmallCurrent = SmallCurrentSample * SmallCurrentRatio;
	
	OutPutVoltSample = ((float)AdccResultRegs.ADCRESULT2  + OutPutVoltBias); 
	OutPutVolt = OutPutVoltSample * OutPutVoltRatio;


	BusVoltSample = ((float)AdcaResultRegs.ADCRESULT3  + BusVolttBias); 
	BusVolt = BusVoltSample * BusVoltRatio;

	AmbientTemperatureSample = ((float)AdccResultRegs.ADCRESULT3  + AmbientTemperatureBias); 
	AmbientTemperature = AmbientTemperatureSample * AmbientTemperatureRatio;

	SinkTemperatureSample = ((float)AdcaResultRegs.ADCRESULT4  + SinkTemperatureBias); 
	SinkTemperature = SinkTemperatureSample * SinkTemperatureRatio;

	//SwitchDIPSample = ((float)AdccResultRegs.ADCRESULT4  + SwitchDIPBias); //SwitchDIP
	//SwitchDIP = SwitchDIPSample * SwitchDIPRatio;
	
	

	VBus.CountTemp++;
	VBus.Sumtemp += BusVolt;
    if(VBus.CountTemp >= cBusVolt100msCnt)  
    {
		VBus.Sum = VBus.Sumtemp;
		VBus.Count = VBus.CountTemp;
		VBus.Sumtemp = 0;
		VBus.CountTemp = 0;

		
    }

	OverCurrentActionSW();
	OverVoltageActionSW();

	SideAControl.IlargesamplePos = PosCurrent;
	SideAControl.IlargesampleNeg = NegCurrent;
	//SideAControl.Ismallsample = AsideSmallIsample;
	
	//SideAControl.PulsePhase = PulsePhase1;
	//SideAControl.step[PulsePhase1].timecountset = 100;

	//  PhaseControlSideA();
	
	if(GetPWMstatus() == PWMrunSts)
	{
		if(GetControlMode() == CloseLoop)
		{
			if(GetWaveForm() == DC)
			{
				sDC_Control(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase0);
			}
			else if(GetWaveForm() == Pulse)
			{
				PhaseControlSideA();
			}
			else
			{
			
			}
		
		}
		else if(GetControlMode() == OpenLoop)
		{
			sOpenLoopControl(cSideApwm,PosOrNegCurrentA,SideAduty);
		}
		else
		{
		
		}
	
	}
	else
	{
		SideAControl.step[PulsePhase0].yn_2 = 0;
        SideAControl.step[PulsePhase0].yn_1 = 0;
        SideAControl.step[PulsePhase0].error_2 = 0;
        SideAControl.step[PulsePhase0].error_1 = 0;
        SideAControl.step[PulsePhase0].feedback_0 = 0;
        SideAControl.step[PulsePhase0].feedback_1 = 0;
	
	}
	  


	   // Clear the interrupt flag
	    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

	    // Acknowledge the interrupt
	    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	

}
/*********************************************************************
Function name:  PWMsync_CAP1_ISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
int syncnt = 0;
interrupt void PWMsync_CAP1_ISR(void)
{
#ifdef INTERRUPT_NESTING_ENABLE

	IER |= (M_INT1 | M_INT13);
	IER &= (M_INT1 | M_INT13);
	
	asm(" nop");
	EINT;
#endif
    syncnt++;
	if(BoardType == UnitCB)
	{
		PWMsyn(&UnitPara);
	}
	else
	{
	//	GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
		CapIntDelayCnt = ECap1Regs.TSCTR - ECap1Regs.CAP1;

		CapIntDelay = (int32_t)(CapIntDelayCnt*0.08333f);
		sSyncStateIdleIsr();
	}
//	if(GetBoardType() == CabinetCB)
//	{
//		
//	}
//	if((GetBoardType() == ModuleCB) || (GetBoardType() == CabinetCB))
//	{
//		if(ECap1Regs.ECFLG.bit.CEVT1 == 1)
//		{
//			ModulePara.ExtSyncSignal = 0;
//			ModuleSyncPeriod = ECap1Regs.CAP1/120;
//
//			if((GetModuleMode() == Parallel) && (abs(ModuleSyncPeriod-ModulePara.SumOfExtSyncPoint*25) < cExtSynThreshold))
//			{
//				MasterExtSyncL;
//				SetExtLineSyncLow;
//				BoardType = ModuleCB;
//			}
//			else
//			{
//				BoardType = CabinetCB;
//			}
//		}
//		else if(ECap1Regs.ECFLG.bit.CEVT2 == 1)
//		{
//			if((GetBoardType() == ModuleCB) && (GetModuleMode() == Parallel))
//			{
//				MasterExtSyncH;
//				SetExtLineSyncHigh;
//			}
//		}
//	}
//		if((BoardType == ModuleCB)&&(BoardRole == SlaveCB))
//		{
//			PWMsyn(&ModulePara);
//		}
		
//		if(BoardType == UnitCB)
//		{
//			PWMsyn(&UnitPara);
//		}
/*
	if(BoardType == UnitCB)
	{

		if ((ECap1Regs.TSCTR - ECap1Regs.CAP1) < 75)
		{
			if (EPwm1Regs.TBCTR < 100)
			
			{
				UnitPara.PWMSyncLockCnt = 0;
				WarnRegs.UnitSync.bit.PWMLockLoss = 0;
				sClrISRWarningCode(cWunitPWMSyncLock);
				UnitPara.PWMSyncLockLoss = pdFALSE;
				
				EPwm1Regs.TBCTL.bit.PHSEN = 1;
				EPwm1Regs.TBCTL.bit.SWFSYNC = 1;
				
			}
			
			else
			{
				UnitPara.PWMSyncLockCnt++;
				if(UnitPara.PWMSyncLockCnt >= UnitPara.PWMSyncLockCntSetting)
			   	{
					UnitPara.PWMSyncLockCnt = 0;
					WarnRegs.UnitSync.bit.PWMLockLoss = 1;
					sSetISRWarningCode(cWunitPWMSyncLock);
			   		UnitPara.PWMSyncLockLoss = pdTRUE;
				   
			   }
			
			}
		}
		UnitPara.PWMSyncSignal = 0;

		UnitPara.PWMSyncbackCnt++;
		UnitPara.PWMSynclossCnt = 0;;
		if(UnitPara.PWMSyncbackCnt >= UnitPara.PWMSyncbackCntSetting)
		{
			UnitPara.PWMSyncbackCnt = 0;
			WarnRegs.UnitSync.bit.PWMSyncLoss = 0;
			sClrISRWarningCode(cWunitPWMSyncLoss);
			UnitPara.PWMSyncSignalLoss = pdFALSE;
		}
		else
		{
		
		}
	}
*/
#ifdef INTERRUPT_NESTING_ENABLE

	DINT;
#endif
    ECap1Regs.ECCLR.all = 0xffff;         // clear ECAP1 interrupt flag
    // Acknowledge this interrupt to receive more interrupts from group 4
    PieCtrlRegs.PIEACK.bit.ACK4 = 1;                // Clear PIEIFR bit
}

/*********************************************************************
Function name:  Linesync_CAP2_ISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
interrupt void Linesync_CAP2_ISR(void)
{
#ifdef INTERRUPT_NESTING_ENABLE

	IER |= (M_INT1 | M_INT13);
	IER &= (M_INT1 | M_INT13);
	
	asm(" nop");
	EINT;
#endif
//	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
	//for Syn loss detect
	if(BoardType == UnitCB)
	{
		CapIntDelayCnt = ECap2Regs.TSCTR - ECap2Regs.CAP1;
		CapIntDelay = (int32_t)(CapIntDelayCnt*0.08333f);
		sSyncStateIdleIsr();
		UnitPara.ExtSyncSignal = 0;

		UnitPara.ExtSyncbackCnt++;
		UnitPara.ExtSynclossCnt = 0;;
		if(UnitPara.ExtSyncbackCnt >= UnitPara.ExtSyncbackCntSetting)
		{
			UnitPara.ExtSyncbackCnt = 0;
			
			WarnRegs.UnitSync.bit.LineSyncLoss = 0;
			sClrISRWarningCode(cWunitLineSyncLoss);			
			UnitPara.ExtSyncSignalLoss = pdFALSE;
		}
		else
		{
		
		}
		if(GetModuleMode() == Parallel)
		{
			UnitPara.ExtSyncStartPoint = 1;
		}
		else
		{
			UnitPara.ExtSyncStartPoint = 0;
		}
		

//		if((GetModuleMode() == Parallel)&&(GetWaveForm() == Pulse))
//		{
//	
//			SideAControl.IsumPos = SideAControl.IPossum_temp2;
//			SideAControl.IsumNeg = SideAControl.INegsum_temp2;
//						
//			SideAControl.VLoadsumPos = SideAControl.VloadPosSum_temp2;
//			SideAControl.VLoadsumNeg = SideAControl.VloadNegSum_temp2;
//
//			SideAControl.timecountset = SideAControl.timecountset_temp;
//
//			SideAControl.timecountsetPos = SideAControl.timecountsetPos_temp;
//			SideAControl.timecountsetNeg = SideAControl.timecountsetNeg_temp;
//					
//			SideAControl.timecountsetPos_temp = 0;
//			SideAControl.timecountsetNeg_temp = 0;
//
//
//			SideAControl.IPossum_temp2 = 0;
//			SideAControl.INegsum_temp2 = 0;
//						
//			SideAControl.VloadPosSum_temp2 = 0;
//			SideAControl.VloadNegSum_temp2 = 0;
//			
//			SideAControl.timecountset_temp = 0;
//		}
	}
	else
	{

	}
#ifdef INTERRUPT_NESTING_ENABLE

	DINT;
#endif
	ECap2Regs.ECCLR.all = 0xffff;		   // clear interrupt flag
	// Acknowledge this interrupt to receive more interrupts from group 4
	PieCtrlRegs.PIEACK.bit.ACK4 = 1;				// Clear PIEIFR bit
}
/*********************************************************************
Function name:  EPWM2_TZ_ISR
Description:    unused
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
interrupt void EPWM2_TZ1_ISR(void)
{
    HwOCRecoveryTimeCount = 0;

    HwOCCount++;
    if(HwOCCount >= HwOCCountSet)
    {
        HwOCCount = HwOCCountSet;

        SideALargePWMOFF_ISR();
        SetSideALargePWMstatus(PWMwaitSts);
        FaultRegs.HWfaultLoad.bit.HwExtLoadOC = 1;
        sSetISRFaultCode(cFHwExtLoadOC);
        OSISREventSend(cPrioSuper,eSuperFault);
    }

    // Clear the interrupt flag
    EALLOW;
    EPwm2Regs.TZCLR.bit.OST = 1;
    EPwm2Regs.TZCLR.bit.INT = 1;

    EPwm3Regs.TZCLR.bit.OST = 1;
    EPwm3Regs.TZCLR.bit.INT = 1;
    EDIS;

	// Acknowledge the interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

/*********************************************************************
Function name:  PhaseControlSideA
Description:
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
//#pragma CODE_SECTION(PhaseControlSideA,ramFuncSection);
void PhaseControlSideA(void)
{
    switch (SideAControl.PulsePhase)
    {
        case PulsePhase1:

		  if(SideAControl.step[PulsePhase1].timecountset == 0)
		  {
			SideAControl.step[PulsePhase1].IPossum = 0;
			SideAControl.step[PulsePhase1].INegsum = 0;
			
			SideAControl.step[PulsePhase1].VloadPosSum = 0;
			SideAControl.step[PulsePhase1].VloadNegSum = 0;
           
          	SideAControl.PulsePhase = PulsePhase2;
          }
          else
          {
             

          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase1);
            break;
          }


        case PulsePhase2:
          
		  if(SideAControl.step[PulsePhase2].timecountset == 0)
		  {
			SideAControl.step[PulsePhase2].IPossum = 0;
			SideAControl.step[PulsePhase2].INegsum = 0;
			
			SideAControl.step[PulsePhase2].VloadPosSum = 0;
			SideAControl.step[PulsePhase2].VloadNegSum = 0;
           
          	SideAControl.PulsePhase = PulsePhase3;
          }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase2);
            break;
          }

          case PulsePhase3:
		  
		  if(SideAControl.step[PulsePhase3].timecountset == 0)
		  {
			SideAControl.step[PulsePhase3].IPossum = 0;
			SideAControl.step[PulsePhase3].INegsum = 0;
			
			SideAControl.step[PulsePhase3].VloadPosSum = 0;
			SideAControl.step[PulsePhase3].VloadNegSum = 0;
           
          	SideAControl.PulsePhase = PulsePhase4;
          }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase3);
            break;
          }

          case PulsePhase4:
			
		 if(SideAControl.step[PulsePhase4].timecountset == 0)
		 {
		   SideAControl.step[PulsePhase4].IPossum = 0;
		   SideAControl.step[PulsePhase4].INegsum = 0;
		   
		   SideAControl.step[PulsePhase4].VloadPosSum = 0;
		   SideAControl.step[PulsePhase4].VloadNegSum = 0;
		  
		   SideAControl.PulsePhase = PulsePhase5;
		 }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase4);
            break;
          }

          case PulsePhase5:

		  if(SideAControl.step[PulsePhase5].timecountset == 0)
		  {
			SideAControl.step[PulsePhase5].IPossum = 0;
			SideAControl.step[PulsePhase5].INegsum = 0;
			
			SideAControl.step[PulsePhase5].VloadPosSum = 0;
			SideAControl.step[PulsePhase5].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase6;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase5);
            break;
          }

          case PulsePhase6:
		  
		  if(SideAControl.step[PulsePhase6].timecountset == 0)
		  {
			SideAControl.step[PulsePhase6].IPossum = 0;
			SideAControl.step[PulsePhase6].INegsum = 0;
			
			SideAControl.step[PulsePhase6].VloadPosSum = 0;
			SideAControl.step[PulsePhase6].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase7;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase6);
            break;
          }

          case PulsePhase7:  
		  
		  if(SideAControl.step[PulsePhase7].timecountset == 0)
		  {
			SideAControl.step[PulsePhase7].IPossum = 0;
			SideAControl.step[PulsePhase7].INegsum = 0;
			
			SideAControl.step[PulsePhase7].VloadPosSum = 0;
			SideAControl.step[PulsePhase7].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase8;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase7);
            break;
          }

          case PulsePhase8:
		  
		  if(SideAControl.step[PulsePhase8].timecountset == 0)
		  {
			SideAControl.step[PulsePhase8].IPossum = 0;
			SideAControl.step[PulsePhase8].INegsum = 0;
			
			SideAControl.step[PulsePhase8].VloadPosSum = 0;
			SideAControl.step[PulsePhase8].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase9;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase8);
            break;
          }

          case PulsePhase9: 
		  
		 if(SideAControl.step[PulsePhase9].timecountset == 0)
		 {
		   SideAControl.step[PulsePhase9].IPossum = 0;
		   SideAControl.step[PulsePhase9].INegsum = 0;
		   
		   SideAControl.step[PulsePhase9].VloadPosSum = 0;
		   SideAControl.step[PulsePhase9].VloadNegSum = 0;
		  
		   SideAControl.PulsePhase = PulsePhase10;
		 }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase9);
            break;
          }

          case PulsePhase10:	 
		  	
		  if(SideAControl.step[PulsePhase10].timecountset == 0)
		  {
			SideAControl.step[PulsePhase10].IPossum = 0;
			SideAControl.step[PulsePhase10].INegsum = 0;
			
			SideAControl.step[PulsePhase10].VloadPosSum = 0;
			SideAControl.step[PulsePhase10].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase11;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase10);
            break;
          }

          case PulsePhase11:
		  
		  if(SideAControl.step[PulsePhase11].timecountset == 0)
		  {
			SideAControl.step[PulsePhase11].IPossum = 0;
			SideAControl.step[PulsePhase11].INegsum = 0;
			
			SideAControl.step[PulsePhase11].VloadPosSum = 0;
			SideAControl.step[PulsePhase11].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase12;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase11);
            break;
          }

          case PulsePhase12:  
		  
		  if(SideAControl.step[PulsePhase12].timecountset == 0)
		  {
			SideAControl.step[PulsePhase12].IPossum = 0;
			SideAControl.step[PulsePhase12].INegsum = 0;
			
			SideAControl.step[PulsePhase12].VloadPosSum = 0;
			SideAControl.step[PulsePhase12].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase13;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase12);
            break;
          }

          case PulsePhase13: 
		  
		  if(SideAControl.step[PulsePhase13].timecountset == 0)
		  {
			SideAControl.step[PulsePhase13].IPossum = 0;
			SideAControl.step[PulsePhase13].INegsum = 0;
			
			SideAControl.step[PulsePhase13].VloadPosSum = 0;
			SideAControl.step[PulsePhase13].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase14;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase13);
            break;
          }

          case PulsePhase14: 
		  
		  if(SideAControl.step[PulsePhase14].timecountset == 0)
		  {
			SideAControl.step[PulsePhase14].IPossum = 0;
			SideAControl.step[PulsePhase14].INegsum = 0;
			
			SideAControl.step[PulsePhase14].VloadPosSum = 0;
			SideAControl.step[PulsePhase14].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase15;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase14);
            break;
          }

          case PulsePhase15:   
		  
		  if(SideAControl.step[PulsePhase15].timecountset == 0)
		  {
			SideAControl.step[PulsePhase15].IPossum = 0;
			SideAControl.step[PulsePhase15].INegsum = 0;
			
			SideAControl.step[PulsePhase15].VloadPosSum = 0;
			SideAControl.step[PulsePhase15].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase16;
		  }

          else
          {
            sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase15);
            break;
          }

          case PulsePhase16:
		  
		  if(SideAControl.step[PulsePhase16].timecountset == 0)
		  {
			SideAControl.step[PulsePhase16].IPossum = 0;
			SideAControl.step[PulsePhase16].INegsum = 0;
			
			SideAControl.step[PulsePhase16].VloadPosSum = 0;
			SideAControl.step[PulsePhase16].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase17;
		  }

          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase16);
            break;
          }


          case PulsePhase17:	
		  
		  if(SideAControl.step[PulsePhase17].timecountset == 0)
		  {
			SideAControl.step[PulsePhase17].IPossum = 0;
			SideAControl.step[PulsePhase17].INegsum = 0;
			
			SideAControl.step[PulsePhase17].VloadPosSum = 0;
			SideAControl.step[PulsePhase17].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase18;
		  }

           else
           {
           	 sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase17);
             break;
           }

           case PulsePhase18:
		   
		  if(SideAControl.step[PulsePhase18].timecountset == 0)
		  {
			SideAControl.step[PulsePhase18].IPossum = 0;
			SideAControl.step[PulsePhase18].INegsum = 0;
			
			SideAControl.step[PulsePhase18].VloadPosSum = 0;
			SideAControl.step[PulsePhase18].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase19;
		  }

           else
           {
           	 sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase18);
             break;
           }

           case PulsePhase19: 
		   
		   if(SideAControl.step[PulsePhase19].timecountset == 0)
		   {
			 SideAControl.step[PulsePhase19].IPossum = 0;
			 SideAControl.step[PulsePhase19].INegsum = 0;
			 
			 SideAControl.step[PulsePhase19].VloadPosSum = 0;
			 SideAControl.step[PulsePhase19].VloadNegSum = 0;
			
			 SideAControl.PulsePhase = PulsePhase20;
		   }

           else
           {
             sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase19);
             break;
           }

           case PulsePhase20:
		   
		  if(SideAControl.step[PulsePhase20].timecountset == 0)
		  {
			SideAControl.step[PulsePhase20].IPossum = 0;
			SideAControl.step[PulsePhase20].INegsum = 0;
			
			SideAControl.step[PulsePhase20].VloadPosSum = 0;
			SideAControl.step[PulsePhase20].VloadNegSum = 0;
		   
			SideAControl.PulsePhase = PulsePhase1;
		  }

           else
           {
           	 sPulseControl(cSideApwm,OutPutVolt,BusVoltAvg,BusVoltAvg,&SideAControl,PulsePhase20);
             break;
           }





       default:

           break;
    }

}

/*
void PhaseControlSideA(void)
{
    switch (SideAControl.PulsePhase)
    {
        case PulsePhase1:

          
          //if((SideAControl.step[PulsePhase1].timecountset == 0) && (SideAControl.step[PulsePhase1].reference == 0))
		  if(SideAControl.step[PulsePhase1].timecountset == 0)
		  {
		  	SideAControl.step[PulsePhase1].Isum = 0.0;
		  	SideAControl.step[PulsePhase1].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase2;
          }
          else
          {
             

          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase1);
          	
            break;
          }


        case PulsePhase2:
          
          //if((SideAControl.step[PulsePhase2].timecountset == 0) && (SideAControl.step[PulsePhase2].reference == 0))
		  if(SideAControl.step[PulsePhase2].timecountset == 0)
          {
          	SideAControl.step[PulsePhase2].Isum = 0.0;
		  	SideAControl.step[PulsePhase2].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase3;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase2);
            break;
          }

          case PulsePhase3:
		  
          //if((SideAControl.step[PulsePhase3].timecountset == 0) && (SideAControl.step[PulsePhase3].reference == 0))		  	
          if(SideAControl.step[PulsePhase3].timecountset == 0)
          {
          	SideAControl.step[PulsePhase3].Isum = 0.0;
		  	SideAControl.step[PulsePhase3].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase4;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase3);
            break;
          }

          case PulsePhase4:
			
         // if((SideAControl.step[PulsePhase4].timecountset == 0) && (SideAControl.step[PulsePhase4].reference == 0))		  	
          if(SideAControl.step[PulsePhase4].timecountset == 0)
          {
             SideAControl.step[PulsePhase4].Isum = 0.0;
		  	 SideAControl.step[PulsePhase4].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase5;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase4);
            break;
          }

          case PulsePhase5:

		  
          //if((SideAControl.step[PulsePhase5].timecountset == 0) && (SideAControl.step[PulsePhase5].reference == 0))
		  if(SideAControl.step[PulsePhase5].timecountset == 0)
          {
            SideAControl.step[PulsePhase5].Isum = 0.0;
		  	SideAControl.step[PulsePhase5].Vsum = 0.0;
			 
            SideAControl.PulsePhase = PulsePhase6;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase5);
            break;
          }

          case PulsePhase6:
		  
          //if((SideAControl.step[PulsePhase6].timecountset == 0) && (SideAControl.step[PulsePhase6].reference == 0))
          if(SideAControl.step[PulsePhase6].timecountset == 0)
          {
            SideAControl.step[PulsePhase6].Isum = 0.0;
		  	SideAControl.step[PulsePhase6].Vsum = 0.0;
			 
            SideAControl.PulsePhase = PulsePhase7;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase6);
            break;
          }

          case PulsePhase7:  
		  
          //if((SideAControl.step[PulsePhase7].timecountset == 0) && (SideAControl.step[PulsePhase7].reference == 0))
          if(SideAControl.step[PulsePhase7].timecountset == 0)
          {
            SideAControl.step[PulsePhase7].Isum = 0.0;
		  	SideAControl.step[PulsePhase7].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase8;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase7);
            break;
          }

          case PulsePhase8:
		  
          //if((SideAControl.step[PulsePhase8].timecountset == 0) && (SideAControl.step[PulsePhase8].reference == 0))
          if(SideAControl.step[PulsePhase8].timecountset == 0)
          {
          	SideAControl.step[PulsePhase8].Isum = 0.0;
		  	SideAControl.step[PulsePhase8].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase9;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase8);
            break;
          }

          case PulsePhase9: 
		  
         // if((SideAControl.step[PulsePhase9].timecountset == 0) && (SideAControl.step[PulsePhase9].reference == 0))
          if(SideAControl.step[PulsePhase9].timecountset == 0)
          {
            SideAControl.step[PulsePhase9].Isum = 0.0;
		  	SideAControl.step[PulsePhase9].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase10;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase9);
            break;
          }

          case PulsePhase10:	  
          //if((SideAControl.step[PulsePhase10].timecountset == 0) && (SideAControl.step[PulsePhase10].reference == 0))
          if(SideAControl.step[PulsePhase10].timecountset == 0)
          {
            SideAControl.step[PulsePhase10].Isum = 0.0;
		  	SideAControl.step[PulsePhase10].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase11;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase10);
            break;
          }

          case PulsePhase11:
		  
          //if((SideAControl.step[PulsePhase11].timecountset == 0) && (SideAControl.step[PulsePhase11].reference == 0))
          if(SideAControl.step[PulsePhase11].timecountset == 0)
          {
            SideAControl.step[PulsePhase11].Isum = 0.0;
		  	SideAControl.step[PulsePhase11].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase12;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase11);
            break;
          }

          case PulsePhase12:  
		  
          //if((SideAControl.step[PulsePhase12].timecountset == 0) && (SideAControl.step[PulsePhase12].reference == 0))
          if(SideAControl.step[PulsePhase12].timecountset == 0)
          {
            SideAControl.step[PulsePhase12].Isum = 0.0;
		  	SideAControl.step[PulsePhase12].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase13;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase12);
            break;
          }

          case PulsePhase13: 
		  
          //if((SideAControl.step[PulsePhase13].timecountset == 0) && (SideAControl.step[PulsePhase13].reference == 0))
          if(SideAControl.step[PulsePhase13].timecountset == 0)
          {
            SideAControl.step[PulsePhase13].Isum = 0.0;
		  	SideAControl.step[PulsePhase13].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase14;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase13);
            break;
          }

          case PulsePhase14: 
		  
          //if((SideAControl.step[PulsePhase14].timecountset == 0) && (SideAControl.step[PulsePhase14].reference == 0))
          if(SideAControl.step[PulsePhase14].timecountset == 0)
          {
          	SideAControl.step[PulsePhase14].Isum = 0.0;
		  	SideAControl.step[PulsePhase14].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase15;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase14);
            break;
          }

          case PulsePhase15:   
		  
          //if((SideAControl.step[PulsePhase15].timecountset == 0) && (SideAControl.step[PulsePhase15].reference == 0))
          if(SideAControl.step[PulsePhase15].timecountset == 0)
          {
          	SideAControl.step[PulsePhase15].Isum = 0.0;
		  	SideAControl.step[PulsePhase15].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase16;
          }
          else
          {
            sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase15);
            break;
          }

          case PulsePhase16:
		  
          //if((SideAControl.step[PulsePhase16].timecountset == 0) && (SideAControl.step[PulsePhase16].reference == 0))
          if(SideAControl.step[PulsePhase16].timecountset == 0)
          {
          	SideAControl.step[PulsePhase16].Isum = 0.0;
		  	SideAControl.step[PulsePhase16].Vsum = 0.0;
			
            SideAControl.PulsePhase = PulsePhase17;
          }
          else
          {
          	sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase16);
            break;
          }


          case PulsePhase17:	
		  
		  //if((SideAControl.step[PulsePhase17].timecountset == 0) && (SideAControl.step[PulsePhase17].reference == 0))
           if(SideAControl.step[PulsePhase17].timecountset == 0)
           {
           	 SideAControl.step[PulsePhase17].Isum = 0.0;
		  	 SideAControl.step[PulsePhase17].Vsum = 0.0;
             SideAControl.PulsePhase = PulsePhase18;
           }
           else
           {
           	 sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase17);
             break;
           }

           case PulsePhase18:
		   
		  //if((SideAControl.step[PulsePhase18].timecountset == 0) && (SideAControl.step[PulsePhase18].reference == 0))
           
           if(SideAControl.step[PulsePhase18].timecountset == 0)
           {
           	SideAControl.step[PulsePhase18].Isum = 0.0;
		  	SideAControl.step[PulsePhase18].Vsum = 0.0;
			
             SideAControl.PulsePhase = PulsePhase19;
           }
           else
           {
           	 sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase18);
             break;
           }

           case PulsePhase19: 
		   
           //if((SideAControl.step[PulsePhase19].timecountset == 0) && (SideAControl.step[PulsePhase19].reference == 0))
           if(SideAControl.step[PulsePhase19].timecountset == 0)
           {
             SideAControl.step[PulsePhase19].Isum = 0.0;
		  	 SideAControl.step[PulsePhase19].Vsum = 0.0;
			 
             SideAControl.PulsePhase = PulsePhase20;
           }
           else
           {
             sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase19);
             break;
           }

           case PulsePhase20:
		   
          // if((SideAControl.step[PulsePhase20].timecountset == 0) && (SideAControl.step[PulsePhase20].reference == 0))
           if(SideAControl.step[PulsePhase20].timecountset == 0)
           {
           	 SideAControl.step[PulsePhase20].Isum = 0.0;
		  	 SideAControl.step[PulsePhase20].Vsum = 0.0;
			 
             SideAControl.PulsePhase = PulsePhase1;
           }
           else
           {
           	 sPulseControl(cSideApwm,OutPutVolt,BusVolt,BusVolt,&SideAControl,PulsePhase20);
             break;
           }





       default:

           break;
    }

}
*/
/*********************************************************************
Function name:  sPulseControl
Description:
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
//#pragma CODE_SECTION(sPulseControl,ramFuncSection);
#pragma CODE_SECTION(sPulseControl,".TI.ramfunc");
void sPulseControl(int SideXpwm,float VloadSample,float VinSample_large,float VinSample_small,stSideXControl* SideX,PulsePowerPulsePhase phase)
{
        float temp;
		float temp2;
		float temp3;
		float temp4;
//		float Cmpss1DACHVALS_PosTemp;
//		float Cmpss2DACHVALS_NegTemp;
		Uint16 PWMorCMPTemp;
		Uint16 PulseCnt1;
		
		PWMorCMPTemp = PWMorCMP;
        SidePWMjudge = SideXpwm;

//		if(SideX->step[phase].referenceStep > SideX->IPosNominal)
//		{
//			SideX->step[phase].referenceStep = SideX->IPosNominal;
//		}
//		else if(SideX->step[phase].referenceStep < SideX->INegNominal)
//		{
//			SideX->step[phase].referenceStep = SideX->INegNominal;
//		}
//		else 
//		{
		
//		}

        //Miller/20241009/for sync begin
        if(u16_PulseResetFlag == 1)
        {
            SideX->PulsePhase = PulsePhase1;
            SideX->SumOfPeriod = SideX->PulseControlCnt;
            SideX->PulseControlCnt = 0;
            SideX->step[PulsePhase1].timecount = 0;
            SideX->step[PulsePhase2].timecount = 0;
            SideX->step[PulsePhase3].timecount = 0;
            SideX->step[PulsePhase4].timecount = 0;
            SideX->step[PulsePhase5].timecount = 0;
            SideX->step[PulsePhase6].timecount = 0;
            SideX->step[PulsePhase7].timecount = 0;
            SideX->step[PulsePhase8].timecount = 0;
            SideX->step[PulsePhase9].timecount = 0;
            SideX->step[PulsePhase10].timecount = 0;
            SideX->step[PulsePhase11].timecount = 0;
            SideX->step[PulsePhase12].timecount = 0;
            SideX->step[PulsePhase13].timecount = 0;
            SideX->step[PulsePhase14].timecount = 0;
            SideX->step[PulsePhase15].timecount = 0;
            SideX->step[PulsePhase16].timecount = 0;
            SideX->step[PulsePhase17].timecount = 0;
            SideX->step[PulsePhase18].timecount = 0;
            SideX->step[PulsePhase19].timecount = 0;
            SideX->step[PulsePhase20].timecount = 0;
            u16_PulseResetFlag = 0;
        }
        //Miller/20241009/for sync end

        if(!AdjustMode)
        {
            if(MixModeEnable)
            {
                if(fabsf(SideX->step[phase].reference) <= SmallCurrModeThreshold)
                {
                    PWMorCMPTemp = cPWMmode;
                }
                else
                {
                    PWMorCMPTemp = PWMorCMP;
                }
            }
        }

        if(CMPmodeSlopeEnable == 1)
        {
            if(SideX->step[phase].timecount == 0)
            {
                if(phase == PulsePhase1)
                {
                    if((SideX->step[PulsePhase1].reference >= 0.0f && SideX->step[PulsePhaseNum].reference <= 0.0f) || \
                       (SideX->step[PulsePhase1].reference <= 0.0f && SideX->step[PulsePhaseNum].reference >= 0.0f))
                    {
                        CMPmodeReferenceTemp = 0.0f;
                    }
                }
                else
                {
                    if((SideX->step[phase].reference >= 0.0f && SideX->step[phase-1].reference <= 0.0f) || \
                       (SideX->step[phase].reference <= 0.0f && SideX->step[phase-1].reference >= 0.0f))
                    {
                        CMPmodeReferenceTemp = 0.0f;
                    }
                }
            }
        }

        SideX->step[phase].timecount++;

        if(SideX->step[phase].timecount < SideX->step[phase].timecountset)
        {
        	SideX->timecountset_temp++;
			
            SideX->PulseControlCnt++;
            if((SideX->step[phase].reference != 0) && (BusVoltAvg > 2000.0f))
            {
                //SideX->step[phase].Vload = VloadSample;

                if(fabsf(SideX->step[phase].reference) < SideX->smalllevel)
                {
                    SideX->step[phase].feedback = fabsf(SideX->Ismallsample);
                    SideX->step[phase].Vinput = VinSample_small;

					if(SideX->step[phase].reference > 0)
                    {
                        //SideX->step[phase].Isum_temp += SideX->Ismallsample;
						SideX->step[phase].IPossum_temp += SideX->Ismallsample;
						SideX->step[phase].VloadPosSum_temp += SideX->step[phase].Vload;
                    }
                    else
                    {

                       // SideX->step[phase].Isum_temp += (SideX->Ismallsample);
						SideX->step[phase].INegsum_temp += (SideX->Ismallsample);
						SideX->step[phase].VloadNegSum_temp += SideX->step[phase].Vload;
                    }

                }
                else
                {
                    SideX->step[phase].Vinput = VinSample_large;

                    if(SideX->step[phase].reference > 0)
                    {

						temp = SideX->step[phase].reference + IavgPosRMSLoopOut - 40.0f; //4A
						if(temp < 0.0f)
						{
						    temp = 0.0f;
						}
						
                        SideX->step[phase].feedback = SideX->IlargesamplePos;
                        //SideX->step[phase].Isum_temp += SideX->step[phase].feedback;
						//SideX->step[phase].IPossum_temp += SideX->step[phase].feedback;

                        if(PWMorCMPTemp == cPWMmode)
                        {
                            if(SideX->step[phase].timecount > (SideX->step[phase].timecountset*0.667f))
                            {
                                SideX->IPossum_temp2 += SideX->step[phase].feedback;
                                //SideX->step[phase].VloadPosSum_temp += SideX->step[phase].Vload;
                                SideX->IloadPhasesum_temp2[phase] += SideX->step[phase].feedback;

                                //SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactor.ratio + PosVoltAdjustFactor.bias;
                                //SideX->VloadPosSum_temp2 += SideX->step[phase].Vload;
                                if(SideX->step[phase].reference <= AdjustFactorSectionDivide[1])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[0].ratio + PosVoltAdjustFactorArray[0].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[2])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[1].ratio + PosVoltAdjustFactorArray[1].bias;
                                }
                                //else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[3])
                                else
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[2].ratio + PosVoltAdjustFactorArray[2].bias;
                                }
                                /*else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[4])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[3].ratio + PosVoltAdjustFactorArray[3].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[5])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[4].ratio + PosVoltAdjustFactorArray[4].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[6])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[5].ratio + PosVoltAdjustFactorArray[5].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[7])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[6].ratio + PosVoltAdjustFactorArray[6].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[8])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[7].ratio + PosVoltAdjustFactorArray[7].bias;
                                }
                                else
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosVoltAdjustFactorArray[8].ratio + PosVoltAdjustFactorArray[8].bias;
                                }*/

                                SideX->timecountsetPos_temp++;
                                SideX->timecountsetPhase_temp2[phase]++;
                            }
                        }
                        else
                        {
                            if(SideX->step[phase].timecount > (SideX->step[phase].timecountset*0.667f))
                            {
                                //SideX->IPossum_temp2 += SideX->step[phase].feedback;
                                //SideX->step[phase].VloadPosSum_temp += SideX->step[phase].Vload;

                                //SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactor.ratio + PosCmpVoltAdjustFactor.bias;
                                //SideX->VloadPosSum_temp2 += SideX->step[phase].Vload;
                                if(SideX->step[phase].reference <= AdjustFactorSectionDivide[1])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[0].ratio + PosCmpVoltAdjustFactorArray[0].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[0].ratio + PosCmpDispCurrAdjustFactorArray[0].bias;
                                    SideX->IloadPhasesum_temp2[phase] += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[0].ratio + PosCmpDispCurrAdjustFactorArray[0].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[2])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[1].ratio + PosCmpVoltAdjustFactorArray[1].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[1].ratio + PosCmpDispCurrAdjustFactorArray[1].bias;
                                    SideX->IloadPhasesum_temp2[phase] += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[1].ratio + PosCmpDispCurrAdjustFactorArray[1].bias;
                                }
                                //else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[3])
                                else
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[2].ratio + PosCmpVoltAdjustFactorArray[2].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[2].ratio + PosCmpDispCurrAdjustFactorArray[2].bias;
                                    SideX->IloadPhasesum_temp2[phase] += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[2].ratio + PosCmpDispCurrAdjustFactorArray[2].bias;
                                }
                                /*else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[4])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[3].ratio + PosCmpVoltAdjustFactorArray[3].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[3].ratio + PosCmpDispCurrAdjustFactorArray[3].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[5])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[4].ratio + PosCmpVoltAdjustFactorArray[4].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[4].ratio + PosCmpDispCurrAdjustFactorArray[4].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[6])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[5].ratio + PosCmpVoltAdjustFactorArray[5].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[5].ratio + PosCmpDispCurrAdjustFactorArray[5].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[7])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[6].ratio + PosCmpVoltAdjustFactorArray[6].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[6].ratio + PosCmpDispCurrAdjustFactorArray[6].bias;
                                }
                                else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[8])
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[7].ratio + PosCmpVoltAdjustFactorArray[7].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[7].ratio + PosCmpDispCurrAdjustFactorArray[7].bias;
                                }
                                else
                                {
                                    SideX->VloadPosSum_temp2 += SideX->step[phase].Vload * PosCmpVoltAdjustFactorArray[8].ratio + PosCmpVoltAdjustFactorArray[8].bias;
                                    SideX->IPossum_temp2 += SideX->step[phase].feedback * PosCmpDispCurrAdjustFactorArray[8].ratio + PosCmpDispCurrAdjustFactorArray[8].bias;
                                }*/

                                SideX->timecountsetPos_temp++;
                                SideX->timecountsetPhase_temp2[phase]++;
                            }
                        }
                    }
                    else
                    {
                    	temp = -SideX->step[phase].reference + IavgNegRMSLoopOut - 40.0f;  //4.0A
                    	if(temp < 0.0f)
                    	{
                    	    temp = 0.0f;
                    	}
						
                        SideX->step[phase].feedback = SideX->IlargesampleNeg;
                        //SideX->step[phase].Isum_temp += (-SideX->step[phase].feedback);
						//SideX->step[phase].INegsum_temp += (-SideX->step[phase].feedback);

                        if(PWMorCMPTemp == cPWMmode)
                        {
                            if(SideX->step[phase].timecount > (SideX->step[phase].timecountset*0.667f))
                            {
                                SideX->INegsum_temp2 += SideX->step[phase].feedback;
                                SideX->IloadPhasesum_temp2[phase] -= SideX->step[phase].feedback; //有符号 负值

                                //SideX->step[phase].VloadNegSum_temp += SideX->step[phase].Vload;
                                //SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactor.ratio + NegVoltAdjustFactor.bias;
                                //SideX->VloadNegSum_temp2 += SideX->step[phase].Vload;
                                if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[1])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[0].ratio + NegVoltAdjustFactorArray[0].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[2])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[1].ratio + NegVoltAdjustFactorArray[1].bias;
                                }
                                //else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[3])
                                else
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[2].ratio + NegVoltAdjustFactorArray[2].bias;
                                }
                                /*else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[4])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[3].ratio + NegVoltAdjustFactorArray[3].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[5])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[4].ratio + NegVoltAdjustFactorArray[4].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[6])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[5].ratio + NegVoltAdjustFactorArray[5].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[7])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[6].ratio + NegVoltAdjustFactorArray[6].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[8])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[7].ratio + NegVoltAdjustFactorArray[7].bias;
                                }
                                else
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegVoltAdjustFactorArray[8].ratio + NegVoltAdjustFactorArray[8].bias;
                                }*/

                                SideX->timecountsetNeg_temp++;
                                SideX->timecountsetPhase_temp2[phase]++;
                            }
                        }
                        else
                        {
                            if(SideX->step[phase].timecount > (SideX->step[phase].timecountset*0.667f))
                            {
                                //SideX->INegsum_temp2 += SideX->step[phase].feedback;
                                //SideX->step[phase].VloadNegSum_temp += SideX->step[phase].Vload;
                                //SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactor.ratio + NegCmpVoltAdjustFactor.bias;
                                //SideX->VloadNegSum_temp2 += SideX->step[phase].Vload;
                                if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[1])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[0].ratio + NegCmpVoltAdjustFactorArray[0].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[0].ratio + NegCmpDispCurrAdjustFactorArray[0].bias;
                                    SideX->IloadPhasesum_temp2[phase] -= SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[0].ratio + NegCmpDispCurrAdjustFactorArray[0].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[2])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[1].ratio + NegCmpVoltAdjustFactorArray[1].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[1].ratio + NegCmpDispCurrAdjustFactorArray[1].bias;
                                    SideX->IloadPhasesum_temp2[phase] -= SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[1].ratio + NegCmpDispCurrAdjustFactorArray[1].bias;
                                }
                                //else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[3])
                                else
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[2].ratio + NegCmpVoltAdjustFactorArray[2].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[2].ratio + NegCmpDispCurrAdjustFactorArray[2].bias;
                                    SideX->IloadPhasesum_temp2[phase] -= SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[2].ratio + NegCmpDispCurrAdjustFactorArray[2].bias;
                                }
                                /*else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[4])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[3].ratio + NegCmpVoltAdjustFactorArray[3].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[3].ratio + NegCmpDispCurrAdjustFactorArray[3].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[5])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[4].ratio + NegCmpVoltAdjustFactorArray[4].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[4].ratio + NegCmpDispCurrAdjustFactorArray[4].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[6])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[5].ratio + NegCmpVoltAdjustFactorArray[5].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[5].ratio + NegCmpDispCurrAdjustFactorArray[5].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[7])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[6].ratio + NegCmpVoltAdjustFactorArray[6].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[6].ratio + NegCmpDispCurrAdjustFactorArray[6].bias;
                                }
                                else if(-SideX->step[phase].reference <= AdjustFactorSectionDivide[8])
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[7].ratio + NegCmpVoltAdjustFactorArray[7].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[7].ratio + NegCmpDispCurrAdjustFactorArray[7].bias;
                                }
                                else
                                {
                                    SideX->VloadNegSum_temp2 += SideX->step[phase].Vload * NegCmpVoltAdjustFactorArray[8].ratio + NegCmpVoltAdjustFactorArray[8].bias;
                                    SideX->INegsum_temp2 += SideX->step[phase].feedback * NegCmpDispCurrAdjustFactorArray[8].ratio + NegCmpDispCurrAdjustFactorArray[8].bias;
                                }*/

                                SideX->timecountsetNeg_temp++;
                                SideX->timecountsetPhase_temp2[phase]++;
                            }
                        }
                    }
                }
//				if(SideX->step[phase].timecount <=  3)
//				{
//					PWMorCMPTemp = cCMPmode;
//					GpioDataRegs.GPASET.bit.GPIO9 = 1;
//				}
//				else
//				{
//					
//					PWMorCMPTemp = cPWMmode;
//					GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
//				}

				//wsy/20240617
                SideX->step[phase].feedback_0 = 2.0*SideX->step[phase].feedback - 1.0*SideX->step[phase].feedback_1;
                //SideX->step[phase].error_0 = (temp - SideX->step[phase].feedback_0) * 0.07;
                //SideX->step[phase].yn = SideX->step[phase].yn_1 + SideX->step[phase].error_0;
                //SideX->step[phase].yn_2 = SideX->step[phase].yn_1;
                //SideX->step[phase].yn_1 = SideX->step[phase].yn;
                //SideX->step[phase].feedback_1 = SideX->step[phase].feedback;

                //SideX->step[phase].error_1 = SideX->step[phase].error_0;

                //SideX->step[phase].error_0 = temp - SideX->step[phase].feedback;
                SideX->step[phase].error_0 = temp - SideX->step[phase].feedback_0;
                SideX->step[phase].feedback_1 = SideX->step[phase].feedback;

                if(PWMorCMPTemp == cPWMmode)
			   {
//				   if(SideX->step[phase].error_0 >= SideX->step[phase].errorUplimit)
//				   {
//				   		SideX->step[phase].error_0 = SideX->step[phase].errorUplimit;
//				   }
//				   else if(SideX->step[phase].error_0 <= SideX->step[phase].errorDwlimit)
//				   {
//				   		SideX->step[phase].error_0 = SideX->step[phase].errorDwlimit;
//				   }
//				   else
//				   {
				   
//				   }


//					SideX->step[phase].yn_2 = SideX->step[phase].A2 * (SideX->step[phase].error_0 - SideX->step[phase].error_1);
					//JBG20240516

					//SideX->step[phase].yn_2 = fabsf(SideX->step[phase].reference) * SideX->step[phase].A2;
					//SideX->step[phase].yn_2 = fabsf(VloadSample);

//						if(SideX->step[phase].feedback > temp)
//						{
//						  SideX->step[phase].yn_1 += (SideX->step[phase].A2 * SideX->step[phase].error_0);
//
//						}
//						else 
						{					   
							SideX->step[phase].yn_1 += (SideX->step[phase].A0 * SideX->step[phase].error_0);
						}

						if((Moduletype == Module_P150A_N450A) || \
						   (Moduletype == Module_P100A_N300A) || \
						   (Moduletype == Module_P50A_N150A))
						{
                            if(SideX->step[phase].yn_1 >= 3600.0)
                            {
                                 SideX->step[phase].yn_1 = 3600.0;
                            }
                            else if(SideX->step[phase].yn_1 < (-3600.0))
                            {
                                 SideX->step[phase].yn_1 = -3600;
                            }
                            else
                            {
                            }
						}
                        else if((Moduletype == Module_P200A_N600A) || \
                                (Moduletype == Module_P150A_N750A))
                        {
                            if(SideX->step[phase].yn_1 >= 4300.0)
                            {
                                 SideX->step[phase].yn_1 = 4300.0;
                            }
                            else if(SideX->step[phase].yn_1 < (-4300.0))
                            {
                                 SideX->step[phase].yn_1 = -4300;
                            }
                            else
                            {
                            }
                        }
                        else
                        {
                            if(SideX->step[phase].yn_1 >= 3600.0)
                            {
                                 SideX->step[phase].yn_1 = 3600.0;
                            }
                            else if(SideX->step[phase].yn_1 < (-3600.0))
                            {
                                 SideX->step[phase].yn_1 = -3600;
                            }
                            else
                            {
                            }
                        }

				   		SideX->step[phase].yn = SideX->step[phase].yn_1 + SideX->step[phase].A1 * SideX->step[phase].error_0;
				   		//JBG20240516
				   		//SideX->step[phase].yn = SideX->step[phase].yn_1 + SideX->step[phase].yn_2 + SideX->step[phase].A1 * SideX->step[phase].error_0;

						SideX->step[phase].duty = (SideX->step[phase].yn)/SideX->step[phase].Vinput;
				}
				else
				{					 


//				  SideX->step[phase].duty = 0.0003*fabsf(SideX->step[phase].reference)+0.12;//testDuty;//0.98;
				SideX->step[phase].duty = 0.98;
//				SideX->step[phase].yn_1 = SideX->step[phase].duty * SideX->step[phase].Vinput*0.42;

				
				}

//				if(SideX->step[phase].timecount >= (SideX->step[phase].timecountset/2.0))
//				{
//				   SideX->step[phase].dutySumTemp += SideX->step[phase].duty;
//				}
				if(SideX->step[phase].duty <= 0.05)
				{
				    SideX->step[phase].Vload = 0.0;
				}
				else
				{
//					if(PWMorCMPTemp == cPWMmode)
//				   		SideX->step[phase].Vload = (SideX->step[phase].duty - 0.05) * SideX->step[phase].Vinput;
//					else
						SideX->step[phase].Vload = VloadSample;
				}
			
                if(SideX->step[phase].duty >= 0.98)
                {
                    SideX->step[phase].duty = 0.98;

                    if(OverLoadCheckEnable == 1)
                    {
                        if(SideX->step[phase].error_0 > OverLoadLevel)//10A 15A
                        {
                            OverLoadCnt++;
                            if(OverLoadCnt >= OverLoadCntSet)
                            {
                                OverLoadCnt = 0;

                                WarnRegs.TempWarn.bit.OverLoad = 1;
                                sSetISRWarningCode(cWHwOverLoad);
                            }
                            OverLoadBackCnt = 0;
                        }
                        else if(SideX->step[phase].error_0 < (OverLoadLevel - 50))
                        {
                            OverLoadBackCnt++;
                            if(OverLoadBackCnt >= OverLoadBackCntSet)
                            {
                                OverLoadBackCnt = 0;
                                WarnRegs.TempWarn.bit.OverLoad = 0;
                                sClrISRWarningCode(cWHwOverLoad);
                            }
                            OverLoadCnt = 0;
                        }
                    }
                    else
                    {
                        OverLoadCnt = 0;
                        OverLoadBackCnt = 0;
                        WarnRegs.TempWarn.bit.OverLoad = 0;
                        sClrISRWarningCode(cWHwOverLoad);
                    }
                }
                else if(SideX->step[phase].duty <= 0.0)
                {
                    SideX->step[phase].duty = 0.0;
                }
                else
                {
					OverLoadCnt = 0;
					OverLoadBackCnt = 0;
					WarnRegs.TempWarn.bit.OverLoad = 0;
					sClrISRWarningCode(cWHwOverLoad);

                }
                if(SideX->step[phase].reference > 0)
                {
					  if(SidePWMjudge == cSideApwm)
					  {
						  if(PWMorCMPTemp == cPWMmode)
						  {
	
							   SideALargePWMON_ISR();					

							   MixModeFlagBefore = MixModeFlag;
							   MixModeFlag = 1;
							   if((MixModeFlagBefore == 2) && (MixModeEnable))
							   {
							       //从状态2变到状态1的过程，保持DACVAL值不变
							   }
							   else
							   {
							       Cmpss1Regs.DACHVALS.bit.DACVAL = Cmpss1DACHVALS_Pos;
							   }


//							  Cmpss1Regs.DACHVALS.bit.DACVAL = Cmpss1Regs.DACHVALS.bit.DACVAL + 300;
							   if(PwmOpenLoopEnable == 0)
							   {
							       pwm1A = SideX->step[phase].duty;
							   }
							   else
							   {
							       pwm1A = (float)PosPWMdutyOpenLoop / 100.0f;
							   }

							   pwm2A = 0.0;
						  }
						  else
						  {
						      MixModeFlagBefore = MixModeFlag;
						      MixModeFlag = 2;

						  		SideALargePWMON_ISR();															
								//Cmpss1Regs.DACHVALS.bit.DACVAL= SideX->step[phase].CMPSSVALUEPos;	
								if(AdjustMode)
								{
								    temp4 = SideX->step[phase].CMPSSVALUEPos;
								}
								else
								{
								    temp3 = SideX->step[phase].CMPSSVALUEPos * PosCurrentRatio - 150.0f;
                                    if(temp3 < 10.0f)
                                    {
                                        temp3 = 10.0f; //1A
                                    }

									if(SideX->step[phase].reference <= AdjustFactorSectionDivide[1])
									{
									    temp3 = temp3 * PosCmpCurrAdjustFactorArray[0].ratio + PosCmpCurrAdjustFactorArray[0].bias;
									}
									else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[2])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[1].ratio + PosCmpCurrAdjustFactorArray[1].bias;
                                    }
                                    //else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[3])
									else
									{
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[2].ratio + PosCmpCurrAdjustFactorArray[2].bias;
                                    }
                                    /*else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[4])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[3].ratio + PosCmpCurrAdjustFactorArray[3].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[5])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[4].ratio + PosCmpCurrAdjustFactorArray[4].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[6])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[5].ratio + PosCmpCurrAdjustFactorArray[5].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[7])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[6].ratio + PosCmpCurrAdjustFactorArray[6].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[8])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[7].ratio + PosCmpCurrAdjustFactorArray[7].bias;
                                    }
                                    else
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[8].ratio + PosCmpCurrAdjustFactorArray[8].bias;
                                    }*/
									temp3 = temp3 / PosCurrentRatio;

                                    //temp3 += IavgPosRMSLoopOut / PosCurrentRatio;
                                    temp3 += IavgPosRMSLoopOut;

                                    if(CMPmodeSlopeEnable == 1)
                                    {
                                        temp2 = CMPmodeSlopePos / PosCurrentRatio;

                                        if((CMPmodeReferenceTemp - temp3) < (-temp2))
                                        {
                                            CMPmodeReferenceTemp += temp2;
                                        }
                                        else if((CMPmodeReferenceTemp - temp3) > temp2)
                                        {
                                            CMPmodeReferenceTemp -= temp2;
                                        }
                                        else
                                        {
                                            CMPmodeReferenceTemp = temp3;
                                        }

                                        temp4 = CMPmodeReferenceTemp;
                                    }
                                    else
                                    {
                                        if((Moduletype == Module_P200A_N600A) || (Moduletype == Module_P150A_N750A))
                                        {
                                            temp4 = temp3;
                                        }
                                        else if(Moduletype == Module_P100A_N300A)
                                        {
                                            temp4 = temp3 *0.49 + SideX->step[phase].timecount * 0.05*temp3;
                                            //temp4 = temp3;
                                        }
                                        else
                                        {
                                            temp4 = temp3 *0.83 + SideX->step[phase].timecount * 0.01275*temp3;
                                        }

                                        if(temp4 >= temp3)
                                        {
                                            temp4 = temp3;
                                        }
                                    }
								}

								if(CmpOpenLoopEnable == 0)
								{
	                                Cmpss1Regs.DACHVALS.bit.DACVAL = temp4;
								}
								else
								{
								    Cmpss1Regs.DACHVALS.bit.DACVAL = PosCMPDACOpenLoop;
								}

								pwm1A = SideX->step[phase].duty;
							 	pwm2A = 0.0;
						  }
						  
					  }
					  else
					  {
	
						  
					  }

                }
                else
                {
					if(SidePWMjudge == cSideApwm)
					{
						if(PWMorCMPTemp == cPWMmode)
						{
					
							SideALargePWMON_ISR();

                            MixModeFlagBefore = MixModeFlag;
                            MixModeFlag = 3;
                            if((MixModeFlagBefore == 4) && (MixModeEnable))
							{
							    //从状态4变到状态3的过程，保持DACVAL值不变
							}
							else
							{
							    CmpssLite2Regs.DACHVALS.bit.DACVAL = Cmpss2DACHVALS_Neg+300;
							}

							pwm1A = 0.0;

                            if(PwmOpenLoopEnable == 0)
                            {
                                pwm2A = SideX->step[phase].duty;
                            }
                            else
                            {
                                pwm2A = (float)NegPWMdutyOpenLoop / 100.0f;
                            }
						}
						else
						{
                            MixModeFlagBefore = MixModeFlag;
                            MixModeFlag = 4;

							SideALargePWMON_ISR();
							//CmpssLite2Regs.DACHVALS.bit.DACVAL = SideX->step[phase].CMPSSVALUENeg;
                            if(AdjustMode)
                            {
                                temp4 = SideX->step[phase].CMPSSVALUENeg;
                            }
                            else
                            {
                                temp3 = SideX->step[phase].CMPSSVALUENeg * NegCurrentRatio - 150.0f;
                                if(temp3 < 10.0f)
                                {
                                    temp3 = 10.0f; //1A
                                }

                                if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[1])
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[0].ratio + NegCmpCurrAdjustFactorArray[0].bias;
                                }
                                else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[2])
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[1].ratio + NegCmpCurrAdjustFactorArray[1].bias;
                                }
                                //else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[3])
                                else
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[2].ratio + NegCmpCurrAdjustFactorArray[2].bias;
                                }
                                /*else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[4])
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[3].ratio + NegCmpCurrAdjustFactorArray[3].bias;
                                }
                                else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[5])
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[4].ratio + NegCmpCurrAdjustFactorArray[4].bias;
                                }
                                else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[6])
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[5].ratio + NegCmpCurrAdjustFactorArray[5].bias;
                                }
                                else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[7])
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[6].ratio + NegCmpCurrAdjustFactorArray[6].bias;
                                }
                                else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[8])
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[7].ratio + NegCmpCurrAdjustFactorArray[7].bias;
                                }
                                else
                                {
                                    temp3 = temp3 * NegCmpCurrAdjustFactorArray[8].ratio + NegCmpCurrAdjustFactorArray[8].bias;
                                }*/
                                temp3 = temp3 / NegCurrentRatio;

                                //temp3 += IavgNegRMSLoopOut / NegCurrentRatio;
                                temp3 += IavgNegRMSLoopOut;

                                if(CMPmodeSlopeEnable == 1)
                                {
                                    temp2 = CMPmodeSlopeNeg / NegCurrentRatio;

                                    if((CMPmodeReferenceTemp - temp3) < (-temp2))
                                    {
                                        CMPmodeReferenceTemp += temp2;
                                    }
                                    else if((CMPmodeReferenceTemp - temp3) > temp2)
                                    {
                                        CMPmodeReferenceTemp -= temp2;
                                    }
                                    else
                                    {
                                        CMPmodeReferenceTemp = temp3;
                                    }

                                    temp4 = CMPmodeReferenceTemp;
                                }
                                else
                                {
                                    if((Moduletype == Module_P200A_N600A) || (Moduletype == Module_P150A_N750A))
                                    {
                                        temp4 = temp3;
                                    }
                                    else if(Moduletype == Module_P100A_N300A)
                                    {
                                        temp4 = temp3 *0.88 + SideX->step[phase].timecount * 0.01*temp3;
                                        //temp4 = temp3;
                                    }
                                    else
                                    {
                                        //temp4 = temp3 *0.92 + SideX->step[phase].timecount * 0.00175*temp3;
                                        temp4 = temp3 *0.92 + SideX->step[phase].timecount * 0.00350*temp3; //wsy/20240704 modify
                                    }

                                    if(temp4 >= temp3)
                                    {
                                       temp4 = temp3;
                                    }
                                }
                            }

                            if(CmpOpenLoopEnable == 0)
                            {
                                CmpssLite2Regs.DACHVALS.bit.DACVAL = temp4;
                            }
                            else
                            {
                                CmpssLite2Regs.DACHVALS.bit.DACVAL = NegCMPDACOpenLoop;
                            }

	                        pwm1A = 0.0;
						    pwm2A = SideX->step[phase].duty;
						}
						
					}
					else
					{
					  
					
					}

                }


            }
            else
            {
                SideX->step[phase].yn_2 = 0;
                SideX->step[phase].yn_1 = 0;
                SideX->step[phase].error_2 = 0;
                SideX->step[phase].error_1 = 0;
                SideX->step[phase].feedback_0 = 0;
                SideX->step[phase].feedback_1 = 0;

                SideALargePWMOFF_ISR();

                MixModeFlagBefore = 0;
                MixModeFlag = 0;
            }

        }
        else
        {
            //Miller/20240929/for sync begin
            //u16_Epwm1Flag = 1;
            //Miller/20240929/for sync end
			
//				if(SideX->step[phase].reference > 0)
//				{
//					SideX->timecountsetPos_temp += SideX->step[phase].timecountset;

//				}
//				else
//				{
//					SideX->timecountsetNeg_temp += SideX->step[phase].timecountset;

//				}

//			SideX->step[phase].dutySum = SideX->step[phase].dutySumTemp;
//			SideX->step[phase].dutySumTemp = 0.0;
			//SideX->step[phase].dutyAvg = SideX->step[phase].dutySum*2.0/SideX->step[phase].timecountset;

			SideX->step[phase].IPossum = SideX->step[phase].IPossum_temp;
			SideX->step[phase].INegsum = SideX->step[phase].INegsum_temp;
			
			SideX->step[phase].VloadPosSum = SideX->step[phase].VloadPosSum_temp;
			SideX->step[phase].VloadNegSum = SideX->step[phase].VloadNegSum_temp;

			SideX->step[phase].IPossum_temp = 0.0;
			SideX->step[phase].INegsum_temp = 0.0;
			
			SideX->step[phase].VloadPosSum_temp = 0.0;
			SideX->step[phase].VloadNegSum_temp = 0.0;



           // SideX->step[phase].Isum = SideX->step[phase].Isum_temp;
           // SideX->step[phase].Vsum = SideX->step[phase].Vsum_temp;
           // SideX->step[phase].Isum_temp = 0.0;
           // SideX->step[phase].Vsum_temp = 0.0;

			//SideX->Vsum_temp += SideX->step[phase].Vsum;
			//SideX->Isum_temp += SideX->step[phase].Isum;
		  	//SideX->timecountset_temp += SideX->step[phase].timecountset;

			SideX->IsumPos_temp += SideX->step[phase].IPossum;
			SideX->IsumNeg_temp += SideX->step[phase].INegsum;
			
			SideX->VLoadsumPos_temp += SideX->step[phase].VloadPosSum;
			SideX->VLoadsumNeg_temp += SideX->step[phase].VloadNegSum;
			

//			SideX->step[phase].referenceBase = 0;
			//Miller/20241009 begin
//            SideX->step[phase].timecount = 0;
			//Miller/20241009 end
//            SideX->step[phase].yn_2 = 0;
//            SideX->step[phase].yn_1 = 0;
//            SideX->step[phase].error_2 = 0;
//            SideX->step[phase].error_1 = 0;

			//SideX->step[phase].yn_2 = 0.8 * SideX->step[phase].yn_2;
           // SideX->step[phase].yn_1 = SideX->step[phase].yn_1 * 0.7;
            
            

            //if(SideX->PulsePhase == PulsePhase1)
			//if(SideX->PulsePhase >= PulsePhase21)
            //Miller/20241009/for sync begin
            if((SideX->PulsePhase == SideAControl.PhaseTimeSetNoZero) &&
               ((SideX->step[SideAControl.PhaseTimeSetNoZero].timecount) >= SideX->step[phase].timecountset))
            //Miller/20241009/for sync end
            {
	      		if(ModulePara.Mode != Parallel)
				{
				  

					SideX->IsumPos = SideX->IPossum_temp2;
					SideX->IsumNeg = SideX->INegsum_temp2;
								
					SideX->VLoadsumPos = SideX->VloadPosSum_temp2;
					SideX->VLoadsumNeg = SideX->VloadNegSum_temp2;

					SideX->timecountsetPos = SideX->timecountsetPos_temp;
					SideX->timecountsetNeg = SideX->timecountsetNeg_temp;

					SideX->timecountset = SideX->timecountset_temp;


							
					SideX->timecountsetPos_temp = 0;
					SideX->timecountsetNeg_temp = 0;



					SideX->IPossum_temp2 = 0;
					SideX->INegsum_temp2 = 0;
								
					SideX->VloadPosSum_temp2 = 0;
					SideX->VloadNegSum_temp2 = 0;
					
					SideX->timecountset_temp = 0;
					
	                for(PulseCnt1 = 1; PulseCnt1 <= 20; PulseCnt1++)
	                {
	                    SideX->IloadPhasesum[PulseCnt1] = SideX->IloadPhasesum_temp2[PulseCnt1];
	                    SideX->timecountsetPhase[PulseCnt1] = SideX->timecountsetPhase_temp2[PulseCnt1];

	                    SideX->IloadPhasesum_temp2[PulseCnt1] = 0;
	                    SideX->timecountsetPhase_temp2[PulseCnt1] = 0;
	                }
		      	}
				if((GetModuleMode() == Parallel)&&(GetWaveForm() == Pulse))
				{
			
					SideX->IsumPos = SideX->IPossum_temp2;
					SideX->IsumNeg = SideX->INegsum_temp2;
								
					SideX->VLoadsumPos = SideX->VloadPosSum_temp2;
					SideX->VLoadsumNeg = SideX->VloadNegSum_temp2;

					SideX->timecountset = SideX->timecountset_temp;

					SideX->timecountsetPos = SideX->timecountsetPos_temp;
					SideX->timecountsetNeg = SideX->timecountsetNeg_temp;
							
					SideX->timecountsetPos_temp = 0;
					SideX->timecountsetNeg_temp = 0;


					SideX->IPossum_temp2 = 0;
					SideX->INegsum_temp2 = 0;
								
					SideX->VloadPosSum_temp2 = 0;
					SideX->VloadNegSum_temp2 = 0;
					
					SideX->timecountset_temp = 0;

                    for(PulseCnt1 = 1; PulseCnt1 <= 20; PulseCnt1++)
                    {
                        SideX->IloadPhasesum[PulseCnt1] = SideX->IloadPhasesum_temp2[PulseCnt1];
                        SideX->timecountsetPhase[PulseCnt1] = SideX->timecountsetPhase_temp2[PulseCnt1];

                        SideX->IloadPhasesum_temp2[PulseCnt1] = 0;
                        SideX->timecountsetPhase_temp2[PulseCnt1] = 0;
                    }
				}
        	}
            //Miller/20241009 begin
            SideX->step[phase].timecount = 0;
            //Miller/20241009 end

			SideX->PulsePhase++;
			if(SideX->PulsePhase >= (PulsePhaseNum + 1))
			//if(SideX->PulsePhase >= PulsePhase21)
            {
                SideX->PulsePhase = PulsePhase1;
				SideX->SumOfPeriod = SideX->PulseControlCnt;
				SideX->PulseControlCnt = 0;
            }

            //Miller/20240929/for sync begin
            //u16_Epwm1Flag = 0;
            //Miller/20240929/for sync end
        }
		if(SidePWMjudge == cSideApwm)
		{
			EPwm3Regs.CMPA.bit.CMPA = (Uint16)(SwitchPeriodReg * (pwm1A));//pwm driver reverse
		    EPwm2Regs.CMPA.bit.CMPA = (Uint16)(SwitchPeriodReg * (pwm2A));			
			
		}
		else
		{

		}
	
}

/*******************************************************************************
Function name:  void    sOverProtect(stOverCheck* input)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void	sOverProtect(stOverCheck* input)
{

		if(input->Enable == pdTRUE)
		{
		
			if(input->Overflag == pdFALSE)
			{
				input->Overbackcount = 0;
				
				if(input->compvalue > input->Overlevel)
				{
					input->Overcount++;
				}
				else
				{
					input->Overcount = 0;
				}
				if(input->Overcount > input->Overcountset)
				{
					input->Overcount = 0;
					input->Overflag = pdTRUE;
				}
				else
				{
					
				}
					
			}
			else
			{
				input->Overcount = 0;

				
				if(input->compvalue < input->Overback)
				{
					input->Overbackcount++;
				}
				else
				{
					input->Overbackcount = 0;
				}
				if(input->Overbackcount > input->Overbackcountset)
				{
					input->Overbackcount = 0;
					input->Overflag = pdFALSE;
				}
				else
				{
					
				}
			}
		}
		else
		{
			input->Overflag = pdFALSE;
			input->Overbackcount = 0;
			input->Overcount = 0;
		}
}
/*******************************************************************************
Function name: int GetPosIoverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int GetPosIoverFlag(void)
{
	return(PosI.Overflag);

}
/*******************************************************************************
Function name: int GetNegIoverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int GetNegIoverFlag(void)
{
	return(NegI.Overflag);

}
/*******************************************************************************
Function name: int GetNegIoverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int GetLoadVoverFlag(void)
{
	return(LoadV.Overflag);

}


/*******************************************************************************
Function name: void	OverCurrentProtectSW(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void	OverCurrentActionSW(void)
{
	PosI.compvalue = PosCurrent;
	NegI.compvalue = NegCurrent;

	sOverProtect(&PosI);
	if(GetPosIoverFlag() == pdTRUE)
	{
		FaultRegs.HWfaultLoad.bit.HwLoadOCpos = 1;
		sSetISRFaultCode(cFHwLoadOCpos);
	
	}
	else
	{
		FaultRegs.HWfaultLoad.bit.HwLoadOCpos = 0;

	}

	sOverProtect(&NegI);
	if(GetNegIoverFlag() == pdTRUE)
	{
		FaultRegs.HWfaultLoad.bit.HwLoadOCneg = 1;
		sSetISRFaultCode(cFHwLoadOCneg);
	}
	else
	{
		FaultRegs.HWfaultLoad.bit.HwLoadOCneg = 0;
	}

}

/*******************************************************************************
Function name: void	OverVoltageActionSW(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void	OverVoltageActionSW(void)
{
	LoadV.compvalue = fabsf(OutPutVolt);
	
	sOverProtect(&LoadV);
	if(GetLoadVoverFlag() == pdTRUE)
	{
        FaultRegs.HWfaultLoad.bit.HwLoadOVpos = 1;
        sSetISRFaultCode(cFHwLoadOVpos);

        FaultRegs.HWfaultLoad.bit.HwLoadOVneg = 1;
        sSetISRFaultCode(cFHwLoadOVneg);
	}
	else
	{
	    FaultRegs.HWfaultLoad.bit.HwLoadOVpos = 0;
	    FaultRegs.HWfaultLoad.bit.HwLoadOVneg = 0;
	}

}

/*********************************************************************
Function name:  sDC_Control
Description:
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
//#pragma CODE_SECTION(sDC_Control,ramFuncSection);
void sDC_Control(int SideXpwm,float VloadSample,float VinSample_large,float VinSample_small,stSideXControl* SideX,PulsePowerPulsePhase phase)
{
        float temp;
		float temp2;
		float temp3;
        float temp4;
        Uint16 PWMorCMPTemp;

        PWMorCMPTemp = PWMorCMP;
        if(!AdjustMode)
        {
            if(MixModeEnable)
            {
                if(fabsf(SideX->step[phase].reference) <= SmallCurrModeThreshold)
                {
                    PWMorCMPTemp = cPWMmode;
                }
                else
                {
                    PWMorCMPTemp = PWMorCMP;
                }
                PWMorCMPSwitchPre = PWMorCMPSwitch;
                PWMorCMPSwitch = PWMorCMPTemp;
                if(PWMorCMPSwitchPre != PWMorCMPSwitch)
                {
                    IavgPosDCRMSLoopOut = 0.0;
                    IavgNegDCRMSLoopOut = 0.0;
                    IavgPosDCRMSLoopInt = 0.0;
                    IavgNegDCRMSLoopInt = 0.0;
                }


            }
        }

        SidePWMjudge = SideXpwm;

            if((SideX->step[phase].reference != 0) && (BusVoltAvg > 2000.0f))
            {
                //SideX->step[phase].Vload = VloadSample;

                if(fabsf(SideX->step[phase].reference) < SideX->smalllevel)
                {
                    SideX->step[phase].feedback = SideX->Ismallsample;
                    SideX->step[phase].Vinput = VinSample_small;

                }
                else
                {
                    SideX->step[phase].Vinput = VinSample_large;
                    SideX->step[phase].feedback = SideX->IlargesamplePos;
                    if(SideX->step[phase].reference > 0)
                    {
                    	temp = SideX->step[phase].reference + IavgPosDCRMSLoopOut - 40.0f;  //-4.0A
                    	if(temp < 0.0f)
                    	{
                    	    temp = 1.0f;
                    	}
                        SideX->step[phase].feedback = SideX->IlargesamplePos;
						SideX->step[phase].IPossum_temp += SideX->step[phase].feedback;
						SideX->step[phase].VloadPosSum_temp += SideX->step[phase].Vload;
                    }
                    else
                    {
                    	temp = -SideX->step[phase].reference + IavgNegDCRMSLoopOut - 40.0f; //-4.0A
                    	if(temp < 0.0f)
                        {
                            temp = 1.0f;
                        }
                        SideX->step[phase].feedback = SideX->IlargesampleNeg;
						SideX->step[phase].INegsum_temp += SideX->step[phase].feedback;
						SideX->step[phase].VloadNegSum_temp += SideX->step[phase].Vload;
                    }
                }

                //JBG 20240617
                SideX->step[phase].feedback_0 = 2.0*SideX->step[phase].feedback - 1.0*SideX->step[phase].feedback_1;
                /*SideX->step[phase].error_0 = (temp - SideX->step[phase].feedback_0) * 0.0266;
			    SideX->step[phase].yn = SideX->step[phase].yn_1 + SideX->step[phase].error_0;
			    SideX->step[phase].yn_2 = SideX->step[phase].yn_1;
			    SideX->step[phase].yn_1 = SideX->step[phase].yn;
			    SideX->step[phase].feedback_1 = SideX->step[phase].feedback;*/

                //SideX->step[phase].error_0 = temp - SideX->step[phase].feedback;
                SideX->step[phase].error_0 = temp - SideX->step[phase].feedback_0;
                SideX->step[phase].feedback_1 = SideX->step[phase].feedback;

			//SideX->step[phase].error_0 = (fabsf(SideX->step[phase].reference) * SideX->step[phase].cali- fabsf(SideX->step[phase].feedback))*SideX->step[phase].gain;
	
//			   if(SideX->step[phase].error_0 >= SideX->step[phase].errorUplimit)
//			   {
//			   		SideX->step[phase].error_0 = SideX->step[phase].errorUplimit;
//			   }
//			   else if(SideX->step[phase].error_0 <= SideX->step[phase].errorDwlimit)
//			   {
//			   		SideX->step[phase].error_0 = SideX->step[phase].errorDwlimit;
//			   }
//			   else
//			   	{
//			   	}



				if(PWMorCMPTemp == cPWMmode)
				{
				   SideX->step[phase].yn_1 += (SideX->step[phase].A0 * SideX->step[phase].error_0);

				   SideX->step[phase].yn = SideX->step[phase].yn_1 + SideX->step[phase].A1 * SideX->step[phase].error_0;
				   
				   //SideX->step[phase].Vinput = 3800.0;//JBG FOR TEST


               

				   SideX->step[phase].duty = (SideX->step[phase].yn)/SideX->step[phase].Vinput;
               }
               else
               {
                  SideX->step[phase].duty = 0.98;

				  SideX->step[phase].yn_1 = 0.0;
               }

				if(SideX->step[phase].duty <= 0.05)
				{
				    SideX->step[phase].Vload = 0.0;
				}
				else
				{
//					if(PWMorCMPTemp == cPWMmode)
//				   		SideX->step[phase].Vload = (SideX->step[phase].duty - 0.05) * SideX->step[phase].Vinput;
//					else
						SideX->step[phase].Vload = VloadSample;
				}

			
                if(SideX->step[phase].duty >= 0.98)
                {
                    SideX->step[phase].duty = 0.98;
                    if(OverLoadCheckEnable == 1)
                    {
                        if(SideX->step[phase].error_0 > OverLoadLevel)//10A 15A
                        {
                            OverLoadCnt++;
                            if(OverLoadCnt >= OverLoadCntSet)
                            {
                                OverLoadCnt = 0;

							//WarnRegs.TempWarn.bit.OverLoad = 1;
							//sSetISRWarningCode(cWHwOverLoad);
                            }
                            OverLoadBackCnt = 0;
                        }
                        else if(SideX->step[phase].error_0 < (OverLoadLevel - 50))
                        {
                            OverLoadBackCnt++;
                            if(OverLoadBackCnt >= OverLoadBackCntSet)
                            {
                                OverLoadBackCnt = 0;

                                WarnRegs.TempWarn.bit.OverLoad = 0;
                                sClrISRWarningCode(cWHwOverLoad);
                            }
                            OverLoadCnt = 0;
                        }
                    }
                    else
                    {
                        OverLoadCnt = 0;
                        OverLoadBackCnt = 0;
                        WarnRegs.TempWarn.bit.OverLoad = 0;
                        sClrISRWarningCode(cWHwOverLoad);
                    }
                }
                else if(SideX->step[phase].duty <= 0.0)
                {
                    SideX->step[phase].duty = 0.0;
                }
                else
                {
                    OverLoadCnt = 0;
                    OverLoadBackCnt = 0;
                    WarnRegs.TempWarn.bit.OverLoad = 0;
                    sClrISRWarningCode(cWHwOverLoad);
                }
                if(SideX->step[phase].reference > 0)
                {
                    if(SideX->step[phase].reference < SideX->smalllevel)
                    {

                    }
                    else
                    {
                        if(SidePWMjudge == cSideApwm)
                        {

                           if(PWMorCMPTemp == cPWMmode)
                           	{

	                            SideALargePWMON_ISR();
	                            if(PwmOpenLoopEnable == 0)
	                            {
	                                pwm1A = SideX->step[phase].duty;
	                            }
	                            else
	                            {
	                                pwm1A = (float)PosPWMdutyOpenLoop / 100.0f;
	                            }
	                            pwm2A = 0.0;
                           	}
						   else
						   	{
								SideALargePWMON_ISR();
								if(AdjustMode)
								    temp4 = SideX->step[phase].CMPSSVALUEPos;
								else
								{
                                    temp3 = SideX->step[phase].CMPSSVALUEPos * PosCurrentRatio - 150.0f;
                                    if(temp3 < 1.0f)
                                    {
                                        temp3 = 1.0f; //0.1A
                                    }

                                    if(SideX->step[phase].reference <= AdjustFactorSectionDivide[1])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[0].ratio + PosCmpCurrAdjustFactorArray[0].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[2])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[1].ratio + PosCmpCurrAdjustFactorArray[1].bias;
                                    }
                                    //else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[3])
                                    else
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[2].ratio + PosCmpCurrAdjustFactorArray[2].bias;
                                    }
                                    /*else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[4])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[3].ratio + PosCmpCurrAdjustFactorArray[3].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[5])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[4].ratio + PosCmpCurrAdjustFactorArray[4].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[6])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[5].ratio + PosCmpCurrAdjustFactorArray[5].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[7])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[6].ratio + PosCmpCurrAdjustFactorArray[6].bias;
                                    }
                                    else if(SideX->step[phase].reference <= AdjustFactorSectionDivide[8])
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[7].ratio + PosCmpCurrAdjustFactorArray[7].bias;
                                    }
                                    else
                                    {
                                        temp3 = temp3 * PosCmpCurrAdjustFactorArray[8].ratio + PosCmpCurrAdjustFactorArray[8].bias;
                                    }*/

                                    temp3 = temp3 / PosCurrentRatio;
                                    temp3 += IavgPosDCRMSLoopOut;

                                    //if(CMPmodeSlopeEnable == 1)
                                    if(1)
                                    {
                                        //temp2 = CMPmodeSlopePos / PosCurrentRatio;
                                        temp2 = 1.0f / PosCurrentRatio; //每25us变化的电流（扩大10倍）//100/25=4 0.4A/us
                                        if((CMPmodeReferenceTemp - temp3) < (-temp2))
                                        {
                                            CMPmodeReferenceTemp += temp2;
                                        }
                                        else if((CMPmodeReferenceTemp - temp3) > temp2)
                                        {
                                            CMPmodeReferenceTemp -= temp2;
                                        }
                                        else
                                        {
                                            CMPmodeReferenceTemp = temp3;
                                        }

                                        temp4 = CMPmodeReferenceTemp;
                                    }
                                    else
                                    {
                                        temp4 = temp3;
                                    }
								}

								if(CmpOpenLoopEnable == 0)
								{
								    Cmpss1Regs.DACHVALS.bit.DACVAL = temp4;
								}
								else
								{
								    Cmpss1Regs.DACHVALS.bit.DACVAL = PosCMPDACOpenLoop;
								}
								pwm1A = SideX->step[phase].duty;
								pwm2A = 0.0;						   	
						   	}
                           
                        }
                        else
                        {
                        
                        }
                    }
                }
                else
                {
                    if(fabsf(SideX->step[phase].reference) < SideX->smalllevel)
                    {
                        if(SidePWMjudge == cSideApwm)
                        {
	                        if(PWMorCMPTemp == cPWMmode)
	                        {

		                        SideALargePWMOFF_ISR();

	                        
	                        }
							else
							{
							
							}
                        
                        }
                        else
                        {
                        
                        }
                    }
                    else
                    {
                        if(SidePWMjudge == cSideApwm)
                        {
                            if(PWMorCMPTemp == cPWMmode)
                            {

	                            SideALargePWMON_ISR();
	                            pwm1A = 0.0;
	                            if(PwmOpenLoopEnable == 0)
	                            {
	                                pwm2A = SideX->step[phase].duty;
	                            }
	                            else
	                            {
	                                pwm2A = (float)NegPWMdutyOpenLoop / 100.0f;
	                            }
                            }
							else
							{
								SideALargePWMON_ISR();
								if(AdjustMode)
								    temp4 = SideX->step[phase].CMPSSVALUENeg;
								else
								{
                                    temp3 = SideX->step[phase].CMPSSVALUENeg * NegCurrentRatio - 150.0f;
                                    if(temp3 < 1.0f)
                                    {
                                        temp3 = 1.0f; //0.1A
                                    }

                                    if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[1])
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[0].ratio + NegCmpCurrAdjustFactorArray[0].bias;
                                    }
                                    else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[2])
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[1].ratio + NegCmpCurrAdjustFactorArray[1].bias;
                                    }
                                    //else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[3])
                                    else
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[2].ratio + NegCmpCurrAdjustFactorArray[2].bias;
                                    }
                                    /*else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[4])
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[3].ratio + NegCmpCurrAdjustFactorArray[3].bias;
                                    }
                                    else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[5])
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[4].ratio + NegCmpCurrAdjustFactorArray[4].bias;
                                    }
                                    else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[6])
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[5].ratio + NegCmpCurrAdjustFactorArray[5].bias;
                                    }
                                    else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[7])
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[6].ratio + NegCmpCurrAdjustFactorArray[6].bias;
                                    }
                                    else if((-SideX->step[phase].reference) <= AdjustFactorSectionDivide[8])
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[7].ratio + NegCmpCurrAdjustFactorArray[7].bias;
                                    }
                                    else
                                    {
                                        temp3 = temp3 * NegCmpCurrAdjustFactorArray[8].ratio + NegCmpCurrAdjustFactorArray[8].bias;
                                    }*/
                                    temp3 = temp3 / NegCurrentRatio;

                                    temp3 += IavgNegDCRMSLoopOut;

                                    //if(CMPmodeSlopeEnable == 1)
                                    if(1)
                                    {
                                        //temp2 = CMPmodeSlopeNeg / NegCurrentRatio;
                                        temp2 = 1.0f / NegCurrentRatio; //每25us变化的电流（扩大10倍）//100/25=4 0.4A/us

                                        if((CMPmodeReferenceTemp - temp3) < (-temp2))
                                        {
                                            CMPmodeReferenceTemp += temp2;
                                        }
                                        else if((CMPmodeReferenceTemp - temp3) > temp2)
                                        {
                                            CMPmodeReferenceTemp -= temp2;
                                        }
                                        else
                                        {
                                            CMPmodeReferenceTemp = temp3;
                                        }

                                        temp4 = CMPmodeReferenceTemp;
                                    }
                                    else
                                    {
                                        temp4 = temp3;
                                    }
								}

								if(CmpOpenLoopEnable == 0)
								{
								    CmpssLite2Regs.DACHVALS.bit.DACVAL = temp4;
								}
								else
								{
								    CmpssLite2Regs.DACHVALS.bit.DACVAL = NegCMPDACOpenLoop;
								}

								pwm1A = 0.0;
								pwm2A = SideX->step[phase].duty;

							}
                        }
                        else
                        {
                        
                        }
                    }
                }


            }
            else
            {
                SideX->step[phase].yn_2 = 0;
                SideX->step[phase].yn_1 = 0;
                SideX->step[phase].error_2 = 0;
                SideX->step[phase].error_1 = 0;
                SideX->step[phase].feedback_0 = 0;
                SideX->step[phase].feedback_1 = 0;

                if(SidePWMjudge == cSideApwm)
                {

                    SideALargePWMOFF_ISR();
                }
                else
                {

                }
            }


		if(SidePWMjudge == cSideApwm)
		{
			EPwm3Regs.CMPA.bit.CMPA = (Uint16)(SwitchPeriodReg * (pwm1A));//pwm driver reverse
	    	EPwm2Regs.CMPA.bit.CMPA = (Uint16)(SwitchPeriodReg * (pwm2A));			
		}
		else
		{
		
		}
}
/*********************************************************************
Function name:  void sOpenLoopControl(int SideXpwm)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void sOpenLoopControl(int SideXpwm,int PosOrNeg,float dutycycle)
{
	SidePWMjudge = SideXpwm;
	
	  if(SidePWMjudge == cSideApwm)
	  {
			if(PosOrNeg == cPositiveCurrent)
			{
			    SideALargePWMON_ISR();
			    pwm1A = dutycycle;
			    pwm2A = 0.0;
			
			}
			else
			{
			    SideALargePWMON_ISR();
			    pwm1A = 0.0;
			    pwm2A = dutycycle;
			
			}
			EPwm3Regs.CMPA.bit.CMPA = (Uint16)(SwitchPeriodReg * (pwm1A));//pwm driver reverse
	    	EPwm2Regs.CMPA.bit.CMPA = (Uint16)(SwitchPeriodReg * (pwm2A));
		 
	 }


}
/*********************************************************************
Function name:  void PWMsyn(stXparallel* input)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void PWMsyn(stXparallel* input)
{
			input->PWMSynclossCnt = 0;
			input->PWMSyncbackCnt++;


			
		input->pwmsyncRange = input->pwmperiod - cPERIOD_Bias;
		//PWMSynRange = wPWM_PERIOD/2 - cPERIOD_Bias;
	
		 input->pwmsyncCTR= EPwm1Regs.TBCTR;

	
		if(input->pwmsyncCTR <= input->pwmsyncRange)
	
		{
			input->PWMSyncLockCnt = 0;
			input->PWMSyncLockLoss = pdFALSE;
			WarnRegs.UnitSync.bit.PWMLockLoss = 0;
			WarnRegs.UnitSync.bit.PWMSyncLoss = 0;
			sClrISRWarningCode(cWunitPWMSyncLock);
			sClrISRWarningCode(cWunitPWMSyncLoss);
			input->PWMSyncSignalLoss = pdFALSE;
			
			if(EPwm1Regs.TBSTS.bit.CTRDIR == 1)
			{			
				input->pwmnewPRD = input->pwmperiod + cPWMSynStep;				
			}
			else
			{	
				input->pwmnewPRD = input->pwmperiod - cPWMSynStep;			  				
			}
		}
		else
		{
		    if(UnitPara.PWMSyncLock == xPWMSyncEn)
		    {
                input->PWMSyncLockCnt++;
                if(input->PWMSyncLockCnt >= input->PWMSyncLockCntSetting)
                {
                    input->PWMSyncLockCnt = 0;
                    input->PWMSyncLockLoss = pdTRUE;
                    WarnRegs.UnitSync.bit.PWMLockLoss = 1;
                    sSetISRWarningCode(cWunitPWMSyncLock);
                }
		    }
		    else
		    {
		        input->PWMSyncLockCnt = 0;
		        input->PWMSyncLockLoss = pdFALSE;
		        WarnRegs.UnitSync.bit.PWMLockLoss = 0;
		        sClrISRWarningCode(cWunitPWMSyncLock);
		    }
		}
		EPwm1Regs.TBPRD = input->pwmnewPRD; 

		if((CMPmodePWMCountMode == 1) && (PWMorCMP == cCMPmode))
	    {
            if(SwitchFreq == Fsw160KHz)
            {
                EPwm2Regs.TBPRD = input->pwmnewPRD/2-1;
                EPwm3Regs.TBPRD = input->pwmnewPRD/2-1;
            }
            else if(SwitchFreq == Fsw120KHz)
            {
                EPwm2Regs.TBPRD = input->pwmnewPRD*2/3-1;
                EPwm3Regs.TBPRD = input->pwmnewPRD*2/3-1;
            }
            else if(SwitchFreq == Fsw80KHz)
            {
                EPwm2Regs.TBPRD = input->pwmnewPRD-1;
                EPwm3Regs.TBPRD = input->pwmnewPRD-1;
            }
            else
            {
                EPwm2Regs.TBPRD = input->pwmnewPRD*2-1;
                EPwm3Regs.TBPRD = input->pwmnewPRD*2-1;
            }
	    }
		else
		{
		    if(SwitchFreq == Fsw160KHz)
            {
                EPwm2Regs.TBPRD = input->pwmnewPRD/4;
                EPwm3Regs.TBPRD = input->pwmnewPRD/4;
            }

            else if(SwitchFreq == Fsw120KHz)
            {
                EPwm2Regs.TBPRD = input->pwmnewPRD/3;
                EPwm3Regs.TBPRD = input->pwmnewPRD/3;
            }
            else if(SwitchFreq == Fsw80KHz)
            {
                EPwm2Regs.TBPRD = input->pwmnewPRD/2;
                EPwm3Regs.TBPRD = input->pwmnewPRD/2;
            }
            else
            {
                EPwm2Regs.TBPRD = input->pwmnewPRD;
                EPwm3Regs.TBPRD = input->pwmnewPRD;
            }
		}

		
//			if(PWMorCMP == cPWMmode)
//			{
//				//EPwm1Regs.TBPRD = wPWMNewPRD;
//				EPwm2Regs.TBPRD = input->pwmnewPRD;
//				EPwm3Regs.TBPRD = input->pwmnewPRD;

//			}
//			else
//			{
//				//EPwm1Regs.TBPRD = wPWMNewPRD * 2;
//				EPwm2Regs.TBPRD = input->pwmnewPRD * 2;
//				EPwm3Regs.TBPRD = input->pwmnewPRD * 2;

//			}
	
			input->PWMSyncSignal = 0;

}
/*********************************************************************
Function name:  void PWMsyn(stXparallel* input)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
ControlType	GetControlMode(void)
{
	return(ControlMode);
}

/*******************************************************************************
Function name:  void    sLevelProtect(stOverCheck* input)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void	sLevelProtect(stOverCheck* input)
{

		if(input->Enable == pdTRUE)
		{
		
			if(input->Overflag == pdFALSE)
			{
				input->Overbackcount = 0;
				
				if(input->compvalue == input->Overlevel)
				{
					input->Overcount++;
				}
				else
				{
					input->Overcount = 0;
				}
				if(input->Overcount > input->Overcountset)
				{
					input->Overcount = 0;
					input->Overflag = pdTRUE;
				}
				else
				{
					
				}
					
			}
			else
			{
				input->Overcount = 0;

				
				if(input->compvalue == input->Overback)
				{
					input->Overbackcount++;
				}
				else
				{
					input->Overbackcount = 0;
				}
				if(input->Overbackcount > input->Overbackcountset)
				{
					input->Overbackcount = 0;
					input->Overflag = pdFALSE;
				}
				else
				{
					
				}
			}
		}
		else
		{
			input->Overflag = pdFALSE;
		}
}

/*********************************************************************
Function name:  void Power24Vdetect(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void Power24Vdetect(void)
{
	Power24Vcheck.compvalue = Power24Vsignal();
	sLevelProtect(&Power24Vcheck);
	if(GetPower24Vunderflag() == pdTRUE)
	{
		//FaultRegs.HWfaultLoad.bit.Hw24Vabnormal = 1;
		WarnRegs.TempWarn.bit.Hw24Vabnormal = 1;
		sSetWarningCode(cWHw24Vabnormal);
		//OSEventSend(cPrioSuper,eSuperFault);
	}
	else
	{
		sClrWarningCode(cWHw24Vabnormal);
		//FaultRegs.HWfaultLoad.bit.Hw24Vabnormal = 0;
		WarnRegs.TempWarn.bit.Hw24Vabnormal = 0;
	}

}
/*********************************************************************
Function name:  void Power24Vdetect(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
int16 GetPower24Vunderflag(void)
{
	return(Power24Vcheck.Overflag);

}

/*********************************************************************
Function name:  canISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
__interrupt void canISR(void)
{
    uint32_t status;
    uint16_t rxMsgData[8];

    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
    status = CAN_getInterruptCause(CANA_BASE);

    //
    // If the cause is a controller status interrupt, then get the status
    //
    if(status == CAN_INT_INT0ID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        //
        status = CAN_getStatus(CANA_BASE);

        //
        // Check to see if an error occurred.
        //
        if(((status  & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 7) &&
           ((status  & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 0))
        {
            //
            // Set a flag to indicate some errors may have occurred.
            //
        }
    }

    //
    // Check if the cause is the transmit message object 1
    //
    else if(status == TX_MSG_OBJ_ID)
    {
        //
        // Getting to this point means that the TX interrupt occurred on
        // message object 1, and the message TX is complete.  Clear the
        // message object interrupt.
        //
        CAN_clearInterruptStatus(CANA_BASE, TX_MSG_OBJ_ID);

        //
        // Increment a counter to keep track of how many messages have been
        // sent.  In a real application this could be used to set flags to
        // indicate when a message is sent.
        //
        txCanMsgCount++;
    }

    //
    // Check if the cause is the receive message object 2
    //
    else if(status == RX_MSG_OBJ_ID)
    {
        //
        // Get the received message
        //
        // CAN_readMessage(CANA_BASE, RX_MSG_OBJ_ID, rxMsgData);
        CAN_readMessage(CANA_BASE, RX_MSG_OBJ_ID, rxMsgData);
        sQDataIn(&queCanRxMsg,rxMsgData,cQCoverLast);

        //
        // Getting to this point means that the RX interrupt occurred on
        // message object 2, and the message RX is complete.  Clear the
        // message object interrupt.
        //
        CAN_clearInterruptStatus(CANA_BASE, RX_MSG_OBJ_ID);

        //
        // Increment a counter to keep track of how many messages have been
        // received. In a real application this could be used to set flags to
        // indicate when a message is received.
        //
        rxCanMsgCount++;
    }

    //
    // If something unexpected caused the interrupt, this would handle it.
    //
    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }

    //
    // Clear the global interrupt flag for the CAN interrupt line
    //
    CAN_clearGlobalInterruptStatus(CANA_BASE, CAN_GLOBAL_INT_CANINT0);

    //
    // Acknowledge this interrupt located in group 9
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
/*********************************************************************
Function name:  void sOverLoad(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
 float disvolt = 500;
void sOverLoad(void)
{
	if(GetPWMstatus() == PWMrunSts)
	{
		if((BusVolt - fabsf(OutPutVolt)) <= disvolt)
		{
			OverLoadCnt++;
			if(OverLoadCnt >= 10)
			{
				OverLoadCnt = 0;
	
				WarnRegs.TempWarn.bit.OverLoad = 1;
				sSetISRWarningCode(cWHwOverLoad);
			}
			else
			{
				WarnRegs.TempWarn.bit.OverLoad = 0;
				sClrISRWarningCode(cWHwOverLoad);
				
			}
			
		}
		else
		{

		}
	}
	else
	{
		OverLoadCnt = 0;
		 WarnRegs.TempWarn.bit.OverLoad = 0;
		sClrWarningCode(cWHwOverLoad);
	
	}


}
/*********************************************************************
Function name:	void Timer1Isr()
Description:	工频周期定时器，与timer1交替使用
Calls:
Called By:
Parameters: 	void
Return: 		void
 *********************************************************************/
interrupt void Timer0Isr()
{

//	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	
	//Timer2Resume();

	if(BoardType == UnitCB)
	{
		if((GetWaveForm() != DC) && (UnitPara.Mode == Parallel))
		{
//			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			StepTimecountClr();
            //Miller/20240929/for sync begin
            /*if(u16_Epwm1Flag == 1)
            {
                u16_PulseResetFlag = 1;
            }
            else
            {
                u16_PulseResetFlag = 0;
            }*/
            //Miller/20240929/for sync end
//			if(stSynComm.u16_SynTimerStart & cSynTimer1) //The timer1 has been started
//			{
//			    Timer0Stop();
//                stSynComm.u16_SynTimerStart &= (~cSynTimer0);
//                stSynComm.u16_SynTimerExecuted = cSynTimerNone;
//			}
//			else
//			{
//			    stSynComm.u16_SynTimerExecuted = cSynTimer0; //the timer0 executed before CAP
//			    Timer0ReSet(TimerPeriod_100ns);
//			    Epwm5Count = 0;
//			    sEnable_PrePulse_Tmr(SynPeriod_100us-1);
//			}
			if(timer1RunFlag == 1)
			{
				timer0RunFlag = 0;
				timer1RunFlag = 0;
			}
			else
			{
				timer0RunFlag = 1;
			}
			EPwm1Regs.ETSEL.bit.INTEN = 0x1;
			EPwm1Regs.TBCTR = cControlperiod-1;	//同步后从同一时刻开始启动
			Timer0Stop();
			//sEnable_PulseCycle_Tmr(TimerPeriod_100ns);
//			sSetUnitExecutedTimer(SynPeriod_100us-1);
		}
	}
	else
	{
		MasterExtSyncL;
		Timer0ReSet(TimerPeriod_100ns);
//		PwmSyncON_Isr();

//		sSynPeriodIsr();
		sSyncLossCheck();
		
		Epwm5Count = 0;
		sEnable_PrePulse_Tmr(SynPeriod_100us - 1);
	}
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

/*********************************************************************
Function name:	void Timer1Isr()
Description:	工频周期定时器，与timer0交替使用
Calls:
Called By:
Parameters: 	void
Return: 		void
 *********************************************************************/
interrupt void Timer1Isr()
{	
//    GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	
	//Timer2Resume();

	if(BoardType == UnitCB)
	{
		if((GetWaveForm() != DC) && (UnitPara.Mode == Parallel))
		{
//			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
//			StepTimecountClr();
            //Miller/20240929/for sync begin
            /*if(u16_Epwm1Flag == 1)
            {
                u16_PulseResetFlag = 1;
            }
            else
            {
                u16_PulseResetFlag = 0;
            }*/
            //Miller/20240929/for sync end
//			if(stSynComm.u16_SynTimerStart & cSynTimer0) //The timer0 has been started
//            {
//                Timer1Stop();
//                stSynComm.u16_SynTimerStart &= (~cSynTimer1);
//                stSynComm.u16_SynTimerExecuted = cSynTimerNone;
//            }
//            else
//            {
//                stSynComm.u16_SynTimerExecuted = cSynTimer1; //the timer1 executed before CAP
//                Timer1ReSet(TimerPeriod_100ns);
//                Epwm5Count = 0;
//                sEnable_PrePulse_Tmr(SynPeriod_100us-1);
//            }
			if(timer0RunFlag == 0)
			{
				EPwm1Regs.ETSEL.bit.INTEN = 0x0;
			}
			timer1RunFlag = 1;
			Timer1Stop();
//			Epwm5Count = 0;
//			sEnable_PrePulse_Tmr(SynPeriod_100us-1);
//			sEnable_PulseCycle_Tmr(TimerPeriod_100ns);
//			sSetUnitExecutedTimer(SynPeriod_100us-1);
		}
	}
	else
	{
		MasterExtSyncL;
		Timer1ReSet(TimerPeriod_100ns);
//		PwmSyncON_Isr();

//		sSynPeriodIsr();
		sSyncLossCheck();
			
		Epwm5Count = 0;
		sEnable_PrePulse_Tmr(SynPeriod_100us - 1);
//		stSynComm.u16_Synchoronized = 0;
	}
}
uint32_t timer2cnt = 0;
interrupt void Timer2Isr()
{
	timer2cnt++;
	if(GetBoardType() == UnitCB)
		{}
	else
	{
		sSynPeriodIsr();
	}
}

/*********************************************************************
Function name:	void epwm4ISR(void)
Description:	25us定时器，用于发送接收总线数据
Calls:
Called By:
Parameters: 	void
Return: 		void
 *********************************************************************/
interrupt void epwm4ISR(void)
{
	sSyncStateMachine();

//	EPWM_clearEventTriggerInterruptFlag(EPWM4_BASE);
	EPwm4Regs.ETCLR.bit.INT = 1;
    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

/*********************************************************************
Function name:	void epwm5ISR(void)
Description:	比工频周期定时器提前中断，提前停止os定时器，防止工频周期定时器未能及时响应
Calls:
Called By:
Parameters: 	void
Return: 		void
 *********************************************************************/
interrupt void epwm5ISR(void)
{
#ifdef INTERRUPT_NESTING_ENABLE
	IER |= (M_INT1 | M_INT13);
	IER &= (M_INT1 | M_INT13);
	PieCtrlRegs.PIEACK.all = 0xFFFF;      // Enable PIE interrupts
	
	asm(" nop");
	EINT;
#endif
	
	Epwm5Count++;

	if(Epwm5Count == PreSynPeriod_100us)
	{
		//Timer2Stop();	//stop os timer
		sDisable_PrePulse_Tmr();
		Epwm5Count = 0;
	}
	if(BoardType != UnitCB)
	{
		if(Epwm5Count == 10)
			MasterExtSyncH;
	}

	

//	EPWM_clearEventTriggerInterruptFlag(EPWM5_BASE);
	EPwm5Regs.ETCLR.bit.INT = 1;
    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
#ifdef INTERRUPT_NESTING_ENABLE

	DINT;
#endif
}

