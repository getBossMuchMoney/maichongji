#ifndef	_EEPROM_PARAM_DEFAULT_H_
#define	_EEPROM_PARAM_DEFAULT_H_

#define UNIT_PARA_DEFAULT_HEAD	0xAABB
#define MODULE_PARAM_DEFAULT_HEAD		0xCCDD
#define UNIT_ADJUST_PARAM_DEFAULT_HEAD	0xEEFF


#define UNIT_PARAM_DEFAULT_TAB {\
	.Magic=UNIT_PARA_DEFAULT_HEAD,				\
	.Len=0,							\
	/*100/((4095/3.3)*(1.6/52)), 100times*/		\
	.OutPutVolt.Ratio = 26190,					\
	/*1.2/3.3*4095*/							\
	.OutPutVolt.Bias = -15350,					\
	.AmbientTemperature.Ratio = 10000,			\
	.AmbientTemperature.Bias = 0,				\
	.SinkTemperature.Ratio = 10000,				\
	.SinkTemperature.Bias = 0,					\
	/*10/((4095/3.3)*(3.3/660)), 10times*/		\
	.PosCurrent.Ratio = 2828,					\
	.PosCurrent.Bias = 0,					    \
	/*10/((4095/3.3)*(3.3/660)), 10times*/		\
	.PosCurrent2.Ratio = 2828,					\
	.PosCurrent2.Bias = 0,					    \
	/*100/40.95 , 100times*/					\
	.SmallCurrent.Ratio = 24420,				\
	.SmallCurrent.Bias = -18910,				\
	/*10/((4095/3.3)*(3.3/660)), 10times*/		\
	.NegCurrent.Ratio = 6017,					\
	.NegCurrent.Bias = 0,						\
	/*10/((4095/3.3)*(3.3/660)), 10times*/		\
	.NegCurrent2.Ratio = 6017,					\
	.NegCurrent2.Bias = 0,					    \
	/*100/40.95 , 100times*/					\
	.BusVolt.Ratio = 13456,						\
	.BusVolt.Bias = 0,							\
	.SwitchDIP.Ratio = 10000,					\
	.SwitchDIP.Bias = 0,						\
	.PwmCmpCtrl = 1,							\
	.MixModeEnable = 1,							\
	.SwFreq = 0,								\
												\
	.BoardType = 1,								\
	.BoardAddr = 8,								\
	.WorkMode = 0,								\
	.DisplayMode = 0,							\
												\
	.PWMSyncEnable = 0,							\
	.LineSyncEnable = 0,						\
												\
	.BusOverCheckEnable = 0,					\
	.BusOverThreshold = 50,						\
	.BusOverBackThreshold = 48,					\
	.BusOverTurnOffEnable = 0,					\
	.BusUnderCheckEnable = 0,					\
	.BusUnderThreshold = 20,				    \
	.BusUnderBackThreshold = 22,			    \
	.BusUnderTurnOffEnable = 0,					\
												\
	.LoadVoltPosAvgOVEnable = 1,				\
	.LoadVoltPosAvgOVThreshold = 1200,			\
	.LoadVoltPosAvgOVBackThreshold = 1100,		\
	.LoadVoltNegAvgOVEnable = 1,				\
	.LoadVoltNegAvgOVThreshold = 3600,			\
	.LoadVoltNegAvgOVBackThreshold = 3400,		\
	.LoadCurrPosAvgOCEnable = 1,				\
	.LoadCurrPosAvgOCThreshold = 800,			\
	.LoadCurrPosAvgOCBackThreshold = 700,		\
	.LoadCurrNegAvgOCEnable = 1,				\
	.LoadCurrNegAvgOCThreshold = 1900,			\
	.LoadCurrNegAvgOCBackThreshold = 1800,		\
												\
	.Power24VUnderEnable = 1,					\
	.Power24VUnderThreshold = 0,				\
	.Power24VUnderBackThreshold = 1,			\
												\
	.OverLoadCheckEnable = 1,					\
	.OverLoadThreshold = 400,					\
												\
	.AmbientTempCheckEnable = 0,				\
	.AmbientTempOverHighEnable = 1,				\
	.AmbientTempOverHighThreshold = 65,			\
	.AmbientTempOverLowEnable = 1,				\
	.AmbientTempOverLowThreshold = 60,			\
	.SinkTempCheckEnable = 1,					\
	.SinkTempOverHighEnable = 1,				\
	.SinkTempOverHighThreshold = 65,			\
	.SinkTempOverLowEnable = 0,					\
	.SinkTempOverLowThreshold = 60,				\
												\
	.PID[0].A0 = 10,						    \
	.PID[0].A1 = 3000,							\
	.PID[0].A2 = 1200,							\
	.PID[1].A0 = 3,								\
	.PID[1].A1 = 2400,							\
	.PID[1].A2 = 1200,							\
	.PID[2].A0 = 4,								\
	.PID[2].A1 = 2800,							\
	.PID[2].A2 = 1200,							\
	.PID[3].A0 = 6,								\
	.PID[3].A1 = 2000,							\
	.PID[3].A2 = 1200,							\
												\
	.UnitAddrMin = 8,							\
	.UnitAddrMax = 11,							\
												\
	.CMPmodePWMCountMode = 0,					\
	.Moduletype = 4,							\
	.AddrMode = 0,								\
	.HwOCCountSet = 5,                          \
	.HwOCRecoveryTimeSet = 20,                  \
	.MixModeRatio = 4,							\
	.PowerSetValue = 32,						\
    /*1.0A/us*/                                 \
    .CMPmodeSlopePos = 100,                     \
    /*3.0A/us*/                                 \
    .CMPmodeSlopeNeg = 300,                     \
    .CMPmodeSlopeEnable = 0,                    \
    .SynDelay = 5,								\
                                                \
    .AmbientTemplevelback = 58,                 \
    .SinkTemplevelback = 58,                    \
                                                \
    .WarnBusOverCheckEnable = 1,                \
    .WarnBusOverThreshold = 47,                 \
    .WarnBusOverBackThreshold = 45,             \
    .WarnBusUnderCheckEnable = 1,               \
    .WarnBusUnderThreshold = 23,                \
    .WarnBusUnderBackThreshold = 25,            \
                                                \
    .HwOCCheckEnable = 0,                       \
    .HwOCFaultTime = 60,                        \
    .HwOCFaultRecoverySet = 10,                 \
                                                \
    .PWMSyncLockEnable = 1,                     \
}

#define MODULE_PARAM_DEFAULT_TAB {\
	.Magic=MODULE_PARAM_DEFAULT_HEAD,					\
	.Len=0,							\
	.BoardType = 0,					\
	.BoardAddr = 1,					\
	.BoardRole = 0,					\
	.WorkMode = 0,					\
	.UnitAddrMin = 8,				\
	.UnitAddrMax = 11,				\
	.MaxCurrent = 500,              \
	.MinCurrent = 1500,             \
	.AddrMode = 0,					\
	.SynDelay = 12,					\
	.SynId = 1,						\
}
#endif
