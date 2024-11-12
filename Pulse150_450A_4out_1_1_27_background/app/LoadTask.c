/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : LoadTask.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define LoadTask_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"

#include "Interrupt.h"
#include "LoadTask.h"
#include "OsConfig.h"
#include "fault.h"
#include "SuperTask.h"
#include "Initial.h"
#include "math.h"
#include "eeprommodule.h"
#include "CanTask.h"




/********************************************************************************
*const define                               *
********************************************************************************/
const stTemperature AmbientTempInit = {	25,                            //value
										60,                            //OverLow
										65,                            //OverHigh
										pdFALSE,                       //TempChkEnable
										pdTRUE,                        //OverLowEnable
										pdTRUE,                        //OverHighEnable
										cLoadTimer10S,                 //OverLowcntset
										cLoadTimer10S,                 //OverHighcntset
										58,                            //levelback
										0,                             //FaultCount
										0,                             //OverLowCount
										0,                             //OverHighCount
										0,                             //BackCount
										cLoadTimer10S,                 //NTCcntset
										cLoadTimer10S,                 //backcntset
										150,                           //NTChighlevel
										-40,                           //NTClowlevel
										TempNormal                     //Status
									 };
const stTemperature SinkTempInit = {	25,                            //value
										60,                            //OverLow
										65,                            //OverHigh
										pdTRUE,                        //TempChkEnable
										pdFALSE,                        //OverLowEnable
										pdTRUE,                        //OverHighEnable
										cLoadTimer10S,                 //OverLowcntset
										cLoadTimer10S,                 //OverHighcntset
										58,                            //levelback
										0,                             //FaultCount
										0,                             //OverLowCount
										0,                             //OverHighCount
										0,                             //BackCount
										cLoadTimer10S,                 //NTCcntset
										cLoadTimer10S,                 //backcntset
										150,                           //NTChighlevel
										-40,                           //NTClowlevel
										TempNormal                     //Status
									 };

//BUS故障
const stBusCheck BusCheckInit = {50.0,                                //highlevel
								48.0,                                 //highback
								20.0,                                  //lowlevel
								22.0,                                  //lowback
								pdFALSE,                              //highflag
								pdFALSE,                              //lowflag
								pdTRUE,                               //highEnable
								pdTRUE,                               //lowEnable
								25.0,                                 //compvalue
								0,                                    //highcount
								cLoadTimer5S,                         //highcountset
								0,                                    //highbackcount
								cLoadTimer5S,                         //highbackcountset
								0,                                    //lowcount
								cLoadTimer5S,                         //lowcountset
								0,                                    //lowbackcount
								cLoadTimer5S,                         //lowbackcountset
								0,                                    //highturnoff
								0                                     //lowturnoff
								};
//BUS告警
const stBusCheck BusCheckWarnInit = {47.0,                            //highlevel
                                     45.0,                            //highback
                                     23.0,                            //lowlevel
                                     25.0,                            //lowback
                                     pdFALSE,                         //highflag
                                     pdFALSE,                         //lowflag
                                     pdTRUE,                          //highEnable
                                     pdTRUE,                          //lowEnable
                                     25.0,                            //compvalue
                                     0,                               //highcount
                                     cLoadTimer5S,                    //highcountset
                                     0,                               //highbackcount
                                     cLoadTimer5S,                    //highbackcountset
                                     0,                               //lowcount
                                     cLoadTimer5S,                    //lowcountset
                                     0,                               //lowbackcount
                                     cLoadTimer5S,                    //lowbackcountset
                                     0,                               //highturnoff
                                     0                                //lowturnoff
                                    };

const stAverageValue BusAvgValueInit = {0,							//float Sum;
								 	    0,							//float Sumtemp;
								 	    0,							//float AverageValue;
								 	    cVBusAdjustRatio,			//float AdjustRatio;
								 	    0,							//Uint32 CountTemp;
								 	    0							//Uint32 Count;
	                              	     };

const uint16_t TEMP_10K_TBLTE[TEMP_10K_LENGTH] =
{
    //AD 温度 --> 偏置10000
    2618 , 7000 , //-30
    2585 , 7500 , //-25
    2542 , 8000 , //-20
    2489 , 8500 , //-15
    2424 , 9000 , //-10
    2345 , 9500 , //-5
    2253 , 10000 , //0
    2147 , 10500 , //5
    2028 , 11000 , //10
    1899 , 11500 , //15
    1761 , 12000 , //20
    1618 , 12500 , //25
    1474 , 13000 , //30
    1331 , 13500 , //35
    1193  , 14000 , //40
    1062  , 14500 , //45
    940  , 15000 , //50
    829  , 15500 , //55
    728  , 16000 , //60
    638  , 16500 , //65
    558  , 17000 , //70
    488  , 17500 , //75
    426 , 18000 , //80
    372 , 18500 , //85
    326 , 19000 , //90
    285 , 19500 , //95
    250 , 20000 , //100
    220 , 20500 , //105
    194 , 21000 , //110
    171  , 21500 , //115
    151  , 22000 , //120
    134  , 22500  //125
};


const stOverCheck LoadVoltPosAvgInit = {1200,                           //float Overlevel,12V;
										1100,                          //float Overback,11V;
										pdFALSE,                          //int16 Overflag;
										1,							//int16 Enable;
										0,							//float compvalue;
										0,							//int16 Overcount;
										0,							//int16 Overbackcount;
										100,							//int16 Overcountset;
										100							//int16 Overbackcountset;
										};

const stOverCheck LoadVoltNegAvgInit = {3600,                           //float Overlevel,36V;
										3400,                          //float Overback,34V;
										pdFALSE,                          //int16 Overflag;
										1,							//int16 Enable;
										0,							//float compvalue;
										0,							//int16 Overcount;
										0,							//int16 Overbackcount;
										100,							//int16 Overcountset;
										100							//int16 Overbackcountset;
										};

const stOverCheck LoadCurrPosAvgInit = {800,                           //float Overlevel,80A;
										700,                          //float Overback,70A;
										pdFALSE,                          //int16 Overflag;
										1,							//int16 Enable;
										0,							//float compvalue;
										0,							//int16 Overcount;
										0,							//int16 Overbackcount;
										100,							//int16 Overcountset;
										100							//int16 Overbackcountset;
										};


const stOverCheck LoadCurrNegAvgInit = {1900,                           //float Overlevel,190A;
										1800,                          //float Overback,180A;
										pdFALSE,                          //int16 Overflag;
										1,							//int16 Enable;
										0,							//float compvalue;
										0,							//int16 Overcount;
										0,							//int16 Overbackcount;
										100,							//int16 Overcountset;
										100							//int16 Overbackcountset;
										};




/********************************************************************************
* Variable declaration                              *
********************************************************************************/
/************************ 滑动窗口滤波器 *****************************/
#define window_size1 40            //滑动窗口长度
float buffer1[window_size1] = {0}; //滑动窗口数据buf
int data_num1 = 0;

#define window_size2 40            //滑动窗口长度
float buffer2[window_size2] = {0}; //滑动窗口数据buf
int data_num2 = 0;

#define window_size3 40            //滑动窗口长度
float buffer3[window_size3] = {0}; //滑动窗口数据buf
int data_num3 = 0;

#define window_size4 40            //滑动窗口长度
float buffer4[window_size4] = {0}; //滑动窗口数据buf
int data_num4 = 0;


/********************************************************************************
* function declaration                              *
********************************************************************************/
void blueLedToggle(void)
{
  static uint32_t counter = 0;
  counter++;
  GPIO_writePin(DEVICE_GPIO_PIN_LED2, counter & 1);
}

