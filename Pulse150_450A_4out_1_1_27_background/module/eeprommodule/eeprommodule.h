/*=============================================================================*
 *         Copyright(c) 2010-2014, Invt Co., Led.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  ThreePhase4_6KW  Inverter
 *
 *  FILENAME : eeprommodule.h
 *  PURPOSE  :
 *  AUTHOR   : Tobias
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2015-05-29       V1.0                          Created.
 *
 *============================================================================*/
#ifndef MODULE_EEPROMMODULE_H_
#define MODULE_EEPROMMODULE_H_
#include "f280013x_device.h"
#include "squeue.h"
//#include "scimodule.h"
#include "scimodbusreg.h"
#include "EepromParamDefault.h"

#ifdef  EEPROMMODULE_GLOBALS
#define EEROMMDL_EXT
#else
#define EEROMMDL_EXT extern
#endif


typedef	struct struct_eecmd {
	Uint16  Cmd;
	Uint16  EepromAddr;
	Uint16* pram;
	Uint16  length;
	Uint16  reversed;
}stEepromCmd;

typedef union {
	Uint16 all[6];
	stEepromCmd bit;
}typeEepromCmd;

typedef struct
{
	Uint16 Ratio;
	int16 Bias;
}sRatioBias;
typedef struct
{
	sRatioBias VavgPos;		//
	sRatioBias VavgNeg;		//
	sRatioBias IavgPos;		//
	sRatioBias IavgNeg;		//
	sRatioBias VavgPosDC;	//
	sRatioBias VavgNegDC;	//
	sRatioBias IavgPosDC;	//
	sRatioBias IavgNegDC;	//
}sAvgPara;
typedef struct
{
	Uint16 A0;	//10000 times
	Uint16 A1;	//10000 times
	Uint16 A2;	//10000 times
}sPIDPara;

#define ADJUST_SECTOR_NUM	4
typedef struct{

	sRatioBias PosCurrAdjustFactor[ADJUST_SECTOR_NUM];
	sRatioBias NegCurrAdjustFactor[ADJUST_SECTOR_NUM];
	sRatioBias PosVoltAdjustFactor[ADJUST_SECTOR_NUM];
	sRatioBias NegVoltAdjustFactor[ADJUST_SECTOR_NUM];

	sRatioBias PosCmpCurrAdjustFactor[ADJUST_SECTOR_NUM];
	sRatioBias NegCmpCurrAdjustFactor[ADJUST_SECTOR_NUM];
	sRatioBias PosCmpVoltAdjustFactor[ADJUST_SECTOR_NUM];
	sRatioBias NegCmpVoltAdjustFactor[ADJUST_SECTOR_NUM];

  	sRatioBias PosCmpDispCurrAdjustFactor[ADJUST_SECTOR_NUM];
	sRatioBias NegCmpDispCurrAdjustFactor[ADJUST_SECTOR_NUM];
}sUnitADJUSTFACTOR;


