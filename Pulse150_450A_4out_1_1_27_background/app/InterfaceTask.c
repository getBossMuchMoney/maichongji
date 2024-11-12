/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : InterfaceTask.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define InterfaceTask_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include <string.h>
#include "driverlib.h"
#include "device.h"
#include "SuperTask.h"
#include "Interrupt.h"
#include "InterfaceTask.h"
#include "OsConfig.h"
#include "Initial.h"
#include "Constant.h"
#include "sciframe.h"
#include "scimodbusreg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Common.h"
#include "HMITask.h"
#include "fault.h"
#include "CanTask.h"
#include "LoadTask.h"
#include "Version.h"
#include "eeprommodule.h"
#include "SyncProc.h"
#include "Rs485RegDef.h"
/********************************************************************************
*const define                               *
********************************************************************************/
#define INTERFACESCI             (SciType)0

#define INTERNALISHOST      1
#define INTERNALISSLAVE     0

/********************************************************************************
* Variable declaration                              *
********************************************************************************/
Cmdstruct intfScisend;


uint16_t intfData[40]={0};

/********************************************************************************
* function declaration                              *
********************************************************************************/
static void datacopy(uint16_t* sou,uint16_t* dir,uint16_t len)
{
  uint16_t *ptrSou;
  uint16_t *ptrDir;
  uint16_t templen;
  uint16_t temp;
  templen = len;
  ptrSou = sou;
  ptrDir = dir;
  while(templen)
  {
    temp = *ptrSou;
    ptrSou++;
    temp <<= 8;
    temp |= *ptrSou;
    ptrSou++;
    templen--;
    *ptrDir = temp;
    ptrDir++;
  }
}
void CalcAdjustFactor(sAdjustValue Val1, sAdjustValue Val2, sRatioBias *factor)
{
	float ratio,bias;
	ratio = (Val1.y-Val2.y)/(Val1.x-Val2.x);
	bias = (Val1.x*Val2.y - Val2.x*Val1.y)/(Val1.x-Val2.x);
	
	factor->Ratio = (Uint16)(ratio*10000.0);
	factor->Bias = (int16)(bias*10.0);

	if((factor->Ratio == 0) || (factor->Ratio > 20000))
	{
		factor->Ratio = 10000;
		factor->Bias = 0;
	}
}

BaseType_t SetCurrent(uint16_t outMode, uint16_t sectNum, int16_t currValue)
{
	BaseType_t  parse = pdFAIL;
	float temp1;
//	float temp2;

//	float Cmpss1DACHVALS_PosTemp;
//	float Cmpss2DACHVALS_NegTemp;

	if((currValue > 0) && (currValue/10 > SideAControl.IPosNominal))
	{
		currValue = (int16_t)SideAControl.IPosNominal*10;
	}
	else if((currValue < 0) && (currValue/10 < SideAControl.INegNominal))
	{
		currValue = (int16_t)SideAControl.INegNominal*10;
	}
		
	if((outMode)&&(sectNum == 0))	//直流模式下只有1可用
	{
	//JBG
//		if(currValue >= 0)
//		{
//			temp1 = (float)fabsf(currValue)*0.99;
//
//		}
//		else
//		{
//			temp1 = (float)(currValue)*0.99;
//
//			
//		}
		
//		SideAControl.step[0].reference = temp1;
		SideAControl.step[0].reference = currValue;

		if(currValue > 0.0)
            //SideAControl.step[0].CMPSSVALUEPos = currValue;
			SideAControl.step[0].CMPSSVALUEPos = (float)currValue / PosCurrentRatio;
		else
            //SideAControl.step[0].CMPSSVALUENeg = -currValue;
			SideAControl.step[0].CMPSSVALUENeg = (float)(-currValue) / NegCurrentRatio;

		parse = pdPASS;
	}
	else if(outMode == 0)
	{
		//JBG
	//					temp1 = (float)fabsf(currValue);

	//					temp1 = temp1 * temp1;
	//					temp1 = 0.0008 * temp1;
	//					temp2 = 0.6972 * ((float)fabsf(currValue));
	//					temp2 = temp2 + temp1;
	//					temp2 = temp2 + 44.458;

	//					if(Ctrldata1.SectASet[regaddress/2].Current >= 0)
	//					{
	//						temp1 = (float)fabsf(currValue);
	//						temp1 = temp1 * temp1;
	//						temp1 = 0.0008 * temp1;
	//						temp2 = 0.6972 * ((float)fabsf(currValue));
	//						temp2 = temp2 + temp1;
	//						temp2 = temp2 + 44.458;	
				
	//					}
	//					else
	//					{
	//						temp1 = (float)(currValue);
	//						temp1 = temp1 * temp1;
	//						temp1 = -0.0003 * temp1;
	//						temp2 = 0.7731 * ((float)(currValue));
	//						temp2 = temp2 + temp1;
	//						temp2 = temp2 - 40.752;


	//					}
		if(AdjustMode)
			temp1 = currValue;
		else
		{
			if(currValue >= 0.0)
			{
				temp1 = currValue;
//				if(currValue > cPosPulseSmallCurrThreshold)
//					temp1 = currValue * PosPulseCurrAdjustFactor.ratio + PosPulseCurrAdjustFactor.bias;
//				else
//					temp1 = currValue * PosPulseSmallCurrAdjustFactor.ratio + PosPulseSmallCurrAdjustFactor.bias;
			}
			else
			{
				temp1 = currValue;
//				if(-currValue > cNegPulseSmallCurrThreshold)
//					temp1 = currValue * NegPulseCurrAdjustFactor.ratio + NegPulseCurrAdjustFactor.bias;
//				else
//					temp1 = currValue * NegPulseSmallCurrAdjustFactor.ratio + NegPulseSmallCurrAdjustFactor.bias;
			}
		}
		SideAControl.step[sectNum+1].reference = temp1;
//		SideAControl.step[sectNum+1].reference = currValue;
		
		if(SideAControl.step[sectNum+1].reference >= 0.0)
		{
//			Cmpss1DACHVALS_PosTemp = SideAControl.step[sectNum+1].reference * cPosCurrentRatioDiv;
//			Cmpss1DACHVALS_PosTemp = Cmpss1DACHVALS_PosTemp - PosCurrentBias;
//			Cmpss1DACHVALS_PosTemp = Cmpss1DACHVALS_PosTemp + 100;
//			Cmpss1DACHVALS_PosTemp = currValue * PosCmpCurrAdjustFactor.ratio + PosCmpCurrAdjustFactor.bias;
//			if(Cmpss1DACHVALS_PosTemp <= 0.0)
//			{
//				Cmpss1DACHVALS_PosTemp = 0.0;
//			}
//			else if(Cmpss1DACHVALS_PosTemp >= Cmpss1DACHVALS_Pos)
//			{
//				Cmpss1DACHVALS_PosTemp = Cmpss1DACHVALS_Pos;
//			}
//			else
//			{
//			
//			}
            //SideAControl.step[sectNum+1].CMPSSVALUEPos = currValue;
			SideAControl.step[sectNum+1].CMPSSVALUEPos = (float)currValue / PosCurrentRatio;
			//Cmpss1Regs.DACHVALS.bit.DACVAL= SideAControl.step[PulsePhase1].CMPSSVALUEPos*0.1;
		}
		else
		{
//			Cmpss2DACHVALS_NegTemp = -SideAControl.step[sectNum+1].reference * cNegCurrentRatioDiv;
//			Cmpss2DACHVALS_NegTemp = Cmpss2DACHVALS_NegTemp - NegCurrentBias;
//			Cmpss2DACHVALS_NegTemp = Cmpss2DACHVALS_NegTemp + 100.0;
			
//			Cmpss2DACHVALS_NegTemp = -currValue * NegCmpCurrAdjustFactor.ratio + NegCmpCurrAdjustFactor.bias;
//			if(Cmpss2DACHVALS_NegTemp <= 0.0)
//			{
//				Cmpss2DACHVALS_NegTemp = 0.0;
//			}
//			else if(Cmpss2DACHVALS_NegTemp >= Cmpss2DACHVALS_Neg)
//			{
//				Cmpss2DACHVALS_NegTemp = Cmpss2DACHVALS_Neg;
//			}
//			else
//			{
//			
//			}
            //SideAControl.step[sectNum+1].CMPSSVALUENeg = -currValue;
	        SideAControl.step[sectNum+1].CMPSSVALUENeg = (float)(-currValue) / NegCurrentRatio;
			//CmpssLite2Regs.DACHVALS.bit.DACVAL= SideAControl.step[PulsePhase1].CMPSSVALUENeg*0.6;

		
		}
		

		parse = pdPASS;
	}

	return parse;
}
void CalcStepTimecount()
{
//	uint16_t step = 0;
//	uint16_t timecount = 0;

	UnitPara.SlaveExtSyncStartPoint = (SyncPhase%360)*UnitPara.SumOfExtSyncPoint/360;
	sSyncPhaseShiftSet((SyncPhase%360)*UnitPara.SumOfExtSyncPoint/4/360);
	
//	if(UnitPara.SlaveExtSyncStartPoint == 0)
//	{
//		InitPulsePhase = 1;
//		InitTimeCount = 0;
//		return;
//	}
//	for(step = 1; step < PulsePhaseNum+1; step++)
//	{
//		if(UnitPara.SlaveExtSyncStartPoint < timecount + SideAControl.step[step].timecountset)
//		{
//			InitPulsePhase = step;
//			InitTimeCount = UnitPara.SlaveExtSyncStartPoint - timecount;
//			break;
//		}
//		timecount += SideAControl.step[step].timecountset;
//	}
}
void sCalcSyncPeriod()
{
	uint16_t i;
	uint16_t temp;
	
	UnitPara.SumOfExtSyncPoint = 0;
	temp = 0;
	for(i=0;i<SECTOR_MAX;i++)
	{
		if(SideAControl.step[i+1].timecountset != 0)
		{
			UnitPara.SumOfExtSyncPoint += SideAControl.step[i+1].timecountset;
			temp = i+1;
		}
		else
		{

		}
	}
	PulsePhaseNum = temp;
	ModulePara.SumOfExtSyncPoint = UnitPara.SumOfExtSyncPoint;
	UnitPara.ExtSynclossCntSetting = UnitPara.SumOfExtSyncPoint*2/40;
	ModulePara.ExtSynclossCntSetting = UnitPara.ExtSynclossCntSetting;

	sSyncPeriodSet(UnitPara.SumOfExtSyncPoint/4);
}
void sSetPulseWideFromModule()
{
	uint16_t i;
	
	for(i=0;i<SECTOR_MAX;i++)
		SideAControl.step[i+1].timecountset = (uint32_t)Ctrldata1.SectASet[i].PulseWide*4;
	
	sCalcSyncPeriod();
}
void sSetPulseWideFromUnit()
{
	uint16_t i;
	uint16_t unitnum = Get485Addr() - SlaveStartAddr;
	
	for(i=0;i<SECTOR_MAX;i++)
		SideAControl.step[i+1].timecountset = (uint32_t)sUnitCtrlReg[unitnum].SectASet[i].PulseWide*4;
	
	sCalcSyncPeriod();
}
void sSetPulseWideFromRemote()
{
	uint16_t i;
	uint16_t unitnum = Get485Addr() - SlaveStartAddr;

	if(GetBoardType() == UnitCB)
	{
		for(i=0;i<SECTOR_MAX;i++)
			SideAControl.step[i+1].timecountset = (uint32_t)sSynRemoteUnitCtrlReg[unitnum].SectASet[i].PulseWide*4;
	}
	else
	{
		for(i=0;i<SECTOR_MAX;i++)
			SideAControl.step[i+1].timecountset = (uint32_t)sSynRemoteCtrlReg.SectASet[i].PulseWide*4;
	}
	
	sCalcSyncPeriod();
}

