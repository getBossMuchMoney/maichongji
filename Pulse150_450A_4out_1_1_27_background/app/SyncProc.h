
#ifndef _SYNCPROC_H
#define _SYNCPROC_H

#include <stdint.h>

#include "device.h"
#include "Initial.h"
#include "ParallelTask.h"
#include "Interrupt.h"


#define cSyncComm_Idle_State        0
#define cSyncComm_Start_State       1
#define cSyncComm_Id_State          2
#define cSyncComm_Data_State		3
#define cSyncComm_Check_State   	4
#define cSyncComm_DataProc_State    5

#define cSyncComm_Tx             0
#define cSyncComm_Rx             1


#define cBitWidth                      3
#define cIdBitNum                      6
#define cDataBitNum                    18
#define cStartBitNum                   1
#define cCheckBitNum                   8
#define cFrameDelay                    5    //5ms
#define cRxDataMask                    0x0003ffff

#define cSynTimerNone                  0
#define cSynTimer0                     1
#define cSynTimer1                     2
#define cSynTimer12                     3

#define cSynLossThrold		50
#define cSynSendDis			2

#define cSynStartThrold		100	//10ms

#define cSynRole_Master		0
#define cSynRole_Slave		1

#define cSynLineBusyNum		4

//GPIO_writePin(9, 1);
//GPIO_writePin(9, 0);
//#define sEnable_Rx_Int()				{ECap1Regs.ECCLR.all = 0xffff;PieCtrlRegs.PIEACK.bit.ACK4 = 1;ECap1Regs.ECEINT.bit.CEVT1 = 1; PieCtrlRegs.PIEIER4.bit.INTx1 = 1;GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;}
//#define sDisable_Rx_Int()				{ECap1Regs.ECEINT.bit.CEVT1 = 0;PieCtrlRegs.PIEIER4.bit.INTx1 = 0;ECap1Regs.ECCLR.all = 0xffff;PieCtrlRegs.PIEACK.bit.ACK4 = 1;GpioDataRegs.GPASET.bit.GPIO9 = 1;}
#define sEnable_Rx_Int()                {ECap1Regs.ECCLR.all = 0xffff;ECap1Regs.ECEINT.bit.CEVT1 = 1;}
#define sDisable_Rx_Int()               {ECap1Regs.ECEINT.bit.CEVT1 = 0;ECap1Regs.ECCLR.all = 0xffff;}
//#define sEnable_25us_Tmr()				{EPwm4Regs.TBCTR = 2999;EPwm4Regs.ETCLR.bit.INT = 1;PieCtrlRegs.PIEACK.bit.ACK3 = 1;EPwm4Regs.ETSEL.bit.INTEN = 0x1;PieCtrlRegs.PIEIER3.bit.INTx4 = 1;}//{Timer0Init(250);}
#define sEnable_25us_Tmr()				{EPwm4Regs.TBCTR = 2999;EPwm4Regs.ETCLR.bit.INT = 1;EPwm4Regs.ETSEL.bit.INTEN = 0x1;}//{Timer0Init(250);}

#define sEnable_Master_PulseCycle_Tmr(period)	{Timer2ReSet(period);}
#define sDisable_Master_PulseCycle_Tmr()		{Timer2Stop();}

//#define sEnable_PulseCycle_Tmr()		{Timer1Init(TimerPeriod_100ns);}
#define sEnable_PulseCycle_Tmr(period)  {sEnablePulseCycleTimer(period);}
#define sEnable_UnitPulseCycle_Tmr(period)   {sEnableUnitPulseCycleTimer(period);}
//#define sDisable_25us_Tmr()				{EPwm4Regs.ETSEL.bit.INTEN = 0x0;PieCtrlRegs.PIEIER3.bit.INTx4 = 0;}//{Timer0Stop();}
#define sDisable_25us_Tmr()				{EPwm4Regs.ETSEL.bit.INTEN = 0x0;}//{Timer0Stop();}

//#define sDisable_PulseCycle_Tmr()		{Timer1Stop();}
#define sDisable_PulseCycle_Tmr()       {Timer0Stop();Timer1Stop();}
#define sDisable_PulseCycle_Start_Tmr()  {sDisablePulseCycleStartTimer();}
#define sEnable_PrePulse_Tmr(period)	{EPwm5Regs.TBCTR = 11999;EPwm5Regs.ETCLR.bit.INT = 1;EPwm5Regs.ETSEL.bit.INTEN = 0x1;PreSynPeriod_100us = (period);}
#define sDisable_PrePulse_Tmr()			{EPwm5Regs.ETSEL.bit.INTEN = 0x0;}

#define sGetSelfSynCycleCnt()			(SynPeriod_100us)//(ModulePara.SumOfExtSyncPoint)


#define mSynTxLo()                    {GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;}
#define mSynTxHi()                    {GpioDataRegs.GPASET.bit.GPIO17 = 1;}

#define mSynRxSts()                   GpioDataRegs.GPHDAT.bit.GPIO230


typedef struct {
	uint16_t u16_CommRole;
	uint16_t u16_CommState;
	uint16_t u16_BitIndex;
	uint16_t u16_BitPosition;
	uint16_t u16_StartBitNum;
	uint16_t u16_IdBitNum;
	uint16_t u16_DataBitNum;
	uint16_t u16_CheckBitNum;
	uint16_t u16_Id;
	uint16_t u16_RxId;
	uint32_t u32_TxData;
	uint32_t u32_RxData;
	uint16_t u16_TxCheck;
	uint16_t u16_RxCheck;
	uint16_t u32_TimerOutCnt;
	uint16_t u32_DelayCntSet;
	uint16_t u32_DelayCnt;
	uint16_t u16_NeedToSync;
	uint16_t u16_Synchoronized;
	uint16_t u16_ErrCnt;
	uint16_t u16_SendRightCnt;
	uint16_t u16_RecvRightCnt;
	uint16_t u16_SynTimerStart;
	uint16_t u16_SynTimerExecuted;
	uint16_t u16_SynRole;
	uint16_t u16_SynCount;
	uint16_t u16_SynSendFlag;
	uint16_t u16_SynLoss;
	uint16_t u16_SynLossCnt;
	uint16_t u16_SynLossThrold;
	uint16_t u16_SynSignal;
	uint16_t u16_SynSendDis;
	uint16_t u16_CurDisCnt;
}SYNC_STRUCT;

extern void sSyncPeriodSet(uint32_t period_100us);
extern void sSyncIdSet(uint16_t synid);
extern void sSyncDelaySet(uint16_t syndelay);
extern void sSyncCommInit(uint16_t synid, uint16_t syndelay);
extern uint16_t sSyncRoleGet();
extern void sSyncStateMachine(void);
extern void sSyncStateIdle(void);
extern void sSyncStateIdleIsr(void);
extern void sEnablePulseCycleTimer(uint32_t u32_TimerPeriod);
extern void sSetUnitExecutedTimer(uint32_t period);
extern void sSyncPhaseShiftSet(uint32_t phase_100us);
extern void sSyncLossCheck(void);
extern void sSynPeriodIsr(void);
extern uint16_t sSyncTimerReach();
extern void sEnableUnitPulseCycleTimer(uint32_t u32_TimerPeriod);

extern SYNC_STRUCT stSynComm;
extern uint32_t SynPeriod_100us;
extern uint32_t PreSynPeriod_100us;
extern uint32_t SynLineIdleCnt;

#endif





