/*********************************************************************
Function name:  LoadTaskInit
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void LoadTaskInit(void)
{
    LoadParameterInit();
}

/*********************************************************************
Function name:  LoadTask
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
//-------------------------------------------------------------------------------------------------
void LoadTask(void * pvParameters)
{
    uint32_t event;
    //BaseType_t err;
	static uint16_t firstFlag = 1;
	
	//LoadParameterInit();

    //while(pdTRUE)
    {

        //err = xSemaphoreTake( xSemaphoreLoad, (TickType_t)Tick20mS );
        event = OSEventPend(cPrioLoad);
        if(event != 0)
        {
           	if(OSEventConfirm(event,eLoadTest))
			{

			}


        }
        else//eLoadTimer
        {
			
			AmbientTempCal();
			AmbientTempChk();
			
			SinkTempCal();
			SinkTempChk();
			
			
			BusCheckAction();
			LoadAverageCal(&SideAControl);
			SetLoadVoltAvg(SideAControl.Vavg);
			SetLoadCurrAvg(SideAControl.Iavg);
			SetLoadVoltPosAvg(SideAControl.VavgPos);
			SetLoadCurrPosAvg(SideAControl.IavgPos);
			SetLoadVoltNegAvg(fabsf(SideAControl.VavgNeg));
			SetLoadCurrNegAvg(fabsf(SideAControl.IavgNeg));
			LoadAvgOverAction();

			CalculationPulsePosAndNegAverageValue();
			DisplayVoltAndCurrFilter();
			DCModeRMSLoop();
			PulseModeRMSLoop();

			if(GetHwExtLoadOC() == 1)
			{
//				ClrHwExtLoadOC();
				if(firstFlag)
				{
					firstFlag = 0;
					OSEventSend(cPrioSuper,eSuperFault);
				}
			}
			Power24Vdetect();

			FAULT_OT_LED_OH();
			RUN_LED_OH();
			
//			if(sbLoadPosVoltHighTimeChk(10) == pdTRUE)
//			{
//			//
//			}
//			else
//			{
//			//
//			}

//			if(sbLoadPosCurrHighTimeChk(10) == pdTRUE)
//			{
//			//
//			}
//			else
//			{
//			//
//			}
			
			



        }
    }
}

/*******************************************************************************
Function name:  void	sTempChk(stTemperature* temper)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void	sTempChk(stTemperature* temper)
{
	//if(GetPowerMode() != cFaultMode)
	{
		if(temper->TempChkEnable == pdTRUE)
		{
			//if(temper->Status == TempNormal)
			{
	//			temper->BackCount = 0;
				
	//			if((temper->value >= temper->NTChighlevel) || (temper->value <= temper->NTClowlevel))
	//			{
	//				temper->FaultCount ++;
	//				if(temper->FaultCount >= temper->NTCcntset)
	//				{
	//					temper->FaultCount = 0;
	//					temper->Status = NTCFault;	
	//				}
	//			}
				//else if(temper->value >= temper->OverHigh)

				if(temper->value >= temper->OverHigh)
				{
					temper->FaultCount  = 0;
					temper->OverLowCount = 0;
					temper->BackCount = 0;
					if(temper->OverHighEnable == pdTRUE)
					{
						temper->OverHighCount ++;
						if(temper->OverHighCount >= temper->OverHighcntset)
						{
							temper->OverHighCount = 0;
							temper->Status = TempOverHigh;
						}
					}
					else
					{
						temper->OverHighCount = 0;

					}
				}
				else if(temper->value >= temper->OverLow)
				{
					temper->FaultCount  = 0;
					temper->OverHighCount = 0;
					temper->BackCount = 0;
					if(temper->OverLowEnable == pdTRUE)
					{
						temper->OverLowCount ++;
						if(temper->OverLowCount >= temper->OverLowcntset)
						{
							temper->OverLowCount = 0;
							temper->Status = TempOverLow;
						}
					}
					else
					{
						temper->OverLowCount = 0;	
					}
				}	
				else if(temper->value <= temper->levelback)
				{	
					temper->FaultCount  = 0;
					temper->OverHighCount = 0;
					temper->OverLowCount = 0;
					
					temper->BackCount ++;
					if(temper->BackCount >= temper->backcntset)
					{
						temper->BackCount = 0;
						temper->Status = TempNormal;
					}			
				}
				else
				{
					temper->FaultCount  = 0;
					temper->OverHighCount = 0;
					temper->OverLowCount = 0;
					temper->BackCount = 0;
				
				}
			}
		
			
		}
		else
		{
			temper->FaultCount	= 0;
			temper->OverHighCount = 0;
			temper->OverLowCount = 0;
			temper->BackCount = 0;	
			temper->Status = TempChkDisable;
		}
	}
	

}
/*******************************************************************************
Function name:  NTC_Status GetAmbientTempStatus(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         NTC_Status
*******************************************************************************/
NTC_Status GetAmbientTempStatus(void)
{
	return(AmbientTemp.Status);
}
/*******************************************************************************
Function name:  void AmbientTempChk(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void AmbientTempChk(void)
{
	AmbientTemp.value = (float)(AmbientDegreeC/100);

	sTempChk(&AmbientTemp);

	if(GetAmbientTempStatus() == NTCFault)
	{
		WarnRegs.TempWarn.bit.AmbientNTCerr = 1;
		sSetWarningCode(cWambientNTCerr);
	
	}
	else if(GetAmbientTempStatus() == TempOverHigh)
	{
		WarnRegs.TempWarn.bit.AmbientOT = 0;
		sClrWarningCode(cWambientOT);
		
		FaultRegs.HWfaultTemp.bit.AmbientHighOT = 1;
		sSetFaultCode(cFambientTempOverHigh);
		OSEventSend(cPrioSuper,eSuperFault);
		
	
	}
	else if(GetAmbientTempStatus() == TempOverLow)
	{
		WarnRegs.TempWarn.bit.AmbientOT = 1;
		sSetWarningCode(cWambientOT);
	
	}
	else
	{
		WarnRegs.TempWarn.bit.AmbientNTCerr = 0;
		sClrWarningCode(cWambientNTCerr);
		
		FaultRegs.HWfaultTemp.bit.AmbientHighOT = 0;
		
		WarnRegs.TempWarn.bit.AmbientOT = 0;
		sClrWarningCode(cWambientOT);
	
	}


}
/*******************************************************************************
Function name:  NTC_Status GetSinkTempStatus(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         NTC_Status
*******************************************************************************/
NTC_Status GetSinkTempStatus(void)
{
	return(SinkTemp.Status);
}
/*******************************************************************************
Function name:  void AmbientTempChk(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void SinkTempChk(void)
{
	static int fanRunCnt=0,fanBackCnt=0;
	
	SinkTemp.value = (float)(SinkDegreeC/100);
	if(SinkTemp.value > cFanRunTemp)
	{
		fanRunCnt ++;
		fanBackCnt = 0;
		if(fanRunCnt > cLoadTimer10S)
		{
			FAN_RUN();
		}
	}
	else if(SinkTemp.value < cFanLevelBack)
	{
		fanRunCnt = 0;
		fanBackCnt++;
		if(fanBackCnt > cLoadTimer10S)
		{
			FAN_STOP();
		}
	}

	sTempChk(&SinkTemp);

	if(GetSinkTempStatus() == NTCFault)
	{
		WarnRegs.TempWarn.bit.SinkNTCerr = 1;
		sSetWarningCode(cWsinkNTCerr);
	}
	else if(GetSinkTempStatus() == TempOverHigh)
	{
		WarnRegs.TempWarn.bit.SinkNTCOT = 0;
		sClrWarningCode(cWsinkNTCOT);
		
		FaultRegs.HWfaultTemp.bit.SinkHighOT = 1;		
		sSetFaultCode(cFsinkTempOverHigh);
		OSEventSend(cPrioSuper,eSuperFault);
	
	}
	else if(GetSinkTempStatus() == TempOverLow)
	{
		WarnRegs.TempWarn.bit.SinkNTCOT = 1;
		sSetWarningCode(cWsinkNTCOT);
	}
	else
	{
		WarnRegs.TempWarn.bit.SinkNTCerr = 0;
		sClrWarningCode(cWsinkNTCerr);

		FaultRegs.HWfaultTemp.bit.SinkHighOT = 0;

		WarnRegs.TempWarn.bit.SinkNTCOT = 0;
		sClrWarningCode(cWsinkNTCOT);
	}

	


}

/*******************************************************************************
Function name:  void LoadParameterInit(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void LoadParameterInit(void)
{
	AmbientTemp = AmbientTempInit;
	SinkTemp = SinkTempInit;
	BusCheck = BusCheckInit;
	VBus = BusAvgValueInit;
	BusCheckWarn = BusCheckWarnInit;

	AmbientTemp.TempChkEnable = sUintEepromPara.AmbientTempCheckEnable;
	AmbientTemp.OverHighEnable = sUintEepromPara.AmbientTempOverHighEnable;
	AmbientTemp.OverHigh = sUintEepromPara.AmbientTempOverHighThreshold;
	AmbientTemp.OverLowEnable = sUintEepromPara.AmbientTempOverLowEnable;
	AmbientTemp.OverLow = sUintEepromPara.AmbientTempOverLowThreshold;
	AmbientTemp.levelback = sUintEepromPara.AmbientTemplevelback;

	SinkTemp.TempChkEnable = sUintEepromPara.SinkTempCheckEnable;
	SinkTemp.OverHighEnable = sUintEepromPara.SinkTempOverHighEnable;
	SinkTemp.OverHigh = sUintEepromPara.SinkTempOverHighThreshold;
	SinkTemp.OverLowEnable = sUintEepromPara.SinkTempOverLowEnable;
	SinkTemp.OverLow = sUintEepromPara.SinkTempOverLowThreshold;
	SinkTemp.levelback = sUintEepromPara.SinkTemplevelback;

	BusCheck.highEnable = sUintEepromPara.BusOverCheckEnable;
	BusCheck.highlevel = sUintEepromPara.BusOverThreshold;
	BusCheck.highback = sUintEepromPara.BusOverBackThreshold;
	//BusCheck.highturnoff = sUintEepromPara.BusOverTurnOffEnable;
	BusCheck.lowEnable = sUintEepromPara.BusUnderCheckEnable;
	BusCheck.lowlevel = sUintEepromPara.BusUnderThreshold;
	BusCheck.lowback = sUintEepromPara.BusUnderBackThreshold;
	//BusCheck.lowturnoff = sUintEepromPara.BusUnderTurnOffEnable;

	BusCheckWarn.highEnable = sUintEepromPara.WarnBusOverCheckEnable;
	BusCheckWarn.highlevel = sUintEepromPara.WarnBusOverThreshold;
	BusCheckWarn.highback = sUintEepromPara.WarnBusOverBackThreshold;
	BusCheckWarn.lowEnable = sUintEepromPara.WarnBusUnderCheckEnable;
	BusCheckWarn.lowlevel = sUintEepromPara.WarnBusUnderThreshold;
	BusCheckWarn.lowback = sUintEepromPara.WarnBusUnderBackThreshold;
	
	LoadVoltPosAvg = LoadVoltPosAvgInit;
	LoadVoltNegAvg = LoadVoltNegAvgInit;
	LoadCurrPosAvg = LoadCurrPosAvgInit;
    LoadCurrNegAvg = LoadCurrNegAvgInit;	

	LoadVoltPosAvg.Enable = sUintEepromPara.LoadVoltPosAvgOVEnable;
	LoadVoltPosAvg.Overlevel = sUintEepromPara.LoadVoltPosAvgOVThreshold;
	LoadVoltPosAvg.Overback = sUintEepromPara.LoadVoltPosAvgOVBackThreshold;
	LoadVoltNegAvg.Enable = sUintEepromPara.LoadVoltNegAvgOVEnable;
	LoadVoltNegAvg.Overlevel = sUintEepromPara.LoadVoltNegAvgOVThreshold;
	LoadVoltNegAvg.Overback = sUintEepromPara.LoadVoltNegAvgOVBackThreshold;
	LoadCurrPosAvg.Enable = sUintEepromPara.LoadCurrPosAvgOCEnable;
	LoadCurrPosAvg.Overlevel = sUintEepromPara.LoadCurrPosAvgOCThreshold;
	LoadCurrPosAvg.Overback = sUintEepromPara.LoadCurrPosAvgOCBackThreshold;
	LoadCurrNegAvg.Enable = sUintEepromPara.LoadCurrNegAvgOCEnable;
	LoadCurrNegAvg.Overlevel = sUintEepromPara.LoadCurrNegAvgOCThreshold;
	LoadCurrNegAvg.Overback = sUintEepromPara.LoadCurrNegAvgOCBackThreshold;
}


/*******************************************************************************
Function name:  void LoadParameterUpdate(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void LoadParameterUpdate(void)
{
    //AmbientTemp = AmbientTempInit;
    //SinkTemp = SinkTempInit;
    //BusCheck = BusCheckInit;
    //VBus = BusAvgValueInit;
    //BusCheckWarn = BusCheckWarnInit;

    AmbientTemp.TempChkEnable = sUintEepromPara.AmbientTempCheckEnable;
    AmbientTemp.OverHighEnable = sUintEepromPara.AmbientTempOverHighEnable;
    AmbientTemp.OverHigh = sUintEepromPara.AmbientTempOverHighThreshold;
    AmbientTemp.OverLowEnable = sUintEepromPara.AmbientTempOverLowEnable;
    AmbientTemp.OverLow = sUintEepromPara.AmbientTempOverLowThreshold;
    AmbientTemp.levelback = sUintEepromPara.AmbientTemplevelback;

    SinkTemp.TempChkEnable = sUintEepromPara.SinkTempCheckEnable;
    SinkTemp.OverHighEnable = sUintEepromPara.SinkTempOverHighEnable;
    SinkTemp.OverHigh = sUintEepromPara.SinkTempOverHighThreshold;
    SinkTemp.OverLowEnable = sUintEepromPara.SinkTempOverLowEnable;
    SinkTemp.OverLow = sUintEepromPara.SinkTempOverLowThreshold;
    SinkTemp.levelback = sUintEepromPara.SinkTemplevelback;

    BusCheck.highEnable = sUintEepromPara.BusOverCheckEnable;
    BusCheck.highlevel = sUintEepromPara.BusOverThreshold;
    BusCheck.highback = sUintEepromPara.BusOverBackThreshold;
    //BusCheck.highturnoff = sUintEepromPara.BusOverTurnOffEnable;
    BusCheck.lowEnable = sUintEepromPara.BusUnderCheckEnable;
    BusCheck.lowlevel = sUintEepromPara.BusUnderThreshold;
    BusCheck.lowback = sUintEepromPara.BusUnderBackThreshold;
    //BusCheck.lowturnoff = sUintEepromPara.BusUnderTurnOffEnable;

    BusCheckWarn.highEnable = sUintEepromPara.WarnBusOverCheckEnable;
    BusCheckWarn.highlevel = sUintEepromPara.WarnBusOverThreshold;
    BusCheckWarn.highback = sUintEepromPara.WarnBusOverBackThreshold;
    BusCheckWarn.lowEnable = sUintEepromPara.WarnBusUnderCheckEnable;
    BusCheckWarn.lowlevel = sUintEepromPara.WarnBusUnderThreshold;
    BusCheckWarn.lowback = sUintEepromPara.WarnBusUnderBackThreshold;

    //LoadVoltPosAvg = LoadVoltPosAvgInit;
    //LoadVoltNegAvg = LoadVoltNegAvgInit;
    //LoadCurrPosAvg = LoadCurrPosAvgInit;
    //LoadCurrNegAvg = LoadCurrNegAvgInit;

    LoadVoltPosAvg.Enable = sUintEepromPara.LoadVoltPosAvgOVEnable;
    LoadVoltPosAvg.Overlevel = sUintEepromPara.LoadVoltPosAvgOVThreshold;
    LoadVoltPosAvg.Overback = sUintEepromPara.LoadVoltPosAvgOVBackThreshold;
    LoadVoltNegAvg.Enable = sUintEepromPara.LoadVoltNegAvgOVEnable;
    LoadVoltNegAvg.Overlevel = sUintEepromPara.LoadVoltNegAvgOVThreshold;
    LoadVoltNegAvg.Overback = sUintEepromPara.LoadVoltNegAvgOVBackThreshold;
    LoadCurrPosAvg.Enable = sUintEepromPara.LoadCurrPosAvgOCEnable;
    LoadCurrPosAvg.Overlevel = sUintEepromPara.LoadCurrPosAvgOCThreshold;
    LoadCurrPosAvg.Overback = sUintEepromPara.LoadCurrPosAvgOCBackThreshold;
    LoadCurrNegAvg.Enable = sUintEepromPara.LoadCurrNegAvgOCEnable;
    LoadCurrNegAvg.Overlevel = sUintEepromPara.LoadCurrNegAvgOCThreshold;
    LoadCurrNegAvg.Overback = sUintEepromPara.LoadCurrNegAvgOCBackThreshold;
}

/*******************************************************************************
Function name:  void	sBusRangeCheck(stBusCheck* input)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void	sBusRangeCheck(stBusCheck* input)
{
	if(input->highEnable == pdTRUE)
	{

		if(input->highflag == pdFALSE)
		{
			input->highbackcount = 0;

			
			if(input->compvalue > input->highlevel)
			{
				input->highcount++;
			}
			else
			{
				input->highcount = 0;
			}
			if(input->highcount > input->highcountset)
			{
				input->highcount = 0;
				input->highflag = pdTRUE;
			}
			else
			{
				
			}
				
		}
		else
		{
			input->highcount = 0;


			
			if(input->compvalue < input->highback)
			{
				input->highbackcount++;
			}
			else
			{
				input->highbackcount = 0;
			}
			if(input->highbackcount > input->highbackcountset)
			{
				input->highbackcount = 0;
				input->highflag = pdFALSE;
			}
			else
			{
				
			}
		}
	}
	else
	{
		input->highflag = pdFALSE;
		input->highcount = 0;
		input->highbackcount = 0;
	}
	if(input->lowEnable == pdTRUE)
	{
		if(input->lowflag == pdFALSE)
		{

			input->lowbackcount = 0;
			
			if(input->compvalue < input->lowlevel)
			{
				input->lowcount++;
			}
			else
			{
				input->lowcount = 0;
			}
			if(input->lowcount > input->lowcountset)
			{
				input->lowcount = 0;
				input->lowflag = pdTRUE;
			}
			else
			{
				
			}
			
				
		}
		else
		{
			input->lowcount = 0;

			
			if(input->compvalue > input->lowback)
			{
				input->lowbackcount++;
			}
			else
			{
				input->lowbackcount = 0;
			}
			if(input->lowbackcount > input->lowbackcountset)
			{
				input->lowbackcount = 0;
				input->lowflag = pdFALSE;
			}
			else
			{
			}
			
		}
	}
	else
	{
		
		input->lowflag = pdFALSE;
		input->lowcount = 0;
		input->lowbackcount = 0;
	}
	

}
/*******************************************************************************
Function name:  GetBusHighFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int16 GetBusHighFlag(void)
{
	return(BusCheck.highflag);
}

/*******************************************************************************
Function name:  GetBusLowFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int16 GetBusLowFlag(void)
{
	return(BusCheck.lowflag);
}

/*******************************************************************************
Function name:  GetBusHighWarnFlag(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int16 GetBusHighWarnFlag(void)
{
    return(BusCheckWarn.highflag);
}

/*******************************************************************************
Function name:  GetBusLowWarnFlag(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int16 GetBusLowWarnFlag(void)
{
    return(BusCheckWarn.lowflag);
}

/*******************************************************************************
Function name:  GetBusLowFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void BusCheckAction(void)
{
	VBus.AverageValue = (AverageValueCal(&VBus))/100;
	BusCheck.compvalue = VBus.AverageValue;
	BusCheckWarn.compvalue = VBus.AverageValue;

	sBusRangeCheck(&BusCheck);
	if(GetBusHighFlag() == pdTRUE)
	{
        FaultRegs.HWfaultLoadAvg.bit.BusOver = 1;
        sSetFaultCode(cFBusOver);
        OSEventSend(cPrioSuper,eSuperFault);
	}
	else
	{
	    FaultRegs.HWfaultLoadAvg.bit.BusOver = 0;
	}

	if((GetBusLowFlag() == pdTRUE) && (PowerOnFinished() == pdTRUE))
	{
	    FaultRegs.HWfaultLoadAvg.bit.BusUnder = 1;
        sSetFaultCode(cFBusUnder);
        OSEventSend(cPrioSuper,eSuperFault);
	}
	else
	{
	    FaultRegs.HWfaultLoadAvg.bit.BusUnder = 0;
	}


	sBusRangeCheck(&BusCheckWarn);
    if(GetBusHighWarnFlag() == pdTRUE)
    {
        WarnRegs.UnitSync.bit.BusOver = 1;
        sSetWarningCode(cWbusOver);
    }
    else
    {
        WarnRegs.UnitSync.bit.BusOver = 0;
        sClrWarningCode(cWbusOver);
    }

    if((GetBusLowWarnFlag() == pdTRUE) && (PowerOnFinished() == pdTRUE))
    {
        WarnRegs.UnitSync.bit.BusUnder = 1;
        sSetWarningCode(cWbusUnder);
    }
    else
    {
        WarnRegs.UnitSync.bit.BusUnder = 0;
        sClrWarningCode(cWbusUnder);
    }
}
/*******************************************************************************
Function name:  void LoadAverageCal(stSideXControl* SideX)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void LoadAverageCal(stSideXControl* SideX)
{
      Uint16 CalCnt1 = 0;
      Uint16 temp = 0;
      Uint16 temp2 = 0;

      //if((SideX->timecountset != 0)&&(GetWaveForm() == Pulse) && (GetPWMstatus() == PWMrunSts))
      //if((SideX->timecountset != 0)&&(GetWaveForm() == Pulse))
      if((GetWaveForm() == Pulse) && (GetPWMstatus() == PWMrunSts))
      {
          for(temp = 1; temp <= 20; temp++)
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
          }

          SideX->Vavg = SideX->Vsum/SideX->timecountset;
          SideX->Iavg = SideX->Isum/SideX->timecountset;
//		  SideX->VavgPos = (SideX->VLoadsumPos/SideX->timecountsetPos) * VavgPosRatio + VavgPosBias;
//		  SideX->VavgNeg = (SideX->VLoadsumNeg/SideX->timecountsetNeg)*VavgNegRatio + VavgNegBias;
		  if(PWMorCMP == cPWMmode)
		  {
//			  if(CurrMode == eSmallCurrMode)
//			  {
//				  SideX->VavgPos = (SideX->VLoadsumPos/SideX->timecountsetPos) * PosSmallVoltAdjustFactor.ratio + PosSmallVoltAdjustFactor.bias;
//				  SideX->VavgNeg = (SideX->VLoadsumNeg/SideX->timecountsetNeg) * NegSmallVoltAdjustFactor.ratio + NegSmallVoltAdjustFactor.bias;
//			  }
//			  else
			  {
			  	if(SideX->timecountsetPos == 0)
		  		{
		  			SideX->VavgPos = 0;
		  		}
				else
				{
//					SideX->VavgPos = (SideX->VLoadsumPos/SideX->timecountsetPos) * PosVoltAdjustFactor.ratio + PosVoltAdjustFactor.bias;
					SideX->VavgPos = (SideX->VLoadsumPos/SideX->timecountsetPos);
				}
				if(SideX->timecountsetNeg == 0)
				{
					SideX->VavgNeg = 0;
				}
				else
				{
//				  SideX->VavgNeg = (SideX->VLoadsumNeg/(long)SideX->timecountsetNeg) * NegVoltAdjustFactor.ratio + NegVoltAdjustFactor.bias;
				  SideX->VavgNeg = (SideX->VLoadsumNeg/(long)SideX->timecountsetNeg);
				}
			  }
			  if(SideX->timecountsetPos == 0)
			  {
			  	SideX->IavgPos = 0;
			  }
			  else
			  {
			  	SideX->IavgPos = (SideX->IsumPos/SideX->timecountsetPos);
			  }
			  if(SideX->timecountsetNeg == 0)
			  {
			  	SideX->IavgNeg = 0;
			  }
			  else
			  {
			  	SideX->IavgNeg = (SideX->IsumNeg/SideX->timecountsetNeg);
			  }
		  }
		  else
		  {	
//			  if(CurrMode == eSmallCurrMode)
//			  {
//				  SideX->VavgPos = (SideX->VLoadsumPos/SideX->timecountsetPos) * PosCmpSmallVoltAdjustFactor.ratio + PosCmpSmallVoltAdjustFactor.bias;
//				  SideX->VavgNeg = (SideX->VLoadsumNeg/SideX->timecountsetNeg) * NegCmpSmallVoltAdjustFactor.ratio + NegCmpSmallVoltAdjustFactor.bias;
//				  
//				  SideX->IavgPos = (SideX->IsumPos/SideX->timecountsetPos) * PosCmpDispSmallCurrAdjustFactor.ratio + PosCmpDispSmallCurrAdjustFactor.bias;
//				  SideX->IavgNeg = (SideX->IsumNeg/SideX->timecountsetNeg) * NegCmpDispSmallCurrAdjustFactor.ratio + NegCmpDispSmallCurrAdjustFactor.bias;
//			  }
//			  else
			  {
			  	if(SideX->timecountsetPos == 0)
			  	{
			  		SideX->VavgPos = 0;
			  		SideX->IavgPos = 0;
			  	}
				else
				{
					SideX->VavgPos = (SideX->VLoadsumPos/SideX->timecountsetPos);
//					SideX->VavgPos = (SideX->VLoadsumPos/SideX->timecountsetPos) * PosCmpVoltAdjustFactor.ratio + PosCmpVoltAdjustFactor.bias;
					//SideX->IavgPos = (SideX->IsumPos/SideX->timecountsetPos) * PosCmpDispCurrAdjustFactor.ratio + PosCmpDispCurrAdjustFactor.bias;
					SideX->IavgPos = (SideX->IsumPos/SideX->timecountsetPos);
				}
				if(SideX->timecountsetNeg == 0)
				{
					SideX->VavgNeg = 0;
					SideX->IavgNeg = 0;
				}
				else
				{
					SideX->VavgNeg = (SideX->VLoadsumNeg/(long)SideX->timecountsetNeg);
//					SideX->VavgNeg = (SideX->VLoadsumNeg/(long)SideX->timecountsetNeg) * NegCmpVoltAdjustFactor.ratio + NegCmpVoltAdjustFactor.bias;
					//SideX->IavgNeg = (SideX->IsumNeg/SideX->timecountsetNeg) * NegCmpDispCurrAdjustFactor.ratio + NegCmpDispCurrAdjustFactor.bias;
					SideX->IavgNeg = (SideX->IsumNeg/SideX->timecountsetNeg);
				}

			  }
		  }

          for(CalCnt1 = 1; CalCnt1 <= 20; CalCnt1++)
          {
              if(SideX->timecountsetPhase[CalCnt1] == 0)
              {
                  SideX->IavgPhase[CalCnt1] = 0;
              }
              else
              {
                  SideX->IavgPhase[CalCnt1] = SideX->IloadPhasesum[CalCnt1] / SideX->timecountsetPhase[CalCnt1];
              }
          }
      }
      else
      {
          SideX->Vavg = 0.0;
          SideX->Iavg = 0.0;
		  SideX->VavgPos = 0.0;
		  SideX->VavgNeg = 0.0;
		  SideX->IavgPos = 0.0;
		  SideX->IavgNeg = 0.0;

		  SideX->IsumPos = 0;
          SideX->IsumNeg = 0;
          SideX->VLoadsumPos = 0;
          SideX->VLoadsumNeg = 0;
          SideX->timecountsetPos = 0;
          SideX->timecountsetNeg = 0;
          SideX->timecountset = 0;

          SideX->timecountsetPos_temp = 0;
          SideX->timecountsetNeg_temp = 0;
          SideX->IPossum_temp2 = 0;
          SideX->INegsum_temp2 = 0;
          SideX->VloadPosSum_temp2 = 0;
          SideX->VloadNegSum_temp2 = 0;
          SideX->timecountset_temp = 0;

          for(CalCnt1 = 1; CalCnt1 <= 20; CalCnt1++)
          {
              SideX->IavgPhase[CalCnt1] = 0.0;
              SideX->IloadPhasesum[CalCnt1] = 0;
              SideX->timecountsetPhase[CalCnt1] = 0;
              SideX->IloadPhasesum_temp2[CalCnt1] = 0;
              SideX->timecountsetPhase_temp2[CalCnt1] = 0;
          }
      }


	  //if((LoadCurrentCnt != 0)&&(GetPowerMode() == cDCMode))
      if((LoadCurrentCnt != 0)&&(GetWaveForm() == DC) && (GetPWMstatus() == PWMrunSts))
      {
      	if(AdjustMode)
		{
			if(SideAControl.step[PulsePhase0].reference > 0)
			{
				SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000);
				SideX->VavgNegDC = 0.0;
				
				SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000);
				SideX->IavgNegDC = 0.0;
			}
			else
			{
				SideX->VavgPosDC = 0.0;
				SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000);

				SideX->IavgPosDC = 0.0;
				SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000);
			}
		}
		else
      	{
      		if(PWMorCMP == cPWMmode)
      		{
		      	if(SideAControl.step[PulsePhase0].reference > 0)
		      	{
	//	      		SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * VavgPosDCRatio + VavgPosDCBias;
//		      		if(SideAControl.step[PulsePhase0].reference <= SmallCurrModeThreshold)
//		      			SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosSmallVoltAdjustFactor.ratio + PosSmallVoltAdjustFactor.bias;
//					else
//						SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactor.ratio + PosVoltAdjustFactor.bias;
		      		if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[1])
		      		{
		      		    SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[0].ratio + PosVoltAdjustFactorArray[0].bias;
		      		}
		      		else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[2])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[1].ratio + PosVoltAdjustFactorArray[1].bias;
                    }
                    //else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[3])
		      		else
		      		{
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[2].ratio + PosVoltAdjustFactorArray[2].bias;
                    }
                    /*else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[4])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[3].ratio + PosVoltAdjustFactorArray[3].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[5])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[4].ratio + PosVoltAdjustFactorArray[4].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[6])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[5].ratio + PosVoltAdjustFactorArray[5].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[7])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[6].ratio + PosVoltAdjustFactorArray[6].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[8])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[7].ratio + PosVoltAdjustFactorArray[7].bias;
                    }
                    else
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosVoltAdjustFactorArray[8].ratio + PosVoltAdjustFactorArray[8].bias;
                    }*/

					SideX->VavgNegDC = 0.0;

					//SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * IavgPosDCRatio + IavgPosDCBias;
					SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000);
					SideX->IavgNegDC = 0.0;
		      	}
				else
				{
				    SideX->VavgPosDC = 0.0;
	//			     SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * VavgNegDCRatio + VavgNegDCBias;
//					 if(-SideAControl.step[PulsePhase0].reference <= SmallCurrModeThreshold)
//					 	SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegSmallVoltAdjustFactor.ratio + NegSmallVoltAdjustFactor.bias;
//					 else
//					 	SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactor.ratio + NegVoltAdjustFactor.bias;
                    if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[1])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[0].ratio + NegVoltAdjustFactorArray[0].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[2])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[1].ratio + NegVoltAdjustFactorArray[1].bias;
                    }
                    //else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[3])
                    else
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[2].ratio + NegVoltAdjustFactorArray[2].bias;
                    }
                    /*else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[4])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[3].ratio + NegVoltAdjustFactorArray[3].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[5])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[4].ratio + NegVoltAdjustFactorArray[4].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[6])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[5].ratio + NegVoltAdjustFactorArray[5].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[7])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[6].ratio + NegVoltAdjustFactorArray[6].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[8])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[7].ratio + NegVoltAdjustFactorArray[7].bias;
                    }
                    else
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegVoltAdjustFactorArray[8].ratio + NegVoltAdjustFactorArray[8].bias;
                    }*/

					 SideX->IavgPosDC = 0.0;
					 //SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) *IavgNegDCRatio + IavgNegDCBias;
					 SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000);
				}
			}
			else
			{
				if(SideAControl.step[PulsePhase0].reference > 0)
				{
//					if(SideAControl.step[PulsePhase0].reference <= SmallCurrModeThreshold)
//		      			SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpSmallVoltAdjustFactor.ratio + PosCmpSmallVoltAdjustFactor.bias;
//					else
//						SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactor.ratio + PosCmpVoltAdjustFactor.bias;
                    if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[1])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[0].ratio + PosCmpVoltAdjustFactorArray[0].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[2])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[1].ratio + PosCmpVoltAdjustFactorArray[1].bias;
                    }
                    //else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[3])
                    else
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[2].ratio + PosCmpVoltAdjustFactorArray[2].bias;
                    }
                    /*else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[4])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[3].ratio + PosCmpVoltAdjustFactorArray[3].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[5])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[4].ratio + PosCmpVoltAdjustFactorArray[4].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[6])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[5].ratio + PosCmpVoltAdjustFactorArray[5].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[7])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[6].ratio + PosCmpVoltAdjustFactorArray[6].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[8])
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[7].ratio + PosCmpVoltAdjustFactorArray[7].bias;
                    }
                    else
                    {
                        SideX->VavgPosDC = (SideAControl.step[PulsePhase0].VloadPosSum/40000) * PosCmpVoltAdjustFactorArray[8].ratio + PosCmpVoltAdjustFactorArray[8].bias;
                    }*/

					SideX->VavgNegDC = 0.0;
					