void sModuleStartCtrl()
{
	uint16_t i;

	for(i=0; i<SECTOR_MAX; i++)
	{
		if(Ctrldata1.Ctrl1.bit.OutMode)
		{
			SetCurrent(1,0,Ctrldata1.SectASet[0].Current);
			break;
		}
		else
		{
			SetCurrent(0,i,Ctrldata1.SectASet[i].Current);
		}
	}
	sSetPulseWideFromModule();
	
	if(GetBoardType() == UnitCB)
	{
		if(Ctrldata1.Ctrl2.bit.loopMode)	//open loop
		{
			SideAduty = Ctrldata1.SectASet[0].PulseWide / 1000.0;
			
			if(Ctrldata1.SectASet[1].PulseWide)
				PosOrNegCurrentA = 1;
			else
				PosOrNegCurrentA = 0;
		}
		
		if(Ctrldata1.Ctrl1.bit.OutMode)
			WaveForm = DC;
		else
			WaveForm = Pulse;

		ModulePara.Mode = Parallel;
		UnitPara.Mode = Parallel;
	
		if((Ctrldata1.Ctrl1.bit.OnOff) && ((Ctrldata1.UnitCtrl.all & (1<<(Get485Addr()-SlaveStartAddr))) == 0))
		{
//			OSEventSend(cPrioSuper,eSuperTurnOn);
			OSEventSend(cPrioCan,eCanPowerOn);
		}
		else
		{
			OSEventSend(cPrioSuper,eSuperTurnOff);
			OSEventSend(cPrioCan,eCanPowerOff);
		}
	}
}

void sUnitSingleStartCtrl()
{
	uint16_t i;
	uint16_t unitnum = Get485Addr() - SlaveStartAddr;

	ModulePara.Mode = Single;
	UnitPara.Mode = Single;

	if(GetBoardType() != UnitCB)
		return;

	if(sUnitCtrlReg[unitnum].Ctrl1.bit.OutMode)
		WaveForm = DC;
	else
		WaveForm = Pulse;
	
	for(i=0; i<SECTOR_MAX; i++)
	{
		if(sUnitCtrlReg[unitnum].Ctrl1.bit.OutMode)
		{
			SetCurrent(1,0,sUnitCtrlReg[unitnum].SectASet[0].Current);
			break;
		}
		else
		{
			SetCurrent(0,i,sUnitCtrlReg[unitnum].SectASet[i].Current);
		}
	}
	sSetPulseWideFromUnit();
	
	if(sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff)
	{
//		OSEventSend(cPrioSuper,eSuperTurnOn);
		OSEventSend(cPrioCan,eCanPowerOn);
	}
	else
	{
		OSEventSend(cPrioSuper,eSuperTurnOff);
		OSEventSend(cPrioCan,eCanPowerOff);
	}
}

BaseType_t IntfWriteParse(uint16_t cmd, uint16_t regaddress, uint16_t len, uint16_t *pdata)
{
	BaseType_t  parse = pdFAIL;
	eHMIRegion region = (eHMIRegion)(regaddress/HMI_REGION_LEN);
	uint16_t offset = 0;
	uint16_t unitnum = Get485Addr() - SlaveStartAddr;
	uint16_t *ptrDir;
	uint16_t tem=0,i;
//	uint16_t onoff = 0,outmode = 0,LocalRemote = 0;
	uint32_t u32tem = 0;
	CtrlNum1 ctrl1;
//	float temp1;
//	float temp2;

//	float Cmpss1DACHVALS_PosTemp;
//	float Cmpss2DACHVALS_NegTemp;
	
	switch(region)
	{
		case eHMIMODULECTRL:
			ptrDir = (uint16_t *)&Ctrldata1;
			ptrDir += regaddress;

			switch(regaddress)
			{
				case eModuleCtrlArea_Ctrl:
				{
					datacopy(pdata,(uint16_t*)&ctrl1,1);
					if(ctrl1.bit.LocalRemote == 0)
					{
						Ctrldata1.Ctrl1.bit.LocalRemote = 0;
						break;
					}
					else
					{
						Ctrldata1.Ctrl1.bit.LocalRemote = 1;
					}
					
					if(sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff)
					{
						sUnitSingleStartCtrl();
						break;
					}
					
					datacopy(pdata,ptrDir,len);
					sModuleStartCtrl();
					parse = pdPASS;
				}
				break;
				
				case eModuleCtrlArea_Curr1:case eModuleCtrlArea_Curr2:case eModuleCtrlArea_Curr3:case eModuleCtrlArea_Curr4:case eModuleCtrlArea_Curr5:
				case eModuleCtrlArea_Curr6:case eModuleCtrlArea_Curr7:case eModuleCtrlArea_Curr8:case eModuleCtrlArea_Curr9:case eModuleCtrlArea_Curr10:
				case eModuleCtrlArea_Curr11:case eModuleCtrlArea_Curr12:case eModuleCtrlArea_Curr13:case eModuleCtrlArea_Curr14:case eModuleCtrlArea_Curr15:
				case eModuleCtrlArea_Curr16:case eModuleCtrlArea_Curr17:case eModuleCtrlArea_Curr18:case eModuleCtrlArea_Curr19:case eModuleCtrlArea_Curr20:
					datacopy(pdata,ptrDir,len);
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 1) && (sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff == 0))
					{
						SetCurrent(Ctrldata1.Ctrl1.bit.OutMode,regaddress/2, Ctrldata1.SectASet[regaddress/2].Current);
					}
					parse = pdPASS;
					break;
				case eModuleCtrlArea_Pulse1:case eModuleCtrlArea_Pulse2:case eModuleCtrlArea_Pulse3:case eModuleCtrlArea_Pulse4:case eModuleCtrlArea_Pulse5:
				case eModuleCtrlArea_Pulse6:case eModuleCtrlArea_Pulse7:case eModuleCtrlArea_Pulse8:case eModuleCtrlArea_Pulse9:case eModuleCtrlArea_Pulse10:
				case eModuleCtrlArea_Pulse11:case eModuleCtrlArea_Pulse12:case eModuleCtrlArea_Pulse13:case eModuleCtrlArea_Pulse14:case eModuleCtrlArea_Pulse15:
				case eModuleCtrlArea_Pulse16:case eModuleCtrlArea_Pulse17:case eModuleCtrlArea_Pulse18:case eModuleCtrlArea_Pulse19:case eModuleCtrlArea_Pulse20:
					datacopy(pdata,ptrDir,len);
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 1) && (sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff == 0))
					{
						sSetPulseWideFromModule();
						
						UnitPara.SlaveExtSyncStartPoint = (Ctrldata1.PhaseShift[Get485Addr()-SlaveStartAddr]%360)*UnitPara.SumOfExtSyncPoint/360;
						sSyncPhaseShiftSet((SyncPhase%360)*UnitPara.SumOfExtSyncPoint/4/360);
					}
					parse = pdPASS;
					break;

				case eModuleCtrlArea_PhaseShift1:case eModuleCtrlArea_PhaseShift2:
				case eModuleCtrlArea_PhaseShift3:case eModuleCtrlArea_PhaseShift4:
					datacopy(pdata,ptrDir,len);
					if(ptrDir[0] >= 180)
						ptrDir[0] = 180;
					
					parse = pdPASS;
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 1) && (sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff == 0))
					{
						UnitPara.SlaveExtSyncStartPoint = (Ctrldata1.PhaseShift[regaddress-eModuleCtrlArea_PhaseShift1]%360)*UnitPara.SumOfExtSyncPoint/360;
					}
				break;
				case eModuleCtrlArea_UnitCtrl:
					if(Ctrldata1.Ctrl1.bit.OnOff)
						break;
					datacopy(pdata,ptrDir,len);
					parse = pdPASS;
					break;
				case eModuleCtrlArea_Ctrl2:
					if((Ctrldata1.Ctrl1.bit.OnOff)&&(sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff))
					{
						break;
					}
					if(Ctrldata1.Ctrl1.bit.LocalRemote == 0)
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					
					if(Ctrldata1.Ctrl2.bit.loopMode)
					{
						ControlMode = OpenLoop;
					}
					else
					{
						ControlMode = CloseLoop;
					}
					parse = pdPASS;
					break;
				case eModuleCtrlArea_SyncPhase:
					datacopy(pdata,&tem,len);
					if(tem >= 180)
						tem = 180;
					
					parse = pdPASS;
					*ptrDir = tem;
					
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 1) && (sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff == 0))
					{
						SyncPhase = Ctrldata1.SyncPhase;
						sSyncPhaseShiftSet((SyncPhase%360)*UnitPara.SumOfExtSyncPoint/4/360);
					}
					break;
				
				default:
					if(regaddress + len < sizeof(sModuleCtrlregion))
					{
						datacopy(pdata,ptrDir,len);
						parse = pdPASS;
					}
				break;
			}
			break;
		case eHMIUNIT1CTRL:
		case eHMIUNIT2CTRL:
		case eHMIUNIT3CTRL:
		case eHMIUNIT4CTRL:
//			unitnum = (region-eHMIUNIT1CTRL)/4;
			ptrDir = (uint16_t *)&sUnitCtrlReg[unitnum];
			offset = regaddress-(region*HMI_REGION_LEN);
			ptrDir += offset;
			switch(offset)
			{
				case eUnintCtrlOffset_Ctrl:
				{
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 0) || (Ctrldata1.Ctrl1.bit.OnOff == 1))
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					sUnitSingleStartCtrl();

					parse = pdPASS;
				}
				break;
				case eUnintCtrlOffset_Curr1:case eUnintCtrlOffset_Curr2:case eUnintCtrlOffset_Curr3:case eUnintCtrlOffset_Curr4:case eUnintCtrlOffset_Curr5:
				case eUnintCtrlOffset_Curr6:case eUnintCtrlOffset_Curr7:case eUnintCtrlOffset_Curr8:case eUnintCtrlOffset_Curr9:case eUnintCtrlOffset_Curr10:
				case eUnintCtrlOffset_Curr11:case eUnintCtrlOffset_Curr12:case eUnintCtrlOffset_Curr13:case eUnintCtrlOffset_Curr14:case eUnintCtrlOffset_Curr15:
				case eUnintCtrlOffset_Curr16:case eUnintCtrlOffset_Curr17:case eUnintCtrlOffset_Curr18:case eUnintCtrlOffset_Curr19:case eUnintCtrlOffset_Curr20:
					datacopy(pdata,ptrDir,len);
					
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 1) && (Ctrldata1.Ctrl1.bit.OnOff == 0))
					{
						SetCurrent(sUnitCtrlReg[unitnum].Ctrl1.bit.OutMode,offset/2, sUnitCtrlReg[unitnum].SectASet[offset/2].Current);
					}
					parse = pdPASS;
					break;
				case eUnintCtrlOffset_Pulse1:case eUnintCtrlOffset_Pulse2:case eUnintCtrlOffset_Pulse3:case eUnintCtrlOffset_Pulse4:case eUnintCtrlOffset_Pulse5:
				case eUnintCtrlOffset_Pulse6:case eUnintCtrlOffset_Pulse7:case eUnintCtrlOffset_Pulse8:case eUnintCtrlOffset_Pulse9:case eUnintCtrlOffset_Pulse10:
				case eUnintCtrlOffset_Pulse11:case eUnintCtrlOffset_Pulse12:case eUnintCtrlOffset_Pulse13:case eUnintCtrlOffset_Pulse14:case eUnintCtrlOffset_Pulse15:
				case eUnintCtrlOffset_Pulse16:case eUnintCtrlOffset_Pulse17:case eUnintCtrlOffset_Pulse18:case eUnintCtrlOffset_Pulse19:case eUnintCtrlOffset_Pulse20:
					datacopy(pdata,ptrDir,len);
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 1) && (Ctrldata1.Ctrl1.bit.OnOff == 0))
					{
						sSetPulseWideFromUnit();
					}
					parse = pdPASS;
					break;
				case eUnintCtrlOffset_SectNum0:	//电流段数
					if(sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff)
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
//					PulsePhaseNum = sUnitCtrlReg[unitnum].SectNum;
					parse = pdPASS;
					break;
				case eUnintCtrlOffset_LoadEepromDef:	//恢复默认参数
					UserClearUnitParamToDefault();
					parse = pdPASS;
					break;

				case eUnintCtrlOffset_PowerSetValue:
					if((Ctrldata1.Ctrl1.bit.OnOff) || (sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff) || (sSynRemoteUnitCtrlReg[unitnum].Ctrl.bit.OnOff))
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					PowerSetValue = sUnitCtrlReg[unitnum].PowerSetValue;
					parse = pdPASS;
					break;
				case eUnintCtrlOffset_PosPWMdutyOpenLoop:
                    datacopy(pdata,ptrDir,len);
                    PosPWMdutyOpenLoop = sUnitCtrlReg[unitnum].PosPWMdutyOpenLoop;
                    if(PosPWMdutyOpenLoop > 98)
                    {
                        PosPWMdutyOpenLoop = 98;
                    }
                    else if(PosPWMdutyOpenLoop < 5)
                    {
                        PosPWMdutyOpenLoop = 5;
                    }
                    parse = pdPASS;
					break;
				case eUnintCtrlOffset_NegPWMdutyOpenLoop:
                    datacopy(pdata,ptrDir,len);
                    NegPWMdutyOpenLoop = sUnitCtrlReg[unitnum].NegPWMdutyOpenLoop;
                    if(NegPWMdutyOpenLoop > 98)
                    {
                        NegPWMdutyOpenLoop = 98;
                    }
                    else if(NegPWMdutyOpenLoop < 5)
                    {
                        NegPWMdutyOpenLoop = 5;
                    }
					break;
				case eUnintCtrlOffset_PosCMPDACOpenLoop:
                    datacopy(pdata,ptrDir,len);
                    PosCMPDACOpenLoop = sUnitCtrlReg[unitnum].PosCMPDACOpenLoop;
                    if(PosCMPDACOpenLoop > 4095)
                    {
                        PosCMPDACOpenLoop = 4095;
                    }
                    parse = pdPASS;
					break;
				case eUnintCtrlOffset_NegCMPDACOpenLoop:
                    datacopy(pdata,ptrDir,len);
                    NegCMPDACOpenLoop = sUnitCtrlReg[unitnum].NegCMPDACOpenLoop;
                    if(NegCMPDACOpenLoop > 4095)
                    {
                        NegCMPDACOpenLoop = 4095;
                    }
                    parse = pdPASS;
					break;
				case eUnintCtrlOffset_AdjustRatio1:
                    datacopy(pdata,ptrDir,len);
                    AdjustRatio1 = sUnitCtrlReg[unitnum].AdjustRatio1;
                    parse = pdPASS;
					break;
				case eUnintCtrlOffset_AdjustRatio2:
                    datacopy(pdata,ptrDir,len);
                    AdjustRatio2 = sUnitCtrlReg[unitnum].AdjustRatio2;
                    parse = pdPASS;
					break;
				case eUnintCtrlOffset_PwmOpenLoopEnable:
                    datacopy(pdata,ptrDir,len);
                    PwmOpenLoopEnable = sUnitCtrlReg[unitnum].PwmOpenLoopEnable;
                    parse = pdPASS;
					break;
				case eUnintCtrlOffset_CmpOpenLoopEnable:
                    datacopy(pdata,ptrDir,len);
                    CmpOpenLoopEnable = sUnitCtrlReg[unitnum].CmpOpenLoopEnable;
                    parse = pdPASS;
					break;
				case eUnintCtrlOffset_LoadAdjustDef:
				    UserClearUnitAdjustParamToDefault();
					LoadAdjustPara();
					break;
					
				case eUnintCtrlOffset_AdjustStart1:
				case eUnintCtrlOffset_AdjustStart2:
					if(Ctrldata1.Ctrl1.bit.LocalRemote == 0)
					{
						break;
					}
					
					if((Ctrldata1.Ctrl1.bit.OnOff) || (sSynRemoteUnitCtrlReg[unitnum].Ctrl.bit.OnOff))
					{
						break;
					}
					datacopy(pdata,&tem,len);
					u32tem = ((uint32_t)(sUnitCtrlReg[unitnum].StartAdjust1.all) | ((uint32_t)sUnitCtrlReg[unitnum].StartAdjust2.all << 16));
					if((u32tem & 0xfffff) && (tem != 0))
					{
						break;
					}
					if(offset == eUnintCtrlOffset_AdjustStart1)
						sUnitCtrlReg[unitnum].StartAdjust1.all = tem;
					else
						sUnitCtrlReg[unitnum].StartAdjust2.all = tem;
					u32tem = ((uint32_t)(sUnitCtrlReg[unitnum].StartAdjust1.all) | ((uint32_t)sUnitCtrlReg[unitnum].StartAdjust2.all << 16));
					AdjustMode = 1;
					if(sUintEepromPara.PwmCmpCtrl)
					{
						PWMorCMP = cCMPmode;
					}
					else
					{
						PWMorCMP = cPWMmode;
					}
					for(i=0;i<ADJUST_SECTOR_NUM*2;i++)
					{
						if(u32tem & ((uint32_t)1<<i))
							break;
					}
					
