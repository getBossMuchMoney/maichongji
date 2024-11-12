

#include "SyncProc.h"
#include "Common.h"

SYNC_STRUCT stSynComm;
void sSyncCommInit(uint16_t synid, uint16_t syndelay);
void sSyncStateIdleState(void);
void sSynStartTx(void);
void sSyncStateIdle(void);
void sSyncStateIdleIsr(void);

void sSyncStateStart(void);
void sSyncReset(void);
void sSyncStateId(void);
void sSyncStateData(void);
void sSyncStateCheck(void);
void sSyncStateDataProc(void);

void sSyncStateMachine(void);
void sEnablePulseCycleTimer(uint32_t u32_TimerPeriod);
void sDisablePulseCycleStartTimer(void);
void sSetExecutedTimer(void);
void sEnableUnitPulseCycleTimer(uint32_t u32_TimerPeriod);

uint32_t SynPeriod_100us = 0;
uint32_t PreSynPeriod_100us = 0;
uint32_t PhaseShiftPeriod_100us = 0;
uint32_t SynLineIdleCnt = 0;
uint32_t SynId = 0;

void togglePin(uint16_t level)
{
	if(level)
		GpioDataRegs.GPASET.bit.GPIO11 = 1;
	else
		GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
}

void sSyncPeriodSet(uint32_t period_100us)
{
	uint32_t preperiod_100us = 0;
	
	if(period_100us < cSynStartThrold)
	{
		if(BoardType == UnitCB)
		{
			DINT;
			Timer0Stop();
			Timer1Stop();
			Timer2Stop();
			//Timer2Resume();
			sDisable_PrePulse_Tmr();
			Epwm5Count = 0;
			EINT;
			if(EPwm1Regs.ETSEL.bit.INTEN == 0x0)
				EPwm1Regs.ETSEL.bit.INTEN = 0x1;
		}
		else
		{
			DINT;
			Timer0Stop();
			Timer1Stop();
			Timer2Stop();
			//Timer2Resume();
			sDisable_Rx_Int();

			sDisable_PrePulse_Tmr();
			Epwm5Count = 0;
			
			sSyncReset();
			EINT;
		}
//		return;
	}
	preperiod_100us = SynPeriod_100us;
	TimerPeriod_100ns = period_100us*1000;
	SynPeriod_100us = period_100us;
	stSynComm.u32_TxData = period_100us;
	if(Timer2IsStart() && (preperiod_100us != period_100us))
	{
		sEnable_Master_PulseCycle_Tmr(TimerPeriod_100ns);
	}
}

void sSyncPhaseShiftSet(uint32_t phase_100us)
{
	PhaseShiftPeriod_100us = phase_100us;
}

void sSyncIdSet(uint16_t synid)
{
    stSynComm.u16_Id = synid;
    stSynComm.u32_DelayCntSet = synid;
}

void sSyncDelaySet(uint16_t syndelay)
{
	CanBusDelay = syndelay;
}

