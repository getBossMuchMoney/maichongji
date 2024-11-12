/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : ParallelTask.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _PARALLELTASK_H_
#define _PARALLELTASK_H_
#ifdef ParallelTask_GLOBALS
#define ParallelTask_EXT
#else
#define ParallelTask_EXT extern
#endif

/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/
#define MasterExtSyncH     SetLineSyncHigh
#define MasterExtSyncL     SetLineSyncLow

#define   cPWMSynNormalCnt    10
#define   cPWMSynStep         4
#define   cPERIOD_Bias        20


#define  cExtSynclossCntSetting    20    //1s
#define  cExtSyncbackCntSetting    20    //1s
#define  cPWMSynclossCntSetting    240	 //1s
#define  cPWMSyncbackCntSetting    20	 //1s
#define  cPWMSyncLockCntSetting    100   //100 times

#define cSumOfExtSyncPoint500us     20
#define cSumOfExtSyncPoint1ms       40
#define cSumOfExtSyncPoint5ms       200
#define cSumOfExtSyncPoint10ms      400
#define cSumOfExtSyncPoint20ms      800







/********************************************************************************
* Variable declaration                              *
********************************************************************************/
typedef enum CB_TYPE//control board type
{
    InvalidCB = 0,
    UnitCB = 1,
    ModuleCB,
    CabinetCB
}CBtype;
ParallelTask_EXT CBtype BoardType;

typedef enum CB_ROLE//control board role,master,slave,invalidRole
{
    InvalidRole = 0,
    MasterCB = 1,
    SlaveCB,
    NewCB
}CBrole;
ParallelTask_EXT CBrole BoardRole;

typedef enum X_EXT_SYNC
{
    xExtSyncDis = 0,
	xExtSyncEn	
}xExtSync;
	
typedef enum x_PWM_SYNC
{
    xPWMSyncDis = 0,
	xPWMSyncEn	
}xPWMSync;

typedef enum x_MODE
{
   Single = 0,
   Parallel	
}xMode;

typedef enum WAVE_TYPE
{
   InvalidWave = 0,
   DC = 1,
   Pulse	
}WaveType;
ParallelTask_EXT  WaveType WaveForm;

ParallelTask_EXT xMode SystemMode;

ParallelTask_EXT Uint32 InitPulsePhase;
ParallelTask_EXT Uint32 InitTimeCount;



typedef enum
{
	eMODULE_IDLE,
	eMODULE_RUN,
	eMODULE_STOP,
}eModuleWorkStatus;

ParallelTask_EXT eModuleWorkStatus ModuleWorkStatus;
ParallelTask_EXT eModuleWorkStatus UnitWorkStatus;




typedef struct X_PARALLEL
{
	xExtSync ExtSync;
	xPWMSync PWMSync;
	xMode    Mode;	
	CBrole Role;
	int ExtSyncStartPoint;
	int ExtSyncSignal;
	int PWMSyncSignal;
	int PWMSyncLockLoss;
	int ExtSyncSignalLoss;
	int PWMSyncSignalLoss;
	int AllSyncSignalLoss;
	Uint32 ExtSynclossCnt;
	Uint32 ExtSyncbackCnt;
	Uint32 ExtSynclossCntSetting;
	Uint32 ExtSyncbackCntSetting;
	Uint32 PWMSynclossCnt;
	Uint32 PWMSyncbackCnt;
	Uint32 PWMSynclossCntSetting;
	Uint32 PWMSyncbackCntSetting;	
	int32 PhaseShiftTime;
	Uint32  PWMSyncLockCnt;
	Uint32 PWMSyncLockCntSetting;
	int32 MasterExtSyncPoint;
	int32 MasterExtSyncStartPoint;
	int32 MasterExtSyncDeltaPoint;
	int MasterExtSyncFlag;
	int32 SlaveExtSyncPoint;
	int32 SlaveExtSyncStartPoint;
	int32 SlaveExtSyncDeltaPoint;
	int SlaveExtSyncFlag;
	int32 SumOfExtSyncPoint;
	Uint32 pwmsyncRange;
	Uint32 pwmperiod;	
	Uint32 pwmsyncCTR;
	Uint32 pwmnewPRD;
	xPWMSync PWMSyncLock;
}stXparallel;
ParallelTask_EXT stXparallel  ModulePara,UnitPara;


