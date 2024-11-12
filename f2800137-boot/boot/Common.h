/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : Common.h
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023Äê12ÔÂ28ÈÕ       V1.0                          Created.
 *
 *============================================================================*/
#ifndef APP_COMMON_H_
#define APP_COMMON_H_

#ifdef Common_GLOBALS
#define Common_EXT
#else
#define Common_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/




/********************************************************************************
* Variable declaration                              *
********************************************************************************/
Common_EXT uint16_t Rs485Addr;

Common_EXT uint16_t Rs485Flag[3];

/********************************************************************************
* function declaration                              *
********************************************************************************/
Common_EXT void AddressCheck(void);

Common_EXT uint16_t Get485Addr(void);

Common_EXT void SetStatus485A(int status);

Common_EXT void SetStatus485B(int status);

Common_EXT void SetStatus485C(int status);

#endif /* APP_COMMON_H_ */
