/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : SuperTask.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define SuperTask_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "Initial.h"
#include "Interrupt.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "SuperTask.h"
#include "OsConfig.h"
#include "LoadTask.h"
#include "fault.h"

/********************************************************************************
*const define                               *
********************************************************************************/


/********************************************************************************
* Variable declaration                              *
********************************************************************************/



/********************************************************************************
* function declaration                              *
********************************************************************************/
void redLedToggle(void)
{
  static uint32_t counter = 0;
  counter++;
  GPIO_writePin(DEVICE_GPIO_PIN_LED1, counter & 1);
}

/*********************************************************************
Function name:  SuperTaskInit
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SuperTaskInit(void)
{
    SetPowerMode(cPowerOnMode);

    //LED_RUN_High();
    LED_RUN_LOW();
}

/*********************************************************************
Function name:  SuperTask
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SuperTask(void * pvParameters)
{
//  SetPowerMode(cPowerOnMode);
//  //LED_RUN_High();
//	LED_RUN_LOW();

    //while(pdTRUE)
    {

        if(GetPowerMode() == cPowerOnMode)
        {
            sPowerOnMode();
        }
        else if(GetPowerMode() == cSelfCheckMode)
        {
			sSelfCheckMode();
        }

        else if(GetPowerMode() == cStandbyMode)
        {
			sStandbyMode();

        }

		else if(GetPowerMode() == cPulseMode)
		{
			sPulseMode();

		
		}
		else if(GetPowerMode() == cDCMode)
		{
			sDcMode();

		
		}
		else if(GetPowerMode() == cOpenLoopMode)
		{
		    sOpenLoopMode();


		}


        else if(GetPowerMode() == cFaultMode)
        {
        	sFaultMode();

        }

		else if(GetPowerMode() == cShutDownMode)
        {
        	sShutDownMode();

        }


        else
        {
            SetPowerMode(cPowerOnMode);
        }
 

    }
}
/*********************************************************************
Function name:  sPowerOnMode
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sPowerOnMode(void)
{
	uint32_t event;
	//BaseType_t err;
	static uint32_t SuperTempCnt = 0;

	//while(pdTRUE)
	{
		//err = xSemaphoreTake( xSemaphoreSuper, (TickType_t)Tick10mS );
		event = OSEventPend(cPrioSuper);
        if(event != 0)
		{
			
			if(OSEventConfirm(event,eSuperFault))
            {
            	SideALargePWMOFF();
				SetPrePowerMode(cPowerOnMode);
				SetPowerMode(cFaultMode);
                SuperTempCnt = 0;
				return;

            }
			if(OSEventConfirm(event,eSuperTurnOff))
			{
				SideALargePWMOFF();
				SetPrePowerMode(cPowerOnMode);
				SetPowerMode(cShutDownMode);
                SuperTempCnt = 0;
				return;
			
			}

			
			if(OSEventConfirm(event,eSuperTest))
			{

			}

           
			
		}
		else//eSuperTimer
		{
		    //GpioDataRegs.GPATOGGLE.bit.GPIO8 = 1;
		    SuperTempCnt++;
			if(SuperTempCnt >= cSuperTimer1s)
			{
				SuperTempCnt = 0;
				SetPowerMode(cStandbyMode);
				return;
			
			}
		}
	}

}

/*********************************************************************
Function name:  sSelfCheckMode
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sSelfCheckMode(void)
{
	uint32_t event;
    //BaseType_t err;

    //while(pdTRUE)
    {
        //err = xSemaphoreTake( xSemaphoreSuper, (TickType_t)Tick10mS );
        event = OSEventPend(cPrioSuper);
        if(event != 0)
        {

			if(OSEventConfirm(event,eSuperFault))
            {
            	SideALargePWMOFF();
				SetPrePowerMode(cSelfCheckMode);
				SetPowerMode(cFaultMode);
				return;

            }
			if(OSEventConfirm(event,eSuperTurnOff))
			{
				SideALargePWMOFF();
				SetPrePowerMode(cSelfCheckMode);
				SetPowerMode(cShutDownMode);
				return;
			
			}




        }
        else//eSuperTimer
        {

        }
    }

}

/*********************************************************************
Function name:  sStandbyMode
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sStandbyMode(void)
{
	uint32_t event;
	static uint32_t first = 1;
    //BaseType_t err;

    //while(pdTRUE)
    {
        //err = xSemaphoreTake( xSemaphoreSuper, (TickType_t)Tick10mS );
        event = OSEventPend(cPrioSuper);
        if(event != 0)
        {
			first = 0;

			if(OSEventConfirm(event,eSuperFault))
            {
            	SideALargePWMOFF();
				SetPrePowerMode(cStandbyMode);
				SetPowerMode(cFaultMode);
                first = 1;
				return;

            }
			if(OSEventConfirm(event,eSuperTurnOff))
			{
				SideALargePWMOFF();
				SetPrePowerMode(cStandbyMode);
				SetPowerMode(cShutDownMode);
                first = 1;
				return;
			
			}

			if(OSEventConfirm(event,eSuperTurnOn))
			{

			    
				if(GetBoardType() == UnitCB)
				{
				
					if(GetControlMode() == CloseLoop)
					{
						if(GetWaveForm() == DC)
						{
							SetPowerMode(cDCMode);
							SetPulsePhase(PulsePhase0);
			                first = 1;
						}
						else if(GetWaveForm() == Pulse)
						{
							SetPowerMode(cPulseMode);
							SetPulsePhase(PulsePhase1);
			                first = 1;
						}
						else
						{
						
						}
					
					}
					else if(GetControlMode() == OpenLoop)
					{
						SetPowerMode(cOpenLoopMode);
		                first = 1;
					}
					else
					{
					
					}
//					if(GetModuleMode() == Single)
						SetSideALargePWMstatus(PWMrunSts);
					return;
					
				}
			
			}



        }
        else//eSuperTimer
        {
			if((first == 0) || (GetPrePowerMode() == cShutDownMode))
			{
				first = 0;
				
			    if(GetBoardType() == UnitCB)
                {

                    if(GetControlMode() == CloseLoop)
                    {
                        if(GetWaveForm() == DC)
                        {
                            SetPowerMode(cDCMode);
							SetPulsePhase(PulsePhase0);
			                first = 1;
                        }
                        else if(GetWaveForm() == Pulse)
                        {
                            SetPowerMode(cPulseMode);
							SetPulsePhase(PulsePhase1);
			                first = 1;
                        }
                        else
                        {

                        }

                    }
                    else if(GetControlMode() == OpenLoop)
                    {
                        SetPowerMode(cOpenLoopMode);
                        first = 1;
                    }
                    else
                    {

                    }
//					if(GetModuleMode() == Single)
                    	SetSideALargePWMstatus(PWMrunSts);
                    return;

                }
			}

        }
    }

}

/*********************************************************************
Function name:  sPulseMode
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sPulseMode(void)
{
	uint32_t event;
    //BaseType_t err;

    //while(pdTRUE)
    {
        //err = xSemaphoreTake( xSemaphoreSuper, (TickType_t)Tick10mS );
        event = OSEventPend(cPrioSuper);
        if(event != 0)
        {

			if(OSEventConfirm(event,eSuperFault))
            {
            	SideALargePWMOFF();
				 SetSideALargePWMstatus(PWMwaitSts);
				SetPrePowerMode(cPulseMode);
				SetPowerMode(cFaultMode);
				return;

            }
			if(OSEventConfirm(event,eSuperTurnOff))
			{
//				if(GetModuleMode() == Single)
				{
					SideALargePWMOFF();
					SetSideALargePWMstatus(PWMwaitSts);
				}
				SetPrePowerMode(cPulseMode);
				SetPowerMode(cShutDownMode);
				return;
			
			}




        }
        else//eSuperTimer
        {
            if((GetWaveForm() == DC) && (GetPWMstatus() == PWMrunSts))
            {
                SetPrePowerMode(cPulseMode);
                SetPowerMode(cDCMode);
                return;
            }
        }
    }

}
/*********************************************************************
Function name:  sDcMode
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sDcMode(void)
{
	uint32_t event;
    //BaseType_t err;

    //while(pdTRUE)
    {
        //err = xSemaphoreTake( xSemaphoreSuper, (TickType_t)Tick10mS );
        event = OSEventPend(cPrioSuper);
        if(event != 0)
        {

			if(OSEventConfirm(event,eSuperFault))
            {
            	SideALargePWMOFF();
				SetSideALargePWMstatus(PWMwaitSts);
				SetPrePowerMode(cDCMode);
				SetPowerMode(cFaultMode);
				return;

            }
			if(OSEventConfirm(event,eSuperTurnOff))
			{
//				if(GetModuleMode() == Single)
				{
					SideALargePWMOFF();
					SetSideALargePWMstatus(PWMwaitSts);
				}
				SetPrePowerMode(cDCMode);
				SetPowerMode(cShutDownMode);
				return;
			
			}




        }
        else//eSuperTimer
        {
            if((GetWaveForm() == Pulse) && (GetPWMstatus() == PWMrunSts))
            {
                SetPrePowerMode(cDCMode);
                SetPowerMode(cPulseMode);
                return;
            }
        }
    }

}
/*********************************************************************
Function name:  sOpenLoopMode
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sOpenLoopMode(void)
{
	uint32_t event;
    //BaseType_t err;

    //while(pdTRUE)
    {
        //err = xSemaphoreTake( xSemaphoreSuper, (TickType_t)Tick10mS );
        event = OSEventPend(cPrioSuper);
        if(event != 0)
        {

			if(OSEventConfirm(event,eSuperFault))
            {
            	SideALargePWMOFF();
				SetSideALargePWMstatus(PWMwaitSts);
				SetPrePowerMode(cOpenLoopMode);
				SetPowerMode(cFaultMode);
				return;

            }
			if(OSEventConfirm(event,eSuperTurnOff))
			{
				SideALargePWMOFF();
				SetSideALargePWMstatus(PWMwaitSts);
				SetPrePowerMode(cOpenLoopMode);
				SetPowerMode(cShutDownMode);
				return;
			
			}
			/*
		    if(OSEventConfirm(event,eSuperTurnOn))
            {

                SetPrePowerMode(cShutDownMode);
                SetPowerMode(cPowerOnMode);
                return;

            }*/




        }
        else//eSuperTimer
        {

        }
    }
}
/*********************************************************************
Function name:  sFaultMode
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sFaultMode(void)
{
	uint32_t event;
    //BaseType_t err;
    static uint16_t FaultModeTimeCnt = 0;
    static uint16_t FaultFlag = 0;

    //while(pdTRUE)
    {
        //err = xSemaphoreTake( xSemaphoreSuper, (TickType_t)Tick10mS );
        event = OSEventPend(cPrioSuper);
        if(event != 0)
        {

			if(OSEventConfirm(event,eSuperTurnOff))
			{
				SideALargePWMOFF();
				SetSideALargePWMstatus(PWMwaitSts);
				SetPrePowerMode(cFaultMode);
				SetPowerMode(cShutDownMode);
				FaultModeTimeCnt = 0;
				FaultFlag = 0;
				HwOCFaultRecoveryFlag = 0;
				return;
			
			}
/*
			if(OSEventConfirm(event,eSuperTurnOn))
			{
				SetPrePowerMode(cFaultMode);
				SetPowerMode(cPowerOnMode);
				FaultFlag = 0;
                HwOCFaultRecoveryFlag = 0;
				return;
			
			}

*/

        }
        else//eSuperTimer
        {
            FaultModeTimeCnt++;
            if(FaultModeTimeCnt >= 10)
            {
                FaultModeTimeCnt = 10;

                if(sGetFaultCode() == cFambientTempOverHigh)
                {
                    if(GetAmbientTempStatus() == TempNormal)
                    {
                        FaultRegs.HWfaultTemp.bit.AmbientHighOT = 0;
                        sClrFaultCode();
                        FaultFlag = 1;
                    }
                }
                else if(sGetFaultCode() == cFsinkTempOverHigh)
                {
                    if(GetSinkTempStatus() == TempNormal)
                    {
                        FaultRegs.HWfaultTemp.bit.SinkHighOT = 0;
                        sClrFaultCode();
                        FaultFlag = 1;
                    }
                }
                else if(sGetFaultCode() == cFBusOver)
                {
                    if(GetBusHighFlag() == pdFALSE)
                    {
                        FaultRegs.HWfaultLoadAvg.bit.BusOver = 0;
                        sClrFaultCode();
                        FaultFlag = 1;
                    }
                }
                else if(sGetFaultCode() == cFBusUnder)
                {
                    if(GetBusLowFlag() == pdFALSE)
                    {
                        FaultRegs.HWfaultLoadAvg.bit.BusUnder = 0;
                        sClrFaultCode();
                        FaultFlag = 1;
                    }
                }
                else if(sGetFaultCode() == cFHwExtLoadOC)
                {
                    if(HwOCFaultRecoveryFlag == 0)
                    {
                        // Clear the interrupt flag
                        EALLOW;
                        EPwm2Regs.TZCLR.bit.OST = 1;
                        EPwm2Regs.TZCLR.bit.INT = 1;

                        EPwm3Regs.TZCLR.bit.OST = 1;
                        EPwm3Regs.TZCLR.bit.INT = 1;
                        EDIS;

                        FaultRegs.HWfaultLoad.bit.HwExtLoadOC = 0;
                        sClrFaultCode();
                        FaultFlag = 1;
                    }
                    else
                    {
                        //do nothing
                    }
                }
                else
                {
                    //do nothing
                }
            }

            if(FaultFlag == 1)
            {
                if((GetPrePowerMode() == cDCMode) || (GetPrePowerMode() == cPulseMode) || ((GetPrePowerMode() == cOpenLoopMode)))
                {
                    if(GetBoardType() == UnitCB)
                    {
                        if(GetControlMode() == CloseLoop)
                        {
                            if(GetWaveForm() == DC)
                            {
                                SetPrePowerMode(cFaultMode);
                                SetPowerMode(cDCMode);
                                SetPulsePhase(PulsePhase0);
                                FaultModeTimeCnt = 0;
                                FaultFlag = 0;
                            }
                            else if(GetWaveForm() == Pulse)
                            {
                                SetPrePowerMode(cFaultMode);
                                SetPowerMode(cPulseMode);
                                SetPulsePhase(PulsePhase1);
                                FaultModeTimeCnt = 0;
                                FaultFlag = 0;
                            }
                            else
                            {

                            }
                        }
                        else if(GetControlMode() == OpenLoop)
                        {
                            SetPrePowerMode(cFaultMode);
                            SetPowerMode(cOpenLoopMode);
                            FaultModeTimeCnt = 0;
                            FaultFlag = 0;
                        }
                        else
                        {

                        }
    //                  if(GetModuleMode() == Single)
                            SetSideALargePWMstatus(PWMrunSts);
                        return;
                    }
                }
                else
                {
                    SetPrePowerMode(cFaultMode);
                    SetPowerMode(cShutDownMode);
                    FaultModeTimeCnt = 0;
                    FaultFlag = 0;
                    return;
                }
            }
        }
    }
}

