/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : shell.h
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023年12月28日       V1.0                          Created.
 *
 *============================================================================*/
#ifndef MODULE_SHELLMODULE_SHELL_H_
#define MODULE_SHELLMODULE_SHELL_H_

#ifdef shell_GLOBALS
#define shell_EXT
#else
#define shell_EXT extern
#endif
/********************************************************************************
 *include header file                              *
 ********************************************************************************/
#include <shelllist.h>

/********************************************************************************
 *const define                               *
 ********************************************************************************/
/*SHELL命令函数类型*/
typedef int (*CmdFun_t)(int argc, char *argv[]);

/*SHELL命令类型*/
typedef struct CmdItem_t
{
    struct list_head list;
    const char *name; /*命令字符串*/
    CmdFun_t fun; /*命令函数*/
    const char *doc; /*帮助描述*/
} Cmd_t;

#define CMD_SUCCESS (0)
#define CMD_FAILURE (-1)
#define CMD_ERRARG  (-2)

enum
{
    eShell_RX_CTRL,
    eShell_RX,
    eShell_RX_WAIT,
    eShell_JUDGE,
    eShell_PRE_PROC,
    eShell_ECHO,
    eShell_CMD_PROC,
    eShell_TX_WAIT
};
/********************************************************************************
 * Variable declaration                              *
 ********************************************************************************/

/********************************************************************************
 * function declaration                              *
 ********************************************************************************/

shell_EXT int shellCmdRegister(const char *name, CmdFun_t fun, const char *doc);
shell_EXT int shellCmdUnregister(const char *name);
shell_EXT void shellCmdUnregisterAll(void);
shell_EXT int shellInit(void);
shell_EXT int shellExcu(void);
shell_EXT int shellArgGet(int argc, char *argv[], ...);
shell_EXT int _help(int argc, char *argv[]);
shell_EXT int shellOn(int argc, char *argv[]);

#endif /* MODULE_SHELLMODULE_SHELL_H_ */