//					if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustSmallCurrMode)
//					{
//					}
//					else
					{
						if(i<ADJUST_SECTOR_NUM)
						{
							SetCurrent(1,0, sUnitCtrlReg[unitnum].PosAdjustPara[i].setValue/10);
							
						}
						else
						{
							SetCurrent(1,0, -(int16_t)(sUnitCtrlReg[unitnum].NegAdjustPara[i-ADJUST_SECTOR_NUM].setValue/10));
						}

//						if(i<16)	//多位被同时使能时只能使能其中一位
//						{
//							sUnitCtrlReg[unitnum].StartAdjust1.all = 1 << i;
//							sUnitCtrlReg[unitnum].StartAdjust2.all = 0;
//						}
//						else if(i<20)
//						{
//							sUnitCtrlReg[unitnum].StartAdjust1.all = 0;
//							sUnitCtrlReg[unitnum].StartAdjust2.all = 1 << (i-16);
//						}
						if(i<8)    //多位被同时使能时只能使能其中一位
                        {
                            sUnitCtrlReg[unitnum].StartAdjust1.all = 1 << i;
                            sUnitCtrlReg[unitnum].StartAdjust2.all = 0;
                        }
                        else
                        {
                            sUnitCtrlReg[unitnum].StartAdjust1.all = 0;
                            sUnitCtrlReg[unitnum].StartAdjust2.all = 0;
                        }						
					}
					WaveForm = DC;

					sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff = (u32tem & 0xfffff)>0?1:0;
					ModulePara.Mode = Single;
					UnitPara.Mode = Single;
					if(sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff)
					{
//						OSEventSend(cPrioSuper,eSuperTurnOn);
						OSEventSend(cPrioCan,eCanPowerOn);
					}
					else
					{
						AdjustMode = 0;
						OSEventSend(cPrioSuper,eSuperTurnOff);
						OSEventSend(cPrioCan,eCanPowerOff);
					}
					parse = pdPASS;
					break;
				
				case eUnintCtrlOffset_AdjustSave:
					if(PWMorCMP == cPWMmode)
					{
						if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustVoltMode)
						{							
							for(i=0;i<ADJUST_SECTOR_NUM-1;i++)
							{
								CalcAdjustFactor(AdjustValue[i],AdjustValue[i+1],	&sUintEepromAdjustPara.AdjustFactor.PosVoltAdjustFactor[i]);
								CalcAdjustFactor(AdjustValue[i+ADJUST_SECTOR_NUM],AdjustValue[i+ADJUST_SECTOR_NUM+1],	&sUintEepromAdjustPara.AdjustFactor.NegVoltAdjustFactor[i]);
							}

							offset = (uint16_t)((uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosVoltAdjustFactor - (uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor);
							UserWriteEeprom(EEPROM_ADJUST_PARA_START+(EEPROM_PARA_OFFSET+offset)*2,(Uint16*)sUintEepromAdjustPara.AdjustFactor.PosVoltAdjustFactor,ADJUST_SECTOR_NUM*4);
						}
						else if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustSmallCurrMode)
						{							
						}
						else
						{							
							for(i=0;i<ADJUST_SECTOR_NUM-1;i++)
							{
								CalcAdjustFactor(AdjustValue[i],AdjustValue[i+1],	&sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor[i]);
								CalcAdjustFactor(AdjustValue[i+ADJUST_SECTOR_NUM],AdjustValue[i+ADJUST_SECTOR_NUM+1],	&sUintEepromAdjustPara.AdjustFactor.NegCurrAdjustFactor[i]);
							}

							offset = (uint16_t)((uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor - (uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor);
							UserWriteEeprom(EEPROM_ADJUST_PARA_START+(EEPROM_PARA_OFFSET+offset)*2,(Uint16*)sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor,ADJUST_SECTOR_NUM*4);
						}
					}
					else
					{
						if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustVoltMode)
						{
							for(i=0;i<ADJUST_SECTOR_NUM-1;i++)
							{
								CalcAdjustFactor(AdjustValue[i],AdjustValue[i+1],	&sUintEepromAdjustPara.AdjustFactor.PosCmpVoltAdjustFactor[i]);
								CalcAdjustFactor(AdjustValue[i+ADJUST_SECTOR_NUM],AdjustValue[i+ADJUST_SECTOR_NUM+1],	&sUintEepromAdjustPara.AdjustFactor.NegCmpVoltAdjustFactor[i]);
							}

							offset = (uint16_t)((uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCmpVoltAdjustFactor - (uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor);
							UserWriteEeprom(EEPROM_ADJUST_PARA_START+(EEPROM_PARA_OFFSET+offset)*2,(Uint16*)sUintEepromAdjustPara.AdjustFactor.PosCmpVoltAdjustFactor,ADJUST_SECTOR_NUM*4);
						}
						else
						{
							for(i=0;i<ADJUST_SECTOR_NUM-1;i++)
							{
								CalcAdjustFactor(AdjustValue[i],AdjustValue[i+1],	&sUintEepromAdjustPara.AdjustFactor.PosCmpCurrAdjustFactor[i]);
								CalcAdjustFactor(AdjustValue[i+ADJUST_SECTOR_NUM],AdjustValue[i+ADJUST_SECTOR_NUM+1],	&sUintEepromAdjustPara.AdjustFactor.NegCmpCurrAdjustFactor[i]);
							}

							offset = (uint16_t)((uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCmpCurrAdjustFactor - (uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor);
							UserWriteEeprom(EEPROM_ADJUST_PARA_START+(EEPROM_PARA_OFFSET+offset)*2,(Uint16*)sUintEepromAdjustPara.AdjustFactor.PosCmpCurrAdjustFactor,ADJUST_SECTOR_NUM*4);

							for(i=0;i<ADJUST_SECTOR_NUM-1;i++)
							{
								CalcAdjustFactor(AdjustValue2[i],AdjustValue2[i+1],	&sUintEepromAdjustPara.AdjustFactor.PosCmpDispCurrAdjustFactor[i]);
								CalcAdjustFactor(AdjustValue2[i+ADJUST_SECTOR_NUM],AdjustValue2[i+ADJUST_SECTOR_NUM+1],	&sUintEepromAdjustPara.AdjustFactor.NegCmpDispCurrAdjustFactor[i]);
							}

							offset = (uint16_t)((uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCmpDispCurrAdjustFactor - (uint16_t*)&sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor);
							UserWriteEeprom(EEPROM_ADJUST_PARA_START+(EEPROM_PARA_OFFSET+offset)*2,(Uint16*)sUintEepromAdjustPara.AdjustFactor.PosCmpDispCurrAdjustFactor,ADJUST_SECTOR_NUM*4);

							
						}
					}
					parse = pdPASS;
					break;
				case eUnintCtrlOffset_AdjustMode:
					if(sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff)
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					for(i=0;i<ADJUST_SECTOR_NUM;i++)
					{
//						sUnitCtrlReg[unitnum].PosAdjustPara[i].setValue = 0;
						sUnitCtrlReg[unitnum].PosAdjustPara[i].actualValue = 0;
//						sUnitCtrlReg[unitnum].NegAdjustPara[i].setValue = 0;
						sUnitCtrlReg[unitnum].NegAdjustPara[i].actualValue = 0;
					}
					memset(AdjustValue,0,sizeof(AdjustValue));
					parse = pdPASS;
					break;
				case eUnintCtrlOffset_AdjustSet1:case eUnintCtrlOffset_AdjustSet2:case eUnintCtrlOffset_AdjustSet3:case eUnintCtrlOffset_AdjustSet4:
					if(sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff)
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					parse = SetCurrent(1,0, sUnitCtrlReg[unitnum].PosAdjustPara[(offset-eUnintCtrlOffset_AdjustSet1)/2].setValue/10);
					break;
				case eUnintCtrlOffset_AdjustAct1:case eUnintCtrlOffset_AdjustAct2:case eUnintCtrlOffset_AdjustAct3:case eUnintCtrlOffset_AdjustAct4:
					if((sUnitCtrlReg[unitnum].StartAdjust1.all & (1<<((offset-eUnintCtrlOffset_AdjustAct1)/2))) == 0)
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					if(PWMorCMP == cPWMmode)
					{
						if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustVoltMode)
						{
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].y = sUnitCtrlReg[unitnum].PosAdjustPara[(offset-eUnintCtrlOffset_AdjustAct1)/2].actualValue;
						
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = SideAControl.VavgPosDC;
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = VavgPosDCFilter;
						}
						else if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustSmallCurrMode)
						{
//							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = sUnitCtrlReg[unitnum].PosAdjustPara[(offset-eUnintCtrlOffset_AdjustAct1)/2].actualValue/10;
//							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].y = SideAControl.step[1].reference;
						}
						else
						{
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].y = sUnitCtrlReg[unitnum].PosAdjustPara[(offset-eUnintCtrlOffset_AdjustAct1)/2].actualValue/10;
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = SideAControl.IavgPosDC;
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = IavgPosDCFilter;
						}
					}
					else
					{
						if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustVoltMode)
						{
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].y = sUnitCtrlReg[unitnum].PosAdjustPara[(offset-eUnintCtrlOffset_AdjustAct1)/2].actualValue;
						
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = SideAControl.VavgPosDC;
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = VavgPosDCFilter;
						}
						else
						{
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = sUnitCtrlReg[unitnum].PosAdjustPara[(offset-eUnintCtrlOffset_AdjustAct1)/2].actualValue/10;
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].y = SideAControl.step[0].CMPSSVALUEPos;
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].y = SideAControl.IavgPosDC;
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct1)/2].y = IavgPosDCFilter;
							AdjustValue2[(offset-eUnintCtrlOffset_AdjustAct1)/2].y = sUnitCtrlReg[unitnum].PosAdjustPara[(offset-eUnintCtrlOffset_AdjustAct1)/2].actualValue/10;
							//AdjustValue2[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = SideAControl.IavgPosDC;
							AdjustValue2[(offset-eUnintCtrlOffset_AdjustAct1)/2].x = IavgPosDCFilter;
						}
					}
					parse = pdPASS;
					break;
				case eUnintCtrlOffset_AdjustSet5:case eUnintCtrlOffset_AdjustSet6:case eUnintCtrlOffset_AdjustSet7:case eUnintCtrlOffset_AdjustSet8:
					if(sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff)
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					parse = SetCurrent(1,0, -(int16_t)(sUnitCtrlReg[unitnum].NegAdjustPara[(offset-eUnintCtrlOffset_AdjustSet5)/2].setValue/10));
					break;
				case eUnintCtrlOffset_AdjustAct5:case eUnintCtrlOffset_AdjustAct6:case eUnintCtrlOffset_AdjustAct7:case eUnintCtrlOffset_AdjustAct8:
					u32tem = ((uint32_t)(sUnitCtrlReg[unitnum].StartAdjust1.all) | ((uint32_t)sUnitCtrlReg[unitnum].StartAdjust2.all << 16));
					if((u32tem & ((uint32_t)1<<((offset-eUnintCtrlOffset_AdjustAct1)/2))) == 0)
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					if(PWMorCMP == cPWMmode)
					{
						if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustVoltMode)
						{
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].y = -(int16_t)sUnitCtrlReg[unitnum].NegAdjustPara[(offset-eUnintCtrlOffset_AdjustAct5)/2].actualValue;
							
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = SideAControl.VavgNegDC;
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = VavgNegDCFilter;
						}						
						else if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustSmallCurrMode)
						{
//							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = -(int16_t)(sUnitCtrlReg[unitnum].NegAdjustPara[(offset-eUnintCtrlOffset_AdjustAct11)/2].actualValue/10);
//							
//							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].y = SideAControl.step[2].reference;						
						}
						else
						{
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].y = sUnitCtrlReg[unitnum].NegAdjustPara[(offset-eUnintCtrlOffset_AdjustAct5)/2].actualValue/10;
							
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+10].x = SideAControl.IavgNegDC;
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = IavgNegDCFilter;
						}
					}
					else
					{
						if(sUnitCtrlReg[unitnum].AdjustMode == eAdjustVoltMode)
						{
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].y = -(int16_t)sUnitCtrlReg[unitnum].NegAdjustPara[(offset-eUnintCtrlOffset_AdjustAct5)/2].actualValue;
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = SideAControl.VavgNegDC;
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = VavgNegDCFilter;
						}
						else
						{
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = sUnitCtrlReg[unitnum].NegAdjustPara[(offset-eUnintCtrlOffset_AdjustAct5)/2].actualValue/10;
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].y = SideAControl.step[0].CMPSSVALUENeg;
							//AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].y = SideAControl.IavgNegDC;
							AdjustValue[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].y = IavgNegDCFilter;
							AdjustValue2[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].y = sUnitCtrlReg[unitnum].NegAdjustPara[(offset-eUnintCtrlOffset_AdjustAct5)/2].actualValue/10;
							//AdjustValue2[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = SideAControl.IavgNegDC;
							AdjustValue2[(offset-eUnintCtrlOffset_AdjustAct5)/2+ADJUST_SECTOR_NUM].x = IavgNegDCFilter;
						}
					}
					parse = pdPASS;
					break;
				
 				
				
				default:
					if(offset < sizeof(sUnitCtrlRegion))
					{
						datacopy(pdata,ptrDir,1);
						parse = pdPASS;
					}
					break;
			}
			break;
		case eHMIUNIT1PARA:
		case eHMIUNIT2PARA:
		case eHMIUNIT3PARA:
		case eHMIUNIT4PARA:
//			unitnum = (region-eHMIUNIT1PARA)/4;
			offset = regaddress-(region*HMI_REGION_LEN);

			if(offset <= eUintParaOffset_SwitchDIPBias)
			{
				ptrDir = (uint16_t *)&sUintEepromPara.OutPutVolt;
				ptrDir += offset - eUintParaOffset_OutPutVoltRatio;
				
				datacopy(pdata,ptrDir,len);
				UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,ptrDir,len);
				
				PosCurrentBias = sUintEepromPara.PosCurrent.Bias/10.0;
				NegCurrentBias = sUintEepromPara.NegCurrent.Bias/10.0;
				PosCurrentBias2 = sUintEepromPara.PosCurrent2.Bias/10.0;
				NegCurrentBias2 = sUintEepromPara.NegCurrent2.Bias/10.0;
				SmallCurrentBias = sUintEepromPara.SmallCurrent.Bias/10.0;
				OutPutVoltBias = sUintEepromPara.OutPutVolt.Bias/10.0;
				BusVolttBias = sUintEepromPara.BusVolt.Bias/10.0;
				AmbientTemperatureBias = sUintEepromPara.AmbientTemperature.Bias/10.0;
				SinkTemperatureBias = sUintEepromPara.SinkTemperature.Bias/10.0;
				SwitchDIPBias = sUintEepromPara.SwitchDIP.Bias/10.0;

				PosCurrentRatio = sUintEepromPara.PosCurrent.Ratio/10000.0;        
				NegCurrentRatio = sUintEepromPara.NegCurrent.Ratio/10000.0;       
				PosCurrentRatio2 = sUintEepromPara.PosCurrent2.Ratio/10000.0;        
				NegCurrentRatio2 = sUintEepromPara.NegCurrent2.Ratio/10000.0;       
				SmallCurrentRatio = sUintEepromPara.SmallCurrent.Ratio/10000.0;       
				OutPutVoltRatio = sUintEepromPara.OutPutVolt.Ratio/10000.0;        
				BusVoltRatio = sUintEepromPara.BusVolt.Ratio/10000.0;            
				AmbientTemperatureRatio = sUintEepromPara.AmbientTemperature.Ratio/10000.0;  
				SinkTemperatureRatio = sUintEepromPara.SinkTemperature.Ratio/10000.0;     
				SwitchDIPRatio = sUintEepromPara.SwitchDIP.Ratio/10000.0;  
			}
			else if(offset <= eUintParaOffset_DisplayMode)
			{				
				Uint16 boardtype = sUintEepromPara.BoardType;

				ptrDir = (uint16_t *)&sUintEepromPara.OutPutVolt;
				ptrDir += offset;
				
				datacopy(pdata,ptrDir,len);
				if(boardtype != sUintEepromPara.BoardType)
				{
					if(sUintEepromPara.BoardType == 0)
					{
						UserClearModuleParamToDefault();
						sUintEepromPara.BoardType = sModuleEepromPara.BoardType;
						sUintEepromPara.BoardAddr = sModuleEepromPara.BoardAddr;
						sUintEepromPara.WorkMode = sModuleEepromPara.WorkMode;
						sUintEepromPara.UnitAddrMin = sModuleEepromPara.UnitAddrMin;
						sUintEepromPara.UnitAddrMax = sModuleEepromPara.UnitAddrMax;
					}
					else
					{
						UserClearUnitParamToDefault();
					}
				}
				else
				{
					if(sUintEepromPara.BoardType == 0)
					{
						sModuleEepromPara.BoardType = sUintEepromPara.BoardType;
						sModuleEepromPara.BoardAddr = sUintEepromPara.BoardAddr;
						sModuleEepromPara.WorkMode = sUintEepromPara.WorkMode;
						sModuleEepromPara.UnitAddrMin = sUintEepromPara.UnitAddrMin;
						sModuleEepromPara.UnitAddrMax = sUintEepromPara.UnitAddrMax;
						offset = &sModuleEepromPara.BoardType - (Uint16*)&sModuleEepromPara;
						UserWriteEeprom(offset*2,(Uint16*)&sModuleParamReg.BoardType,6);
					}
					else
					{
						UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,ptrDir,len);
						if(sUintEepromPara.PwmCmpCtrl)
						{
							PWMorCMP = cCMPmode;
						}
						else
						{
							PWMorCMP = cPWMmode;
						}
					
						MixModeEnable = sUintEepromPara.MixModeEnable;

						DisplayMode = (eDISPLAYMODE)sUintEepromPara.DisplayMode;
					}
				}
			}
			else if(offset <= eUintParaOffset_PID_160K_A2)
			{
				ptrDir = (uint16_t *)&sUintEepromPara.OutPutVolt;
				ptrDir += offset - eUintParaOffset_OutPutVoltRatio;
				
				datacopy(pdata,ptrDir,len);
				UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,ptrDir,len);
				
				UnitPara.ExtSync = (xExtSync)sUintEepromPara.LineSyncEnable;
				UnitPara.PWMSync = (xPWMSync)sUintEepromPara.PWMSyncEnable;

				LoadParameterUpdate();

				Power24Vcheck.Overlevel = sUintEepromPara.Power24VUnderThreshold;
				Power24Vcheck.Overback = sUintEepromPara.Power24VUnderBackThreshold;
				Power24Vcheck.Enable = sUintEepromPara.Power24VUnderEnable;

				OverLoadLevel = sUintEepromPara.OverLoadThreshold;
				OverLoadCheckEnable = sUintEepromPara.OverLoadCheckEnable;

				for (i = 0; i < 22; i++)
				{					
					SideAControl.step[i].A2 = sUintEepromPara.PID[SwitchFreq].A2/10000.0f;
					SideAControl.step[i].A1 = sUintEepromPara.PID[SwitchFreq].A1/10000.0f;
					SideAControl.step[i].A0 = sUintEepromPara.PID[SwitchFreq].A0/10000.0f;
				}
			}
			else if(offset <= eUintParaOffset_SynDelay)
			{
				Uint16 moduletype = sUintEepromPara.Moduletype;

				ptrDir = (uint16_t *)&sUintEepromPara.OutPutVolt;
				ptrDir += offset;
				
				datacopy(pdata,ptrDir,len);

			    UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,ptrDir,len);

			    if(moduletype != sUintEepromPara.Moduletype)
			    {
			    	UserClearUnitParamToDefault();
					datacopy(pdata,ptrDir,len);
			        UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,ptrDir,1);
			        if(sUintEepromPara.Moduletype == 0) //100A/-300A
			        {
                        sUintEepromPara.PosCurrent.Ratio = 4693;
                        sUintEepromPara.PosCurrent.Bias = 0;
                        sUintEepromPara.PosCurrent2.Ratio = 4693;
                        sUintEepromPara.PosCurrent2.Bias = 0;
                        sUintEepromPara.SmallCurrent.Ratio = 24420;
                        sUintEepromPara.SmallCurrent.Bias = -18910;
                        sUintEepromPara.NegCurrent.Ratio = 12034;
                        sUintEepromPara.NegCurrent.Bias = 0;
                        sUintEepromPara.NegCurrent2.Ratio = 12034;
                        sUintEepromPara.NegCurrent2.Bias = 0;

                        sUintEepromPara.LoadVoltPosAvgOVThreshold = 1200;
                        sUintEepromPara.LoadVoltPosAvgOVBackThreshold = 1100;
                        sUintEepromPara.LoadVoltNegAvgOVThreshold = 3600;
                        sUintEepromPara.LoadVoltNegAvgOVBackThreshold = 3400;
                        sUintEepromPara.LoadCurrPosAvgOCThreshold = 1300;
                        sUintEepromPara.LoadCurrPosAvgOCBackThreshold = 1200;
                        sUintEepromPara.LoadCurrNegAvgOCThreshold = 3500;
                        sUintEepromPara.LoadCurrNegAvgOCBackThreshold = 3400;
			        }
			        else if(sUintEepromPara.Moduletype == 1) //150A/-450A
			        {
                        sUintEepromPara.PosCurrent.Ratio = 6238;
                        sUintEepromPara.PosCurrent.Bias = 0;
                        sUintEepromPara.PosCurrent2.Ratio = 6238;
                        sUintEepromPara.PosCurrent2.Bias = 0;
                        sUintEepromPara.SmallCurrent.Ratio = 24420;
                        sUintEepromPara.SmallCurrent.Bias = -18910;
                        sUintEepromPara.NegCurrent.Ratio = 13273;
                        sUintEepromPara.NegCurrent.Bias = 0;
                        sUintEepromPara.NegCurrent2.Ratio = 13273;
                        sUintEepromPara.NegCurrent2.Bias = 0;

                        sUintEepromPara.LoadVoltPosAvgOVThreshold = 1200;
                        sUintEepromPara.LoadVoltPosAvgOVBackThreshold = 1100;
                        sUintEepromPara.LoadVoltNegAvgOVThreshold = 3600;
                        sUintEepromPara.LoadVoltNegAvgOVBackThreshold = 3400;
                        sUintEepromPara.LoadCurrPosAvgOCThreshold = 1800;
                        sUintEepromPara.LoadCurrPosAvgOCBackThreshold = 1700;
                        sUintEepromPara.LoadCurrNegAvgOCThreshold = 4800;
                        sUintEepromPara.LoadCurrNegAvgOCBackThreshold = 4700;
			        }
			        else if(sUintEepromPara.Moduletype == 2) //200A/-600A
                    {
                        sUintEepromPara.PosCurrent.Ratio = 8224;
                        sUintEepromPara.PosCurrent.Bias = 0;
                        sUintEepromPara.PosCurrent2.Ratio = 8224;
                        sUintEepromPara.PosCurrent2.Bias = 0;
                        sUintEepromPara.SmallCurrent.Ratio = 24420;
                        sUintEepromPara.SmallCurrent.Bias = -18910;
                        sUintEepromPara.NegCurrent.Ratio = 21088;
                        sUintEepromPara.NegCurrent.Bias = 0;
                        sUintEepromPara.NegCurrent2.Ratio = 21088;
                        sUintEepromPara.NegCurrent2.Bias = 0;

                        sUintEepromPara.LoadVoltPosAvgOVThreshold = 1200;
                        sUintEepromPara.LoadVoltPosAvgOVBackThreshold = 1100;
                        sUintEepromPara.LoadVoltNegAvgOVThreshold = 3600;
                        sUintEepromPara.LoadVoltNegAvgOVBackThreshold = 3400;
                        sUintEepromPara.LoadCurrPosAvgOCThreshold = 2500;
                        sUintEepromPara.LoadCurrPosAvgOCBackThreshold = 2400;
                        sUintEepromPara.LoadCurrNegAvgOCThreshold = 6600;
                        sUintEepromPara.LoadCurrNegAvgOCBackThreshold = 6500;
                    }
			        else if(sUintEepromPara.Moduletype == 3) //150A/-750A
                    {
                        sUintEepromPara.PosCurrent.Ratio = 6017;
                        sUintEepromPara.PosCurrent.Bias = 0;
                        sUintEepromPara.PosCurrent2.Ratio = 6017;
                        sUintEepromPara.PosCurrent2.Bias = 0;
                        sUintEepromPara.SmallCurrent.Ratio = 24420;
                        sUintEepromPara.SmallCurrent.Bias = -18910;
                        sUintEepromPara.NegCurrent.Ratio = 21490;
                        sUintEepromPara.NegCurrent.Bias = 0;
                        sUintEepromPara.NegCurrent2.Ratio = 21490;
                        sUintEepromPara.NegCurrent2.Bias = 0;

                        sUintEepromPara.LoadVoltPosAvgOVThreshold = 1200;
                        sUintEepromPara.LoadVoltPosAvgOVBackThreshold = 1100;
                        sUintEepromPara.LoadVoltNegAvgOVThreshold = 3600;
                        sUintEepromPara.LoadVoltNegAvgOVBackThreshold = 3400;
                        sUintEepromPara.LoadCurrPosAvgOCThreshold = 1800;
                        sUintEepromPara.LoadCurrPosAvgOCBackThreshold = 1700;
                        sUintEepromPara.LoadCurrNegAvgOCThreshold = 7800;
                        sUintEepromPara.LoadCurrNegAvgOCBackThreshold = 7700;
                    }
			        else if(sUintEepromPara.Moduletype == 4) //50A/-150A
                    {
                        sUintEepromPara.PosCurrent.Ratio = 2828;
                        sUintEepromPara.PosCurrent.Bias = 0;
                        sUintEepromPara.PosCurrent2.Ratio = 2828;
                        sUintEepromPara.PosCurrent2.Bias = 0;
                        sUintEepromPara.SmallCurrent.Ratio = 24420;
                        sUintEepromPara.SmallCurrent.Bias = -18910;
                        sUintEepromPara.NegCurrent.Ratio = 6017;
                        sUintEepromPara.NegCurrent.Bias = 0;
                        sUintEepromPara.NegCurrent2.Ratio = 6017;
                        sUintEepromPara.NegCurrent2.Bias = 0;

                        sUintEepromPara.LoadVoltPosAvgOVThreshold = 1200;
                        sUintEepromPara.LoadVoltPosAvgOVBackThreshold = 1100;
                        sUintEepromPara.LoadVoltNegAvgOVThreshold = 3600;
                        sUintEepromPara.LoadVoltNegAvgOVBackThreshold = 3400;
                        sUintEepromPara.LoadCurrPosAvgOCThreshold = 800;
                        sUintEepromPara.LoadCurrPosAvgOCBackThreshold = 700;
                        sUintEepromPara.LoadCurrNegAvgOCThreshold = 1900;
                        sUintEepromPara.LoadCurrNegAvgOCBackThreshold = 1800;
                    }

			        offset = &sUintEepromPara.PosCurrent.Ratio - (Uint16*)&sUintEepromPara;
			        UserWriteEeprom(offset*2,(Uint16*)&sUintEepromPara.PosCurrent.Ratio,10);

			        offset = &sUintEepromPara.LoadVoltPosAvgOVEnable - (Uint16*)&sUintEepromPara;
			        UserWriteEeprom(offset*2,(Uint16*)&sUintEepromPara.LoadVoltPosAvgOVEnable,12);

					LoadHwCurrPara();
					LoadParameterUpdate();
			    }
				else
				{
					PowerSetValue = sUintEepromPara.PowerSetValue;
					HwOCCountSet = sUintEepromPara.HwOCCountSet;
					HwOCRecoveryTimeSet = sUintEepromPara.HwOCRecoveryTimeSet;
					if(Moduletype == Module_P100A_N300A)
					{
						SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*100/100*10;
						if(SmallCurrModeThreshold < 90)
						{
						    SmallCurrModeThreshold = 90;
						}
					}
					else if(Moduletype == Module_P150A_N450A)
				    {
						SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*150/100*10;
				    }
					else if(Moduletype == Module_P200A_N600A)
				    {
						SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*200/100*10;
						if(SmallCurrModeThreshold < 150)
						{
						    SmallCurrModeThreshold = 150;
						}
				    }
					else if(Moduletype == Module_P150A_N750A)
				    {
						SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*150/100*10;
						if(SmallCurrModeThreshold < 120)
				        {
				            SmallCurrModeThreshold = 120;
				        }
				    }
					else if(Moduletype == Module_P50A_N150A)
				    {
						SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*50/100*10;
				    }
					else
					{
						SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*50/100*10;
					}
					if(SmallCurrModeThreshold >= 450)
						SmallCurrModeThreshold = 450;
					
					CMPmodeSlopePos = (float)sUintEepromPara.CMPmodeSlopePos * 25.0f / 10.0f; //每25us变化的电流（扩大10倍）
				    if(CMPmodeSlopePos > 1250.0f)   //1250/25=50 5A/us
				    {
				        CMPmodeSlopePos = 1250.0f;
				    }
				    else if(CMPmodeSlopePos < 75.0f)//75/25=3 0.3A/us
				    {
				        CMPmodeSlopePos = 75.0f;
				    }

				    CMPmodeSlopeNeg = (float)sUintEepromPara.CMPmodeSlopeNeg * 25.0f / 10.0f; //每25us变化的电流（扩大10倍）
				    if(CMPmodeSlopeNeg > 1250.0f) //1250/25=50 5A/us
				    {
				        CMPmodeSlopeNeg = 1250.0f;
				    }
				    else if(CMPmodeSlopeNeg < 250.0f) //250/25=10 1A/us
				    {
				        CMPmodeSlopeNeg = 250.0f;
				    }

					
					if((PWMorCMP == cCMPmode) || (MixModeEnable == 1))
					{
						CMPmodeSlopeEnable = sUintEepromPara.CMPmodeSlopeEnable;
					}
					else
					{
						CMPmodeSlopeEnable = 0;
					}

					sSyncDelaySet(sUintEepromPara.SynDelay);
				}
			
				parse = pdPASS;
				
			}
			else if(offset <= eUintParaOffset_PWMSyncLockEnable)
			{
				ptrDir = (uint16_t *)&sUintEepromPara.OutPutVolt;
				ptrDir += offset - eUintParaOffset_OutPutVoltRatio;
				
				datacopy(pdata,ptrDir,len);
				UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,ptrDir,len);
				
				AmbientTemp.levelback = sUintEepromPara.AmbientTemplevelback;
				SinkTemp.levelback = sUintEepromPara.SinkTemplevelback;

				BusCheckWarn.highEnable = sUintEepromPara.WarnBusOverCheckEnable;
				BusCheckWarn.highlevel = sUintEepromPara.WarnBusOverThreshold;
				BusCheckWarn.highback = sUintEepromPara.WarnBusOverBackThreshold;
				BusCheckWarn.lowEnable = sUintEepromPara.WarnBusUnderCheckEnable;
				BusCheckWarn.lowlevel = sUintEepromPara.WarnBusUnderThreshold;
				BusCheckWarn.lowback = sUintEepromPara.WarnBusUnderBackThreshold;

				HwOCCheckEnable = sUintEepromPara.HwOCCheckEnable;
				HwOCFaultTime = (Uint32)sUintEepromPara.HwOCFaultTime * 1000000UL / 25;
				HwOCFaultRecoverySet = sUintEepromPara.HwOCFaultRecoverySet;
				UnitPara.PWMSyncLock = (xPWMSync)sUintEepromPara.PWMSyncLockEnable;
			}
			else if(offset <= eUintParaOffset_NegCmpDispCurrAdjustBias4)
			{
				ptrDir = (uint16_t *)sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor;
				ptrDir += offset - eUintParaOffset_PosCurrAdjustRatio1;

				datacopy(pdata,ptrDir,len);
				offset=offset-eUintParaOffset_PosCurrAdjustRatio1;
				UserWriteEeprom(EEPROM_ADJUST_PARA_START+(EEPROM_PARA_OFFSET+offset)*2,(Uint16*)sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor+offset,len*2);

				LoadAdjustPara();
			}
			break;
	}
	
	intfScisend.cmd = cmd;
	intfScisend.regaddress = regaddress;
	intfScisend.pdata = ptrDir;
	intfScisend.len = len;
	return parse;
}