ParallelTask_EXT int PWMorCMP;
ParallelTask_EXT Uint32 intPERIOD;
ParallelTask_EXT Uint32 wPWMSynCTR;
ParallelTask_EXT Uint32 wCapSynCTR;
ParallelTask_EXT Uint32 wPWMNewPRD;
ParallelTask_EXT Uint32 wPWMSynNormalCnt;
ParallelTask_EXT Uint32 wPWMSynNormal;
ParallelTask_EXT Uint32 PERIOD;
ParallelTask_EXT Uint32 PWMSynRange;
ParallelTask_EXT Uint16 wPWM_PERIOD;

ParallelTask_EXT Uint32 ModuleParaTestCnt;

//ParallelTask_EXT Uint16 SwitchDIPAddr123;
ParallelTask_EXT Uint16 SwitchDIPAddr1;
ParallelTask_EXT Uint16 SwitchDIPAddr2;
ParallelTask_EXT Uint16 SwitchDIPAddr3;
ParallelTask_EXT Uint16 SwitchDIPAddr4;
ParallelTask_EXT Uint16 SwitchDIPAddr;
ParallelTask_EXT Uint16 RemoteCommAddr;

/********************************************************************************
* function declaration                              *
********************************************************************************/
ParallelTask_EXT void ParallelTaskInit(void);
ParallelTask_EXT void ParallelTask(void * pvParameters);
ParallelTask_EXT CBrole GetModuleRole(void);
ParallelTask_EXT void ClrModuleExtSyncStartPoint(void);
ParallelTask_EXT void ExtSynSignalSetting(void);
ParallelTask_EXT void StepTimecountClr(void);
ParallelTask_EXT int GetModuleExtSyncStartPoint(void);
ParallelTask_EXT void SetExtSyncSignal(void);
ParallelTask_EXT void SetExtSyncStartPoint(void);
ParallelTask_EXT void SetPWMSyncSignal(void);
ParallelTask_EXT void SyncCheck(stXparallel* input);
ParallelTask_EXT CBtype GetBoardType(void);
ParallelTask_EXT void SetBoardType(CBtype type);
ParallelTask_EXT CBrole GetUnitRole(void);
ParallelTask_EXT xMode GetUnitMode(void);
ParallelTask_EXT xMode GetModuleMode(void);

ParallelTask_EXT int GetUnitExtSyncStartPoint(void);
ParallelTask_EXT void ClrUnitExtSyncStartPoint(void);
ParallelTask_EXT CBrole GetModuleRole(void);
ParallelTask_EXT WaveType GetWaveForm(void);
ParallelTask_EXT int GetUnitPWMSyncSignalLoss(void);
ParallelTask_EXT void UnitPWMSyncSignalLoss(void);
ParallelTask_EXT void UnitExtSyncSignalLoss(void);
ParallelTask_EXT int GetUnitExtSyncSignalLoss(void);
ParallelTask_EXT void AddressCheck(void);
ParallelTask_EXT void DataInput(void);
ParallelTask_EXT xMode GetSystemMode(void);
ParallelTask_EXT void SetUnitWorkStatus(eModuleWorkStatus status);
ParallelTask_EXT eModuleWorkStatus GetUnitWorkStatus();
ParallelTask_EXT void SetModuleWorkStatus(eModuleWorkStatus status);
ParallelTask_EXT eModuleWorkStatus GetModuleWorkStatus();
ParallelTask_EXT void MasterExtSynCtrl();


#endif /* _PARALLELTASK_H_ */