//					if(SideAControl.step[PulsePhase0].reference <= SmallCurrModeThreshold)
//						SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispSmallCurrAdjustFactor.ratio + PosCmpDispSmallCurrAdjustFactor.bias;
//					else
//						SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactor.ratio + PosCmpDispCurrAdjustFactor.bias;
					if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[1])
					{
					    SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[0].ratio + PosCmpDispCurrAdjustFactorArray[0].bias;
					}
					else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[2])
                    {
                        SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[1].ratio + PosCmpDispCurrAdjustFactorArray[1].bias;
                    }
					//else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[3])
					else
					{
                        SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[2].ratio + PosCmpDispCurrAdjustFactorArray[2].bias;
                    }
                    /*else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[4])
                    {
                        SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[3].ratio + PosCmpDispCurrAdjustFactorArray[3].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[5])
                    {
                        SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[4].ratio + PosCmpDispCurrAdjustFactorArray[4].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[6])
                    {
                        SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[5].ratio + PosCmpDispCurrAdjustFactorArray[5].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[7])
                    {
                        SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[6].ratio + PosCmpDispCurrAdjustFactorArray[6].bias;
                    }
                    else if(SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[8])
                    {
                        SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[7].ratio + PosCmpDispCurrAdjustFactorArray[7].bias;
                    }
                    else
                    {
                        SideX->IavgPosDC = (SideAControl.step[PulsePhase0].IPossum/40000) * PosCmpDispCurrAdjustFactorArray[8].ratio + PosCmpDispCurrAdjustFactorArray[8].bias;
                    }*/

					SideX->IavgNegDC = 0.0;
				}
				else
				{		
					SideX->VavgPosDC = 0.0;
//					if(-SideAControl.step[PulsePhase0].reference <= SmallCurrModeThreshold)
//						SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpSmallVoltAdjustFactor.ratio + NegCmpSmallVoltAdjustFactor.bias;
//					else
//						SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactor.ratio + NegCmpVoltAdjustFactor.bias;
					if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[1])
					{
					    SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[0].ratio + NegCmpVoltAdjustFactorArray[0].bias;
					}
					else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[2])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[1].ratio + NegCmpVoltAdjustFactorArray[1].bias;
                    }
                    //else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[3])
					else
					{
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[2].ratio + NegCmpVoltAdjustFactorArray[2].bias;
                    }
                    /*else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[4])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[3].ratio + NegCmpVoltAdjustFactorArray[3].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[5])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[4].ratio + NegCmpVoltAdjustFactorArray[4].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[6])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[5].ratio + NegCmpVoltAdjustFactorArray[5].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[7])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[6].ratio + NegCmpVoltAdjustFactorArray[6].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[8])
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[7].ratio + NegCmpVoltAdjustFactorArray[7].bias;
                    }
                    else
                    {
                        SideX->VavgNegDC = (SideAControl.step[PulsePhase0].VloadNegSum/40000) * NegCmpVoltAdjustFactorArray[8].ratio + NegCmpVoltAdjustFactorArray[8].bias;
                    }*/

					SideX->IavgPosDC = 0.0;
