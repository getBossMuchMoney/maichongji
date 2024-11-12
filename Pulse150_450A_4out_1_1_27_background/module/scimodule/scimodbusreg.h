#ifndef _SCIMODBUSREG_H_
#define _SCIMODBUSREG_H_


//===============================================================
//MODBUS reg define
//===============================================================


typedef struct CTRL_STRUCT
{
  uint16_t 	OnOff:1;       //onoff   1：on  0：off  //
  uint16_t 	LoopMode:1;   // loopmode   0：const currrent     1：const volt
  uint16_t	res1:1;       //
  uint16_t 	OutMode:1;    // 0：pulse    1：Dc   //
  uint16_t  res2:8;    //
  uint16_t  SynRole:1;	//0:syn-slave 1:syn-master
  uint16_t  Online:1;    //0:offline 1:online
  uint16_t  res3:1;    //
  uint16_t  LocalRemote:1; // 0---remote   1---local  //
}ctrlStruct;

typedef union CTRL_NUM1
{
  uint16_t all;
  ctrlStruct bit;
}CtrlNum1;

typedef struct st_falt
{
    Uint16 OverCurrent      :1;             // 0 过流
    Uint16 OverVolt      	:1;             // 1 过压
    Uint16 Over45C    		:1;             // 2 环境温度超出45C
    Uint16 Over65C       	:1;             // 3 环境温度超出65C
    Uint16 Over75C      	:1;             // 4 功率板温度超出75C
    Uint16 PwrOff_OT      	:1;             // 5 环境温度过高关机
    Uint16 Leakage       	:1;             // 6 漏水检测
    Uint16 XFMR_OT			:1;				// 7 变压器过热

    Uint16 ComErr_Lcd       :1;             // 8 触摸屏通讯异常
    Uint16 ComErr_Remote    :1;             // 9 远程通讯异常
    Uint16 ComErr_Sync		:1;             // 10 主从机通讯异常(同步通讯异常）
    Uint16 RecoilErr_A    	:1;             // 11 A面反溃异常
    Uint16 RecoilErr_B      :1;             // 12 B面反溃异常
    Uint16 OverRange_SetA	:1;				// 13 A面设置值超出范围
    Uint16 OverRange_SetB	:1;				// 14 B面设置值超出范围
    Uint16 EEPROMErr        :1;             // 15 EEPROM 存取出错

	Uint16 OVOC2DC_A		:1;				// 16 A面过压过流转直流
	Uint16 OVOC2DC_B		:1;				// 17 B面过压过流转直流
	Uint16 Pwr1_Fault		:1;				// 18 前级电源1本机故障
	Uint16 Pwr1_ComErr		:1;				// 19 前级电源1通讯故障
	Uint16 Pwr2_Fault		:1;				// 20 前级电源2本机故障
	Uint16 Pwr2_ComErr		:1;				// 21 前级电源2通讯故障 
	Uint16 Reserved1		:10;            // 备用
}tp_FaultBit;

typedef union
{
    uint16_t FaultReg[2];
    tp_FaultBit FaultBit;
}tp_Fault;

typedef struct 
{
  int16_t Current; 	    // 电流 单位1A，两段正负脉冲电流
  uint16_t PulseWide; 	// 脉冲宽度 单位0.1ms，两段正负脉冲的宽度
}SectorInfo;


typedef struct CTRL_STRUCT2
{
    uint16_t mod1LoadDef :1;    //模块1内部存储写入默认值
    uint16_t mod2LoadDef :1;    //模块2内部存储写入默认值
    uint16_t mod3LoadDef :1;    //模块3内部存储写入默认值
    uint16_t mod4LoadDef :1;    //模块4内部存储写入默认值
    uint16_t mod5LoadDef :1;    //模块5内部存储写入默认值
    uint16_t mod6LoadDef :1;    //模块6内部存储写入默认值
    uint16_t mod7LoadDef :1;    //模块7内部存储写入默认值
    uint16_t mod8LoadDef :1;    //模块8内部存储写入默认值
    uint16_t res :4;            //
    uint16_t workMode :1;       //0--正常模式   1--强制小电流板工作（启用小电流时有效）
    uint16_t testMode :1;       //调试界面   1---调试模式  0--正常
    uint16_t loopMode :1;       //调节模式   0：闭环     1：开环
    uint16_t resetErr :1;       //故障复位   置1故障复位
} ctrlStruct2;

