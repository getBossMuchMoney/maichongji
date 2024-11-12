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
/********************************************************************************
*const define                               *
********************************************************************************/


/********************************************************************************
* Variable declaration                              *
********************************************************************************/
volatile uint32_t txCanMsgCount = 0;
volatile uint32_t rxCanMsgCount = 0;

/*********************************************************************
Function name:  epwm1ISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
interrupt void epwm1ISR(void)
{

	


    ADC_A1_ISR_Count++;
#if 0

	ExtSynSignalSetting();

	
	 if(GetBoardType() == UnitCB) 
	 {
	 	UnitPWMSyncSignalLoss();
	 }
  
	DACHVALSTest++;
	// Cmpss1Regs.DACHVALS.bit.DACVAL = DACHVALSTest;
	 if(DACHVALSTest >= 3000)
	 {
	     DACHVALSTest = 0;
	 }

  
	ADC_A1_ISR_Count++;
	if(ADC_A1_ISR_Count >= 100)
	{
		ADC_A1_ISR_Count = 0;
		 
		//OSISREventSend(cPrioSuper,eSuperTest,xSemaphoreSuper);
		
	}
	
	while(ADC_isBusy(ADCA_BASE) == pdTRUE);

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

	SwitchDIPSample = ((float)AdccResultRegs.ADCRESULT4  + SwitchDIPBias); //SwitchDIP
	SwitchDIP = SwitchDIPSample * SwitchDIPRatio;
	
	

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
/*
	if(GetPWMstatus() == cPWMrunSts)
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
	
	}
*/
#endif
   // EPwm1Regs.ETCLR.bit.INT = 1;
   // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
       // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM1_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);


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

#if 0


	 SideALargePWMON_ISR();

	 ExtSynSignalSetting();

	
	 if(GetBoardType() == UnitCB) 
	 {
	 	UnitPWMSyncSignalLoss();
	 }
  
	DACHVALSTest++;
	// Cmpss1Regs.DACHVALS.bit.DACVAL = DACHVALSTest;
	 if(DACHVALSTest >= 3000)
	 {
	     DACHVALSTest = 0;
	 }

  
	ADC_A1_ISR_Count++;
	if(ADC_A1_ISR_Count >= 100)
	{
		ADC_A1_ISR_Count = 0;
		 
		//OSISREventSend(cPrioSuper,eSuperTest,xSemaphoreSuper);
		
	}
	


  
   


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

	SwitchDIPSample = ((float)AdccResultRegs.ADCRESULT4  + SwitchDIPBias); //SwitchDIP
	SwitchDIP = SwitchDIPSample * SwitchDIPRatio;
	
	

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
	
	if(GetPWMstatus() == cPWMrunSts)
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
	
	}
	  
#endif

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
interrupt void PWMsync_CAP1_ISR(void)
{
#if 0
    //		if((BoardType == ModuleCB)&&(BoardRole == SlaveCB))
//		{
//			PWMsyn(&ModulePara);
//		}
		
		if(BoardType == UnitCB)
		{
			PWMsyn(&UnitPara);
		}
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
#endif
    ECap1Regs.ECCLR.bit.CEVT1 = 1;         // clear CEVT1 interrupt flag
    ECap1Regs.ECCLR.bit.INT = 1;            // clear ECAP global interrupt flag
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
	//for Syn loss detect
#if 0
	
	if(BoardType == UnitCB)
	{

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
		UnitPara.ExtSyncStartPoint = 1;
	}


#endif


	ECap2Regs.ECCLR.bit.CEVT1 = 1;		   // clear CEVT1 interrupt flag
	ECap2Regs.ECCLR.bit.INT = 1;			// clear ECAP global interrupt flag
	// Acknowledge this interrupt to receive more interrupts from group 4
	PieCtrlRegs.PIEACK.bit.ACK4 = 1;				// Clear PIEIFR bit

	

}
/*********************************************************************
Function name:  EPWM2_TZ_ISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
interrupt void EPWM2_TZ1_ISR(void)
{

	// Clear the interrupt flag	
	EALLOW;
//	EPwm2Regs.TZCLR.bit.CBC = 1;
	EPwm2Regs.TZCLR.bit.OST = 1;
	EPwm2Regs.TZCLR.bit.INT = 1;
	EDIS;
	//SideALargePWMOFF_ISR();

//	FaultRegs.HWfaultLoad.bit.HwExtLoadOC = 1;
//	sSetISRFaultCode(cFHwExtLoadOC);
	
	
	// Acknowledge the interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;


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
      Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

