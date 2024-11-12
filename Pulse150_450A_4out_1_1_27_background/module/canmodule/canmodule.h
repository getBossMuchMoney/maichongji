/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : canmodule.h
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023��12��28��       V1.0                          Created.
 *
 *============================================================================*/
#ifndef MODULE_CANMODULE_CANMODULE_H_
#define MODULE_CANMODULE_CANMODULE_H_

#ifdef canmodule_GLOBALS
#define canmodule_EXT
#else
#define canmodule_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "can.h"
#include "squeue.h"
/********************************************************************************
*const define                               *
********************************************************************************/
#define MAXRXMSGCNT 48 /*QUEUE�б���Ľ���֡������*/



/********************************************************************************
* Variable declaration                              *
********************************************************************************/
canmodule_EXT unsigned char ucCanRXMsgBuffer[MAXRXMSGCNT][8];
canmodule_EXT QUEUE queCanRxMsg;


/********************************************************************************
* function declaration                              *
********************************************************************************/
canmodule_EXT void CanDataInit();
canmodule_EXT void CanTxRx(void);


#endif /* MODULE_CANMODULE_CANMODULE_H_ */
