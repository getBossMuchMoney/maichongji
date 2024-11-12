#ifndef _SCIMODBUSREG_H_
#define _SCIMODBUSREG_H_


//===============================================================
//MODBUS reg define
//===============================================================


typedef struct CTRL_STRUCT
{
  uint16_t 	OnOff:1;       //onoff   1��on  0��off  //
  uint16_t 	LoopMode:1;   // loopmode   0��const currrent     1��const volt
  uint16_t	res1:1;       //
  uint16_t 	OutMode:1;    // 0��pulse    1��Dc   //
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
    Uint16 OverCurrent      :1;             // 0 ����
    Uint16 OverVolt      	:1;             // 1 ��ѹ
    Uint16 Over45C    		:1;             // 2 �����¶ȳ���45C
    Uint16 Over65C       	:1;             // 3 �����¶ȳ���65C
    Uint16 Over75C      	:1;             // 4 ���ʰ��¶ȳ���75C
    Uint16 PwrOff_OT      	:1;             // 5 �����¶ȹ��߹ػ�
    Uint16 Leakage       	:1;             // 6 ©ˮ���
    Uint16 XFMR_OT			:1;				// 7 ��ѹ������

    Uint16 ComErr_Lcd       :1;             // 8 ������ͨѶ�쳣
    Uint16 ComErr_Remote    :1;             // 9 Զ��ͨѶ�쳣
    Uint16 ComErr_Sync		:1;             // 10 ���ӻ�ͨѶ�쳣(ͬ��ͨѶ�쳣��
    Uint16 RecoilErr_A    	:1;             // 11 A�淴���쳣
    Uint16 RecoilErr_B      :1;             // 12 B�淴���쳣
    Uint16 OverRange_SetA	:1;				// 13 A������ֵ������Χ
    Uint16 OverRange_SetB	:1;				// 14 B������ֵ������Χ
    Uint16 EEPROMErr        :1;             // 15 EEPROM ��ȡ����

	Uint16 OVOC2DC_A		:1;				// 16 A���ѹ����תֱ��
	Uint16 OVOC2DC_B		:1;				// 17 B���ѹ����תֱ��
	Uint16 Pwr1_Fault		:1;				// 18 ǰ����Դ1��������
	Uint16 Pwr1_ComErr		:1;				// 19 ǰ����Դ1ͨѶ����
	Uint16 Pwr2_Fault		:1;				// 20 ǰ����Դ2��������
	Uint16 Pwr2_ComErr		:1;				// 21 ǰ����Դ2ͨѶ���� 
	Uint16 Reserved1		:10;            // ����
}tp_FaultBit;

typedef union
{
    uint16_t FaultReg[2];
    tp_FaultBit FaultBit;
}tp_Fault;

typedef struct 
{
  int16_t Current; 	    // ���� ��λ1A�����������������
  uint16_t PulseWide; 	// ������ ��λ0.1ms��������������Ŀ��
}SectorInfo;


typedef struct CTRL_STRUCT2
{
    uint16_t mod1LoadDef :1;    //ģ��1�ڲ��洢д��Ĭ��ֵ
    uint16_t mod2LoadDef :1;    //ģ��2�ڲ��洢д��Ĭ��ֵ
    uint16_t mod3LoadDef :1;    //ģ��3�ڲ��洢д��Ĭ��ֵ
    uint16_t mod4LoadDef :1;    //ģ��4�ڲ��洢д��Ĭ��ֵ
    uint16_t mod5LoadDef :1;    //ģ��5�ڲ��洢д��Ĭ��ֵ
    uint16_t mod6LoadDef :1;    //ģ��6�ڲ��洢д��Ĭ��ֵ
    uint16_t mod7LoadDef :1;    //ģ��7�ڲ��洢д��Ĭ��ֵ
    uint16_t mod8LoadDef :1;    //ģ��8�ڲ��洢д��Ĭ��ֵ
    uint16_t res :4;            //
    uint16_t workMode :1;       //0--����ģʽ   1--ǿ��С�����幤��������С����ʱ��Ч��
    uint16_t testMode :1;       //���Խ���   1---����ģʽ  0--����
    uint16_t loopMode :1;       //����ģʽ   0���ջ�     1������
    uint16_t resetErr :1;       //���ϸ�λ   ��1���ϸ�λ
} ctrlStruct2;

