/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : Background.h
 *  PURPOSE  :
 *  AUTHOR   : wushiyuan
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2024-09-23       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _Background_H_
#define _Background_H_
#ifdef Background_GLOBALS
#define Background_EXT
#else
#define Background_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
* Variable declaration                              *
********************************************************************************/
Background_EXT uint16_t wBackgroundTimeBaseCnt;

/********************************************************************************
* function declaration                              *
********************************************************************************/
Background_EXT void BackgroundLoop(void);

#endif