//					if(-SideAControl.step[PulsePhase0].reference <= SmallCurrModeThreshold)
//						SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispSmallCurrAdjustFactor.ratio + NegCmpDispSmallCurrAdjustFactor.bias;
//					else
//						SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactor.ratio + NegCmpDispCurrAdjustFactor.bias;
					if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[1])
					{
					    SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[0].ratio + NegCmpDispCurrAdjustFactorArray[0].bias;
					}
					else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[2])
                    {
                        SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[1].ratio + NegCmpDispCurrAdjustFactorArray[1].bias;
                    }
                    //else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[3])
					else
					{
                        SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[2].ratio + NegCmpDispCurrAdjustFactorArray[2].bias;
                    }
                    /*else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[4])
                    {
                        SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[3].ratio + NegCmpDispCurrAdjustFactorArray[3].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[5])
                    {
                        SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[4].ratio + NegCmpDispCurrAdjustFactorArray[4].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[6])
                    {
                        SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[5].ratio + NegCmpDispCurrAdjustFactorArray[5].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[7])
                    {
                        SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[6].ratio + NegCmpDispCurrAdjustFactorArray[6].bias;
                    }
                    else if(-SideAControl.step[PulsePhase0].reference <= AdjustFactorSectionDivide[8])
                    {
                        SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[7].ratio + NegCmpDispCurrAdjustFactorArray[7].bias;
                    }
                    else
                    {
                        SideX->IavgNegDC = (SideAControl.step[PulsePhase0].INegsum/40000) * NegCmpDispCurrAdjustFactorArray[8].ratio + NegCmpDispCurrAdjustFactorArray[8].bias;
                    }*/
				}
			}
		}
      }
      else
      {
		  SideX->VavgPosDC = 0.0;
		  SideX->VavgNegDC = 0.0;
		  SideX->IavgPosDC = 0.0;
		  SideX->IavgNegDC = 0.0;

		  SideX->step[PulsePhase0].VloadPosSum = 0;
		  SideX->step[PulsePhase0].VloadNegSum = 0;
		  SideX->step[PulsePhase0].IPossum = 0;
		  SideX->step[PulsePhase0].INegsum = 0;

		  SideX->step[PulsePhase0].IPossum_temp = 0;
		  SideX->step[PulsePhase0].VloadPosSum_temp = 0;
		  SideX->step[PulsePhase0].INegsum_temp = 0;
		  SideX->step[PulsePhase0].VloadNegSum_temp = 0;
      }

}
/*******************************************************************************
Function name:  float GetLoadVoltAverage(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
float GetLoadVoltAverage(void)
{
  return(SideAControl.Vavg);

}

/*******************************************************************************
Function name:  float GetLoadCurrentAverage(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
float GetLoadCurrentAverage(void)
{
  return(SideAControl.Iavg);

}

/*********************************************************************
Function name:  AverageValueCal
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
float AverageValueCal(stAverageValue* output)
{
	float outvalue = 0.0f;

	if(output->Count)
	{
		outvalue = (output->Sum/output->Count);
		outvalue = outvalue * output->AdjustRatio;
	}
	else
	{
	   outvalue = 0.0;
	}

	return(outvalue);
}

/*********************************************************************
Function name:  sbOverLevelChk
Description:    
Calls:
Called By:
Parameters:     
Return:         
 *********************************************************************/