/********************************************************************************
要求新增数据只能在后边依次增加
**********************************************************************************/
typedef struct
{
	Uint16 Magic;			//0xAABB
	Uint16 Len;				
//	sAvgPara VoltCurravg[4];
	sRatioBias OutPutVolt;				//
	sRatioBias AmbientTemperature;		//
	sRatioBias SinkTemperature;			//
	sRatioBias PosCurrent;				//
	sRatioBias PosCurrent2;				//
	sRatioBias SmallCurrent;			//
	sRatioBias NegCurrent;				//
	sRatioBias NegCurrent2;				//
	sRatioBias BusVolt;					//
	sRatioBias SwitchDIP;				//
	Uint16 PwmCmpCtrl;					//数字模拟控制
	Uint16 MixModeEnable;				//混合模式使能
  	Uint16 SwFreq;						//开关频率

	Uint16 BoardType;		//0：模块控制板，1：单元控制板
	Uint16 BoardAddr;
	Uint16 WorkMode;		//0:同步模式，1：独立模式
	Uint16 DisplayMode;		//0:正常模式，1：展机模式

	Uint16 PWMSyncEnable;
	Uint16 LineSyncEnable;
	
	Uint16 BusOverCheckEnable;
	Uint16 BusOverThreshold;
	Uint16 BusOverBackThreshold;
	Uint16 BusOverTurnOffEnable;
	Uint16 BusUnderCheckEnable;
	Uint16 BusUnderThreshold;
	Uint16 BusUnderBackThreshold;
	Uint16 BusUnderTurnOffEnable;

	Uint16 LoadVoltPosAvgOVEnable;
	Uint16 LoadVoltPosAvgOVThreshold;
	Uint16 LoadVoltPosAvgOVBackThreshold;
	Uint16 LoadVoltNegAvgOVEnable;
	Uint16 LoadVoltNegAvgOVThreshold;
	Uint16 LoadVoltNegAvgOVBackThreshold;
	Uint16 LoadCurrPosAvgOCEnable;
	Uint16 LoadCurrPosAvgOCThreshold;
	Uint16 LoadCurrPosAvgOCBackThreshold;
	Uint16 LoadCurrNegAvgOCEnable;
	Uint16 LoadCurrNegAvgOCThreshold;
	Uint16 LoadCurrNegAvgOCBackThreshold;

	Uint16 Power24VUnderEnable;
	Uint16 Power24VUnderThreshold;
	Uint16 Power24VUnderBackThreshold;

	Uint16 OverLoadCheckEnable;
	Uint16 OverLoadThreshold;

	Uint16 AmbientTempCheckEnable;
	Uint16 AmbientTempOverHighEnable;
	Uint16 AmbientTempOverHighThreshold;
	Uint16 AmbientTempOverLowEnable;
	Uint16 AmbientTempOverLowThreshold;
	Uint16 SinkTempCheckEnable;
	Uint16 SinkTempOverHighEnable;
	Uint16 SinkTempOverHighThreshold;
	Uint16 SinkTempOverLowEnable;
	Uint16 SinkTempOverLowThreshold;

	sPIDPara PID[4]; 

	Uint16 UnitAddrMin;
	Uint16 UnitAddrMax;

	Uint16 CMPmodePWMCountMode;         //0:UP-DOWN Count Mode  1:UP Count Mode
	Uint16 Moduletype;      			//0:100A/-300A  1:150A/-450A  2:200A/-600A  3:150A/-750A  4:50A/-150A
	Uint16 AddrMode;					//0:拨码开关模式，1：手动设置模式
	Uint16 HwOCCountSet;                //硬件过流保护频率设置
	Uint16 HwOCRecoveryTimeSet;         //硬件过流恢复时间设置
	Uint16 MixModeRatio;				//混和模式分界比例
	Uint16 PowerSetValue;				//前级电源电压
    Uint16 CMPmodeSlopePos;             //模拟比较模式上升斜率 xxA/us 参数值扩大100倍
    Uint16 CMPmodeSlopeNeg;             //模拟比较模式下降斜率 xxA/us 参数值扩大100倍
    Uint16 CMPmodeSlopeEnable;          //模拟比较模式斜率功能使能/禁止
	Uint16 SynDelay;
	
	Uint16 AmbientTemplevelback;        //环境温度恢复阈值
	Uint16 SinkTemplevelback;           //散热器温度恢复阈值

    Uint16 WarnBusOverCheckEnable;      //BUS电压过压告警使能
    Uint16 WarnBusOverThreshold;        //BUS电压过压告警阈值
    Uint16 WarnBusOverBackThreshold;    //BUS电压过压告警恢复值
    Uint16 WarnBusUnderCheckEnable;     //BUS电压欠压告警使能
    Uint16 WarnBusUnderThreshold;       //BUS电压欠压告警阈值
    Uint16 WarnBusUnderBackThreshold;   //BUS电压欠压告警恢复值

    Uint16 HwOCCheckEnable;             //硬件过流故障使能禁止参数
    Uint16 HwOCFaultTime;               //硬件过流故障恢复时间:秒
    Uint16 HwOCFaultRecoverySet;        //硬件过流故障恢复次数

    Uint16 PWMSyncLockEnable;           //PWM同步锁丢失使能
	
}sUnitEEPROM;



