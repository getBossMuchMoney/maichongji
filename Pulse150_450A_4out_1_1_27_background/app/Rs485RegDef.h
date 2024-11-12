#ifndef __RS485_ADDR_H__
#define __RS485_ADDR_H__


typedef enum
{
	eModuleCtrlArea_Ctrl,
	eModuleCtrlArea_Curr1,	eModuleCtrlArea_Pulse1,
	eModuleCtrlArea_Curr2,	eModuleCtrlArea_Pulse2,
	eModuleCtrlArea_Curr3,	eModuleCtrlArea_Pulse3,
	eModuleCtrlArea_Curr4,	eModuleCtrlArea_Pulse4,
	eModuleCtrlArea_Curr5,	eModuleCtrlArea_Pulse5,
	eModuleCtrlArea_Curr6,	eModuleCtrlArea_Pulse6,
	eModuleCtrlArea_Curr7,	eModuleCtrlArea_Pulse7,
	eModuleCtrlArea_Curr8,	eModuleCtrlArea_Pulse8,
	eModuleCtrlArea_Curr9,	eModuleCtrlArea_Pulse9,
	eModuleCtrlArea_Curr10,	eModuleCtrlArea_Pulse10,
	eModuleCtrlArea_Curr11,	eModuleCtrlArea_Pulse11,
	eModuleCtrlArea_Curr12,	eModuleCtrlArea_Pulse12,
	eModuleCtrlArea_Curr13,	eModuleCtrlArea_Pulse13,
	eModuleCtrlArea_Curr14,	eModuleCtrlArea_Pulse14,
	eModuleCtrlArea_Curr15,	eModuleCtrlArea_Pulse15,
	eModuleCtrlArea_Curr16,	eModuleCtrlArea_Pulse16,
	eModuleCtrlArea_Curr17,	eModuleCtrlArea_Pulse17,
	eModuleCtrlArea_Curr18,	eModuleCtrlArea_Pulse18,
	eModuleCtrlArea_Curr19,	eModuleCtrlArea_Pulse19,
	eModuleCtrlArea_Curr20,	eModuleCtrlArea_Pulse20,
	eModuleCtrlArea_PhaseShift1,
	eModuleCtrlArea_PhaseShift2,
	eModuleCtrlArea_PhaseShift3,
	eModuleCtrlArea_PhaseShift4,
	eModuleCtrlArea_SectNum,
	eModuleCtrlArea_UnitCtrl,
	
	eModuleCtrlArea_Ctrl2 = 83,
	eModuleCtrlArea_LoadEepromDef,
	eModuleCtrlArea_SyncPhase,
}eModuleCtrlArea;

typedef enum
{
	eModuleStatusArea_WorkStatus,
	eModuleStatusArea_Fault,
	eModuleStatusArea_Warning,
	eModuleStatusArea_Ver1,
	eModuleStatusArea_Ver2,
	eModuleStatusArea_Ver3,
	eModuleStatusArea_MachineType,
}eModuleStatusArea;


typedef enum
{
	eModuleParaOffset_BoardType,
	eModuleParaOffset_BoardAddr,
	eModuleParaOffset_BoardRole,
	eModuleParaOffset_WorkMode,
	eModuleParaOffset_UnitAddrMin,
	eModuleParaOffset_UnitAddrMax,
	eModuleParaOffset_MaxCurr,
	eModuleParaOffset_MinCurr,
	eModuleParaOffset_AddrMode,
	eModuleParaOffset_SynDelay,
	eModuleParaOffset_SynId,
}eModuleParaOffset;


