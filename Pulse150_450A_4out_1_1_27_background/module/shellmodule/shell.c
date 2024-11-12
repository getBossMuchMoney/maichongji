/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : shell.c
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023年12月28日       V1.0                          Created.
 *
 *============================================================================*/
#define shell_GLOBALS     1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "device.h"
#include "shellprint.h"
#include "Constant.h"
#include "Common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Initial.h"

#ifdef SHELL_ENABLE
/********************************************************************************
*const define                               *
********************************************************************************/
#define SHELL_LINE_MAX_LEN 64
#define SHELL_PARAM_MAX_NUM 10
#define SHELL_MAX_PRINT 64

#define setStatus485 SetStatus485A

//#define getTimerTickCur xTaskGetTickCount
#define getTimerTickCur BackgroundGetTickCount
/********************************************************************************
* Variable declaration                              *
********************************************************************************/
char shellLine[SHELL_LINE_MAX_LEN];
char *shellParam[SHELL_PARAM_MAX_NUM];

uint32_t shellOnFlag = 0;

struct list_head __cmd_list;

/********************************************************************************
* function declaration                              *
********************************************************************************/

static Uint16 shellGets(Uint16 *buf,Uint32 len)
{
    /*需要自己提供获取字符串的函数 */
    Uint16 i;
    int c;

    for (i = 0; i < len; i++)
    {
        c = getch();
        if(c == -1)
            break;

        buf[i] = c;
    }
    return i;
}

static Uint16 shellPutn(char*buf,Uint32 len)
{
    return printchn(buf,len);
}

static Uint16 shellPuts(char* buf)
{
    return prints(buf);
}

static Uint16 shellParamSem(char* line, char *paramArry[], Uint16 arryLen)
{
    Uint16 i=0,ret=0;
    char *ptr = NULL;
    ptr = strtok(line," ");
    if(ptr==NULL)
    {
        return 0;
    }

    for(i=0; ((ptr!=NULL) && (i<arryLen)); i++)
    {
        paramArry[i] = ptr;
        ptr = strtok(NULL,",");
    }
    ret = i;
    return ret;
}

static inline Cmd_t * shellFindCmd(const char * name)
{
    struct list_head *pos =NULL, *n = NULL;
    Cmd_t * cmd=NULL;

    list_for_each_safe(pos,n, &__cmd_list)
    {
        cmd = list_entry(pos, Cmd_t, list);

        if(cmd == NULL)
        {
            return NULL;
        }
        if(0==strcmp(name,cmd->name))
        {
            return cmd;
        }
    }
    return NULL;
}

int shellCmdRegister(const char * name, CmdFun_t fun, const char * doc)
{
    Cmd_t * cmd = NULL;

    cmd = malloc(sizeof(Cmd_t));
    if(cmd == NULL)
    {
        return -1;
    }
    else
    {
        memset(cmd,0, sizeof(Cmd_t));
        if(cmd == NULL)
        {
            return -1;
        }
        cmd->name = name;
        cmd->fun = fun;
        cmd->doc = doc;
        list_add_tail(&cmd->list, &__cmd_list);
        return 0;
    }
}

int shellCmdUnregister(const char * name)
{
    struct list_head *pos=NULL, *n=NULL;
    Cmd_t * cmd=NULL;
    list_for_each_safe(pos, n, &__cmd_list)
    {
        cmd = list_entry(pos, Cmd_t, list);
        if(cmd != NULL)
        {
            if(0==strcmp(name, cmd->name))
            {
                list_del(&cmd->list);
                free(cmd);
                return 0;
            }
        }
    }
    return -1;
}

void shellCmdUnregisterAll(void)
{
    struct list_head *pos=NULL, *n=NULL;
    Cmd_t * cmd=NULL;

    list_for_each_safe(pos, n, &__cmd_list)
    {
        cmd = list_entry(pos, Cmd_t, list);
        if(cmd != NULL)
        {
            list_del(&cmd->list);
            free(cmd);
        }
    }
}

