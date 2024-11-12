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
#define UART_REMAP_ENABLE   0   //stdout/stdin��ӳ�䵽����
#if UART_REMAP_ENABLE
#define REMAP_UART_BASE   SCIA_BASE
#endif

#define STATUS_485_RECEIVED     0
#define STATUS_485_SEND         1

#define ACDC_BOARD_TEST 1   //ǰ�ڵ��Բ���5Kw��Դ���˺꣬��ʽʹ�ùر�

//#define SHELL_ENABLE	//shell���ܿ���


#define SLAVEMAX           4
#define SLAVESTART         8    //��վ��ʼ���

#define MSG_DATA_LENGTH    8
#define TX_MSG_OBJ_ID      1
#define RX_MSG_OBJ_ID      2

#define  UartTimerPrdValue 0xFFFF

#define dif8(cur,pre)	(((cur)>(pre))?((cur)-(pre)):(0xff-(pre)+(cur)+1))
#define dif16(cur,pre)	(((cur)>(pre))?((cur)-(pre)):(0xffff-(pre)+(cur)+1))
#define dif32(cur,pre)	(((cur)>(pre))?((cur)-(pre)):(0xffffffff-(pre)+(cur)+1))
/********************************************************************************
* Variable declaration                              *
********************************************************************************/