typedef enum
{
	eUnintCtrlOffset_Ctrl,
	eUnintCtrlOffset_Curr1	,	eUnintCtrlOffset_Pulse1,
	eUnintCtrlOffset_Curr2	,	eUnintCtrlOffset_Pulse2,
	eUnintCtrlOffset_Curr3	,	eUnintCtrlOffset_Pulse3,
	eUnintCtrlOffset_Curr4	,	eUnintCtrlOffset_Pulse4,
	eUnintCtrlOffset_Curr5	,	eUnintCtrlOffset_Pulse5,
	eUnintCtrlOffset_Curr6	,	eUnintCtrlOffset_Pulse6,
	eUnintCtrlOffset_Curr7	,	eUnintCtrlOffset_Pulse7,
	eUnintCtrlOffset_Curr8	,	eUnintCtrlOffset_Pulse8,
	eUnintCtrlOffset_Curr9	,	eUnintCtrlOffset_Pulse9,
	eUnintCtrlOffset_Curr10	,	eUnintCtrlOffset_Pulse10,
	eUnintCtrlOffset_Curr11	,	eUnintCtrlOffset_Pulse11,
	eUnintCtrlOffset_Curr12	,	eUnintCtrlOffset_Pulse12,
	eUnintCtrlOffset_Curr13	,	eUnintCtrlOffset_Pulse13,
	eUnintCtrlOffset_Curr14	,	eUnintCtrlOffset_Pulse14,
	eUnintCtrlOffset_Curr15	,	eUnintCtrlOffset_Pulse15,
	eUnintCtrlOffset_Curr16	,	eUnintCtrlOffset_Pulse16,
	eUnintCtrlOffset_Curr17	,	eUnintCtrlOffset_Pulse17,
	eUnintCtrlOffset_Curr18	,	eUnintCtrlOffset_Pulse18,
	eUnintCtrlOffset_Curr19	,	eUnintCtrlOffset_Pulse19,
	eUnintCtrlOffset_Curr20	,	eUnintCtrlOffset_Pulse20,
	
	eUnintCtrlOffset_SectNum0,
	eUnintCtrlOffset_LoadEepromDef,
	eUnintCtrlOffset_PowerSetValue,
	eUnintCtrlOffset_PosPWMdutyOpenLoop,
	eUnintCtrlOffset_NegPWMdutyOpenLoop,
	eUnintCtrlOffset_PosCMPDACOpenLoop,
	eUnintCtrlOffset_NegCMPDACOpenLoop,
	eUnintCtrlOffset_AdjustRatio1,
	eUnintCtrlOffset_AdjustRatio2,
	eUnintCtrlOffset_PwmOpenLoopEnable,
	eUnintCtrlOffset_CmpOpenLoopEnable,
	eUnintCtrlOffset_LoadAdjustDef,
	
	eUnintCtrlOffset_resd = eUnintCtrlOffset_LoadAdjustDef + 5,

	eUnintCtrlOffset_AdjustStart1,
	eUnintCtrlOffset_AdjustStart2,
	eUnintCtrlOffset_PosAdjustNum,
	eUnintCtrlOffset_NegAdjustNum,
	eUnintCtrlOffset_AdjustSave,
	eUnintCtrlOffset_AdjustMode,
	
	eUnintCtrlOffset_AdjustSet1,	eUnintCtrlOffset_AdjustAct1,
	eUnintCtrlOffset_AdjustSet2,	eUnintCtrlOffset_AdjustAct2,
	eUnintCtrlOffset_AdjustSet3,	eUnintCtrlOffset_AdjustAct3,
	eUnintCtrlOffset_AdjustSet4,	eUnintCtrlOffset_AdjustAct4,
	eUnintCtrlOffset_AdjustSet5,	eUnintCtrlOffset_AdjustAct5,
	eUnintCtrlOffset_AdjustSet6,	eUnintCtrlOffset_AdjustAct6,
	eUnintCtrlOffset_AdjustSet7,	eUnintCtrlOffset_AdjustAct7,
	eUnintCtrlOffset_AdjustSet8,	eUnintCtrlOffset_AdjustAct8,
	
}eUintCtrlOffset;


