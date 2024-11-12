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
Ҫ����������ֻ���ں����������
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
	Uint16 PwmCmpCtrl;					//����ģ�����
	Uint16 MixModeEnable;				//���ģʽʹ��
  	Uint16 SwFreq;						//����Ƶ��

	Uint16 BoardType;		//0��ģ����ư壬1����Ԫ���ư�
	Uint16 BoardAddr;
	Uint16 WorkMode;		//0:ͬ��ģʽ��1������ģʽ
	Uint16 DisplayMode;		//0:����ģʽ��1��չ��ģʽ

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
	Uint16 AddrMode;					//0:���뿪��ģʽ��1���ֶ�����ģʽ
	Uint16 HwOCCountSet;                //Ӳ����������Ƶ������
	Uint16 HwOCRecoveryTimeSet;         //Ӳ�������ָ�ʱ������
	Uint16 MixModeRatio;				//���ģʽ�ֽ����
	Uint16 PowerSetValue;				//ǰ����Դ��ѹ
    Uint16 CMPmodeSlopePos;             //ģ��Ƚ�ģʽ����б�� xxA/us ����ֵ����100��
    Uint16 CMPmodeSlopeNeg;             //ģ��Ƚ�ģʽ�½�б�� xxA/us ����ֵ����100��
    Uint16 CMPmodeSlopeEnable;          //ģ��Ƚ�ģʽб�ʹ���ʹ��/��ֹ
	Uint16 SynDelay;
	
	Uint16 AmbientTemplevelback;        //�����¶Ȼָ���ֵ
	Uint16 SinkTemplevelback;           //ɢ�����¶Ȼָ���ֵ

    Uint16 WarnBusOverCheckEnable;      //BUS��ѹ��ѹ�澯ʹ��
    Uint16 WarnBusOverThreshold;        //BUS��ѹ��ѹ�澯��ֵ
    Uint16 WarnBusOverBackThreshold;    //BUS��ѹ��ѹ�澯�ָ�ֵ
    Uint16 WarnBusUnderCheckEnable;     //BUS��ѹǷѹ�澯ʹ��
    Uint16 WarnBusUnderThreshold;       //BUS��ѹǷѹ�澯��ֵ
    Uint16 WarnBusUnderBackThreshold;   //BUS��ѹǷѹ�澯�ָ�ֵ

    Uint16 HwOCCheckEnable;             //Ӳ����������ʹ�ܽ�ֹ����
    Uint16 HwOCFaultTime;               //Ӳ���������ϻָ�ʱ��:��
    Uint16 HwOCFaultRecoverySet;        //Ӳ���������ϻָ�����

    Uint16 PWMSyncLockEnable;           //PWMͬ������ʧʹ��
	
}sUnitEEPROM;



EEROMMDL_EXT sUnitEEPROM sUintEepromPara;
#ifdef  EEPROMMODULE_GLOBALS
EEROMMDL_EXT sUnitEEPROM sUintEepromParaDef=UNIT_PARAM_DEFAULT_TAB;
#else
EEROMMDL_EXT sUnitEEPROM sUintEepromParaDef;
#endif

/********************************************************************************
Ҫ����������ֻ���ں����������
**********************************************************************************/
typedef struct
{
	Uint16 Magic;			//0xEEFF
	Uint16 Len;		
	sUnitADJUSTFACTOR AdjustFactor;
}sUnitEEPROMAdjust;

EEROMMDL_EXT sUnitEEPROMAdjust sUintEepromAdjustPara;

/********************************************************************************
Ҫ����������ֻ���ں����������
**********************************************************************************/
typedef struct
{
	Uint16 Magic;			//0xCCDD
	Uint16 Len;	
	Uint16 BoardType;		//0��ģ����ư壬1����Ԫ���ư�
	Uint16 BoardAddr;
	Uint16 BoardRole;		//0��������	1���ӻ�
	Uint16 WorkMode;		//0:ͬ��ģʽ��1������ģʽ
	Uint16 UnitAddrMin;
	Uint16 UnitAddrMax;
	Uint16 MaxCurrent;      //�������
	Uint16 MinCurrent;      //�������
	Uint16 AddrMode;		//0:���뿪��ģʽ��1���ֶ�����ģʽ
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