void sSyncCommInit(uint16_t synid, uint16_t syndelay)
{
	TimerPeriod_100ns = 0;
	SynPeriod_100us = 0;
	TimerProcDelay = 0;
	CommTimeoutProhibitSendSyncFlag = 0;

	CanBusDelay = syndelay;

	stSynComm.u16_CommRole = cSyncComm_Rx;
	stSynComm.u16_CommState = cSyncComm_Idle_State;
	
	stSynComm.u16_BitPosition = 0;
	stSynComm.u16_BitIndex = 0;
	stSynComm.u16_DataBitNum = cDataBitNum;
	stSynComm.u16_StartBitNum = cStartBitNum;
	stSynComm.u16_CheckBitNum = cCheckBitNum;
	stSynComm.u16_IdBitNum = cIdBitNum;

	stSynComm.u32_TimerOutCnt = 0;
	stSynComm.u32_RxData = 0;
	stSynComm.u32_TxData = 0;
	stSynComm.u16_TxCheck = 0;
	stSynComm.u16_RxCheck = 0;
	stSynComm.u16_Id = synid;
	stSynComm.u16_RxId = 0;

	stSynComm.u32_DelayCnt = 0;
	stSynComm.u32_DelayCntSet = synid;
	stSynComm.u16_Synchoronized = 0;
	stSynComm.u16_NeedToSync = 0;

	stSynComm.u16_SynTimerStart = cSynTimerNone;
	stSynComm.u16_SynTimerExecuted = cSynTimerNone;

	stSynComm.u16_SynRole = cSynRole_Slave;
	stSynComm.u16_SynCount = 0;
	stSynComm.u16_SynSendFlag = 0;

	stSynComm.u16_SynLoss = 0;
	stSynComm.u16_SynLossCnt = 0;
	stSynComm.u16_SynLossThrold = cSynLossThrold;
	stSynComm.u16_SynSignal = 0;
	
	stSynComm.u16_CurDisCnt = 0;
	stSynComm.u16_SynSendDis = cSynSendDis;
}

uint16_t sSyncRoleGet()
{
	return stSynComm.u16_SynRole;
}

//Execute in 25us
void sSyncStateMachine(void)
{
	switch(stSynComm.u16_CommState)
	{
		case cSyncComm_Idle_State:
		//sSyncStateIdleIsr();
		break;

		case cSyncComm_Start_State:
		sSyncStateStart();
		break;

		case cSyncComm_Id_State:
		sSyncStateId();
		break;

		case cSyncComm_Data_State:
		sSyncStateData();
		break;

		case cSyncComm_Check_State:
		sSyncStateCheck();
		break;

		case cSyncComm_DataProc_State:
		sSyncStateDataProc();
		break;

		default:
		break;
	}
}


//Execute in 1ms background
void sSyncStateIdle(void)
{

    DINT;
    //if(stSynComm.u16_CommState == cSyncComm_Idle_State)
    if((stSynComm.u16_CommState == cSyncComm_Idle_State) &&
       (stSynComm.u16_Synchoronized == 0))
    {
        if((stSynComm.u32_DelayCnt++ >= stSynComm.u32_DelayCntSet) &&
               (stSynComm.u32_DelayCntSet != 0) && (SynPeriod_100us >= cSynStartThrold) &&
               (mSynRxSts() == 1))
        {
            stSynComm.u32_DelayCnt = 0;
            stSynComm.u16_CommRole = cSyncComm_Tx;
            sEnable_25us_Tmr();
			sEnable_Master_PulseCycle_Tmr(TimerPeriod_100ns);
            sEnable_PulseCycle_Tmr(TimerPeriod_100ns-1000);
            sDisable_Rx_Int();
            mSynTxLo();
            stSynComm.u16_CommState = cSyncComm_Start_State;
//			stSynComm.u16_SynRole = cSynRole_Master;
        }
        else
        {
            mSynTxHi();
            sEnable_Rx_Int();
            stSynComm.u16_CommRole = cSyncComm_Rx;
        }
    }
    EINT;
}