/*********************************************************************
Function name:  void sShutDownMode(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sShutDownMode(void)
{
	uint32_t event;
    //BaseType_t err;

    //while(pdTRUE)
    {
        //err = xSemaphoreTake( xSemaphoreSuper, (TickType_t)Tick10mS );
        event = OSEventPend(cPrioSuper);
        if(event != 0)
        {

            if(OSEventConfirm(event,eSuperFault))
            {
				SetPrePowerMode(cShutDownMode);
				SetPowerMode(cFaultMode);
				return;

            }
			if(OSEventConfirm(event,eSuperTurnOn))
			{
				SetPrePowerMode(cShutDownMode);
				SetPowerMode(cPowerOnMode);
				return;
			
			}



        }
        else//eSuperTimer
        {

        }
    }

}
/*********************************************************************
Function name:  void SetPowerMode(uint16_t mode)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SetPowerMode(uint16_t mode)
{
	wPowerMode = mode;

}
/*********************************************************************
Function name:  void SetPrePowerMode(uint16_t mode)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SetPrePowerMode(uint16_t mode)
{
	wPrePowerMode = mode;

}

/*********************************************************************
Function name:   GetPowerMode
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
uint16_t GetPowerMode()
{
    return wPowerMode;
}

/*********************************************************************
Function name:  void GetPrePowerMode(uint16_t mode)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
uint16_t GetPrePowerMode()
{
	return wPrePowerMode;

}

/*********************************************************************
Function name:  void SetPrePowerMode(uint16_t mode)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void SetPulsePhase(PulsePowerPulsePhase Phase)
{
	SideAControl.PulsePhase = Phase;

}