int shellInit(void)
{
    /*初始化命令链表*/
    if(&__cmd_list == NULL)
        return -1;

    INIT_LIST_HEAD(&__cmd_list);
    shellCmdRegister("?",_help, "[none]: Display all supported commands.");
    shellCmdRegister("shellon",shellOn, "[485addr]: shell on/off.");

    return 0;
}
int shellCmdPreProcess(char* line)
{
    char cmd[20]="shellon ";
    char tem[20]={0};
    int addr = Get485Addr();   //需改为485地址判断 todo

    if(0!=strncmp(line,cmd,strlen(cmd)))
        return -1;

    memcpy(tem,line,19);    //只检查前19字节
    Uint16 paramNum = shellParamSem(tem, shellParam, SHELL_PARAM_MAX_NUM);
    if(paramNum == 2)
    {
        if(atol(shellParam[1]) != addr)
        {
            shellOnFlag = 0;
            printDisable();
            return -2;
        }
        else                            //shellon addr
        {
            shellOnFlag = 1;
            printEnable();
            return 0;
        }
    }
    return -3;
}
int shellExcu(void)
{
    static int step=0;
    static Uint16 count=0;
    static Uint32 curtime = 0;

    Uint16 len = 0;
    Uint16 paramNum = 0;
    int r=0;

    switch(step)
    {
    case eShell_RX_CTRL: //rx ctrl
        setStatus485(STATUS_485_RECEIVED);
        count = 0;
        step = eShell_RX;
        break;
    case eShell_RX: //rx
        len = shellGets((Uint16*)(shellLine+count),SHELL_LINE_MAX_LEN-count);
        if(len == 0)
        {
            step = eShell_RX_WAIT;
            curtime = getTimerTickCur();
        }
        else
        {
            count += len;
        }
        break;
    case eShell_RX_WAIT: //wait rx finish
        len = shellGets((Uint16*)(shellLine+count),SHELL_LINE_MAX_LEN-count);
        if(len != 0)
        {
            count += len;
            step = eShell_RX;
        }
        else if(getTimerTickCur() - curtime > 20)
        {
            step = eShell_JUDGE;
        }
        break;
    case eShell_JUDGE: //judge
        if((shellLine[count-2] == '\r') && (shellLine[count-1] == '\n'))
        {
            step = eShell_PRE_PROC;
        }
        else
        {
            count = 0;
            step = eShell_RX_CTRL;
            memset(shellLine,0,SHELL_LINE_MAX_LEN);
        }
        break;
    case eShell_PRE_PROC: //pre process

        shellCmdPreProcess(shellLine);

        if(!shellOnFlag)
            step = eShell_RX_CTRL;
        else
            step = eShell_ECHO;
        break;
    case eShell_ECHO: //echo
        setStatus485(STATUS_485_SEND);
        DEVICE_DELAY_US(30);
        shellPutn(shellLine,count);

        shellLine[count - 2] = '\0';
        shellLine[count - 1] = '\0';
        count = 0;
        step = eShell_CMD_PROC;
        break;
    case eShell_CMD_PROC: //cmd process
        paramNum = shellParamSem(shellLine, shellParam, SHELL_PARAM_MAX_NUM);
        if(paramNum)
        {

            Cmd_t * cmd = shellFindCmd(shellParam[0]);
            if(!cmd)
            {
                shellPuts("Unrecognized command, try \"?\".\r\n\r\n");
                shellPrintf("$LY_%d:",Get485Addr());
                step = eShell_TX_WAIT;
                break;
            }

            r = cmd->fun(paramNum-1,&shellParam[1]);/*运行命令函数*/
            if(r==CMD_SUCCESS)
            {
                shellPuts("\r\nCommand execution SUCCEED.\r\n\r\n");
            }
            else if(r==CMD_FAILURE)
            {
                shellPuts("\r\nCommand execution FAIL.\r\n\r\n");
            }
            else if(r==CMD_ERRARG)
            {
                shellPuts("\r\nCommand execution err: illegal argument.\r\n\r\n");
            }
            else
            {
                shellPuts("\r\nCommand execution err: unknown err.\r\n\r\n");
            }
        }
        shellPrintf("$LY_%d:",Get485Addr());
        step = eShell_TX_WAIT;
        break;
    case eShell_TX_WAIT: //wait send finish
        DEVICE_DELAY_US(30);
        step = eShell_RX_CTRL;
        break;
    default:
        step = eShell_RX_CTRL;
        count = 0;
        break;
    }
    return 0;
}

int shellArgGet(int argc,char*argv[],...)
{
    unsigned long long val = 0;
    int i;
    int num = 0;
    va_list ap;
    va_start(ap,argv);

    for(num = 0; num < argc; num++)
    {
        unsigned long addr = va_arg(ap,unsigned long);

        char* pstr = (char*)addr;

        if((argv[num][0] == '\"')&&(argv[num][strlen(argv[num])-1] == '\"'))
        {
            for(i=0;i<strlen(argv[num])-2;i++)
                *(pstr+i) = argv[num][i+1];
        }
        else if((argv[num][0]=='0')&&((argv[num][1]=='x')||(argv[num][1]=='X')))
        {
            val = 0;
            for(i=2;argv[num][i];i++)
            {
                if((argv[num][i] >= '0') && (argv[num][i] <= '9'))
                {
                    val = (val<<4) + argv[num][i] - '0';
                }
                else if((argv[num][i] >= 'A') && (argv[num][i] <= 'F'))
                {
                    val = (val<<4) + argv[num][i] - 'A' + 10;
                }
                else if((argv[num][i] >= 'a') && (argv[num][i] <= 'f'))
                {
                    val = (val<<4) + argv[num][i] - 'a' + 10;
                }
                else
                    break;
            }
            *(unsigned long*)pstr = val;
        }
        else if(strstr(argv[num],"."))
        {
            *(float*)pstr = atof(argv[num]);
        }
        else
        {
            *(unsigned long*)pstr = atol(argv[num]);
        }
    }

    va_end(ap);

    return num;
}

int shellOn(int argc, char*argv[])
{
    long addr;
    uint16_t local = 0;

    shellArgGet(argc, argv, &addr);

    local = Get485Addr();
    if (addr == local)
    {
        shellOnFlag = 1;
        printEnable();

        shellPrintf("shell on\n");
    }
    else
    {
        shellOnFlag = 0;
        printDisable();
    }

    return CMD_SUCCESS;
}

int _help(int argc, char*argv[])
{
    struct list_head *pos=NULL, *n=NULL;
    Cmd_t * cmd=NULL;
    list_for_each_safe(pos, n, &__cmd_list)
    {
        cmd = list_entry(pos, Cmd_t, list);
        if(cmd == NULL)
            return 0;
        shellPrintf("%-32s - %s\r\n", cmd->name, cmd->doc);
    }

    return CMD_SUCCESS;
}

#endif