//Execute in the RX falling edge interrupt
void sSyncStateIdleIsr(void)
{
	if(SynPeriod_100us < cSynStartThrold)		//10ms
	{
		return;
	}
	
	if(BoardType == UnitCB)
	{
//		GpioDataRegs.GPASET.bit.GPIO9 = 1;
	    sEnable_UnitPulseCycle_Tmr(TimerPeriod_100ns-CapIntDelay-CanBusDelay-PhaseShiftPeriod_100us*1000-1000);
//		sSetUnitExecutedTimer(SynPeriod_100us-1-PhaseShiftPeriod_100us);
	}
	else
	{
	    if(stSynComm.u16_CommState == cSyncComm_Idle_State)
	    {
	        if(stSynComm.u16_CommRole == cSyncComm_Rx)
	        {
	            //if(stSynComm.u32_DelayCntSet != 0)
	            {
	                sDisable_Rx_Int();
	                sEnable_25us_Tmr();
	                sEnable_PulseCycle_Tmr(TimerPeriod_100ns-CapIntDelay-CanBusDelay-1000);
//					sEnable_PulseCycle_Tmr(TimerPeriod_100ns - CanBusDelay);
	                //stSynComm.u32_DelayCntSet += cFrameDelay; //Add delay for the next tx
	                stSynComm.u16_BitPosition = 0;
	                stSynComm.u16_BitIndex = 0;
	//                stSynComm.u16_DataBitNum = cDataBitNum;
	//                stSynComm.u16_StartBitNum = cStartBitNum;
	//                stSynComm.u16_CheckBitNum = cCheckBitNum;
	//                stSynComm.u16_IdBitNum = cIdBitNum;

	                stSynComm.u32_TimerOutCnt = 0;
	                stSynComm.u32_RxData = 0;
	                //stSynComm.u32_TxData = 0;
	                //stSynComm.u16_TxCheck = 0;
	                stSynComm.u16_RxCheck = 0;
	                stSynComm.u16_RxId = 0;
	                //stSynComm.u32_DelayCnt = 0;
	                stSynComm.u16_NeedToSync= 0;
	                stSynComm.u16_CommState = cSyncComm_Start_State;
	            }
	        }
	    }
	}
}


void sSyncReset(void)
{
	stSynComm.u16_BitPosition = 0;
	stSynComm.u16_BitIndex = 0;
	stSynComm.u16_DataBitNum = cDataBitNum;
	stSynComm.u16_StartBitNum = cStartBitNum;
	stSynComm.u16_CheckBitNum = cCheckBitNum;
	stSynComm.u16_IdBitNum = cIdBitNum;

	stSynComm.u32_TimerOutCnt = 0;
	stSynComm.u32_RxData = 0;
	//stSynComm.u32_TxData = 0;
	//stSynComm.u16_TxCheck = 0;
	stSynComm.u16_RxCheck = 0;
	stSynComm.u16_RxId = 0;
	stSynComm.u32_DelayCnt = 0;
	stSynComm.u16_NeedToSync= 0;
	stSynComm.u16_Synchoronized = 0;

	stSynComm.u16_CommRole = cSyncComm_Rx;
	stSynComm.u16_CommState = cSyncComm_Idle_State;

	stSynComm.u16_SynTimerStart = cSynTimerNone;
	stSynComm.u16_SynTimerExecuted = cSynTimerNone;

	stSynComm.u16_SynRole = cSynRole_Slave;
	stSynComm.u16_SynCount = 0;
	stSynComm.u16_SynSendFlag = 0;
	
	stSynComm.u16_SynLoss = 0;
	stSynComm.u16_SynLossCnt = 0;
	stSynComm.u16_SynLossThrold = cSynLossThrold;
	stSynComm.u16_SynSignal = 0;

	stSynComm.u16_CurDisCnt = 0;
	stSynComm.u16_SynSendDis = cSynSendDis;
}


void sSyncStateStart(void)
{
	if(stSynComm.u16_CommRole == cSyncComm_Tx)
	{
		mSynTxLo();
	}
	else
	{
		mSynTxHi();
		/*if((mSynRxSts != 0) &&
		   (stSynComm.u16_BitIndex == 1))		//Error
		{
			//stSynComm.u16_BitIndex = 0;
			//stSynComm.u16_CommState = cSyncComm_Idle_State;
		}*/
	}
	SynLineIdleCnt = 0;
	stSynComm.u16_BitIndex++;
	if(stSynComm.u16_BitIndex >= ((cBitWidth * cStartBitNum)-1))
	{
		stSynComm.u16_BitIndex = 0;
		stSynComm.u16_CommState = cSyncComm_Id_State;
	}
}

