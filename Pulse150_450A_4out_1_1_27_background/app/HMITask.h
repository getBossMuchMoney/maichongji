/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : HMITask.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _HMITask_H_
#define _HMITask_H_
#ifdef HMITask_GLOBALS
#define HMITask_EXT
#else
#define HMITask_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "scimodbusreg.h"

/********************************************************************************
*const define                               *
********************************************************************************/
typedef enum
{
	eHMIMODULECTRL,
	eHMIMODULESTATUS,
	eHMIMODULEREMOTE,
	eHMIMODULEPARA,

	eHMIUNIT1CTRL,
	eHMIUNIT1STATUS,
	eHMIUNIT1REMOTE,
	eHMIUNIT1PARA,

	eHMIUNIT2CTRL,
	eHMIUNIT2STATUS,
	eHMIUNIT2REMOTE,
	eHMIUNIT2PARA,

	eHMIUNIT3CTRL,
	eHMIUNIT3STATUS,
	eHMIUNIT3REMOTE,
	eHMIUNIT3PARA,

	eHMIUNIT4CTRL,
	eHMIUNIT4STATUS,
	eHMIUNIT4REMOTE,
	eHMIUNIT4PARA,
}eHMIRegion;

#define HMI_REGION_LEN	0x100

#define HMI_REG_BASE	0
#define HMI_REG_MODULE_BASE		0
#define HMI_REG_MODULE_CTRL		HMI_REG_MODULE_BASE
#define HMI_REG_MODULE_STATUS	(HMI_REG_MODULE_CTRL+HMI_REGION_LEN)
#define HMI_REG_MODULE_REMOTE	(HMI_REG_MODULE_STATUS+HMI_REGION_LEN)
#define HMI_REG_MODULE_PARA		(HMI_REG_MODULE_REMOTE+HMI_REGION_LEN)

#define HMI_REG_UNIT1_CTRL		(HMI_REG_MODULE_PARA+HMI_REGION_LEN)	//0X400
#define HMI_REG_UNIT1_STATUS	(HMI_REG_UNIT1_CTRL+HMI_REGION_LEN)
#define HMI_REG_UNIT1_REMOTE	(HMI_REG_UNIT1_STATUS+HMI_REGION_LEN)
#define HMI_REG_UNIT1_PARA		(HMI_REG_UNIT1_REMOTE+HMI_REGION_LEN)

#define HMI_REG_UNIT2_CTRL		(HMI_REG_UNIT1_PARA+HMI_REGION_LEN)		//0X800
#define HMI_REG_UNIT2_STATUS	(HMI_REG_UNIT2_CTRL+HMI_REGION_LEN)
#define HMI_REG_UNIT2_REMOTE	(HMI_REG_UNIT2_STATUS+HMI_REGION_LEN)
#define HMI_REG_UNIT2_PARA		(HMI_REG_UNIT2_REMOTE+HMI_REGION_LEN)

#define HMI_REG_UNIT3_CTRL		(HMI_REG_UNIT2_PARA+HMI_REGION_LEN)	//0XC00
#define HMI_REG_UNIT3_STATUS	(HMI_REG_UNIT3_CTRL+HMI_REGION_LEN)
#define HMI_REG_UNIT3_REMOTE	(HMI_REG_UNIT3_STATUS+HMI_REGION_LEN)
#define HMI_REG_UNIT3_PARA		(HMI_REG_UNIT3_REMOTE+HMI_REGION_LEN)

#define HMI_REG_UNIT4_CTRL		(HMI_REG_UNIT3_PARA+HMI_REGION_LEN)	//0X1000
#define HMI_REG_UNIT4_STATUS	(HMI_REG_UNIT4_CTRL+HMI_REGION_LEN)
#define HMI_REG_UNIT4_REMOTE	(HMI_REG_UNIT4_STATUS+HMI_REGION_LEN)
#define HMI_REG_UNIT4_PARA		(HMI_REG_UNIT4_REMOTE+HMI_REGION_LEN)

/********************************************************************************
* Variable declaration                              *
********************************************************************************/

HMITask_EXT sModuleCtrlregion  Ctrldata1;


HMITask_EXT uint16_t MachineWorkMode;
HMITask_EXT uint16_t SyncPhase;


HMITask_EXT sModuleStatusRegion sModuleStatusReg;
HMITask_EXT sModuleParamRegion sModuleParamReg;


HMITask_EXT sUnitCtrlRegion sUnitCtrlReg[4];
HMITask_EXT sSynRemoteCtrlRegion sSynRemoteCtrlReg;
HMITask_EXT sSynRemoteUintCtrlRegion sSynRemoteUnitCtrlReg[4];
HMITask_EXT sUnitStatusRegion sUnitStatusReg[4];
HMITask_EXT sSynRemoteStatusRegion sSynRemoteStatusReg[4];

//HMITask_EXT sUnitParamRegion sUnitParamReg[4];


/********************************************************************************
* function declaration                              *
********************************************************************************/
HMITask_EXT void HMITaskInit(void);
HMITask_EXT void HMITask(void * pvParameters);

//因模式改变而清空相关设置参数
HMITask_EXT void CfgParaClr();



#endif
