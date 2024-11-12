/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : Common.c
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023年12月28日       V1.0                          Created.
 *
 *============================================================================*/
#define Common_GLOBALS     1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include <stdint.h>
#include "Constant.h"
#include "Common.h"
#include "f28x_project.h"
#include "ParallelTask.h"


/********************************************************************************
*const define                               *
********************************************************************************/

/********************************************************************************
* Variable declaration                              *
********************************************************************************/

/********************************************************************************
* function declaration                              *
********************************************************************************/

/*********************************************************************
Function name:  Get485Addr
Description:
Calls:
Called By:
Parameters:     void
Return:         uint16_t
*********************************************************************/
uint16_t Get485Addr(void)
{
    return SwitchDIPAddr;
}

/*********************************************************************
Function name:  SetStatus485A
Description:
Calls:
Called By:
Parameters:     int
Return:         void
*********************************************************************/
void SetStatus485A(int status)
{
    if (status == STATUS_485_SEND)
    {
        GpioDataRegs.GPHSET.bit.GPIO242 = 1;
    }
    else
    {
        GpioDataRegs.GPHCLEAR.bit.GPIO242 = 1;
    }
}

/*********************************************************************
Function name:  SetStatus485B
Description:
Calls:
Called By:
Parameters:     int
Return:         void
*********************************************************************/
void SetStatus485B(int status)
{
    if (status == STATUS_485_SEND)
    {
        GpioDataRegs.GPASET.bit.GPIO20 = 1;
    }
    else
    {
        GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;
    }
}

/*********************************************************************
Function name:  SetStatus485B
Description:
Calls:
Called By:
Parameters:     int
Return:         void
*********************************************************************/
void SetStatus485C(int status)
{
    if (status == STATUS_485_SEND)
    {
        GpioDataRegs.GPASET.bit.GPIO16 = 1;
    }
    else
    {
        GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;
    }
}
/*********************************************************************
Function name:  waveFifoInit
Description:
Calls:
Called By:
Parameters:     eWaveType,float,uint16_t,uint16_t
Return:         void
*********************************************************************/
void waveFifoInit(eWaveType wavetype, float *wavebuf, uint16_t buflen, uint16_t nextlen)
{
	if(wavetype >= MAX_WAVE_FIFO)
		return;
	if(nextlen > buflen - 1)
		return;
	sWaveFifo[wavetype].buf = wavebuf;
	sWaveFifo[wavetype].head = 0;
	sWaveFifo[wavetype].tail = 0;
	sWaveFifo[wavetype].used = 0;
	sWaveFifo[wavetype].trigpos = -1;
	sWaveFifo[wavetype].buflen = buflen;
	sWaveFifo[wavetype].nextlen = nextlen;
}
/*********************************************************************
Function name:  waveRecord
Description:用于波形记录，未触发时数据循环覆盖，触发后按初始化设置保留指定长度后
			将数据按顺序整理并停止更新
Calls:
Called By:
Parameters:     eWaveType,uint16_t,float
Return:         void
*********************************************************************/
void waveRecord(eWaveType wavetype, uint16_t trig, float data)
{
	struWaveFifo *psWaveFifo;
	uint16_t i,j;

	if(wavetype >= MAX_WAVE_FIFO)
		return;
	
	psWaveFifo = &sWaveFifo[wavetype];
	
	if(psWaveFifo->buf == NULL)
		return;

	
	if(psWaveFifo->trigpos >= 0)
	{
		if((psWaveFifo->head == psWaveFifo->tail)&&(psWaveFifo->used))	//存满
		{
			return;
		}
	}
	
	if((psWaveFifo->head == psWaveFifo->tail)&&(psWaveFifo->used))	//存满
	{
		psWaveFifo->tail ++;
		if(psWaveFifo->tail >= psWaveFifo->buflen)
			psWaveFifo->tail = 0;
	}
	
	psWaveFifo->buf[psWaveFifo->head++] = data;
	psWaveFifo->used = 1;
	
	if(psWaveFifo->head >= psWaveFifo->buflen)
		psWaveFifo->head = 0;
	

	if((trig)&&(psWaveFifo->trigpos<0))		//数据整理
	{
		if(psWaveFifo->head == 0)			//记录数据触发位置
			psWaveFifo->trigpos = psWaveFifo->buflen - 1;
		else
			psWaveFifo->trigpos = psWaveFifo->head - 1;	

		if(psWaveFifo->nextlen>(psWaveFifo->buflen-psWaveFifo->head))
		{
			for(i=psWaveFifo->nextlen-(psWaveFifo->buflen-psWaveFifo->head),j=0;j<psWaveFifo->buflen-psWaveFifo->nextlen;i++,j++)
			{
				psWaveFifo->buf[j] = psWaveFifo->buf[i];
			}
			psWaveFifo->tail = 0;
			psWaveFifo->head = psWaveFifo->buflen - psWaveFifo->nextlen;
			psWaveFifo->trigpos = psWaveFifo->head - 1;
		}
		else if((psWaveFifo->head == psWaveFifo->tail)&&(psWaveFifo->used))	//存满
		{
			for(i=psWaveFifo->head+psWaveFifo->nextlen,j=psWaveFifo->buflen-psWaveFifo->nextlen;j>0;i++,j--)
			{
				psWaveFifo->buf[j] = psWaveFifo->buf[i%psWaveFifo->buflen];
			}
			psWaveFifo->tail = 0;
			psWaveFifo->head = psWaveFifo->buflen - psWaveFifo->nextlen;
			psWaveFifo->trigpos = psWaveFifo->head - 1;
		}		
	}
	

}