void sSyncStateId(void)
{
	uint16_t u16_Temp;
	if(stSynComm.u16_CommRole == cSyncComm_Tx)
	{
		if(stSynComm.u16_Id & ((uint16_t)0x1 << (cIdBitNum - stSynComm.u16_BitPosition - 1)))
		{
			mSynTxHi();
			u16_Temp = 1;
		}
		else
		{
			mSynTxLo();
			u16_Temp = 0;
		}

		if((stSynComm.u16_BitIndex == 1) &&
			(u16_Temp != mSynRxSts()))
		{
		    if(stSynComm.u16_CommRole == cSyncComm_Tx)
		    {
//		        if(stSynComm.u16_Synchoronized == 0) //never synchronized
//		        {
//		            //stSynComm.u32_DelayCnt = 0;
//		            stSynComm.u16_Synchoronized = 0;
//		            sDisable_PulseCycle_Start_Tmr();       //stop the timer
//		        }
		        stSynComm.u16_CommRole = cSyncComm_Rx;
		    }
		}
	}
	//else
	{
		if(stSynComm.u16_BitIndex == 1)
		{
			if(mSynRxSts() != 0)
			{
				stSynComm.u16_RxId = ((stSynComm.u16_RxId<<1) | 0x1);
			}
			else
			{
				stSynComm.u16_RxId = stSynComm.u16_RxId<<1;
			}
		}
	}
	
	if(stSynComm.u16_BitIndex++ >= (cBitWidth-1))
	{
		stSynComm.u16_BitIndex = 0;
		if(++stSynComm.u16_BitPosition >= stSynComm.u16_IdBitNum)
		{
			stSynComm.u16_BitPosition = 0;
			stSynComm.u16_BitIndex = 0;
			stSynComm.u16_CommState = cSyncComm_Data_State;
			stSynComm.u16_RxId &= 0x3F;
		}
	}
}

void sSyncStateData(void)
{
	uint16_t u16_CheckSumTemp;

	
	if(stSynComm.u16_CommRole == cSyncComm_Tx)
	{
		if(stSynComm.u32_TxData & ((uint32_t)0x1 << (cDataBitNum - stSynComm.u16_BitPosition - 1)))
		{
			mSynTxHi();
		}
		else
		{
			mSynTxLo();
		}
	}
	if(stSynComm.u16_BitIndex == 1)
	{
		if(mSynRxSts() != 0)
		{
			stSynComm.u32_RxData = ((stSynComm.u32_RxData<<1) | 0x1);
		}
		else
		{
			stSynComm.u32_RxData = stSynComm.u32_RxData<<1;
		}
	}

	if(stSynComm.u16_BitIndex++ >= (cBitWidth-1))
	{
		stSynComm.u16_BitIndex = 0;
		if(++stSynComm.u16_BitPosition >= stSynComm.u16_DataBitNum)
		{
			stSynComm.u16_BitPosition = 0;
			stSynComm.u16_BitIndex = 0;
			stSynComm.u16_CommState = cSyncComm_Check_State;
			u16_CheckSumTemp = 0;
			u16_CheckSumTemp += stSynComm.u16_Id;
			u16_CheckSumTemp += (stSynComm.u32_TxData & 0x0ffff);
			u16_CheckSumTemp += (stSynComm.u32_TxData>>16)& 0x0ffff;
			stSynComm.u16_TxCheck = u16_CheckSumTemp & 0xff;
			stSynComm.u32_RxData &= cRxDataMask;
		}
	}
}


