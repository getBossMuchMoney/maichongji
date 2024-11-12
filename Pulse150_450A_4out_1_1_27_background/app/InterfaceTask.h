/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : InterfaceTask.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _InterfaceTask_H_
#define _InterfaceTask_H_
#ifdef InterfaceTask_GLOBALS
#define InterfaceTask_EXT
#else
#define InterfaceTask_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/

typedef enum
{
	eDisplayMode_Normal,
	eDisplayMode_EXHIBITION,
}eDISPLAYMODE;


/********************************************************************************
* Variable declaration                              *
********************************************************************************/
InterfaceTask_EXT eDISPLAYMODE DisplayMode;



/********************************************************************************
* function declaration                              *
********************************************************************************/
InterfaceTask_EXT void InterfaceTaskInit(void);
InterfaceTask_EXT void InterfaceTask(void * pvParameters);
InterfaceTask_EXT void sSetPulseWideFromModule();
InterfaceTask_EXT void sSetPulseWideFromUnit();
InterfaceTask_EXT void sSetPulseWideFromRemote();
InterfaceTask_EXT void sModuleStartCtrl();
InterfaceTask_EXT void sUnitSingleStartCtrl();
InterfaceTask_EXT void sSynRemoteStatusUpdate(uint16_t unitnum);


#endif
