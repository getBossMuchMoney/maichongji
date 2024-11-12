/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : Constant.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/


/********************************************************************************
*include header file                              *
********************************************************************************/


/********************************************************************************
*const define                               *
********************************************************************************/
#define UART_REMAP_ENABLE   0   //stdout/stdin÷ÿ”≥…‰µΩ¥Æø⁄
#if UART_REMAP_ENABLE
#define REMAP_UART_BASE   SCIA_BASE
#endif

#define STATUS_485_RECEIVED     0
#define STATUS_485_SEND         1

#define SHELL_UART_BASE     SCIA_BASE


#define MSG_DATA_LENGTH    8
#define TX_MSG_OBJ_ID      1
#define RX_MSG_OBJ_ID      2

#define  UartTimerPrdValue 0xFFFF

#define APP_Addr            0x88000
#define APP_LEN             0x17F00

#define BOOT_WAIT_TIME  5
/********************************************************************************
* Variable declaration                              *
********************************************************************************/