void sSyncStateCheck(void)
{
	if(stSynComm.u16_CommRole == cSyncComm_Tx)
	{
		if(stSynComm.u16_TxCheck & ((uint16_t)0x1 << (cCheckBitNum - stSynComm.u16_BitPosition - 1)))
		{
			mSynTxHi();
		}
		else
		{
			mSynTxLo();
		}
	}
	if(stSynComm.u16_BitIndex == 1)
	{
		if(mSynRxSts() != 0)
		{
			stSynComm.u16_RxCheck = ((stSynComm.u16_RxCheck<<1) | 0x1);
		}
		else
		{
			stSynComm.u16_RxCheck = stSynComm.u16_RxCheck<<1;
		}
	}

	if(stSynComm.u16_BitIndex++ >= (cBitWidth-1))
	{
		stSynComm.u16_BitIndex = 0;
		if(++stSynComm.u16_BitPosition >= stSynComm.u16_CheckBitNum)
		{
			stSynComm.u16_BitPosition = 0;
			stSynComm.u16_BitIndex = 0;
			stSynComm.u16_RxCheck &= 0xff;
			stSynComm.u16_CommState = cSyncComm_DataProc_State;
		}
	}
}

void sSyncStateDataProc(void)
{
	uint16_t u16_CheckSumTemp;

	mSynTxHi();

	u16_CheckSumTemp = 0;
	u16_CheckSumTemp += stSynComm.u16_RxId;
	u16_CheckSumTemp += (stSynComm.u32_RxData & 0x0ffff);
	u16_CheckSumTemp += (stSynComm.u32_RxData>>16)& 0x0ffff;
	u16_CheckSumTemp &= 0xff;

	if(u16_CheckSumTemp == stSynComm.u16_RxCheck)	//RX Success
	{
		if(stSynComm.u16_CommRole == cSyncComm_Tx)
		{
		    sDisable_25us_Tmr(); //Close the 25us int
			mSynTxHi();          //release the bus
			stSynComm.u16_CommRole = cSyncComm_Rx;
			//stSynComm.u32_DelayCnt = 0;
			stSynComm.u16_Synchoronized = 1;
			sEnable_Rx_Int();
			sSetExecutedTimer();
			stSynComm.u16_SendRightCnt++;
			stSynComm.u16_SynRole = cSynRole_Master;
			stSynComm.u16_SynLoss = 0;
		}
		else
		{
		    stSynComm.u16_RecvRightCnt++;
			sDisable_25us_Tmr(); //Close the 25us int
			//stSynComm.u32_DelayCnt = 0;
			if(stSynComm.u32_RxData == sGetSelfSynCycleCnt())
			{
				//synchronization enable
				stSynComm.u16_Synchoronized = 1;
				stSynComm.u16_NeedToSync = 1;
				sSetExecutedTimer();
				stSynComm.u16_SynRole = cSynRole_Slave;
				stSynComm.u16_SynSignal = 0;
				sDisable_Master_PulseCycle_Tmr();
			}
			else
			{				
				if(stSynComm.u16_Synchoronized == 0) //The module has not been synchronized
				{
				    sDisable_PulseCycle_Tmr();  //stop all the timer
				}
				else
				{
				    sDisable_PulseCycle_Start_Tmr(); //stop the start timer
				}
				//synchronization disable, then free running
//				stSynComm.u16_Synchoronized = 0;
			}
			sEnable_Rx_Int();
		}
	}
	else
	{
	    stSynComm.u16_ErrCnt++;
	    stSynComm.u16_CommRole = cSyncComm_Rx;
        //stSynComm.u32_DelayCnt = 0;
        //stSynComm.u16_Synchoronized = 0;
        //sDisable_PulseCycle_Tmr();
        sDisable_PulseCycle_Start_Tmr();
        sDisable_25us_Tmr(); //Close the 25us int
        sEnable_Rx_Int();
	}
	SynLineIdleCnt = 0;
	stSynComm.u16_CommState = cSyncComm_Idle_State;
}


void sEnablePulseCycleTimer(uint32_t u32_TimerPeriod)
{
    if(stSynComm.u16_SynTimerExecuted == cSynTimerNone)
    {
        Timer0ReSet(u32_TimerPeriod);
        stSynComm.u16_SynTimerStart = cSynTimer0;
    }
    else if(stSynComm.u16_SynTimerExecuted == cSynTimer0)
    {
        Timer1ReSet(u32_TimerPeriod);
        stSynComm.u16_SynTimerStart = cSynTimer1;
    }
    else if(stSynComm.u16_SynTimerExecuted == cSynTimer1)
    {
        Timer0ReSet(u32_TimerPeriod);
        stSynComm.u16_SynTimerStart = cSynTimer0;
    }
}

