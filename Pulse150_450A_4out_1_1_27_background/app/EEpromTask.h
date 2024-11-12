/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : EEpromTask.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _EEpromTask_H_
#define _EEpromTask_H_
#ifdef EEpromTask_GLOBALS
#define EEpromTask_EXT
#else
#define EEpromTask_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/
#define	EepromCmdSize		6
#define	EepromCmdMaxNum		10
#define	EepromCmdBufferSize	(EepromCmdMaxNum*EepromCmdSize)




/********************************************************************************
* Variable declaration                              *
********************************************************************************/
EEpromTask_EXT Uint16 EepromCmdBuffer[EepromCmdBufferSize]; //6*10



/********************************************************************************
* function declaration                              *
********************************************************************************/
EEpromTask_EXT void EEpromTaskInit(void);
EEpromTask_EXT void EEpromTask(void * pvParameters);



#endif
