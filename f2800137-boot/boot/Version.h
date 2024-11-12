/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : Version.h
 *  PURPOSE  :
 *  AUTHOR   : liusongjie
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _Version_H_
#define _Version_H_
#ifdef Version_GLOBALS
#define Version_EXT
#else
#define Version_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/
#define PRJ_VER "Pulse150_450A_BOOT"
#define BOOT_VER "1.0.0"



/********************************************************************************
* Variable declaration                              *
********************************************************************************/



/********************************************************************************
* function declaration                              *
********************************************************************************/
Version_EXT char* buildTime();
Version_EXT void versionReg();
Version_EXT void versionPrint();

#endif
