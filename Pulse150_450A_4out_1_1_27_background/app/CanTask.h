/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : CanTask.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _CanTask_H_
#define _CanTask_H_
#ifdef CanTask_GLOBALS
#define CanTask_EXT
#else
#define CanTask_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/

typedef enum
{
	ePowerOff = 0,
	ePowerOn = 1
}ePOWERSTATUS;



/********************************************************************************
* Variable declaration                              *
********************************************************************************/
typedef struct
{
	Uint16 communicationFault;		//通讯故障
	Uint16 isPowerOn;				// 1 开机，0关机
	Uint16 CCOrCV;					//稳压还是稳流
	Uint16 autoOrManual;			 //手动还是自动
	Uint16 localMachineFault;		//本机故障，BCD, 00正常,01过流、02误差、03过热、04同步丢失
	Uint16 actualCurrent;			//实际电流, 0.1A
	Uint16 actualVoltage;			//实际电压,10mV

} sACDC_WdResp;
#ifdef CanTask_GLOBALS
CanTask_EXT sACDC_WdResp acdcWdResp = {
					.communicationFault = 0,
					};
#else
CanTask_EXT sACDC_WdResp acdcWdResp;
#endif

CanTask_EXT uint16_t PowerSetValue;

/********************************************************************************
* function declaration                              *
********************************************************************************/
CanTask_EXT	int16_t PowerOnFinished();

CanTask_EXT void CanTaskInit(void);
CanTask_EXT void CanTask(void * pvParameters);

CanTask_EXT uint16_t writeTxbuf_WD(uint16_t power,uint16_t vol,uint16_t value);


#endif