typedef union CTRL_NUM2
{
  uint16_t all;
  ctrlStruct2 bit;
}CtrlNum2;


typedef struct CTRLMODE_STRUCT
{
    uint16_t res1 :1;			//
    uint16_t synmode :1;		//0-独立工作模式 1-同步工作模式
    uint16_t res2 :10;			//
    uint16_t powermode :1;		//前级电源模式 0-固定 1-自动
    uint16_t pwmorcmp :1;		//0-数字控制 1-模拟控制
} ctrlModeStruct;

typedef union CTRL_MODE
{
  uint16_t all;
  ctrlModeStruct bit;
}uCtrlMode;

typedef struct _StruUnitCtrl
{
	uint16_t unit1Disable :1;			//0:normal,1:disable
	uint16_t unit2Disable :1;			//0:normal,1:disable
	uint16_t unit3Disable :1;			//0:normal,1:disable
	uint16_t unit4Disable :1;			//0:normal,1:disable
}StruUnitCtrl;

typedef union UNIT_CTRL
{
  uint16_t all;
  StruUnitCtrl bit;
}uUnitCtlr;


#define SECTOR_MAX	20  // sector number

typedef	 struct
{
  CtrlNum1 Ctrl1;
  SectorInfo SectASet[SECTOR_MAX];
  int16_t PhaseShift[4];			//41-44 相移
  uint16_t SectNum;					//45 电流段数
  uUnitCtlr UnitCtrl;				//46 子卡控制
  uint16_t resv1[34];
  uint16_t PhaseShiftAB;            // 81 AB面相移
  uint16_t PowerDebugVolt;          // 82 前级电源调试电压值
  CtrlNum2 Ctrl2;                   // 83 控制信号2
  uint16_t LoadEepromDef;			// 84 恢复默认参数
  uint16_t SyncPhase;               // 85 同步相移角度

}sModuleCtrlregion;

typedef struct
{
	uint16_t res:15;			//
	uint16_t remoteCommErr:1;		//远端通讯异常
}sModuleFaultInfo;

typedef union
{
  uint16_t all;
  sModuleFaultInfo bit;
}uModuleFaultInfo;

typedef struct
{
	uint16_t res:16;			//
}sModuleWarnInfo;

typedef union
{
  uint16_t all;
  sModuleWarnInfo bit;
}uModuleWarnInfo;


typedef struct 
{
	CtrlNum1 workStatus;
  	uModuleFaultInfo faultInfo;
	uModuleWarnInfo warnInfo;
	uint16_t ver[3];	
	uint16_t Machine;
}sModuleStatusRegion;

typedef struct 
{	
	uint16_t BoardType;		//0：模块控制板，1：单元控制板
	uint16_t BoardAddr;
	uint16_t BoardRole;		//0：主机，	1：从机
	uint16_t WorkMode;		//0:同步模式，1：独立模式
	uint16_t UnitAddrMin;
	uint16_t UnitAddrMax;
	uint16_t MaxCurrent;
	uint16_t MinCurrent;
	uint16_t AddrMode;		//0:拨码开关模式，1：手动设置模式
	uint16_t SynDelay;
	uint16_t SynId;
}sModuleParamRegion;


typedef struct 
{
  uint16_t setValue; 	    // 设置值
  uint16_t actualValue; 		// 实际值
}sAdjustPara;

typedef struct 
{
	uint16_t startAdjust1 :1;			//0:stop,1:start
	uint16_t startAdjust2 :1;			//0:stop,1:start
	uint16_t startAdjust3 :1;			//0:stop,1:start
	uint16_t startAdjust4 :1;			//0:stop,1:start
	uint16_t startAdjust5 :1;			//0:stop,1:start
	uint16_t startAdjust6 :1;			//0:stop,1:start
	uint16_t startAdjust7 :1;			//0:stop,1:start
	uint16_t startAdjust8 :1;			//0:stop,1:start
	uint16_t startAdjust9 :1;			//0:stop,1:start
	uint16_t startAdjust10 :1;			//0:stop,1:start
	uint16_t startAdjust11 :1;			//0:stop,1:start
	uint16_t startAdjust12 :1;			//0:stop,1:start
	uint16_t startAdjust13 :1;			//0:stop,1:start
	uint16_t startAdjust14 :1;			//0:stop,1:start
	uint16_t startAdjust15 :1;			//0:stop,1:start
	uint16_t startAdjust16 :1;			//0:stop,1:start
}sStartAdjust1;