EEROMMDL_EXT sUnitEEPROM sUintEepromPara;
#ifdef  EEPROMMODULE_GLOBALS
EEROMMDL_EXT sUnitEEPROM sUintEepromParaDef=UNIT_PARAM_DEFAULT_TAB;
#else
EEROMMDL_EXT sUnitEEPROM sUintEepromParaDef;
#endif

/********************************************************************************
要求新增数据只能在后边依次增加
**********************************************************************************/
typedef struct
{
	Uint16 Magic;			//0xEEFF
	Uint16 Len;		
	sUnitADJUSTFACTOR AdjustFactor;
}sUnitEEPROMAdjust;

EEROMMDL_EXT sUnitEEPROMAdjust sUintEepromAdjustPara;

/********************************************************************************
要求新增数据只能在后边依次增加
**********************************************************************************/
typedef struct
{
	Uint16 Magic;			//0xCCDD
	Uint16 Len;	
	Uint16 BoardType;		//0：模块控制板，1：单元控制板
	Uint16 BoardAddr;
	Uint16 BoardRole;		//0：主机，	1：从机
	Uint16 WorkMode;		//0:同步模式，1：独立模式
	Uint16 UnitAddrMin;
	Uint16 UnitAddrMax;
	Uint16 MaxCurrent;      //额定正电流
	Uint16 MinCurrent;      //额定负电流
	Uint16 AddrMode;		//0:拨码开关模式，1：手动设置模式
	Uint16 SynDelay;
	Uint16 SynId;
}sModuleEEPROM;

EEROMMDL_EXT sModuleEEPROM sModuleEepromPara;
#ifdef  EEPROMMODULE_GLOBALS
EEROMMDL_EXT sModuleEEPROM sModuleEepromParaDef=MODULE_PARAM_DEFAULT_TAB;
#else
EEROMMDL_EXT sModuleEEPROM sModuleEepromParaDef;
#endif


#define EEPROM_SLAVE_ADDR       0x50

#define EEPROM_SIZE_BYTE        2048 

#define EEPROM_PARA_START		0
#define EEPROM_ADJUST_PARA_START	512

#define	EEPROM_CMD_READ		0
#define	EEPROM_CMD_WRITE	1

#define EEPROM_PARA_OFFSET	2

EEROMMDL_EXT typeEepromCmd EepromCmdSendBySciMdb;
EEROMMDL_EXT Uint32 EepromInitalDelay;
EEROMMDL_EXT Uint16 EepromGridStandNeedInit;
EEROMMDL_EXT QUEUE  EepromCmdeQueue;

EEROMMDL_EXT void I2CWrite(uint16_t slaveAddr, uint16_t memAddr, uint16_t *srcAddr,uint16_t byteCount);
EEROMMDL_EXT uint16_t I2CRead(uint16_t slaveAddr, uint16_t memAddr, uint16_t *dstAddr, uint16_t byteCount);
EEROMMDL_EXT Uint16	EepromReadAword(Uint16 EepromAddr,Uint16 *dat);
EEROMMDL_EXT Uint16 EepromWriteAword(Uint16 EepromAddr,Uint16 data);
EEROMMDL_EXT Uint16 ReadEepromDataLong(Uint16 EepromAddr,Uint16* pRAM,Uint16 wordLength);
EEROMMDL_EXT Uint16 WriteEepromDataLong(Uint16 EepromAddr,Uint16* pRAM,Uint16 Length);
EEROMMDL_EXT Uint16 LoadEeprom(void);
EEROMMDL_EXT Uint16  VerifyUnitEeprom(void);
EEROMMDL_EXT Uint16 VerifyUnitAdjustEeprom(void);
EEROMMDL_EXT Uint16 VerifyModuleEeprom(void);
EEROMMDL_EXT void ClearUnitParamToDefault(void);
EEROMMDL_EXT void ClearUnitAdjustParamToDefault(void);
EEROMMDL_EXT void ClearModuleParamToDefault(void);
EEROMMDL_EXT void UserWriteEeprom(Uint16 MdbAddr,Uint16* pReg,Uint16 len);
EEROMMDL_EXT void UserClearModuleParamToDefault(void);
EEROMMDL_EXT void UserClearUnitParamToDefault(void);
EEROMMDL_EXT void UserClearUnitAdjustParamToDefault(void);



#endif