void warnStatusUpdate(uint16_t unitnum)
{
	if(unitnum >= 4)
		return;
	sUnitStatusReg[unitnum].warnInfo1.bit.SinkOT = WarnRegs.TempWarn.bit.SinkOT;
	sUnitStatusReg[unitnum].warnInfo1.bit.AmbientOT = WarnRegs.TempWarn.bit.AmbientOT;
	sUnitStatusReg[unitnum].warnInfo1.bit.AmbientNTCerr = WarnRegs.TempWarn.bit.AmbientNTCerr;
	sUnitStatusReg[unitnum].warnInfo1.bit.SinkNTCerr = WarnRegs.TempWarn.bit.SinkNTCerr;
	sUnitStatusReg[unitnum].warnInfo1.bit.SinkNTCOT = WarnRegs.TempWarn.bit.SinkNTCOT;
	sUnitStatusReg[unitnum].warnInfo1.bit.PWMSyncLoss = WarnRegs.UnitSync.bit.PWMSyncLoss;
	sUnitStatusReg[unitnum].warnInfo1.bit.PWMLockLoss = WarnRegs.UnitSync.bit.PWMLockLoss;
	sUnitStatusReg[unitnum].warnInfo1.bit.LineSyncLoss = WarnRegs.UnitSync.bit.LineSyncLoss;
	sUnitStatusReg[unitnum].warnInfo1.bit.LineLockLoss = WarnRegs.UnitSync.bit.LineLockLoss;
	sUnitStatusReg[unitnum].warnInfo1.bit.BusOver = WarnRegs.UnitSync.bit.BusOver;
	sUnitStatusReg[unitnum].warnInfo1.bit.BusUnder = WarnRegs.UnitSync.bit.BusUnder;
	sUnitStatusReg[unitnum].warnInfo1.bit.LoadPosAvgOV = WarnRegs.UnitSync.bit.LoadPosAvgOV;
	sUnitStatusReg[unitnum].warnInfo1.bit.LoadNegAvgOV = WarnRegs.UnitSync.bit.LoadNegAvgOV;
	sUnitStatusReg[unitnum].warnInfo1.bit.LoadPosAvgOC = WarnRegs.UnitSync.bit.LoadPosAvgOC;
	sUnitStatusReg[unitnum].warnInfo1.bit.LoadNegAvgOC = WarnRegs.UnitSync.bit.LoadNegAvgOC;
	sUnitStatusReg[unitnum].warnInfo1.bit.OverLoad = WarnRegs.TempWarn.bit.OverLoad;
}

void faultStatusUpdate(uint16_t unitnum)
{
	if(unitnum >= 4)
		return;
	sUnitStatusReg[unitnum].faultInfo1.bit.HwLoadOVpos = FaultRegs.HWfaultLoad.bit.HwLoadOVpos;
	sUnitStatusReg[unitnum].faultInfo1.bit.HwLoadOVneg = FaultRegs.HWfaultLoad.bit.HwLoadOVneg;
	sUnitStatusReg[unitnum].faultInfo1.bit.HwLoadOCpos = FaultRegs.HWfaultLoad.bit.HwLoadOCpos;
	sUnitStatusReg[unitnum].faultInfo1.bit.HwLoadOCneg = FaultRegs.HWfaultLoad.bit.HwLoadOCneg;
	sUnitStatusReg[unitnum].faultInfo1.bit.HwExtLoadOC = FaultRegs.HWfaultLoad.bit.HwExtLoadOC;
	sUnitStatusReg[unitnum].faultInfo1.bit.Hw24Vabnormal = WarnRegs.TempWarn.bit.Hw24Vabnormal;//FaultRegs.HWfaultLoad.bit.Hw24Vabnormal;
	sUnitStatusReg[unitnum].faultInfo1.bit.HwLoadAvgOV = FaultRegs.HWfaultLoadAvg.bit.HwLoadAvgOV;
	sUnitStatusReg[unitnum].faultInfo1.bit.HwLoadAvgOC = FaultRegs.HWfaultLoadAvg.bit.HwLoadAvgOC;
	sUnitStatusReg[unitnum].faultInfo1.bit.BusOver = FaultRegs.HWfaultLoadAvg.bit.BusOver;
	sUnitStatusReg[unitnum].faultInfo1.bit.BusUnder = FaultRegs.HWfaultLoadAvg.bit.BusUnder;	
	sUnitStatusReg[unitnum].faultInfo1.bit.AmbientHighOT = FaultRegs.HWfaultTemp.bit.AmbientHighOT;
	sUnitStatusReg[unitnum].faultInfo1.bit.SinkHighOT = FaultRegs.HWfaultTemp.bit.SinkHighOT;
	sUnitStatusReg[unitnum].faultInfo1.bit.PWMSyncLoss = FaultRegs.UnitSync.bit.PWMSyncLoss;
	sUnitStatusReg[unitnum].faultInfo1.bit.PWMLockLoss = FaultRegs.UnitSync.bit.PWMLockLoss;
	sUnitStatusReg[unitnum].faultInfo1.bit.LineSyncLoss = FaultRegs.UnitSync.bit.LineSyncLoss;
	sUnitStatusReg[unitnum].faultInfo1.bit.LineLockLoss = FaultRegs.UnitSync.bit.LineLockLoss;
	sUnitStatusReg[unitnum].faultInfo2.bit.EepromRead = FaultRegs.HwDriver.bit.EepromRead;
	sUnitStatusReg[unitnum].faultInfo2.bit.EepromWrite = FaultRegs.HwDriver.bit.EepromWrite;
	#if ACDC_BOARD_TEST
//	if((unitnum == 1)||(unitnum == 3))
	{
		sUnitStatusReg[unitnum].faultInfo2.bit.communicationFault = acdcWdResp.communicationFault?1:0;
		sUnitStatusReg[unitnum].faultInfo2.bit.OC = (acdcWdResp.localMachineFault==1)?1:0;
		sUnitStatusReg[unitnum].faultInfo2.bit.Err = (acdcWdResp.localMachineFault==2)?1:0;
		sUnitStatusReg[unitnum].faultInfo2.bit.OV = (acdcWdResp.localMachineFault==3)?1:0;
		sUnitStatusReg[unitnum].faultInfo2.bit.PhaseDef = (acdcWdResp.localMachineFault==4)?1:0;
	}
	#endif
}