typedef struct 
{
	uint16_t startAdjust17:1;			//0:stop,1:start
	uint16_t startAdjust18:1;			//0:stop,1:start
	uint16_t startAdjust19:1;			//0:stop,1:start
	uint16_t startAdjust20:1;			//0:stop,1:start

	uint16_t res :12;			//resd
}sStartAdjust2;

typedef union
{
  uint16_t all;
  sStartAdjust1 bit;
}uStartAdjust1;
typedef union
{
  uint16_t all;
  sStartAdjust2 bit;
}uStartAdjust2;

typedef enum
{
	eAdjustCurrMode,
	eAdjustVoltMode,
	eAdjustSmallCurrMode,
}eAdjustMode;
	
typedef struct 
{
	CtrlNum1 Ctrl1;
  	SectorInfo SectASet[SECTOR_MAX];
	uint16_t SectNum;
	uint16_t LoadEepromDef;

	uint16_t PowerSetValue;	//前级电源电压
	uint16_t PosPWMdutyOpenLoop;
	uint16_t NegPWMdutyOpenLoop;
	uint16_t PosCMPDACOpenLoop;
	uint16_t NegCMPDACOpenLoop;
	uint16_t AdjustRatio1;
	uint16_t AdjustRatio2;

	uint16_t PwmOpenLoopEnable;
	uint16_t CmpOpenLoopEnable;

	uint16_t res[6];
	uStartAdjust1 StartAdjust1;
	uStartAdjust2 StartAdjust2;
	
	uint16_t PosAdjustNum;
	uint16_t NegAdjustNum;
	uint16_t SaveAdjust;
	eAdjustMode AdjustMode;

	sAdjustPara PosAdjustPara[4];
	sAdjustPara NegAdjustPara[4];
	
}sUnitCtrlRegion;

typedef struct
{
	uint16_t HwLoadOVpos:1;			//负载正向电压瞬时值过压
	uint16_t HwLoadOVneg:1;			//负载负向电压瞬时值过压
	uint16_t HwLoadOCpos:1;			//负载正向电流瞬时值过压
	uint16_t HwLoadOCneg:1;			//负载负向电流瞬时值过压
	uint16_t HwExtLoadOC:1;			//负载硬件正负向过流
	uint16_t Hw24Vabnormal:1;		//24V电源低压
	uint16_t HwLoadAvgOV:1;			//负载电压平均值过压
	uint16_t HwLoadAvgOC:1;			//负载电流平均值过流
	uint16_t BusOver:1;				//母线过压
	uint16_t BusUnder:1;			//母线欠压
	uint16_t AmbientHighOT:1;		//环境温度高过温
	uint16_t SinkHighOT:1;			//散热器温度高过温
	uint16_t PWMSyncLoss:1;			//PWM同步信号丢失
	uint16_t PWMLockLoss:1;			//PWM同步信号失锁
	uint16_t LineSyncLoss:1;		//工频同步信号丢失
	uint16_t LineLockLoss:1;		//工频同步信号失锁
}sUnitFaultInfo1;

typedef enum
{
	eUnitFaultInfo1_HwLoadOVpos,
	eUnitFaultInfo1_HwLoadOVneg,
	eUnitFaultInfo1_HwLoadOCpos,
	eUnitFaultInfo1_HwLoadOCneg,
	eUnitFaultInfo1_HwExtLoadOC,
	eUnitFaultInfo1_Hw24Vabnormal,
	eUnitFaultInfo1_HwLoadAvgOV,
	eUnitFaultInfo1_HwLoadAvgOC,
	eUnitFaultInfo1_BusOver,
	eUnitFaultInfo1_BusUnder,
	eUnitFaultInfo1_AmbientHighOT,
	eUnitFaultInfo1_SinkHighOT,
	eUnitFaultInfo1_PWMSyncLoss,
	eUnitFaultInfo1_PWMLockLoss,
	eUnitFaultInfo1_LineSyncLoss,
	eUnitFaultInfo1_LineLockLoss,
}eUnitFaultInfo1_bit;

