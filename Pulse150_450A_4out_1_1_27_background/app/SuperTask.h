/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : SuperTask.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _SuperTask_H_
#define _SuperTask_H_
#ifdef SuperTask_GLOBALS
#define SuperTask_EXT
#else
#define SuperTask_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

#include "Interrupt.h"

/********************************************************************************
*const define                               *
********************************************************************************/
#define cSuperTimer1s                 100

/*******************************************************************
//pulse power mode
*******************************************************************/
#define cPowerOnMode                 1
#define cSelfCheckMode               2
#define cStandbyMode                 3
#define cPulseMode                   4
#define cDCMode                      5
#define cOpenLoopMode                6
#define cFaultMode                   7
#define cShutDownMode                8




/********************************************************************************
* Variable declaration                              *
********************************************************************************/
SuperTask_EXT uint16_t wPowerMode;
SuperTask_EXT uint16_t wPrePowerMode;



/********************************************************************************
* function declaration                              *
********************************************************************************/
SuperTask_EXT void SuperTaskInit(void);
SuperTask_EXT void SuperTask(void * pvParameters);
SuperTask_EXT void sPowerOnMode(void);
SuperTask_EXT void sSelfCheckMode(void);
SuperTask_EXT void sStandbyMode(void);
SuperTask_EXT void sPulseMode(void);
SuperTask_EXT void sDcMode(void);
SuperTask_EXT void sOpenLoopMode(void);
SuperTask_EXT void sFaultMode(void);
SuperTask_EXT void sShutDownMode(void);
SuperTask_EXT void redLedToggle(void);
SuperTask_EXT void SetPowerMode(uint16_t mode);
SuperTask_EXT void SetPrePowerMode(uint16_t mode);
SuperTask_EXT uint16_t GetPrePowerMode();
SuperTask_EXT uint16_t GetPowerMode();
SuperTask_EXT void SetPulsePhase(PulsePowerPulsePhase Phase);


#endif