void sEnableUnitPulseCycleTimer(uint32_t u32_TimerPeriod)
{
	Timer0ReSet(u32_TimerPeriod);
	Timer1ReSet(u32_TimerPeriod-250);
	stSynComm.u16_SynTimerStart = cSynTimer0;
//    if(stSynComm.u16_SynTimerStart == cSynTimerNone)
//    {
//        Timer0ReSet(u32_TimerPeriod);
//        stSynComm.u16_SynTimerStart |= cSynTimer0;
//        stSynComm.u16_SynTimerExecuted = cSynTimerNone;
//    }
//    else if((stSynComm.u16_SynTimerStart & cSynTimer0) &&
//            ((stSynComm.u16_SynTimerStart & cSynTimer1) == 0))
//    {
//        Timer1ReSet(u32_TimerPeriod);
//        stSynComm.u16_SynTimerStart |= cSynTimer1;
//        if(stSynComm.u16_SynTimerExecuted == cSynTimer0)  //The tmr0 int is occurred
//        {
//            Timer0Stop();
//            stSynComm.u16_SynTimerStart &= (~cSynTimer0);
//            stSynComm.u16_SynTimerExecuted = cSynTimerNone;
//        }
//    }
//    else if((stSynComm.u16_SynTimerStart & cSynTimer1) &&
//            ((stSynComm.u16_SynTimerStart & cSynTimer0) == 0))
//    {
//        Timer0ReSet(u32_TimerPeriod);
//        stSynComm.u16_SynTimerStart |= cSynTimer0;
//        if(stSynComm.u16_SynTimerExecuted == cSynTimer1)  //The tmr1 int is occurred
//        {
//            Timer1Stop();
//            stSynComm.u16_SynTimerStart &= (~cSynTimer1);
//            stSynComm.u16_SynTimerExecuted = cSynTimerNone;
//        }
//    }
//    else
//    {
//        Timer1Stop();
//        stSynComm.u16_SynTimerStart &= (~cSynTimer1);
//        stSynComm.u16_SynTimerExecuted = cSynTimerNone;
//    }
}

void sDisablePulseCycleStartTimer(void)
{
    if(stSynComm.u16_SynTimerStart == cSynTimer0)
    {
        Timer0Stop();
    }
    else if(stSynComm.u16_SynTimerStart == cSynTimer1)
    {
        Timer1Stop();
    }
    else
    {
        Timer0Stop();
        Timer1Stop();
    }
	//Timer2Resume();
}

void sSetExecutedTimer(void)
{
    if(stSynComm.u16_SynTimerStart == cSynTimer0)
    {
        stSynComm.u16_SynTimerExecuted = cSynTimer0;
        Timer1Stop();
		Epwm5Count = 0;
		sEnable_PrePulse_Tmr(SynPeriod_100us - 25);
		MasterExtSyncH;
    }
    else if(stSynComm.u16_SynTimerStart == cSynTimer1)
    {
        stSynComm.u16_SynTimerExecuted = cSynTimer1;
        Timer0Stop();
		Epwm5Count = 0;
		sEnable_PrePulse_Tmr(SynPeriod_100us - 25);
		MasterExtSyncH;
    }
}

void sSetUnitExecutedTimer(uint32_t period)
{
   /* if(stSynComm.u16_SynTimerStart == cSynTimer0)
    {
        //stSynComm.u16_SynTimerExecuted = cSynTimer0;
		//Timer1Stop();
		Epwm5Count = 0;
		sEnable_PrePulse_Tmr(period);
    }
    else if(stSynComm.u16_SynTimerStart == cSynTimer1)
    {
       // stSynComm.u16_SynTimerExecuted = cSynTimer1;
		//Timer0Stop();
		Epwm5Count = 0;
		sEnable_PrePulse_Tmr(period);
    }*/

    Epwm5Count = 0;
    sEnable_PrePulse_Tmr(period);
}