typedef union
{
  uint16_t all;
  sUnitFaultInfo1 bit;
}unUnitFaultInfo1;
typedef struct
{
	uint16_t communicationFault:1;	//前级电源通讯故障
	uint16_t OC:1;					//过流
	uint16_t Err:1;					//误差
	uint16_t OV:1;					//过压
	uint16_t PhaseDef:1;			//缺相
	uint16_t EepromRead:1;
	uint16_t EepromWrite:1;
	uint16_t resd:9;				//

}sUnitFaultInfo2;

typedef enum
{
	eUnitFaultInfo2_communicationFault,
	eUnitFaultInfo2_OC,
	eUnitFaultInfo2_Err,
	eUnitFaultInfo2_OV,
	eUnitFaultInfo2_PhaseDef,
	eUnitFaultInfo2_EepromRead,
	eUnitFaultInfo1_EepromWrite,
}eUnitFaultInfo2_bit;

typedef union
{
  uint16_t all;
  sUnitFaultInfo2 bit;
}unUnitFaultInfo2;

typedef struct
{
	uint16_t SinkOT:1;				//散热器过温
	uint16_t AmbientOT:1;			//环境温度过温
	uint16_t AmbientNTCerr:1;		//环境NTC故障
	uint16_t SinkNTCerr:1;			//散热器NTC故障
	uint16_t SinkNTCOT:1;			//散热器低温
	uint16_t PWMSyncLoss:1;			//PWM同步信号丢失
	uint16_t PWMLockLoss:1;			//PWM同步信号失锁
	uint16_t LineSyncLoss:1;		//工频同步信号丢失
	uint16_t LineLockLoss:1;		//工频同步信号失锁
	uint16_t BusOver:1;				//母线过压
	uint16_t BusUnder:1;			//母线欠压
	uint16_t LoadPosAvgOV:1;		//负载正向电压平均值过压
	uint16_t LoadNegAvgOV:1;		//负载负向电压平均值过压
	uint16_t LoadPosAvgOC:1;		//负载正向电流平均值过压
	uint16_t LoadNegAvgOC:1;		//负载负向电流平均值过压
	uint16_t OverLoad:1;			//过载
}sUnitWarnInfo1;

typedef enum
{
	eUnitWarnInfo1_SinkOT,
	eUnitWarnInfo1_AmbientOT,
	eUnitWarnInfo1_AmbientNTCerr,
	eUnitWarnInfo1_SinkNTCerr,
	eUnitWarnInfo1_SinkNTCOT,
	eUnitWarnInfo1_PWMSyncLoss,
	eUnitWarnInfo1_PWMLockLoss,
	eUnitWarnInfo1_LineSyncLoss,
	eUnitWarnInfo1_LineLockLoss,
	eUnitWarnInfo1_BusOver,
	eUnitWarnInfo1_BusUnder,
	eUnitWarnInfo1_LoadPosAvgOV,
	eUnitWarnInfo1_LoadNegAvgOV,
	eUnitWarnInfo1_LoadPosAvgOC,
	eUnitWarnInfo1_LoadNegAvgOC,
	eUnitWarnInfo1_OverLoad,
}eUnitWarnInfo1_bit;

typedef union
{
  uint16_t all;
  sUnitWarnInfo1 bit;
}unUnitWarnInfo1;

typedef struct 
{
	CtrlNum1 workStatus;
  	unUnitFaultInfo1 faultInfo1;
	unUnitFaultInfo2 faultInfo2;
	unUnitFaultInfo1 res1[2];
	unUnitWarnInfo1 warnInfo1;
	unUnitWarnInfo1 res2;
	int16_t LoadPosAvgC;			//正向峰值电流
	int16_t LoadNegAvgC;			//负向峰值电流
	int16_t LoadPosAvgV;			//正向峰值电压
	int16_t LoadNegAvgV;			//负向峰值电压
	uint16_t ver[3];
	uint16_t PowerVolt;
	uint16_t PowerCurr;
	uint16_t BusVolt;
	uint16_t SinkTemp;			
	int16_t SectorAvgCurr[20];	
}sUnitStatusRegion;

typedef struct
{
  uint16_t 	OnOff:4;       //onoff   1：on  0：off  //
  uint16_t 	OutMode:1;    // 0：pulse    1：Dc   //
  uint16_t  res2:11;
}SynRemoteCtrlStruct;

typedef union
{
  uint16_t all;
  SynRemoteCtrlStruct bit;
}sSynRemoteCtrlNum1;

typedef struct 
{
  int16_t Current[4]; 	    // 电流 单位1A，两段正负脉冲电流
  uint16_t PulseWide; 	// 脉冲宽度 单位0.1ms，两段正负脉冲的宽度
}sSynRemoteSectorInfo;