int16_t   sbOverLevelChk(int16 wCompareData, int16 wHighLever, int16 bFilter, int16 *pbCounter)
{
    	if(wCompareData > wHighLever)
		(*pbCounter)++;
    	else
		(*pbCounter)= 0;

    	if((*pbCounter) >= bFilter)
    	{
		(*pbCounter)= 0;
		return(pdTRUE);
    	}
    	else
    		return(pdFALSE);
}

/*********************************************************************
Function name:  sbUnderLevelChk
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
int16_t   sbUnderLevelChk(int16 wCompareData, int16 wLowLever, int16 bFilter, int16 *pbCounter)
{
    	if(wCompareData < wLowLever)
		(*pbCounter)++;
    	else
		(*pbCounter)= 0;

    	if((*pbCounter) >= bFilter)
    	{
		(*pbCounter)= 0;
		return(pdTRUE);
    	}
    	else
    		return(pdFALSE);
}

/*********************************************************************
Function name:  sbLoadPosVoltHighTimeChk
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
int16_t sbLoadPosVoltHighTimeChk(int16 bHighFilter)
{
    	
	if((GetPowerMode()==cStandbyMode)||(GetPowerMode()==cFaultMode)||(GetPowerMode()==cShutDownMode)||(GetPowerMode()==cPowerOnMode))
	{
		LoadVoltHighCnt= 0;
		return(pdFALSE);
	}
	else
	{
    		if(sbOverLevelChk(LoadVoltAvgPos,LoadHighLimitVolt,bHighFilter,&LoadVoltHighCnt)==pdTRUE)
    			return(pdTRUE);
			else
    			return(pdFALSE);
    	}
}

/*********************************************************************
Function name:  sbLoadNegVoltHighTimeChk
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
int16_t sbLoadNegVoltHighTimeChk(int16 bHighFilter)
{
    	
	if((GetPowerMode()==cStandbyMode)||(GetPowerMode()==cFaultMode)||(GetPowerMode()==cShutDownMode)||(GetPowerMode()==cPowerOnMode))
	{
		LoadVoltHighCnt= 0;
		return(pdFALSE);
	}
	else
	{
    		if(sbOverLevelChk(LoadVoltAvgNeg,LoadHighLimitVolt,bHighFilter,&LoadVoltHighCnt)==pdTRUE)
    			return(pdTRUE);
			else
    			return(pdFALSE);
    	}
}

/*********************************************************************
Function name:  sbLoadPosCurrHighTimeChk
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
int16_t sbLoadPosCurrHighTimeChk(int16 bHighFilter)
{
    	
	if((GetPowerMode()==cStandbyMode)||(GetPowerMode()==cFaultMode)||(GetPowerMode()==cShutDownMode)||(GetPowerMode()==cPowerOnMode))
	{
		LoadVoltHighCnt= 0;
		return(pdFALSE);
	}
	else
	{
    		if(sbOverLevelChk(LoadCurrAvgPos,LoadHighLimitCurr,bHighFilter,&LoadCurrHighCnt)==pdTRUE)
    			return(pdTRUE);
			else
    			return(pdFALSE);
    	}
}

/*********************************************************************
Function name:  sbLoadNegCurrHighTimeChk
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
int16_t sbLoadNegCurrHighTimeChk(int16 bHighFilter)
{
    	
	if((GetPowerMode()==cStandbyMode)||(GetPowerMode()==cFaultMode)||(GetPowerMode()==cShutDownMode)||(GetPowerMode()==cPowerOnMode))
	{
		LoadVoltHighCnt= 0;
		return(pdFALSE);
	}
	else
	{
    		if(sbOverLevelChk(LoadCurrAvgNeg,LoadHighLimitCurr,bHighFilter,&LoadCurrHighCnt)==pdTRUE)
    			return(pdTRUE);
			else
    			return(pdFALSE);
    	}
}

/*********************************************************************
Function name:  SetLoadVoltAvg
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void SetLoadVoltAvg(int16 Avg)
{
	LoadVoltAvg = Avg;

}

void SetLoadVoltPosAvg(int16 Avg)
{
	LoadVoltAvgPos = Avg;

}

void SetLoadVoltNegAvg(int16 Avg)
{
	LoadVoltAvgNeg = Avg;

}

/*********************************************************************
Function name:  SetLoadCurrAvg
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void SetLoadCurrAvg(int16 Avg)
{
	LoadCurrAvg = Avg;

}

void SetLoadCurrPosAvg(int16 Avg)
{
	LoadCurrAvgPos = Avg;

}

void SetLoadCurrNegAvg(int16 Avg)
{
	LoadCurrAvgNeg = Avg;

}

/*******************************************************************************
Function name: void	LoadAvgOverAction(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void	LoadAvgOverAction(void)
{
	//if(GetPowerMode() == cPulseMode)
	if(GetWaveForm() == Pulse)
	{
		LoadVoltPosAvg.compvalue = SideAControl.VavgPos;
		LoadCurrPosAvg.compvalue = SideAControl.IavgPos;	
		LoadVoltNegAvg.compvalue = fabsf(SideAControl.VavgNeg);	
		LoadCurrNegAvg.compvalue = fabsf(SideAControl.IavgNeg);

	}
	else
	{
		LoadVoltPosAvg.compvalue = SideAControl.VavgPosDC;
		LoadCurrPosAvg.compvalue = SideAControl.IavgPosDC;	
		LoadVoltNegAvg.compvalue = fabsf(SideAControl.VavgNegDC);	
		LoadCurrNegAvg.compvalue = fabsf(SideAControl.IavgNegDC);
	}


	sOverProtect(&LoadVoltPosAvg);
	if(GetLoadAvgVoltPosOverFlag() == pdTRUE)
	{
		WarnRegs.UnitSync.bit.LoadPosAvgOV = 1;
		sSetWarningCode(cWloadPosAvgOV);
	
	}
	else
	{
		WarnRegs.UnitSync.bit.LoadPosAvgOV = 0;
		sClrWarningCode(cWloadPosAvgOV);
	}

	sOverProtect(&LoadVoltNegAvg);
	if(GetLoadAvgVoltNegOverFlag() == pdTRUE)
	{
		WarnRegs.UnitSync.bit.LoadNegAvgOV = 1;
		sSetWarningCode(cWloadNegAvgOV);
	}
	else
	{
		WarnRegs.UnitSync.bit.LoadNegAvgOV = 0;
		sClrWarningCode(cWloadNegAvgOV);
	}

	sOverProtect(&LoadCurrPosAvg);
	if(GetLoadAvgCurrPosOverFlag() == pdTRUE)
	{
		WarnRegs.UnitSync.bit.LoadPosAvgOC = 1;
		sSetWarningCode(cWloadPosAvgOC);
	
	}
	else
	{
		WarnRegs.UnitSync.bit.LoadPosAvgOC = 0;
		sClrWarningCode(cWloadPosAvgOC);
	}

	sOverProtect(&LoadCurrNegAvg);
	if(GetLoadAvgCurrNegOverFlag() == pdTRUE)
	{
		WarnRegs.UnitSync.bit.LoadNegAvgOC = 1;
		sSetWarningCode(cWloadNegAvgOC);
	}
	else
	{
		WarnRegs.UnitSync.bit.LoadNegAvgOC = 0;
		sClrWarningCode(cWloadNegAvgOC);
	}

}

/*******************************************************************************
Function name: int GetLoadAvgVoltPosOverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int GetLoadAvgVoltPosOverFlag(void)
{
	return(LoadVoltPosAvg.Overflag);

}

/*******************************************************************************
Function name: int GetLoadAvgVoltPosOverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int GetLoadAvgVoltNegOverFlag(void)
{
	return(LoadVoltNegAvg.Overflag);

}

/*******************************************************************************
Function name: int GetLoadAvgCurrPosOverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int GetLoadAvgCurrPosOverFlag(void)
{
	return(LoadCurrPosAvg.Overflag);

}

/*******************************************************************************
Function name: int GetLoadAvgVoltPosOverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
int GetLoadAvgCurrNegOverFlag(void)
{
	return(LoadCurrNegAvg.Overflag);

}
/*******************************************************************************
Function name: int GetLoadAvgVoltPosOverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
Uint16 GetHwExtLoadOC(void)
{
	return(FaultRegs.HWfaultLoad.bit.HwExtLoadOC);

}

/*******************************************************************************
Function name: int GetLoadAvgVoltPosOverFlag(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void ClrHwExtLoadOC(void)
{
	 FaultRegs.HWfaultLoad.bit.HwExtLoadOC = 0;

}

/*********************************************************************
Function name:  void RUN_LED_OH(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void RUN_LED_OH(void)
{
	if((GetPowerMode() == cPulseMode) || (GetPowerMode() == cDCMode) || (GetPowerMode() == cOpenLoopMode))
	{
		LED_RUN_LOW();
	}
	else
	{
		LED_RUN_High();
	}
}

/*********************************************************************
Function name:  void FAULT_OT_LED_OH(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void FAULT_OT_LED_OH(void)
{
	int ledstatus = cLoadLedOff;
	int delaytime = 0;
	
	

	if(GetHwExtLoadOC())
	{
		ledstatus = cLoadLedOn;
	}
	else if((FaultRegs.CabinetSync.all != 0) || (FaultRegs.HWfaultLoad.all != 0)
	     ||(FaultRegs.HWfaultLoadAvg.all != 0)||(FaultRegs.HWfaultTemp.all != 0)
	     ||(FaultRegs.MOduleSync.all != 0)||(FaultRegs.UnitSync.all != 0))
	{
		ledstatus = cLoadLedBlinkFast;
		delaytime = cLoadTimer300MS;
	}
	else if((WarnRegs.TempWarn.all != 0)||(WarnRegs.UnitSync.all != 0))
	{
		ledstatus = cLoadLedBlinkSlow;
		delaytime = cLoadTimer1S;
	}


	if(ledstatus == cLoadLedOff)
	{
		LED_OH_High();
	}
	else if(ledstatus == cLoadLedOn)
	{
		LED_OH_LOW();
	}
	else
	{
		LED_OH_Cnt++;
		if(LED_OH_Cnt==delaytime)
		{
			LED_OH_High();
		}
		else if(LED_OH_Cnt>=delaytime*2)
		{
			LED_OH_LOW();
			LED_OH_Cnt = 0;
		}
	}

//
//	if(WarnRegs.TempWarn.all != 0)
//	{
//		LED_OH_High();	
//	}
//	else if((FaultRegs.CabinetSync.all != 0) || (FaultRegs.HWfaultLoad.all != 0)
//		     ||(FaultRegs.HWfaultLoadAvg.all != 0)||(FaultRegs.HWfaultTemp.all != 0)
//		     ||(FaultRegs.MOduleSync.all != 0)||(FaultRegs.UnitSync.all != 0))
//	{
//	      LED_OH_Cnt++;
//		   switch (LED_OH_Cnt)
//		   {
//		        case cLoadTimer1S:
//			     
//				LED_OH_High();
//
//		   	    break;
//
//				case cLoadTimer2S:
//			     
//				LED_OH_LOW();
//				
//				LED_OH_Cnt = 0;
//
//		   	    break;
//
//			default:
//
//           		break;
//
//				
//		   }
//				
//
//	}
//	else
//	{
//		LED_OH_LOW();
//	}
}
/*********************************************************************
Function name:  void AmbientTempCal(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void AmbientTempCal(void)
{

    AmbientTempSum = FILTER(5, (int32_t)AmbientTemperature,  AmbientTempSum);
    AmbientTempADC = AmbientTempSum >> 16;
    AmbientDegreeC = (int16_t)(GetEndValue(TEMP_10K_TBLTE, TEMP_10K_LENGTH, AmbientTempADC)-10000);

}

/*********************************************************************
Function name:  void SinkTempCal(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SinkTempCal(void)
{
    SinkTempSum = FILTER(5, (int32_t)SinkTemperature,  SinkTempSum);
    SinkTempADC = SinkTempSum >> 16;
    SinkDegreeC = (int16_t)(GetEndValue(TEMP_10K_TBLTE, TEMP_10K_LENGTH, SinkTempADC)-10000);

}
/*********************************************************************
Function name:  uint16_t GetEndValue(const uint16_t * ptbl, uint16_t tblsize, uint16_t dat)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
uint16_t GetEndValue(const uint16_t * ptbl, uint16_t tblsize, uint16_t dat)
{
    uint16_t i, t_u16linenum;
    uint32_t x1 = 0, y1 = 0, x2 = 1, y2 = 1;
    uint16_t t_u16tmpa, t_u16tmpb;
    int32_t  t_i32tmpa, t_i32tmpb;
    uint32_t k, b;
    int32_t  ret;
    const uint16_t * p ;
    p = ptbl;

    t_u16linenum = tblsize - 1;
    for (i = 0; i < tblsize - 2; i = i + 2)
    {
        t_u16tmpa = p[i];
        t_u16tmpb = p[i + 2];

        if (((dat >= t_u16tmpa) && (dat <= t_u16tmpb))
         || ((dat <= t_u16tmpa) && (dat >= t_u16tmpb))
           )
        {
            x1 = t_u16tmpa;
            x2 = t_u16tmpb;
            y1 = p[i + 1];
            y2 = p[i + 3];
            break;
        }
    }

    if (i >= t_u16linenum - 1)
    {
        p = ptbl;
        t_u16tmpa = p[0];
        t_u16tmpb = p[tblsize - 2];

        if (t_u16tmpa <= t_u16tmpb)
        {
            if (dat >= t_u16tmpb)
            {
                t_u16tmpa = p[tblsize - 1];
            }
            else
            {
                t_u16tmpa = p[1];
            }
        }
        else
        {
            if (dat >= t_u16tmpa)
            {
                t_u16tmpa = p[1];
            }
            else
            {
                t_u16tmpa =  p[tblsize - 1];
            }
        }
        return t_u16tmpa;
    }
    else
    {
        if (x2 < x1)
        {
            ret = x2;
            x2  = x1;
            x1  = ret;
            ret = y2;
            y2  = y1;
            y1  = ret;
        }

        if (y2 >= y1)
        {
            t_i32tmpa = y1 * x2;
            t_i32tmpb = y2 * x1;
            ret = dat;
            k = y2 - y1;
            ret = ret * k;
            if (t_i32tmpa >= t_i32tmpb)
            {
                b = t_i32tmpa - t_i32tmpb;
                ret = ret + b;
            }
            else
            {
                b = t_i32tmpb - t_i32tmpa;
                ret = ret  - b;
            }
            ret = ret / (x2 - x1);
        }
        else
        {
            t_i32tmpa = y1 * x2;
            t_i32tmpb = y2 * x1;
            ret = dat;
            k = y1 - y2;
            ret = ret * k;
            b = t_i32tmpa - t_i32tmpb;
            ret = b - ret;
            ret = ret / (x2 - x1);
        }
        return (ret & 0xffff);
    }
}

/*******************************************************************************
Function name: DisplayVoltAndCurrFilter
Description:   屏幕上显示值滤波
Calls:
Called By:
Parameters:    void
Return:        void
*******************************************************************************/
void DisplayVoltAndCurrFilter(void)
{
    static uint16_t IavgPosFilterFlag = 0;
    static uint16_t IavgNegFilterFlag = 0;
    static uint16_t VavgPosFilterFlag = 0;
    static uint16_t VavgNegFilterFlag = 0;
    static uint16_t IavgPosDCFilterFlag = 0;
    static uint16_t IavgNegDCFilterFlag = 0;
    static uint16_t VavgPosDCFilterFlag = 0;
    static uint16_t VavgNegDCFilterFlag = 0;
    uint16_t Temp1 = 0;
    uint16_t i = 0;

    //if((GetPWMstatus() == PWMwaitSts) || (SideALargePWMstatus == cPWMwaitSts))
    if(GetPWMstatus() == PWMwaitSts)
    {
        IavgPosFilter = 0.0f;
        IavgNegFilter = 0.0f;
        VavgPosFilter = 0.0f;
        VavgNegFilter = 0.0f;
        IavgPosDCFilter = 0.0f;
        IavgNegDCFilter = 0.0f;
        VavgPosDCFilter = 0.0f;
        VavgNegDCFilter = 0.0f;

        IavgPosFilterFlag = 0;
        IavgNegFilterFlag = 0;
        VavgPosFilterFlag = 0;
        VavgNegFilterFlag = 0;
        IavgPosDCFilterFlag = 0;
        IavgNegDCFilterFlag = 0;
        VavgPosDCFilterFlag = 0;
        VavgNegDCFilterFlag = 0;

        for(Temp1 = 1; Temp1 <= 20; Temp1++)
        {
            IavgPhaseFilter[Temp1] = 0.0f;
        }

        IavgPosFilterDisPlay = 0.0f;
        IavgNegFilterDisPlay = 0.0f;
        IavgPosDCFilterDisPlay = 0.0f;
        IavgNegDCFilterDisPlay = 0.0f;

        data_num1 = 0;
        data_num2 = 0;
        data_num3 = 0;
        data_num4 = 0;
        for(i = 0; i < window_size1; i++)
        {
            buffer1[i] = 0;
        }
        for(i = 0; i < window_size2; i++)
        {
            buffer2[i] = 0;
        }
        for(i = 0; i < window_size3; i++)
        {
            buffer3[i] = 0;
        }
        for(i = 0; i < window_size4; i++)
        {
            buffer4[i] = 0;
        }
    }
    else if(GetPWMstatus() == PWMrunSts)
    {
        //1
        if(SideAControl.IavgPos != 0.0f)
        {
            if(IavgPosFilterFlag == 0)
            {
                IavgPosFilterFlag = 1;
                IavgPosFilter = fabsf(SideAControl.IavgPos);
            }
            else if(IavgPosFilterFlag == 1)
            {
                IavgPosFilter = 0.9f * IavgPosFilter + 0.1f * fabsf(SideAControl.IavgPos);
            }

            if(fabsf(PulsePosAverageValueSet - IavgPosFilter) <= 500.0f)
            {
                //IavgPosFilter = PulsePosAverageValueSet;
            }
        }
        else
        {
            IavgPosFilterFlag = 0;
            IavgPosFilter = 0.0f;
        }

        //2
        if(SideAControl.IavgNeg != 0.0f)
        {
            if(IavgNegFilterFlag == 0)
            {
                IavgNegFilterFlag = 1;
                IavgNegFilter = fabsf(SideAControl.IavgNeg);
            }
            else if(IavgNegFilterFlag == 1)
            {
                IavgNegFilter = 0.95f * IavgNegFilter + 0.05f * fabsf(SideAControl.IavgNeg);
            }

            if(fabsf(PulseNegAverageValueSet - IavgNegFilter) <= 500.0f)
            {
                //IavgNegFilter = PulseNegAverageValueSet;
            }
        }
        else
        {
            IavgNegFilterFlag = 0;
            IavgNegFilter = 0.0f;
        }

        //3
        if(SideAControl.VavgPos != 0.0f)
        {
            if(VavgPosFilterFlag == 0)
            {
                VavgPosFilterFlag = 1;
                VavgPosFilter = fabsf(SideAControl.VavgPos);
            }
            else if(VavgPosFilterFlag == 1)
            {
                VavgPosFilter = 0.9f * VavgPosFilter + 0.1f * fabsf(SideAControl.VavgPos);
            }
        }
        else
        {
            VavgPosFilterFlag = 0;
            VavgPosFilter = 0.0f;
        }

        //4
        if(SideAControl.VavgNeg != 0.0f)
        {
            if(VavgNegFilterFlag == 0)
            {
                VavgNegFilterFlag = 1;
                VavgNegFilter = fabsf(SideAControl.VavgNeg);
            }
            else if(VavgNegFilterFlag == 1)
            {
                VavgNegFilter = 0.9f * VavgNegFilter + 0.1f * fabsf(SideAControl.VavgNeg);
            }
        }
        else
        {
            VavgNegFilterFlag = 0;
            VavgNegFilter = 0.0f;
        }

        //5
        if(SideAControl.IavgPosDC != 0.0f)
        {
            if(IavgPosDCFilterFlag == 0)
            {
                IavgPosDCFilterFlag = 1;
                IavgPosDCFilter = fabsf(SideAControl.IavgPosDC);
            }
            else if(IavgPosDCFilterFlag == 1)
            {
                IavgPosDCFilter = 0.9f * IavgPosDCFilter + 0.1f * fabsf(SideAControl.IavgPosDC);
            }

            if(fabsf(fabsf(SideAControl.step[0].reference) - IavgPosDCFilter) <= 500.0f)
            {
                //IavgPosDCFilter = fabsf(SideAControl.step[0].reference);
            }
        }
        else
        {
            IavgPosDCFilterFlag = 0;
            IavgPosDCFilter = 0.0f;
        }

        //6
        if(SideAControl.IavgNegDC != 0.0f)
        {
            if(IavgNegDCFilterFlag == 0)
            {
                IavgNegDCFilterFlag = 1;
                IavgNegDCFilter = fabsf(SideAControl.IavgNegDC);
            }
            else if(IavgNegDCFilterFlag == 1)
            {
                IavgNegDCFilter = 0.9f * IavgNegDCFilter + 0.1f * fabsf(SideAControl.IavgNegDC);
            }

            if(fabsf(fabsf(SideAControl.step[0].reference) - IavgNegDCFilter) <= 500.0f)
            {
                //IavgNegDCFilter = fabsf(SideAControl.step[0].reference);
            }
        }
        else
        {
            IavgNegDCFilterFlag = 0;
            IavgNegDCFilter = 0.0f;
        }

        //7
        if(SideAControl.VavgPosDC != 0.0f)
        {
            if(VavgPosDCFilterFlag == 0)
            {
                VavgPosDCFilterFlag = 1;
                VavgPosDCFilter = SideAControl.VavgPosDC;
            }
            else if(VavgPosDCFilterFlag == 1)
            {
                VavgPosDCFilter = 0.9f * VavgPosDCFilter + 0.1f * SideAControl.VavgPosDC;
            }
        }
        else
        {
            VavgPosDCFilterFlag = 0;
            VavgPosDCFilter = 0.0f;
        }

        //8
        if(SideAControl.VavgNegDC != 0.0f)
        {
            if(VavgNegDCFilterFlag == 0)
            {
                VavgNegDCFilterFlag = 1;
                VavgNegDCFilter = SideAControl.VavgNegDC;
            }
            else if(VavgNegDCFilterFlag == 1)
            {
                VavgNegDCFilter = 0.9f * VavgNegDCFilter + 0.1f * SideAControl.VavgNegDC;
            }
        }
        else
        {
            VavgNegDCFilterFlag = 0;
            VavgNegDCFilter = 0.0f;
        }

        //每段电流滤波值（20段）
        if(GetWaveForm() == Pulse)
        {
            for(Temp1 = 1; Temp1 <= 20; Temp1++)
            {
                if(SideAControl.IavgPhase[Temp1] == 0.0f)
                {
                    IavgPhaseFilter[Temp1] = 0.0f;
                }
                else
                {
                    IavgPhaseFilter[Temp1] = 0.9f * IavgPhaseFilter[Temp1] + 0.1f * SideAControl.IavgPhase[Temp1];
                }
            }
        }
        else
        {
            for(Temp1 = 1; Temp1 <= 20; Temp1++)
            {
                IavgPhaseFilter[Temp1] = 0.0f;
            }
        }

        /*
        if(fabsf(IavgPosFilter - PulsePosAverageValueSet) <=  (PulsePosAverageValueSet * 0.05f))
        {
            IavgPosFilterDisPlay = PulsePosAverageValueSet;
        }
        else
        {
            IavgPosFilterDisPlay = IavgPosFilter;
        }

        if(fabsf(IavgNegFilter - PulseNegAverageValueSet) <=  (PulseNegAverageValueSet * 0.05f))
        {
            IavgNegFilterDisPlay = PulseNegAverageValueSet;
        }
        else
        {
            IavgNegFilterDisPlay = IavgNegFilter;
        }

        if(fabsf(IavgPosDCFilter - fabsf(SideAControl.step[0].reference)) <=  (fabsf(SideAControl.step[0].reference) * 0.05f))
        {
            IavgPosDCFilterDisPlay = fabsf(SideAControl.step[0].reference);
        }
        else
        {
            IavgPosDCFilterDisPlay = IavgPosDCFilter;
        }

        if(fabsf(IavgNegDCFilter - fabsf(SideAControl.step[0].reference)) <=  (fabsf(SideAControl.step[0].reference) * 0.05f))
        {
            IavgNegDCFilterDisPlay = fabsf(SideAControl.step[0].reference);
        }
        else
        {
            IavgNegDCFilterDisPlay = IavgNegDCFilter;
        }*/

        IavgPosFilterDisPlay = sliding_average_filter1(IavgPosFilter);
        IavgNegFilterDisPlay = sliding_average_filter2(IavgNegFilter);
        IavgPosDCFilterDisPlay = sliding_average_filter3(IavgPosDCFilter);
        IavgNegDCFilterDisPlay = sliding_average_filter4(IavgNegDCFilter);
    }
    else
    {
        //do nothing
    }
}

