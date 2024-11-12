/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : shellprint.h
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023Äê12ÔÂ28ÈÕ       V1.0                          Created.
 *
 *============================================================================*/
#ifndef MODULE_SHELLMODULE_SHELLPRINT_H_
#define MODULE_SHELLMODULE_SHELLPRINT_H_

#ifdef shellprint_GLOBALS
#define shellprint_EXT
#else
#define shellprint_EXT extern
#endif

#ifdef SHELL_ENABLE

/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/




/********************************************************************************
* Variable declaration                              *
********************************************************************************/



/********************************************************************************
* function declaration                              *
********************************************************************************/
shellprint_EXT int getch();
shellprint_EXT void printEnable();
shellprint_EXT void printDisable();
shellprint_EXT void printch(const char ch);
shellprint_EXT int printchn(const char *buf, int len);
shellprint_EXT int prints(const char* buf);
shellprint_EXT void shellPrintf(const char *format,...);

#endif
#endif /* MODULE_SHELLMODULE_SHELLPRINT_H_ */
