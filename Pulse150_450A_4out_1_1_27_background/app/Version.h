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
#define PRJ_VER "Pulse150_450A_APP"
#define APP_VER "1.0.0"

#define APP_BVER	1		//大版本：用于产品重大功能的改变
#define APP_MVER	1		//中版本：用于产品模块功能的改变
#define APP_SVER	27		//小版本：用于功能优化调整的改变


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