void sSyncLossCheck(void)
{
	if((stSynComm.u16_CommState == cSyncComm_Idle_State) &&
		(stSynComm.u16_SynRole == cSynRole_Slave) &&
	    (stSynComm.u16_Synchoronized == 1))
	{
		if(stSynComm.u16_SynSignal)
		{
			stSynComm.u16_SynLossCnt++;
			if(stSynComm.u16_SynLossCnt >= stSynComm.u16_SynLossThrold)
			{
				stSynComm.u16_SynLoss = 1;
				stSynComm.u16_SynRole = cSynRole_Master;
				stSynComm.u16_CurDisCnt = 0;
				sEnable_Master_PulseCycle_Tmr(TimerPeriod_100ns);
			}
		}
		else
		{
			stSynComm.u16_SynLossCnt = 0;
			stSynComm.u16_SynLoss = 0;
		}
		stSynComm.u16_SynSignal = 1;
	}
}
void sSynPeriodIsr(void)
{
	stSynComm.u16_CurDisCnt++;
	if(stSynComm.u16_CurDisCnt >= stSynComm.u16_SynSendDis)
	{
		stSynComm.u16_CurDisCnt = stSynComm.u16_SynSendDis + 1;
	}
	if((stSynComm.u16_CommState == cSyncComm_Idle_State) &&
	            (stSynComm.u16_Synchoronized == 1))
	{
		if((stSynComm.u16_SynRole == cSynRole_Master) || (stSynComm.u16_SynLoss == 1))
	    {
	        if((mSynRxSts() == 1) && (SynLineIdleCnt > cSynLineBusyNum) && (stSynComm.u16_SynSendDis < stSynComm.u16_CurDisCnt))
	        {
	            sDisable_Rx_Int();
				mSynTxLo();
	//			sEnable_PulseCycle_Tmr(TimerPeriod_100ns - TimerProcDelay);
				sEnable_PulseCycle_Tmr(TimerPeriod_100ns-1000);
				
	            stSynComm.u32_DelayCnt = 0;
	            stSynComm.u16_CommRole = cSyncComm_Tx;
	            sEnable_25us_Tmr();
	            
	            stSynComm.u16_CommState = cSyncComm_Start_State;
				stSynComm.u16_CurDisCnt = 0;
	        }
		}
	    else
	    {
	        mSynTxHi();
	        sEnable_Rx_Int();
	        stSynComm.u16_CommRole = cSyncComm_Rx;

//			if(stSynComm.u16_SynSignal)
//			{
//				stSynComm.u16_SynLossCnt++;
//				if(stSynComm.u16_SynLossCnt >= stSynComm.u16_SynLossThrold)
//				{
//					stSynComm.u16_SynLoss = 1;
//					stSynComm.u16_SynRole = cSynRole_Master;
//					stSynComm.u16_CurDisCnt = 0;
//				}
//			}
//			else
//			{
//				stSynComm.u16_SynLossCnt = 0;
//				stSynComm.u16_SynLoss = 0;
//			}
//			stSynComm.u16_SynSignal = 1;
	    }
	}
}

uint16_t sSyncTimerReach()
{
	uint16_t reached = 0;
//	if(stSynComm.u16_SynTimerExecuted == cSynTimer0)
//    {
//        if(CpuTimer0Regs.TIM.all < 1800)	//15us=150°Ùns=1800/12
//        {
//        	reached = 1;
//        }
//    }
//    else if(stSynComm.u16_SynTimerExecuted == cSynTimer1)
//    {
//        if(CpuTimer1Regs.TIM.all < 1800)	//15us=150°Ùns=1800/12
//        {
//        	reached = 1;
//        }
//    }
	return reached;
}





