typedef union CTRL_NUM2
{
  uint16_t all;
  ctrlStruct2 bit;
}CtrlNum2;


typedef struct CTRLMODE_STRUCT
{
    uint16_t res1 :1;			//
    uint16_t synmode :1;		//0-��������ģʽ 1-ͬ������ģʽ
    uint16_t res2 :10;			//
    uint16_t powermode :1;		//ǰ����Դģʽ 0-�̶� 1-�Զ�
    uint16_t pwmorcmp :1;		//0-���ֿ��� 1-ģ�����
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
  int16_t PhaseShift[4];			//41-44 ����
  uint16_t SectNum;					//45 ��������
  uUnitCtlr UnitCtrl;				//46 �ӿ�����
  uint16_t resv1[34];
  uint16_t PhaseShiftAB;            // 81 AB������
  uint16_t PowerDebugVolt;          // 82 ǰ����Դ���Ե�ѹֵ
  CtrlNum2 Ctrl2;                   // 83 �����ź�2
  uint16_t LoadEepromDef;			// 84 �ָ�Ĭ�ϲ���
  uint16_t SyncPhase;               // 85 ͬ�����ƽǶ�

}sModuleCtrlregion;

typedef struct
{
	uint16_t res:15;			//
	uint16_t remoteCommErr:1;		//Զ��ͨѶ�쳣
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
	uint16_t BoardType;		//0��ģ����ư壬1����Ԫ���ư�
	uint16_t BoardAddr;
	uint16_t BoardRole;		//0��������	1���ӻ�
	uint16_t WorkMode;		//0:ͬ��ģʽ��1������ģʽ
	uint16_t UnitAddrMin;
	uint16_t UnitAddrMax;
	uint16_t MaxCurrent;
	uint16_t MinCurrent;
	uint16_t AddrMode;		//0:���뿪��ģʽ��1���ֶ�����ģʽ
	uint16_t SynDelay;
	uint16_t SynId;
}sModuleParamRegion;