void DisplayResultUpdate(uint16_t unitNum)
{
	uint16_t i;
	
	if(DisplayMode == eDisplayMode_EXHIBITION)
	{
		if(sUnitCtrlReg[unitNum].Ctrl1.bit.OnOff)
		{
			if(Ctrldata1.Ctrl2.bit.loopMode == 1)
			{
				if(PosOrNegCurrentA)
				{
					sUnitStatusReg[unitNum].LoadPosAvgC = PosCurrent;
					sUnitStatusReg[unitNum].LoadNegAvgC = 0;
				}
				else
				{
					sUnitStatusReg[unitNum].LoadPosAvgC = 0;
					sUnitStatusReg[unitNum].LoadNegAvgC = NegCurrent;
				}
			}
			else if((sUnitStatusReg[unitNum].workStatus.bit.OutMode == 0) && (PulsePhaseNum <= 2))
			{
				if((sUnitCtrlReg[unitNum].SectASet[0].Current > 0) && (sUnitCtrlReg[unitNum].SectASet[1].Current < 0))
				{
					sUnitStatusReg[unitNum].LoadPosAvgC = sUnitCtrlReg[unitNum].SectASet[0].Current;
					sUnitStatusReg[unitNum].LoadNegAvgC = -sUnitCtrlReg[unitNum].SectASet[1].Current;
				}
				else if((sUnitCtrlReg[unitNum].SectASet[0].Current < 0) && (sUnitCtrlReg[unitNum].SectASet[1].Current > 0))
				{
					sUnitStatusReg[unitNum].LoadPosAvgC = sUnitCtrlReg[unitNum].SectASet[1].Current;
					sUnitStatusReg[unitNum].LoadNegAvgC = -sUnitCtrlReg[unitNum].SectASet[0].Current;
				}
				else
				{
					if(sUnitCtrlReg[unitNum].SectASet[0].Current < 0)
					{
						sUnitStatusReg[unitNum].LoadPosAvgC = 0;
						sUnitStatusReg[unitNum].LoadNegAvgC = ((-sUnitCtrlReg[unitNum].SectASet[0].Current)*sUnitCtrlReg[unitNum].SectASet[0].PulseWide \
															+ (-sUnitCtrlReg[unitNum].SectASet[1].Current)*sUnitCtrlReg[unitNum].SectASet[1].PulseWide)	\
															/ (sUnitCtrlReg[unitNum].SectASet[0].PulseWide + sUnitCtrlReg[unitNum].SectASet[1].PulseWide);
					}
					else
					{
						sUnitStatusReg[unitNum].LoadPosAvgC = (sUnitCtrlReg[unitNum].SectASet[0].Current*sUnitCtrlReg[unitNum].SectASet[0].PulseWide \
															+ sUnitCtrlReg[unitNum].SectASet[1].Current*sUnitCtrlReg[unitNum].SectASet[1].PulseWide)	\
															/ (sUnitCtrlReg[unitNum].SectASet[0].PulseWide + sUnitCtrlReg[unitNum].SectASet[1].PulseWide);
						sUnitStatusReg[unitNum].LoadNegAvgC = 0;
					}
//					sUnitStatusReg[unitNum].LoadPosAvgC = SideAControl.IavgPos;
//					sUnitStatusReg[unitNum].LoadNegAvgC = SideAControl.IavgNeg;
				}
				
				sUnitStatusReg[unitNum].LoadPosAvgV = SideAControl.VavgPos;
				sUnitStatusReg[unitNum].LoadNegAvgV = SideAControl.VavgNeg;
			}
			else if((sUnitStatusReg[unitNum].workStatus.bit.OutMode == 0) && (PulsePhaseNum > 2))
			{
				sUnitStatusReg[unitNum].LoadPosAvgC = SideAControl.IavgPos;
				sUnitStatusReg[unitNum].LoadNegAvgC = SideAControl.IavgNeg;
				sUnitStatusReg[unitNum].LoadPosAvgV = SideAControl.VavgPos;
				sUnitStatusReg[unitNum].LoadNegAvgV = SideAControl.VavgNeg;
			}
			else if(sUnitStatusReg[unitNum].workStatus.bit.OutMode == 1)
			{
				if(sUnitCtrlReg[unitNum].SectASet[0].Current > 0)
				{
					sUnitStatusReg[unitNum].LoadPosAvgC = sUnitCtrlReg[unitNum].SectASet[0].Current;
					sUnitStatusReg[unitNum].LoadNegAvgC = 0;
				}
				else
				{
					sUnitStatusReg[unitNum].LoadPosAvgC = 0;
					sUnitStatusReg[unitNum].LoadNegAvgC = sUnitCtrlReg[unitNum].SectASet[0].Current;
				}
				sUnitStatusReg[unitNum].LoadPosAvgV = SideAControl.VavgPosDC;
				sUnitStatusReg[unitNum].LoadNegAvgV = SideAControl.VavgNegDC;
			}
			else 
			{
				
			}
		}
		else
		{
			sUnitStatusReg[unitNum].LoadPosAvgC = 0;
			sUnitStatusReg[unitNum].LoadNegAvgC = 0;
			sUnitStatusReg[unitNum].LoadPosAvgV = 0;
			sUnitStatusReg[unitNum].LoadNegAvgV = 0;
		}
	}
	else
	{
		if(Ctrldata1.Ctrl2.bit.loopMode == 1)
		{
			if(PosOrNegCurrentA)
			{
				sUnitStatusReg[unitNum].LoadPosAvgC = PosCurrent;
				sUnitStatusReg[unitNum].LoadNegAvgC = 0;
			}
			else
			{
				sUnitStatusReg[unitNum].LoadPosAvgC = 0;
				sUnitStatusReg[unitNum].LoadNegAvgC = NegCurrent;
			}
		}
		else if(sUnitStatusReg[unitNum].workStatus.bit.OutMode == 0)
		{
			//sUnitStatusReg[unitNum].LoadPosAvgC = SideAControl.IavgPos;
			//sUnitStatusReg[unitNum].LoadNegAvgC = SideAControl.IavgNeg;
			//sUnitStatusReg[unitNum].LoadPosAvgV = SideAControl.VavgPos;
			//sUnitStatusReg[unitNum].LoadNegAvgV = SideAControl.VavgNeg;

            sUnitStatusReg[unitNum].LoadPosAvgC = IavgPosFilterDisPlay;
            sUnitStatusReg[unitNum].LoadNegAvgC = IavgNegFilterDisPlay;
            sUnitStatusReg[unitNum].LoadPosAvgV = (int16_t)(VavgPosFilter/10.0f);
            sUnitStatusReg[unitNum].LoadNegAvgV = (int16_t)(VavgNegFilter/10.0f);
		}
		else
		{
			//sUnitStatusReg[unitNum].LoadPosAvgC = SideAControl.IavgPosDC;
			//sUnitStatusReg[unitNum].LoadNegAvgC = SideAControl.IavgNegDC;
			//sUnitStatusReg[unitNum].LoadPosAvgV = SideAControl.VavgPosDC;
			//sUnitStatusReg[unitNum].LoadNegAvgV = SideAControl.VavgNegDC;

            sUnitStatusReg[unitNum].LoadPosAvgC = IavgPosDCFilterDisPlay;
            sUnitStatusReg[unitNum].LoadNegAvgC = IavgNegDCFilterDisPlay;
            sUnitStatusReg[unitNum].LoadPosAvgV = (uint16_t)(fabsf(VavgPosDCFilter/10.0f));
            sUnitStatusReg[unitNum].LoadNegAvgV = (uint16_t)(fabsf(VavgNegDCFilter/10.0f));
		}

        for(i=0;i<20;i++)
        {
            sUnitStatusReg[unitNum].SectorAvgCurr[i] = IavgPhaseFilter[i+1];
        }

		sUnitStatusReg[unitNum].SinkTemp = (uint16_t)(SinkDegreeC/10.0f);
	}
}
BaseType_t IntfReadParse(uint16_t regaddress, uint16_t len)
{
	BaseType_t  parse = pdFAIL;
	eHMIRegion region = (eHMIRegion)((regaddress+len-1)/HMI_REGION_LEN);
	uint16_t offset;
	uint16_t *ptrDir;
	uint16_t unitNum;
//	uint32_t temp;
//	if(0x2000 == regaddress)
//	{
//		parse = pdPASS;
//		intfData[0] = ulTaskGetIdleRunTimePercent();
//		temp = ulTaskGetIdleRunTimeCounter();
//		intfData[1] = (uint16_t)(temp>>16);
//		intfData[2] = (uint16_t)(temp);
//		ptrDir = (uint16_t *)&intfData;
//	}
	switch(region)
	{
		case eHMIMODULECTRL:
//			parse = pdPASS;
//			ptrDir = (uint16_t *)&Ctrldata1;
//			ptrDir += regaddress;
			break;
		case eHMIMODULESTATUS:
//			parse = pdPASS;
//			sModuleStatusReg.workStatus.all = Ctrldata1.Ctrl1.all;
//			sModuleStatusReg.workStatus.bit.Online = 1;
//			ptrDir = (uint16_t *)&sModuleStatusReg;
//			ptrDir += regaddress-HMI_REG_MODULE_STATUS;
			break;
		case eHMIMODULEREMOTE:
			break;
		case eHMIMODULEPARA:
			break;
		case eHMIUNIT1CTRL:
		case eHMIUNIT2CTRL:
		case eHMIUNIT3CTRL:
		case eHMIUNIT4CTRL:
			parse = pdPASS;
			ptrDir = (uint16_t *)&sUnitCtrlReg[(region-eHMIUNIT1CTRL)/4];
			ptrDir += regaddress-(region*HMI_REGION_LEN);
			break;
		case eHMIUNIT1STATUS:
		case eHMIUNIT2STATUS:
		case eHMIUNIT3STATUS:
		case eHMIUNIT4STATUS:
			parse = pdPASS;
			unitNum = (region-eHMIUNIT1STATUS)>>2;	//(region-eHMIUNIT1STATUS)/4

			
			sUnitStatusReg[unitNum].workStatus.bit.OnOff = (GetPWMstatus() == PWMrunSts)?1:0;
			sUnitStatusReg[unitNum].workStatus.bit.OutMode = (WaveForm == DC)?1:0;
			sUnitStatusReg[unitNum].workStatus.bit.Online = 1;
			

			warnStatusUpdate(unitNum);
			faultStatusUpdate(unitNum);
			DisplayResultUpdate(unitNum);
			
			sUnitStatusReg[unitNum].PowerVolt = acdcWdResp.actualVoltage*10;
			sUnitStatusReg[unitNum].PowerCurr = acdcWdResp.actualCurrent*10;
			sUnitStatusReg[unitNum].BusVolt = (uint16_t)(VBus.AverageValue*100);
			
			sUnitStatusReg[unitNum].ver[0] = APP_BVER;
			sUnitStatusReg[unitNum].ver[1] = APP_MVER;
			sUnitStatusReg[unitNum].ver[2] = APP_SVER;
			ptrDir = (uint16_t *)&sUnitStatusReg[unitNum];
			ptrDir += regaddress-(region*HMI_REGION_LEN);
			break;
		case eHMIUNIT1PARA:
		case eHMIUNIT2PARA:
		case eHMIUNIT3PARA:
		case eHMIUNIT4PARA:
			parse = pdPASS;
			unitNum = (region-eHMIUNIT1PARA)/4;
			offset = regaddress-(region*HMI_REGION_LEN);
			
			if(offset <= eUintParaOffset_resd)
			{
				ptrDir = (uint16_t *)&sUintEepromPara.OutPutVolt;
				ptrDir += offset;
			}
			else if(offset <= eUintParaOffset_NegCmpDispCurrAdjustBias4)
			{
				ptrDir = (uint16_t *)sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor;
				ptrDir += offset - eUintParaOffset_PosCurrAdjustRatio1;
			}
			break;
		
	}
	intfScisend.cmd = CMD_READ;
	intfScisend.len = len;
	intfScisend.regaddress = regaddress;
	intfScisend.pdata = ptrDir;
	return parse;
}
void sSynRemoteStatusUpdate(uint16_t unitnum)
{
	sSynRemoteStatusRegion *pSynRemoteStatusReg = &sSynRemoteStatusReg[unitnum];
	
	pSynRemoteStatusReg->status1.bit.OnOff = sUnitStatusReg[unitnum].workStatus.bit.OnOff;
	pSynRemoteStatusReg->status1.bit.OutMode = sUnitStatusReg[unitnum].workStatus.bit.OutMode;
	pSynRemoteStatusReg->status1.bit.local = 0;
	pSynRemoteStatusReg->status1.bit.remote = 0;
	pSynRemoteStatusReg->status1.bit.AutoManual = Ctrldata1.Ctrl1.bit.LocalRemote;
	pSynRemoteStatusReg->status2.bit.HwExtLoadOC = 0;//sUnitStatusReg[unitnum].faultInfo1.bit.HwExtLoadOC;
	pSynRemoteStatusReg->status2.bit.OverLoad = sUnitStatusReg[unitnum].warnInfo1.bit.OverLoad;
	if(sUnitStatusReg[unitnum].faultInfo1.bit.SinkHighOT)// || sUnitStatusReg[unitnum].warnInfo1.bit.SinkNTCOT)
		pSynRemoteStatusReg->status2.bit.SinkOT = 1;
	else
		pSynRemoteStatusReg->status2.bit.SinkOT = 0;
	if(sUnitStatusReg[unitnum].faultInfo1.bit.AmbientHighOT)// || sUnitStatusReg[unitnum].warnInfo1.bit.AmbientOT)
		pSynRemoteStatusReg->status2.bit.AmbientOT = 1;
	else
		pSynRemoteStatusReg->status2.bit.AmbientOT = 0;
	
//	if(sUnitStatusReg[unitnum].warnInfo1.bit.PWMSyncLoss || sUnitStatusReg[unitnum].warnInfo1.bit.PWMLockLoss 
//	|| sUnitStatusReg[unitnum].warnInfo1.bit.LineLockLoss || sUnitStatusReg[unitnum].warnInfo1.bit.LineSyncLoss
//	|| sUnitStatusReg[unitnum].faultInfo2.bit.EepromRead || sUnitStatusReg[unitnum].faultInfo2.bit.EepromWrite)
//		pSynRemoteStatusReg->status2.bit.PWMSyncLoss = 1;
//	else
		pSynRemoteStatusReg->status2.bit.PWMSyncLoss = 0;

//	if(sUnitStatusReg[unitnum].faultInfo1.bit.BusOver || sUnitStatusReg[unitnum].warnInfo1.bit.BusOver)
//		pSynRemoteStatusReg->status2.bit.BusOver = 1;
//	else
		pSynRemoteStatusReg->status2.bit.BusOver = 0;	//母线过压不上报
	
//	if(sUnitStatusReg[unitnum].faultInfo1.bit.BusUnder || sUnitStatusReg[unitnum].warnInfo1.bit.BusUnder)
//		pSynRemoteStatusReg->status2.bit.BusUnder = 1;
//	else
		pSynRemoteStatusReg->status2.bit.BusUnder = 0;	//母线欠压不上报
	pSynRemoteStatusReg->status2.bit.LoadPosAvgOV = sUnitStatusReg[unitnum].warnInfo1.bit.LoadPosAvgOV;
	pSynRemoteStatusReg->status2.bit.LoadNegAvgOV = sUnitStatusReg[unitnum].warnInfo1.bit.LoadNegAvgOV;
	pSynRemoteStatusReg->status2.bit.LoadPosAvgOC = sUnitStatusReg[unitnum].warnInfo1.bit.LoadPosAvgOC;
	pSynRemoteStatusReg->status2.bit.LoadNegAvgOC = sUnitStatusReg[unitnum].warnInfo1.bit.LoadNegAvgOC;
	pSynRemoteStatusReg->status2.bit.Hw24Vabnormal = 0;//sUnitStatusReg[unitnum].faultInfo1.bit.Hw24Vabnormal;
	
	#if ACDC_BOARD_TEST

	if(sUnitStatusReg[unitnum].faultInfo2.all)
		pSynRemoteStatusReg->status2.bit.communicationFault = 1;
	else
		pSynRemoteStatusReg->status2.bit.communicationFault = 0;

	#endif

	pSynRemoteStatusReg->LoadPosAvgC = sUnitStatusReg[unitnum].LoadPosAvgC;
	pSynRemoteStatusReg->LoadNegAvgC = sUnitStatusReg[unitnum].LoadNegAvgC;
	pSynRemoteStatusReg->LoadPosAvgV = sUnitStatusReg[unitnum].LoadPosAvgV;
	pSynRemoteStatusReg->LoadNegAvgV = sUnitStatusReg[unitnum].LoadNegAvgV;
}
BaseType_t IntfRemoteRWMultParse(Cmdstruct *incmd)
{
	BaseType_t  parse = pdFAIL;
	uint16_t *ptrDir;
//	uint16_t tem;
	uint16_t unitnum = Get485Addr() - SlaveStartAddr;

	uint16_t readlen = incmd->len;
	uint16_t writelen = incmd->len2;
	uint16_t *pdata = incmd->pdata;
	uint16_t i,writeflag = 0;

//	sSynRemoteStatusRegion *pSynRemoteStatusReg = &sSynRemoteStatusReg[unitnum];

	if(MachineWorkMode == 0)
	{
		if(Ctrldata1.Ctrl1.bit.LocalRemote == 0)
		{
			writeflag = 1;
//			if(pUnitCtrlReg->Ctrl1.bit.OnOff == 1)
//			{
//				datacopy(pdata,&tem,1);
//				if(tem & (1<<unitnum) != 0)	//启动后非关机指令不响应写
//				{
//					writeflag = 0;
//				}
//			}
			if(writeflag)
			{
				ptrDir = (uint16_t *)&sSynRemoteUnitCtrlReg[unitnum];

				datacopy(pdata,ptrDir,writelen);
				
//				pUnitCtrlReg->Ctrl1.bit.LocalRemote = 0;
//				if(pUnitCtrlReg->Ctrl1.bit.OnOff == 0)
//				{
//					pUnitCtrlReg->Ctrl1.bit.OutMode = sSynRemoteUnitCtrlReg[unitnum].Ctrl.bit.OutMode;
//				}
//				if(sSynRemoteUnitCtrlReg[unitnum].Ctrl.bit.OnOff & (1<<unitnum))
//					pUnitCtrlReg->Ctrl1.bit.OnOff = 1;
//				else
//					pUnitCtrlReg->Ctrl1.bit.OnOff = 0;
//				
//				pUnitStatusReg->workStatus.bit.OnOff = pUnitCtrlReg->Ctrl1.bit.OnOff;
//				pUnitStatusReg->workStatus.bit.OutMode = pUnitCtrlReg->Ctrl1.bit.OutMode;

				if(sSynRemoteUnitCtrlReg[unitnum].Ctrl.bit.OutMode)
					WaveForm = DC;
				else
					WaveForm = Pulse;

				ModulePara.Mode = Parallel;
				UnitPara.Mode = Parallel;
				
				for(i=0; i<SECTOR_MAX; i++)
				{
					if(sSynRemoteUnitCtrlReg[unitnum].Ctrl.bit.OutMode)
					{
						SetCurrent(1,i,sSynRemoteUnitCtrlReg[unitnum].SectASet[i].Current);
						break;
					}
					else
					{
						SetCurrent(sSynRemoteUnitCtrlReg[unitnum].Ctrl.bit.OutMode,i,sSynRemoteUnitCtrlReg[unitnum].SectASet[i].Current);
					}
				}
				sSetPulseWideFromRemote();
				//移相
				if(sSynRemoteUnitCtrlReg[unitnum].SynPhase >= 180)
					sSynRemoteUnitCtrlReg[unitnum].SynPhase = 180;
				SyncPhase = sSynRemoteUnitCtrlReg[unitnum].SynPhase;
				sSyncPhaseShiftSet((SyncPhase%360)*UnitPara.SumOfExtSyncPoint/4/360);
				UnitPara.SlaveExtSyncStartPoint = 0;
				
				if(sSynRemoteUnitCtrlReg[unitnum].Ctrl.bit.OnOff & (1<<unitnum))
				{
					SetUnitWorkStatus(eMODULE_RUN);
//					OSEventSend(cPrioSuper,eSuperTurnOn);
					OSEventSend(cPrioCan,eCanPowerOn);
				}
				else
				{
					OSEventSend(cPrioSuper,eSuperTurnOff);
					OSEventSend(cPrioCan,eCanPowerOff);
					SetUnitWorkStatus(eMODULE_STOP);
				}
			}
		}
		warnStatusUpdate(unitnum);
		faultStatusUpdate(unitnum);
		DisplayResultUpdate(unitnum);
		sSynRemoteStatusUpdate(unitnum);
	}
	parse = pdPASS;

	
	intfScisend.cmd = CMD_READWRITE_MULT;
	intfScisend.len = readlen;
	intfScisend.address = incmd->address;
	intfScisend.pdata = (uint16_t*)&sSynRemoteStatusReg[unitnum];
	return parse;
}