typedef enum
{
	eUintParaOffset_OutPutVoltRatio,
	eUintParaOffset_OutPutVoltBias,
	eUintParaOffset_AmbientTemperatureRatio,
	eUintParaOffset_AmbientTemperatureBias,
	eUintParaOffset_SinkTemperatureRatio,
	eUintParaOffset_SinkTemperatureBias,
	eUintParaOffset_PosCurrentRatio,
	eUintParaOffset_PosCurrentBias,
	eUintParaOffset_PosCurrent2Ratio,
	eUintParaOffset_PosCurrent2Bias,
	eUintParaOffset_SmallCurrentRatio,
	eUintParaOffset_SmallCurrentBias,
	eUintParaOffset_NegCurrentRatio,
	eUintParaOffset_NegCurrentBias,
	eUintParaOffset_NegCurrent2Ratio,
	eUintParaOffset_NegCurrent2Bias,
	eUintParaOffset_BusVoltRatio,
	eUintParaOffset_BusVoltBias,
	eUintParaOffset_SwitchDIPRatio,
	eUintParaOffset_SwitchDIPBias,
	
	eUintParaOffset_PwmCmpCtrl,
	eUintParaOffset_MixModeEnable,
	eUintParaOffset_SwFreq,
	eUintParaOffset_BoardType,
	eUintParaOffset_BoardAddr,
	eUintParaOffset_WorkMode,
	eUintParaOffset_DisplayMode,

	eUintParaOffset_PWMSyncEnable,
	eUintParaOffset_LineSyncEnable,
	eUintParaOffset_BusOverCheckEnable,
	eUintParaOffset_BusOverThreshold,
	eUintParaOffset_BusOverBackThreshold,
	eUintParaOffset_BusOverTurnOffEnable,
	eUintParaOffset_BusUnderCheckEnable,
	eUintParaOffset_BusUnderThreshold,
	eUintParaOffset_BusUnderBackThreshold,
	eUintParaOffset_BusUnderTurnOffEnable,
	
	eUintParaOffset_LoadVoltPosAvgOVEnable,
	eUintParaOffset_LoadVoltPosAvgOVThreshold,
	eUintParaOffset_LoadVoltPosAvgOVBackThreshold,
	eUintParaOffset_LoadVoltNegAvgOVEnable,
	eUintParaOffset_LoadVoltNegAvgOVThreshold,
	eUintParaOffset_LoadVoltNegAvgOVBackThreshold,
	eUintParaOffset_LoadCurrPosAvgOCEnable,
	eUintParaOffset_LoadCurrPosAvgOCThreshold,
	eUintParaOffset_LoadCurrPosAvgOCBackThreshold,
	eUintParaOffset_LoadCurrNegAvgOCEnable,
	eUintParaOffset_LoadCurrNegAvgOCThreshold,
	eUintParaOffset_LoadCurrNegAvgOCBackThreshold,
	eUintParaOffset_Power24VUnderEnable,
	eUintParaOffset_Power24VUnderThreshold,
	eUintParaOffset_Power24VUnderBackThreshold,
	eUintParaOffset_OverLoadCheckEnable,
	eUintParaOffset_OverLoadThreshold,
	eUintParaOffset_AmbientTempCheckEnable,
	eUintParaOffset_AmbientTempOverHighEnable,
	eUintParaOffset_AmbientTempOverHighThreshold,
	eUintParaOffset_AmbientTempOverLowEnable,
	eUintParaOffset_AmbientTempOverLowThreshold,
	eUintParaOffset_SinkTempCheckEnable,
	eUintParaOffset_SinkTempOverHighEnable,
	eUintParaOffset_SinkTempOverHighThreshold,
	eUintParaOffset_SinkTempOverLowEnable,
	eUintParaOffset_SinkTempOverLowThreshold,

	eUintParaOffset_PID_40K_A0,
	eUintParaOffset_PID_40K_A1,
	eUintParaOffset_PID_40K_A2,
	eUintParaOffset_PID_80K_A0,
	eUintParaOffset_PID_80K_A1,
	eUintParaOffset_PID_80K_A2,
	eUintParaOffset_PID_120K_A0,
	eUintParaOffset_PID_120K_A1,
	eUintParaOffset_PID_120K_A2,
	eUintParaOffset_PID_160K_A0,
	eUintParaOffset_PID_160K_A1,
	eUintParaOffset_PID_160K_A2,

	eUintParaOffset_UnitAddrMin,
	eUintParaOffset_UnitAddrMax,
	
	eUintParaOffset_CMPmodePWMCountMode,
	eUintParaOffset_Moduletype,
	eUintParaOffset_AddrMode,
	eUintParaOffset_HwOCCountSet,
	eUintParaOffset_HwOCRecoveryTimeSet,
	eUintParaOffset_MixModeRatio,
	eUintParaOffset_PowerSetValue,
	eUintParaOffset_CMPmodeSlopePos,
	eUintParaOffset_CMPmodeSlopeNeg,
	eUintParaOffset_CMPmodeSlopeEnable,
	eUintParaOffset_SynDelay,

	eUintParaOffset_AmbientTemplevelback,        //环境温度恢复阈值
	eUintParaOffset_SinkTemplevelback,           //散热器温度恢复阈值

    eUintParaOffset_WarnBusOverCheckEnable,      //BUS电压过压告警使能
    eUintParaOffset_WarnBusOverThreshold,        //BUS电压过压告警阈值
    eUintParaOffset_WarnBusOverBackThreshold,    //BUS电压过压告警恢复值
    eUintParaOffset_WarnBusUnderCheckEnable,     //BUS电压欠压告警使能
    eUintParaOffset_WarnBusUnderThreshold,       //BUS电压欠压告警阈值
    eUintParaOffset_WarnBusUnderBackThreshold,   //BUS电压欠压告警恢复值

    eUintParaOffset_HwOCCheckEnable,             //硬件过流故障使能禁止参数
    eUintParaOffset_HwOCFaultTime,               //硬件过流故障恢复时间:秒
    eUintParaOffset_HwOCFaultRecoverySet,        //硬件过流故障恢复次数

    eUintParaOffset_PWMSyncLockEnable,           //PWM同步锁丢失使能
    
	eUintParaOffset_resd = 175,						//如需增加参数在此之前增加
	eUintParaOffset_PosCurrAdjustRatio1,
  	eUintParaOffset_PosCurrAdjustBias1,
	eUintParaOffset_PosCurrAdjustRatio2,
  	eUintParaOffset_PosCurrAdjustBias2,
  	eUintParaOffset_PosCurrAdjustRatio3,
  	eUintParaOffset_PosCurrAdjustBias3,
  	eUintParaOffset_PosCurrAdjustRatio4,
  	eUintParaOffset_PosCurrAdjustBias4,
	
	eUintParaOffset_NegCurrAdjustRatio1,
	eUintParaOffset_NegCurrAdjustBias1,
	eUintParaOffset_NegCurrAdjustRatio2,
	eUintParaOffset_NegCurrAdjustBias2,
	eUintParaOffset_NegCurrAdjustRatio3,
	eUintParaOffset_NegCurrAdjustBias3,
	eUintParaOffset_NegCurrAdjustRatio4,
	eUintParaOffset_NegCurrAdjustBias4,
	
  	eUintParaOffset_PosVoltAdjustRatio1,
  	eUintParaOffset_PosVoltAdjustBias1,
	eUintParaOffset_PosVoltAdjustRatio2,
  	eUintParaOffset_PosVoltAdjustBias2,
  	eUintParaOffset_PosVoltAdjustRatio3,
  	eUintParaOffset_PosVoltAdjustBias3,
  	eUintParaOffset_PosVoltAdjustRatio4,
  	eUintParaOffset_PosVoltAdjustBias4,

	eUintParaOffset_NegVoltAdjustRatio1,
	eUintParaOffset_NegVoltAdjustBias1,
	eUintParaOffset_NegVoltAdjustRatio2,
	eUintParaOffset_NegVoltAdjustBias2,
	eUintParaOffset_NegVoltAdjustRatio3,
	eUintParaOffset_NegVoltAdjustBias3,
	eUintParaOffset_NegVoltAdjustRatio4,
	eUintParaOffset_NegVoltAdjustBias4,

	eUintParaOffset_PosCmpCurrAdjustRatio1,
  	eUintParaOffset_PosCmpCurrAdjustBias1,
  	eUintParaOffset_PosCmpCurrAdjustRatio2,
  	eUintParaOffset_PosCmpCurrAdjustBias2,
  	eUintParaOffset_PosCmpCurrAdjustRatio3,
  	eUintParaOffset_PosCmpCurrAdjustBias3,
  	eUintParaOffset_PosCmpCurrAdjustRatio4,
  	eUintParaOffset_PosCmpCurrAdjustBias4,
	
	eUintParaOffset_NegCmpCurrAdjustRatio1,
	eUintParaOffset_NegCmpCurrAdjustBias1,
	eUintParaOffset_NegCmpCurrAdjustRatio2,
	eUintParaOffset_NegCmpCurrAdjustBias2,
	eUintParaOffset_NegCmpCurrAdjustRatio3,
	eUintParaOffset_NegCmpCurrAdjustBias3,
	eUintParaOffset_NegCmpCurrAdjustRatio4,
	eUintParaOffset_NegCmpCurrAdjustBias4,
	
  	eUintParaOffset_PosCmpVoltAdjustRatio1,
  	eUintParaOffset_PosCmpVoltAdjustBias1,
  	eUintParaOffset_PosCmpVoltAdjustRatio2,
  	eUintParaOffset_PosCmpVoltAdjustBias2,
  	eUintParaOffset_PosCmpVoltAdjustRatio3,
  	eUintParaOffset_PosCmpVoltAdjustBias3,
  	eUintParaOffset_PosCmpVoltAdjustRatio4,
  	eUintParaOffset_PosCmpVoltAdjustBias4,

	eUintParaOffset_NegCmpVoltAdjustRatio1,
	eUintParaOffset_NegCmpVoltAdjustBias1,
	eUintParaOffset_NegCmpVoltAdjustRatio2,
	eUintParaOffset_NegCmpVoltAdjustBias2,
	eUintParaOffset_NegCmpVoltAdjustRatio3,
	eUintParaOffset_NegCmpVoltAdjustBias3,
	eUintParaOffset_NegCmpVoltAdjustRatio4,
	eUintParaOffset_NegCmpVoltAdjustBias4,

	eUintParaOffset_PosCmpDispCurrAdjustRatio1,
  	eUintParaOffset_PosCmpDispCurrAdjustBias1,
  	eUintParaOffset_PosCmpDispCurrAdjustRatio2,
  	eUintParaOffset_PosCmpDispCurrAdjustBias2,
  	eUintParaOffset_PosCmpDispCurrAdjustRatio3,
  	eUintParaOffset_PosCmpDispCurrAdjustBias3,
  	eUintParaOffset_PosCmpDispCurrAdjustRatio4,
  	eUintParaOffset_PosCmpDispCurrAdjustBias4,
	
	eUintParaOffset_NegCmpDispCurrAdjustRatio1,
	eUintParaOffset_NegCmpDispCurrAdjustBias1,
	eUintParaOffset_NegCmpDispCurrAdjustRatio2,
	eUintParaOffset_NegCmpDispCurrAdjustBias2,
	eUintParaOffset_NegCmpDispCurrAdjustRatio3,
	eUintParaOffset_NegCmpDispCurrAdjustBias3,
	eUintParaOffset_NegCmpDispCurrAdjustRatio4,
	eUintParaOffset_NegCmpDispCurrAdjustBias4,
}eUnitParaOffset;

#endif