typedef struct 
{
  uint16_t setValue; 	    // ����ֵ
  uint16_t actualValue; 		// ʵ��ֵ
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

	uint16_t PowerSetValue;	//ǰ����Դ��ѹ
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
	uint16_t HwLoadOVpos:1;			//���������ѹ˲ʱֵ��ѹ
	uint16_t HwLoadOVneg:1;			//���ظ����ѹ˲ʱֵ��ѹ
	uint16_t HwLoadOCpos:1;			//�����������˲ʱֵ��ѹ
	uint16_t HwLoadOCneg:1;			//���ظ������˲ʱֵ��ѹ
	uint16_t HwExtLoadOC:1;			//����Ӳ�����������
	uint16_t Hw24Vabnormal:1;		//24V��Դ��ѹ
	uint16_t HwLoadAvgOV:1;			//���ص�ѹƽ��ֵ��ѹ
	uint16_t HwLoadAvgOC:1;			//���ص���ƽ��ֵ����
	uint16_t BusOver:1;				//ĸ�߹�ѹ
	uint16_t BusUnder:1;			//ĸ��Ƿѹ
	uint16_t AmbientHighOT:1;		//�����¶ȸ߹���
	uint16_t SinkHighOT:1;			//ɢ�����¶ȸ߹���
	uint16_t PWMSyncLoss:1;			//PWMͬ���źŶ�ʧ
	uint16_t PWMLockLoss:1;			//PWMͬ���ź�ʧ��
	uint16_t LineSyncLoss:1;		//��Ƶͬ���źŶ�ʧ
	uint16_t LineLockLoss:1;		//��Ƶͬ���ź�ʧ��
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
	uint16_t communicationFault:1;	//ǰ����ԴͨѶ����
	uint16_t OC:1;					//����
	uint16_t Err:1;					//���
	uint16_t OV:1;					//��ѹ
	uint16_t PhaseDef:1;			//ȱ��
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
	uint16_t SinkOT:1;				//ɢ��������
	uint16_t AmbientOT:1;			//�����¶ȹ���
	uint16_t AmbientNTCerr:1;		//����NTC����
	uint16_t SinkNTCerr:1;			//ɢ����NTC����
	uint16_t SinkNTCOT:1;			//ɢ��������
	uint16_t PWMSyncLoss:1;			//PWMͬ���źŶ�ʧ
	uint16_t PWMLockLoss:1;			//PWMͬ���ź�ʧ��
	uint16_t LineSyncLoss:1;		//��Ƶͬ���źŶ�ʧ
	uint16_t LineLockLoss:1;		//��Ƶͬ���ź�ʧ��
	uint16_t BusOver:1;				//ĸ�߹�ѹ
	uint16_t BusUnder:1;			//ĸ��Ƿѹ
	uint16_t LoadPosAvgOV:1;		//���������ѹƽ��ֵ��ѹ
	uint16_t LoadNegAvgOV:1;		//���ظ����ѹƽ��ֵ��ѹ
	uint16_t LoadPosAvgOC:1;		//�����������ƽ��ֵ��ѹ
	uint16_t LoadNegAvgOC:1;		//���ظ������ƽ��ֵ��ѹ
	uint16_t OverLoad:1;			//����
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
	int16_t LoadPosAvgC;			//�����ֵ����
	int16_t LoadNegAvgC;			//�����ֵ����
	int16_t LoadPosAvgV;			//�����ֵ��ѹ
	int16_t LoadNegAvgV;			//�����ֵ��ѹ
	uint16_t ver[3];
	uint16_t PowerVolt;
	uint16_t PowerCurr;
	uint16_t BusVolt;
	uint16_t SinkTemp;			
	int16_t SectorAvgCurr[20];	
}sUnitStatusRegion;

typedef struct
{
  uint16_t 	OnOff:4;       //onoff   1��on  0��off  //
  uint16_t 	OutMode:1;    // 0��pulse    1��Dc   //
  uint16_t  res2:11;
}SynRemoteCtrlStruct;

typedef union
{
  uint16_t all;
  SynRemoteCtrlStruct bit;
}sSynRemoteCtrlNum1;

typedef struct 
{
  int16_t Current[4]; 	    // ���� ��λ1A�����������������
  uint16_t PulseWide; 	// ������ ��λ0.1ms��������������Ŀ��
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
  uint16_t OnOff:1;       		//onoff   1��on  0��off  //
  uint16_t res1:2;       		//
  uint16_t OutMode:1;    		// 0��pulse    1��Dc   //
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
	uint16_t HwExtLoadOC:1; 		//����Ӳ�����������
	uint16_t OverLoad:1;			//����
	uint16_t SinkOT:1;				//ɢ��������
	uint16_t AmbientOT:1;			//�����¶ȹ���
	uint16_t PWMSyncLoss:1; 		//PWMͬ���źŶ�ʧ
	uint16_t BusOver:1; 			//ĸ�߹�ѹ
	uint16_t BusUnder:1;			//ĸ��Ƿѹ
	uint16_t LoadPosAvgOV:1;		//���������ѹƽ��ֵ��ѹ
	uint16_t LoadNegAvgOV:1;		//���ظ����ѹƽ��ֵ��ѹ
	uint16_t LoadPosAvgOC:1;		//�����������ƽ��ֵ����
	uint16_t LoadNegAvgOC:1;		//���ظ������ƽ��ֵ����
	uint16_t Hw24Vabnormal:1;		//24V��Դ��ѹ
	uint16_t communicationFault:1;	//ǰ����ԴͨѶ����

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
	int16_t LoadPosAvgC;			//�����ֵ����
	int16_t LoadNegAvgC;			//�����ֵ����
	int16_t LoadPosAvgV;			//�����ֵ��ѹ
	int16_t LoadNegAvgV;			//�����ֵ��ѹ
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


