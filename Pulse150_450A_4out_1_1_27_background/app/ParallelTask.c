/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : ParallelTask.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define ParallelTask_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "SuperTask.h"
#include "LoadTask.h"
#include "Interrupt.h"
#include "CanTask.h"
#include "InterfaceTask.h"
#include "EEpromTask.h"
#include "ParallelTask.h"
#include "HMITask.h"
#include "Initial.h"
#include "OsConfig.h"
#include "fault.h"
#include "SyncProc.h"


/********************************************************************************
*const define                               *
********************************************************************************/
const stXparallel UnitParaInit = {	xExtSyncEn,             /*xExtSync ExtSync*/
									xPWMSyncEn,             /*xPWMSync PWMSync*/
									Parallel,				/*xMode	 Mode*/	
									InvalidRole,			/*CBrole Role*/
									0,						/*int ExtSyncStartPoint*/
									1,						/*int ExtSyncSignal*/
									1,						/*int PWMSyncSignal*/
									0,						/*int PWMSyncLockLoss*/
									0,						/*int ExtSyncSignalLoss*/
									0,						/*int PWMSyncSignalLoss*/
									0,						/*int AllSyncSignalLoss*/
									0,						/*Uint32 ExtSynclossCnt*/
									0,						/*Uint32 ExtSyncbackCnt*/
									cExtSynclossCntSetting,	/*Uint32 ExtSynclossCntSetting*/
									cExtSyncbackCntSetting,	/*Uint32 ExtSyncbackCntSetting*/
									0,						/*Uint32 PWMSynclossCnt*/
									0,						/*Uint32 PWMSyncbackCnt*/
									cPWMSynclossCntSetting,	/*Uint32 PWMSynclossCntSetting*/
									cPWMSyncbackCntSetting,	/*Uint32 PWMSyncbackCntSetting*/	
									0,						/*int32 PhaseShiftTime*/
									0,						/*Uint32	PWMSyncLockCnt*/
									cPWMSyncLockCntSetting,	/*Uint32 PWMSyncLockCntSetting*/
									0,						/*int32 MasterExtSyncPoint*/
									0,						/*int32 MasterExtSyncStartPoint*/
									0,						/*int32 MasterExtSyncDeltaPoint*/
									0,						/*int MasterExtSyncFlag*/
									0,						/*int32 SlaveExtSyncPoint*/
									0,						/*int32 SlaveExtSyncStartPoint*/
									0,						/*int32 SlaveExtSyncDeltaPoint*/
									0,						/*int SlaveExtSyncFlag*/
									0,						/*int32 SumOfExtSyncPoint*/
									0,						/*Uint32 pwmsyncRange*/
									cControlperiod,				/*Uint32 pwmperiod*/	
									0,						/*Uint32 pwmsyncCTR*/
									cControlperiod				/*Uint32 pwmnewPRD*/
								};



/********************************************************************************
* Variable declaration                              *
********************************************************************************/