typedef	 struct
{
  sSynRemoteCtrlNum1 Ctrl;
  sSynRemoteSectorInfo SectASet[SECTOR_MAX];
  uint16_t SynPhase;
  uint16_t SynPhaseRes[4];
}sSynRemoteCtrlRegion;

typedef	 struct
{
  sSynRemoteCtrlNum1 Ctrl;
  SectorInfo SectASet[SECTOR_MAX];
  uint16_t SynPhase;
}sSynRemoteUintCtrlRegion;


typedef struct
{
  uint16_t OnOff:1;       		//onoff   1：on  0：off  //
  uint16_t res1:2;       		//
  uint16_t OutMode:1;    		// 0：pulse    1：Dc   //
  uint16_t res2:9;    			//
  uint16_t local:1;				//0:normal,1:abnormal
  uint16_t remote:1;			//0:normal,1:abnormal
  uint16_t AutoManual:1;				//
}sSynRemoteStatus1;

typedef union
{
  uint16_t all;
  sSynRemoteStatus1 bit;
}uSynRemoteStatus1;

typedef struct
{
	uint16_t HwExtLoadOC:1; 		//负载硬件正负向过流
	uint16_t OverLoad:1;			//过载
	uint16_t SinkOT:1;				//散热器过温
	uint16_t AmbientOT:1;			//环境温度过温
	uint16_t PWMSyncLoss:1; 		//PWM同步信号丢失
	uint16_t BusOver:1; 			//母线过压
	uint16_t BusUnder:1;			//母线欠压
	uint16_t LoadPosAvgOV:1;		//负载正向电压平均值过压
	uint16_t LoadNegAvgOV:1;		//负载负向电压平均值过压
	uint16_t LoadPosAvgOC:1;		//负载正向电流平均值过流
	uint16_t LoadNegAvgOC:1;		//负载负向电流平均值过流
	uint16_t Hw24Vabnormal:1;		//24V电源低压
	uint16_t communicationFault:1;	//前级电源通讯故障

}sSynRemoteStatus2;

typedef union
{
  uint16_t all;
  sSynRemoteStatus2 bit;
}uSynRemoteStatus2;


typedef struct
{
	uSynRemoteStatus1 status1;
	uSynRemoteStatus2 status2;
	int16_t LoadPosAvgC;			//正向峰值电流
	int16_t LoadNegAvgC;			//负向峰值电流
	int16_t LoadPosAvgV;			//正向峰值电压
	int16_t LoadNegAvgV;			//负向峰值电压
}sSynRemoteStatusRegion;

typedef struct 
{
	uint16_t VavgPosRatio;		
	uint16_t VavgNegRatio;		
	uint16_t IavgPosRatio;		
	uint16_t IavgNegRatio;		
	uint16_t VavgPosDCRatio;	
	uint16_t VavgNegDCRatio;	
	uint16_t IavgPosDCRatio;	
	uint16_t IavgNegDCRatio;	
	uint16_t OutPutVoltRatio;				
	uint16_t AmbientTemperatureRatio;		
	uint16_t SinkTemperatureRatio;			
	uint16_t PosCurrentRatio;				
	uint16_t PosCurrentRatio2;				
	uint16_t SmallCurrentRatio;			
	uint16_t NegCurrentRatio;				
	uint16_t NegCurrentRatio2;				
	uint16_t BusVoltRatio;					
	uint16_t SwitchDIPRatio;				

	int16_t VavgPosBias;		
	int16_t VavgNegBias;		
	int16_t IavgPosBias;		
	int16_t IavgNegBias;		
	int16_t VavgPosDCBias;	
	int16_t VavgNegDCBias;	
	int16_t IavgPosDCBias;	
	int16_t IavgNegDCBias;	
	int16_t OutPutVoltBias; 				
	int16_t AmbientTemperatureBias; 		
	int16_t SinkTemperatureBias; 			
	int16_t PosCurrentBias; 				
	int16_t PosCurrentBias2; 			
	int16_t SmallCurrentBias; 			
	int16_t NegCurrentBias; 				
	int16_t NegCurrentBias2; 				
	int16_t BusVolttBias; 				
	int16_t SwitchDIPBias;				
}sUnitParamRegion;


#endif   //_SCIMODBUSREG_H_