/*******************************************************************************
Function name: CalculationPulsePosAndNegAverageValue
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void CalculationPulsePosAndNegAverageValue(void)
{
    uint16_t temp = 0;
    float PulsePosAverageValueTemp = 0.0f;
    float PulseNegAverageValueTemp = 0.0f;
    float PulsePosTotalTimeTemp = 0.0f;
    float PulseNegTotalTimeTemp = 0.0f;

    for(temp = 1;temp <= PulsePhaseNum; temp++)
    {
        if(SideAControl.step[temp].reference > 0.0f)
        {
            PulsePosAverageValueTemp += SideAControl.step[temp].reference * (float)(SideAControl.step[temp].timecountset);
            PulsePosTotalTimeTemp += (float)(SideAControl.step[temp].timecountset);
        }
        else if(SideAControl.step[temp].reference < 0.0f)
        {
            PulseNegAverageValueTemp += fabsf(SideAControl.step[temp].reference) * (float)(SideAControl.step[temp].timecountset);
            PulseNegTotalTimeTemp += (float)(SideAControl.step[temp].timecountset);
        }
        else
        {
            //do nothing
        }
    }

    if(PulsePosTotalTimeTemp != 0.0f)
    {
        PulsePosAverageValueSet = PulsePosAverageValueTemp / PulsePosTotalTimeTemp;
    }
    else
    {
        PulsePosAverageValueSet = 0.0f;
    }

    if(PulseNegTotalTimeTemp != 0.0f)
    {
        PulseNegAverageValueSet = PulseNegAverageValueTemp / PulseNegTotalTimeTemp;
    }
    else
    {
        PulseNegAverageValueSet = 0.0f;
    }
}

/*******************************************************************************
Function name: DCModeRMSLoop
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void DCModeRMSLoop(void)
{
    if((GetPWMstatus() == PWMrunSts) && (GetWaveForm() == DC) && (AdjustMode == 0))
    {
        DCModeRMSLoopFreqCnt++;
        if(DCModeRMSLoopFreqCnt > 3)
        {
            DCModeRMSLoopFreqCnt = 0;
        }

        if(DCModeRMSLoopFreqCnt == 3)
        {
            if(SideAControl.step[0].reference > 0.0f)
            {
                IavgPosDCRMSLoopErr = fabsf(SideAControl.step[0].reference) - IavgPosDCFilter;
                if(IavgPosDCRMSLoopErr > 50.0f)
                {
                    IavgPosDCRMSLoopErr = 50.0f;
                }
                else if(IavgPosDCRMSLoopErr < -50.0f)
                {
                    IavgPosDCRMSLoopErr = -50.0f;
                }

                IavgPosDCRMSLoopInt += IavgPosDCRMSLoopErr * 0.015f;
                if(IavgPosDCRMSLoopInt > 300.0f)
                {
                    IavgPosDCRMSLoopInt = 300.0f;
                }
                else if(IavgPosDCRMSLoopInt < -300.0f)
                {
                    IavgPosDCRMSLoopInt = -300.0f;
                }
                IavgPosDCRMSLoopOut = IavgPosDCRMSLoopErr * 0.15f + IavgPosDCRMSLoopInt;

                IavgNegDCRMSLoopInt = 0.0f;
            }
            else if(SideAControl.step[0].reference < 0.0f)
            {
                IavgNegDCRMSLoopErr = fabsf(SideAControl.step[0].reference) - IavgNegDCFilter;
                if(IavgNegDCRMSLoopErr > 50.0f)
                {
                    IavgNegDCRMSLoopErr = 50.0f;
                }
                else if(IavgNegDCRMSLoopErr < -50.0f)
                {
                    IavgNegDCRMSLoopErr = -50.0f;
                }

                IavgNegDCRMSLoopInt += IavgNegDCRMSLoopErr * 0.015f;
                if(IavgNegDCRMSLoopInt > 300.0f)
                {
                    IavgNegDCRMSLoopInt = 300.0f;
                }
                else if(IavgNegDCRMSLoopInt < -300.0f)
                {
                    IavgNegDCRMSLoopInt = -300.0f;
                }
                IavgNegDCRMSLoopOut = IavgNegDCRMSLoopErr * 0.15f + IavgNegDCRMSLoopInt;

                IavgPosDCRMSLoopInt = 0.0f;
            }
            else
            {
                IavgPosDCRMSLoopErr = 0.0f;
                IavgPosDCRMSLoopInt = 0.0f;
                IavgNegDCRMSLoopErr = 0.0f;
                IavgNegDCRMSLoopInt = 0.0f;
                IavgPosDCRMSLoopOut = 0.0f;
                IavgNegDCRMSLoopOut = 0.0f;
            }
        }
    }
    else
    {
        IavgPosDCRMSLoopErr = 0.0f;
        IavgPosDCRMSLoopInt = 0.0f;
        IavgNegDCRMSLoopErr = 0.0f;
        IavgNegDCRMSLoopInt = 0.0f;
        IavgPosDCRMSLoopOut = 0.0f;
        IavgNegDCRMSLoopOut = 0.0f;
        DCModeRMSLoopFreqCnt = 0;
    }
}


/*******************************************************************************
Function name: PulseModeRMSLoop
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*******************************************************************************/
void PulseModeRMSLoop(void)
{
    if((GetPWMstatus() == PWMrunSts) && (GetWaveForm() == Pulse) && (AdjustMode == 0))
    {
        PulseModeRMSLoopFreqCnt++;
        if(PulseModeRMSLoopFreqCnt > 8)
        {
            PulseModeRMSLoopFreqCnt = 0;
        }

        if(PulseModeRMSLoopFreqCnt == 8)
        {
            {
                IavgPosRMSLoopErr = PulsePosAverageValueSet - IavgPosFilter;
                if(IavgPosRMSLoopErr > 100.0f)
                {
                    IavgPosRMSLoopErr = 100.0f;
                }
                else if(IavgPosRMSLoopErr < -100.0f)
                {
                    IavgPosRMSLoopErr = -100.0f;
                }

                IavgPosRMSLoopInt += IavgPosRMSLoopErr * 0.015f;
                if(IavgPosRMSLoopInt > 300.0f)
                {
                    IavgPosRMSLoopInt = 300.0f;
                }
                else if(IavgPosRMSLoopInt < -300.0f)
                {
                    IavgPosRMSLoopInt = -300.0f;
                }
                IavgPosRMSLoopOut = IavgPosRMSLoopErr * 0.15f + IavgPosRMSLoopInt;
            }

            {
                IavgNegRMSLoopErr = PulseNegAverageValueSet - IavgNegFilter;
                if(IavgNegRMSLoopErr > 100.0f)
                {
                    IavgNegRMSLoopErr = 100.0f;
                }
                else if(IavgNegRMSLoopErr < -100.0f)
                {
                    IavgNegRMSLoopErr = -100.0f;
                }

                IavgNegRMSLoopInt += IavgNegRMSLoopErr * 0.015f;
                if(IavgNegRMSLoopInt > 300.0f)
                {
                    IavgNegRMSLoopInt = 300.0f;
                }
                else if(IavgNegRMSLoopInt < -300.0f)
                {
                    IavgNegRMSLoopInt = -300.0f;
                }
                IavgNegRMSLoopOut = IavgNegRMSLoopErr * 0.15f + IavgNegRMSLoopInt;
            }
        }
    }
    else
    {
        IavgPosRMSLoopErr = 0.0f;
        IavgNegRMSLoopErr = 0.0f;
        IavgPosRMSLoopInt = 0.0f;
        IavgNegRMSLoopInt = 0.0f;
        IavgPosRMSLoopOut = 0.0f;
        IavgNegRMSLoopOut = 0.0f;
        PulseModeRMSLoopFreqCnt = 0;
    }
}