/********************************************************************************
* function declaration                              *
********************************************************************************/
/*********************************************************************
Function name:  ParallelTaskInit
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void ParallelTaskInit(void)
{
    //add Init code
}

/*********************************************************************
Function name:  ParallelTask
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void ParallelTask(void * pvParameters)
{
   uint32_t event;
   //BaseType_t err;


    //while(pdTRUE)
    {

        //err = xSemaphoreTake( xSemaphoreParallel, (TickType_t)Tick4mS );
        event = OSEventPend(cPrioParallel);
        if(event != 0)
        {

            if(OSEventConfirm(event,eParallelTest))
            {

            }


        }
        else//eParallelTimer
        {

        	if((GetBoardType() == ModuleCB) || (GetBoardType() == CabinetCB))
        	{
        		sSyncStateIdle();
        		SyncCheck(&ModulePara); 
//        		if(GetPWMstatus() == PWMwaitSts)
//        		{
//					BoardType = ModuleCB;
//					PieCtrlRegs.PIEIER4.bit.INTx2 = 1;//Enable PIE group 4 interrupt 2 for CAP2,Line SYNC
//        		}
//				else
//        		{
					if(ModulePara.ExtSyncSignalLoss == pdTRUE)
					{
						BoardType = CabinetCB;
						
//						CanSimulateSendStart(RemoteCommAddr,ModulePara.SumOfExtSyncPoint*25);
//						PieCtrlRegs.PIEIER4.bit.INTx2 = 0;//DISable PIE group 4 interrupt 2 for CAP2,Line SYNC
					}
//					else
//					{
//						if(abs(ModuleSyncPeriod-ModulePara.SumOfExtSyncPoint*25) < cExtSynThreshold)
//						{
//							BoardType = ModuleCB;
//							PieCtrlRegs.PIEIER4.bit.INTx2 = 1;//Enable PIE group 4 interrupt 2 for CAP2,Line SYNC
//						}
//						else
//						{
//							BoardType = CabinetCB;
//							PieCtrlRegs.PIEIER4.bit.INTx2 = 0;//DISable PIE group 4 interrupt 2 for CAP2,Line SYNC
//						}
//					}
//				}
        	}
			if(GetBoardType() == UnitCB)
        	{
        		UnitPara.PWMSyncSignal = 1;
				
        		UnitExtSyncSignalLoss();
	        	if(GetUnitPWMSyncSignalLoss() == pdTRUE)
	        	{
	        		//action
	        		
	        		
	        	}
				if(GetUnitExtSyncSignalLoss() == pdTRUE)
				{
					//action
				}
				
			}
			DataInput();
			

        	
			

        }
    }
}
/*********************************************************************
Function name:  CB_ROLE GetModuleRole(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
CBrole GetModuleRole(void)
{
	return(ModulePara.Role);
}
/*********************************************************************
Function name:  CB_ROLE GetUnitRole(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
CBrole GetUnitRole(void)
{
	return(UnitPara.Role);
}

/*********************************************************************
Function name: xMode GetModuleMode(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
xMode GetModuleMode(void)
{
	return(ModulePara.Mode);
}
/*********************************************************************
Function name: xMode GeUnitMode(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
xMode GetUnitMode(void)
{
	return(UnitPara.Mode);
}
/*********************************************************************

Function name:  int GetModuleExtSyncStartPoint(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
int GetModuleExtSyncStartPoint(void)
{
	return(ModulePara.ExtSyncStartPoint);

}
/*********************************************************************

Function name:  int GetModuleExtSyncStartPoint(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
int GetUnitExtSyncStartPoint(void)
{
	return(UnitPara.ExtSyncStartPoint);

}

/*********************************************************************
Function name: void ClrModuleExtSyncStartPoint(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void ClrModuleExtSyncStartPoint(void)
{
	ModulePara.ExtSyncStartPoint = 0;

}
/*********************************************************************
Function name: void ClrModuleExtSyncStartPoint(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void ClrUnitExtSyncStartPoint(void)
{
	UnitPara.ExtSyncStartPoint = 0;

}

/*********************************************************************
Function name: void ExtSynSignalSetting(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void ExtSynSignalSetting(void)
{

//	if(UnitPara.SlaveExtSyncStartPoint >= UnitPara.SumOfExtSyncPoint)
//	{
//		UnitPara.SlaveExtSyncStartPoint = UnitPara.SumOfExtSyncPoint;
//	}

	if(GetModuleMode() == Parallel)
	{	
		if(GetBoardType() == UnitCB) 
		{
			if(GetUnitExtSyncStartPoint() == 1)
			{
				UnitPara.SlaveExtSyncDeltaPoint = 0;
				ClrUnitExtSyncStartPoint();
				
//				if(GetUnitWorkStatus() == eMODULE_RUN)
//				{
//					UnitPara.SlaveExtSyncDeltaPoint = 0;
//				
//					if(GetWaveForm() != DC)
//					{
//						StepTimecountClr();
//					}
//					SetSideALargePWMstatus(PWMrunSts);
//					ClrUnitExtSyncStartPoint();
//				}
//				else
//				{
//					UnitPara.SlaveExtSyncDeltaPoint = 0;
//				
//					if(GetWaveForm() != DC)
//					{
//						StepTimecountClr();
//					}
//					SetSideALargePWMstatus(PWMwaitSts);
//					ClrUnitExtSyncStartPoint();	
//					
//				}
			}
			else
			{}

		
			UnitPara.SlaveExtSyncDeltaPoint++;
			UnitPara.SlaveExtSyncPoint = UnitPara.SlaveExtSyncDeltaPoint + UnitPara.SlaveExtSyncStartPoint;
			//UnitPara.SlaveExtSyncPoint = UnitPara.SlaveExtSyncDeltaPoint;// + UnitPara.SlaveExtSyncStartPoint;
			if(UnitPara.SlaveExtSyncPoint >(UnitPara.SumOfExtSyncPoint))
			{
				UnitPara.SlaveExtSyncPoint = 0;
				UnitPara.SlaveExtSyncDeltaPoint = 0;
				UnitPara.SlaveExtSyncFlag = 1;

//				if(GetWaveForm() != DC)
//				{
//					StepTimecountClr();
//				}
				
			}
			else
			{
				
			}		
//			if(UnitPara.SlaveExtSyncPoint > (UnitPara.SumOfExtSyncPoint >>3))
//			{

//			}
//			else 
//			{

				
//			}
//			if(UnitPara.SlaveExtSyncPoint <= 5)
//			{

//			}
//			if(UnitPara.SlaveExtSyncPoint == (UnitPara.SumOfExtSyncPoint >> 1))
//			{


//			}

		}
		else
		{
				UnitPara.SlaveExtSyncPoint = 0;
				UnitPara.SlaveExtSyncDeltaPoint = 0;
				UnitPara.SlaveExtSyncFlag = 0;	
		}
		
		if(GetModuleRole() == MasterCB)
		{

			//ModulePara.MasterExtSyncStartPoint = 0;
			ModulePara.MasterExtSyncDeltaPoint++;
			ModulePara.MasterExtSyncPoint = ModulePara.MasterExtSyncDeltaPoint + ModulePara.MasterExtSyncStartPoint;	
			if(ModulePara.MasterExtSyncPoint >ModulePara.SumOfExtSyncPoint)
			{
				ModulePara.MasterExtSyncPoint = 0;
				ModulePara.MasterExtSyncDeltaPoint = 0;
				ModulePara.MasterExtSyncFlag = 1;
			}
			else
			{
			
			}

//			if(ModulePara.MasterExtSyncPoint > (ModulePara.SumOfExtSyncPoint>>3))
//			{
//				MasterExtSyncH;
//			}
//			else
//			{
//				MasterExtSyncL;
//			}

//			if(ModulePara.MasterExtSyncPoint <= 5)
//			{
//				MasterExtSyncL;
//			}
//			if(ModulePara.MasterExtSyncPoint  == (ModulePara.SumOfExtSyncPoint >> 1))
//			{
//
//				MasterExtSyncH;
//
//			}
//			if(CabinetCB == GetBoardType())
//			{
//				
//				if(ModulePara.MasterExtSyncPoint == 0)
//				{
//					MasterExtSyncL;
//					SetExtLineSyncLow;
//				}
//				else if(ModulePara.MasterExtSyncPoint  == (ModulePara.SumOfExtSyncPoint >> 1))
//				{
//					MasterExtSyncH;
//					SetExtLineSyncHigh;
//				}
//			}
//			else
//			{
//
//			}

		}
		else
		{
			MasterExtSyncH;
			ModulePara.MasterExtSyncPoint = 0;
			ModulePara.MasterExtSyncDeltaPoint = 0;
			ModulePara.MasterExtSyncFlag = 0;
		}
	}
	else
	{
		MasterExtSyncH;
		ModulePara.MasterExtSyncPoint = 0;
		ModulePara.MasterExtSyncDeltaPoint = 0;
		ModulePara.MasterExtSyncFlag = 0;
	}

}
/*********************************************************************
Function name: void StepTimecountClr(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
extern  uint16_t  u16_PulseResetFlag;
void StepTimecountClr(void)
{
//	PulsePowerPulsePhase phase = SideAControl.PulsePhase;
	
//	if(SideAControl.step[phase].timecountset - SideAControl.step[phase].timecount <= 2)
    //Miller/20241009/for syn clear begin
    /*{
	//	SideAControl.PulsePhase = (PulsePowerPulsePhase)InitPulsePhase;
		SideAControl.PulsePhase = PulsePhase1;
		SideAControl.SumOfPeriod = SideAControl.PulseControlCnt;
        SideAControl.PulseControlCnt = 0;
		SideAControl.step[PulsePhase1].timecount = 0;
		SideAControl.step[PulsePhase2].timecount = 0;
		SideAControl.step[PulsePhase3].timecount = 0;
		SideAControl.step[PulsePhase4].timecount = 0;
		SideAControl.step[PulsePhase5].timecount = 0;
		SideAControl.step[PulsePhase6].timecount = 0;
		SideAControl.step[PulsePhase7].timecount = 0;
		SideAControl.step[PulsePhase8].timecount = 0;
		SideAControl.step[PulsePhase9].timecount = 0;
		SideAControl.step[PulsePhase10].timecount = 0;
		SideAControl.step[PulsePhase11].timecount = 0;
		SideAControl.step[PulsePhase12].timecount = 0;
		SideAControl.step[PulsePhase13].timecount = 0;
		SideAControl.step[PulsePhase14].timecount = 0;
		SideAControl.step[PulsePhase15].timecount = 0;
		SideAControl.step[PulsePhase16].timecount = 0;
		SideAControl.step[PulsePhase17].timecount = 0;
		SideAControl.step[PulsePhase18].timecount = 0;
		SideAControl.step[PulsePhase19].timecount = 0;
		SideAControl.step[PulsePhase20].timecount = 0;

		
	//	SideAControl.step[InitPulsePhase].timecount = InitTimeCount;
	}*/

    u16_PulseResetFlag = 1;
    //Miller/20241009/for syn clear end


}
/*********************************************************************
Function name: void SetExtSyncSignal(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SetExtSyncSignal(void)
{
	ModulePara.ExtSyncSignal = 1;

}
/*********************************************************************
Function name: void SetExtSyncStartPoint(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SetExtSyncStartPoint(void)
{
	ModulePara.ExtSyncStartPoint = 1;

}
/*********************************************************************
Function name: void SetPWMSyncSignal(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SetPWMSyncSignal(void)
{
	ModulePara.PWMSyncSignal = 1;

}
/*********************************************************************
Function name: void SyncCheck(stXparallel* input)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SyncCheck(stXparallel* input)
{
	
	if((input->SumOfExtSyncPoint != 0) && (input->Mode == Parallel))
	{
//		input->PWMSynclossCnt++;
//		input->PWMSyncbackCnt = 0;
//		if(input->PWMSynclossCnt >= input->PWMSynclossCntSetting)
//		{
//			input->PWMSynclossCnt = 0;
//			input->PWMSyncSignalLoss = pdTRUE;
//		}
//		if(input->PWMSyncbackCnt >= input->PWMSyncbackCntSetting)
//		{
//			input->PWMSyncbackCnt = 0;
//			input->PWMSyncSignalLoss = pdFALSE;
//		}
//		if(input->PWMSyncSignalLoss == pdTRUE)
//		{
//			//action
//		}
//		else
//		{
//		
//		}
//
//		if(input->PWMSyncLockLoss == pdTRUE)
//		{
//			//action
//		}
		if(input->ExtSyncSignal == 1)
		{
			input->ExtSynclossCnt++;
			input->ExtSyncbackCnt = 0;
			if(input->ExtSynclossCnt >= input->ExtSynclossCntSetting)
			{
				input->ExtSynclossCnt = 0;
				input->ExtSyncSignalLoss = pdTRUE;
			}
			if(input->ExtSyncbackCnt >= input->ExtSyncbackCntSetting)
			{
				input->ExtSyncbackCnt = 0;
				input->ExtSyncSignalLoss = pdFALSE;
			}
			if(input->ExtSyncSignalLoss == pdTRUE)
			{
				//action
			}
			else
			{
			
			}
		}
		else
		{
			input->ExtSynclossCnt = 0;
			input->ExtSyncbackCnt = 0;
			input->ExtSyncSignalLoss = pdFALSE;
		}
		input->ExtSyncSignal = 1;
	}
	else
	{
//		input->PWMSyncSignalLoss = pdFALSE;
//		input->ExtSyncSignalLoss = pdFALSE;
	
	}

}
/*********************************************************************
Function name: CB_TYPE GetBoardType(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
CBtype GetBoardType(void)
{
	return(BoardType);
}

/*********************************************************************
Function name: void SetBoardType(CBtype type)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SetBoardType(CBtype type)
{
	BoardType = type;
}


/*********************************************************************
Function name:  WaveType GetWaveForm(void)
Description:
Calls:
Called By:      sSuperTask()
Parameters:     void
Return:         void
*********************************************************************/
WaveType GetWaveForm(void)
{
    return(WaveForm);
}