BaseType_t IntfRemoteStopCmdParse(uint16_t len,uint16_t *pdata)
{
	BaseType_t  parse = pdFAIL;

	uint16_t tem;
	
	datacopy(pdata,&tem,1);
	
	if(tem & (1<<(Get485Addr()-SlaveStartAddr)) == 0)
	{
		OSEventSend(cPrioSuper,eSuperTurnOff);
		SetUnitWorkStatus(eMODULE_STOP);
		sUnitCtrlReg[Get485Addr()-SlaveStartAddr].Ctrl1.bit.OnOff = 0;
		sUnitStatusReg[Get485Addr()-SlaveStartAddr].workStatus.bit.OnOff = 0;
	}
	

	return parse;
}

void IntfSciRecvNormal(Cmdstruct *incmd,Cmdstruct *outcmd)
{
  BaseType_t  parse = pdFAIL;
 // incmd->used = pdFAIL;
  outcmd->used = pdFAIL;
  switch(incmd->cmd)
  {
    case CMD_WRITE:
      parse = IntfWriteParse(CMD_WRITE,incmd->regaddress,incmd->len,incmd->pdata);
      break;
    case CMD_WRITE_ONE:
      parse = IntfWriteParse(CMD_WRITE_ONE,incmd->regaddress,1,incmd->pdata);
      break;
    case CMD_READ:
      parse = IntfReadParse(incmd->regaddress,incmd->len);
      break;
	case CMD_STOP:
	  parse = IntfRemoteStopCmdParse(incmd->len,incmd->pdata);
	  break;
	case CMD_READWRITE_MULT:
	  parse = IntfRemoteRWMultParse(incmd);
	  break;
  }
  if((pdFAIL != parse)&&(BADDRESS != incmd->address))
  {
    *outcmd = intfScisend;
    outcmd->used = pdTRUE;
  }
}

void IntfSciRecvError(Cmdstruct *outcmd)
{
	outcmd->used = pdFAIL;
}

/*********************************************************************
Function name:  InterfaceTaskInit
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InterfaceTaskInit(void)
{
    sciframeinit(INTERFACESCI, Get485Addr(), INTERNALISSLAVE,GetBoardType(),
                 IntfSciRecvNormal, IntfSciRecvError,120*250);
}

/*********************************************************************
Function name:  InterfaceTask
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InterfaceTask(void * pvParameters)
{
    uint32_t event;
    //BaseType_t err;

    //sciframeinit(INTERFACESCI, Get485Addr(), INTERNALISHOST,GetBoardType(),
                 //IntfSciRecvNormal, IntfSciRecvError);
    //while (pdTRUE)
    {
        //err = xSemaphoreTake(xSemaphoreInterface, (TickType_t)Tick4mS);
        event = OSEventPend(cPrioInterface);
        if(event != 0)
        {

            if (OSEventConfirm(event, eInterfaceTest))
            {

            }

        }
        else    //eInterfaceTimer
        {
            SciGroupManage(INTERFACESCI);	
			if((scigroup[INTERFACESCI].recTimeout)&&(GetPWMstatus() == PWMrunSts))
			{
				OSEventSend(cPrioSuper,eSuperTurnOff);
			}
        }
    }
}