/*********************************************************************
Function name:
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
/*********************** 滑动窗口滤波函数 ****************************/
float sliding_average_filter1(float value1)
{
  float output1 = 0;

  if (data_num1 < window_size1) //不满窗口，先填充
  {
    buffer1[data_num1++] = value1;
    output1 = value1; //返回相同的值
  }
  else
  {
    int i = 0;
    float sum = 0;

    //memcpy(&buffer1[0], &buffer1[1], (window_size1- 1) * 4); //将1之后的数据移到0之后，即移除头部
    for(i = 0; i < (window_size1 - 1); i++)
    {
        buffer1[i] = buffer1[i+1];
		sum += buffer1[i];							//每一次都计算，可以避免累计浮点计算误差
    }
    buffer1[window_size1 - 1] = value1;                       //即添加尾部

    sum += buffer1[window_size1 - 1];

    output1 = sum / window_size1;
  }

  return output1;
}

/////
/*********************** 滑动窗口滤波函数 ****************************/
float sliding_average_filter2(float value2)
{
  float output2 = 0;

  if (data_num2 < window_size2) //不满窗口，先填充
  {
    buffer2[data_num2++] = value2;
    output2 = value2; //返回相同的值
  }
  else
  {
    int i = 0;
    float sum = 0;

    //memcpy(&buffer2[0], &buffer2[1], (window_size2- 1) * 4); //将1之后的数据移到0之后，即移除头部
    for(i = 0; i < (window_size2 - 1); i++)
    {
        buffer2[i] = buffer2[i+1];
		sum += buffer2[i];			//每一次都计算，可以避免累计浮点计算误差
    }
    buffer2[window_size2 - 1] = value2;                       //即添加尾部

    sum += buffer2[window_size2 - 1];

    output2 = sum / window_size2;
  }

  return output2;
}