/*********************************************************************
Function name: int GetUnitParaPWMSyncSignalLoss(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
int GetUnitPWMSyncSignalLoss(void)
{

	return(UnitPara.PWMSyncSignalLoss);
}
/*********************************************************************
Function name: int GetUnitParaPWMSyncSignalLoss(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
int GetUnitExtSyncSignalLoss(void)
{

	return(UnitPara.ExtSyncSignalLoss);
}
/*********************************************************************

Function name: void UnitPWMSyncSignalLoss(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void UnitPWMSyncSignalLoss(void)
{
	if((UnitPara.PWMSyncSignal == 1) && (UnitPara.PWMSync == xPWMSyncEn))
	{
	  if(UnitPara.PWMSyncSignalLoss == pdFALSE)
	  {
		  UnitPara.PWMSyncbackCnt = 0;
		  UnitPara.PWMSynclossCnt++;
		  if(UnitPara.PWMSynclossCnt >= UnitPara.PWMSynclossCntSetting)
		  {
			  UnitPara.PWMSynclossCnt = 0;
			  WarnRegs.UnitSync.bit.PWMSyncLoss = 1;
			  sSetISRWarningCode(cWunitPWMSyncLoss);
			  UnitPara.PWMSyncSignalLoss = pdTRUE;
			  
			  EPwm1Regs.TBPRD = cControlperiod;
			  EPwm2Regs.TBPRD = SwitchPeriodReg;
			  EPwm3Regs.TBPRD = SwitchPeriodReg;
			  
		  }
		  else
		  {
		  
		  } 			  
	  }
	}
	else
	{		
		WarnRegs.UnitSync.bit.PWMSyncLoss = 0;
		sClrISRWarningCode(cWunitPWMSyncLoss);
		UnitPara.PWMSyncSignalLoss = pdFALSE;
	}
	
}
/*********************************************************************
Function name: void UnitExtSyncSignalLoss(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void UnitExtSyncSignalLoss(void)
{
	if((UnitPara.SumOfExtSyncPoint != 0) && (GetModuleMode() == Parallel) && (GetBoardType() == UnitCB) && (UnitPara.ExtSync != 0))
	{	

		if(UnitPara.ExtSyncSignal == 1)
		{
			  if(UnitPara.ExtSyncSignalLoss == pdFALSE)
			  {
				  UnitPara.ExtSyncbackCnt = 0;
				  UnitPara.ExtSynclossCnt++;
				  if(UnitPara.ExtSynclossCnt >= UnitPara.ExtSynclossCntSetting)
				  {
					  UnitPara.ExtSynclossCnt = 0;
					  if((GetWaveForm() == Pulse) && (UnitPara.SumOfExtSyncPoint != 0))
					  {
						  WarnRegs.UnitSync.bit.LineSyncLoss = 1;
						  sSetWarningCode(cWunitLineSyncLoss);
						  UnitPara.ExtSyncSignalLoss = pdTRUE;
					  }
					  else
					  {
					  	 WarnRegs.UnitSync.bit.LineSyncLoss = 0;
						 sClrWarningCode(cWunitLineSyncLoss);
						 UnitPara.ExtSyncSignalLoss = pdFALSE;
					  }

				  }
				  else
				  {
				  
				  } 			  
			  }
		}
			UnitPara.ExtSyncSignal = 1; 
	}
	else
	{
		WarnRegs.UnitSync.bit.LineSyncLoss = 0;
		sClrWarningCode(cWunitLineSyncLoss);
		UnitPara.ExtSyncSignalLoss = pdFALSE;
	}
}
/*********************************************************************
Function name: void AddressCheck(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void AddressCheck(void)
{
    /*
	Uint16 switchtem[100];
//	Uint16 avg,min=4096,max=0;
	Uint32 sum = 0;
	int count = 0,adccount = ADC_A1_ISR_Count;
	while(1)
	{
		if(adccount != ADC_A1_ISR_Count)
		{
			while(ADC_isBusy(ADCC_BASE) == pdTRUE);
			if(count >= 100)
				break;
			switchtem[count] = AdccResultRegs.ADCRESULT4;
			sum += switchtem[count];
//			if(min > switchtem[count])
//				min = switchtem[count];
//			if(max < switchtem[count])
//				max = switchtem[count];
			adccount = ADC_A1_ISR_Count;
			count++;
		}
	}
//	avg = sum/count;

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
	
	//SwitchDIPAddr = 9;	//for test
//	if(SwitchDIPAddr <= 7)
//	{
//
//		BoardType = ModuleCB;
//		UnitPara.Role = InvalidRole;		
//		if(SwitchDIPAddr == 1)
//		{
//			PwmSyncON_TZ();
//
//			ModulePara.Role = MasterCB;		
//		}
//		else
//		{
//			ModulePara.Role = SlaveCB;	
//			PwmSyncOff_TZ();		
//		}
//
//		PieCtrlRegs.PIEIER4.bit.INTx1 = 0;//DISable PIE group 4 interrupt 1 for CAP1,PWM SYNC
//        PieCtrlRegs.PIEIER4.bit.INTx2 = 0;//DISable PIE group 4 interrupt 2 for CAP2,Line SYNC
//	}
//	else
//	{
//		ModulePara.Role = InvalidRole;	
//		PwmSyncOff_TZ();
//		BoardType = UnitCB;
//		UnitPara.Role = SlaveCB;
//		PieCtrlRegs.PIEIER4.bit.INTx1 = 1;//Enable PIE group 4 interrupt 1 for CAP1,PWM SYNC
////        PieCtrlRegs.PIEIER4.bit.INTx2 = 1;//Enable PIE group 4 interrupt 2 for CAP2,Line SYNC
////        
//		
//	}

	
	ModulePara.Mode = Parallel;




}
/*********************************************************************
Function name: void AddressCheck(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void DataInput(void)
{
	if((SIN1 == 1) && (SIN2 == 1))
	{
	
	}
	else if((SIN1 == 0) && (SIN2 == 1))
	{
	
	}
	else if((SIN1 == 1) && (SIN2 == 0))
	{
	
	}
	else
	{
	
	}

}
/*********************************************************************
Function name: xMode GetSystemMode(void)
Description:
Calls:
Called By:
Parameters:     
Return:         
*********************************************************************/
xMode GetSystemMode(void)
{
	return(SystemMode);
}

/*********************************************************************
Function name: SetUnitWorkStatus(eModuleWorkStatus status)
Description:
Calls:
Called By:
Parameters:     
Return:         
*********************************************************************/
void SetUnitWorkStatus(eModuleWorkStatus status)
{
	UnitWorkStatus = status;
}

/*********************************************************************
Function name: GetUnitWorkStatus()
Description:
Calls:
Called By:
Parameters:     
Return:         
*********************************************************************/
eModuleWorkStatus GetUnitWorkStatus()
{
	return UnitWorkStatus;
}

/*********************************************************************
Function name: SetModuleWorkStatus(eModuleWorkStatus status)
Description:
Calls:
Called By:
Parameters:     
Return:         
*********************************************************************/
void SetModuleWorkStatus(eModuleWorkStatus status)
{
	ModuleWorkStatus = status;
}

/*********************************************************************
Function name: GetModuleWorkStatus()
Description:
Calls:
Called By:
Parameters:     
Return:         
*********************************************************************/
eModuleWorkStatus GetModuleWorkStatus()
{
	return ModuleWorkStatus;
}