/////
/*********************** 滑动窗口滤波函数 ****************************/
float sliding_average_filter3(float value3)
{
  float output3 = 0;

  if (data_num3 < window_size3) //不满窗口，先填充
  {
    buffer3[data_num3++] = value3;
    output3 = value3; //返回相同的值
  }
  else
  {
    int i = 0;
    float sum = 0;

    //memcpy(&buffer3[0], &buffer3[1], (window_size3- 1) * 4); //将1之后的数据移到0之后，即移除头部
    for(i = 0; i < (window_size3 - 1); i++)
    {
        buffer3[i] = buffer3[i+1];
		sum += buffer3[i];			//每一次都计算，可以避免累计浮点计算误差
    }
    buffer3[window_size3 - 1] = value3;                       //即添加尾部

    sum += buffer3[window_size3 - 1];

    output3 = sum / window_size3;
  }

  return output3;
}
////////////
float sliding_average_filter4(float value4)
{
  float output4 = 0;

  if (data_num4 < window_size4) //不满窗口，先填充
  {
    buffer4[data_num4++] = value4;
    output4 = value4; //返回相同的值
  }
  else
  {
    int i = 0;
    float sum = 0;

    //memcpy(&buffer4[0], &buffer4[1], (window_size4- 1) * 4); //将1之后的数据移到0之后，即移除头部
    for(i = 0; i < (window_size4 - 1); i++)
    {
        buffer4[i] = buffer4[i+1];
		sum += buffer4[i];		//每一次都计算，可以避免累计浮点计算误差
    }
    buffer4[window_size4 - 1] = value4;                       //即添加尾部

    sum += buffer4[window_size4 - 1];

    output4 = sum / window_size4;
  }

  return output4;
}
