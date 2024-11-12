/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : Common.h
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023年12月28日       V1.0                          Created.
 *
 *============================================================================*/
#ifndef APP_COMMON_H_
#define APP_COMMON_H_

#ifdef Common_GLOBALS
#define Common_EXT
#else
#define Common_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/
typedef struct
{
	uint16_t head;		//fifo head
	uint16_t tail;		//fifo tail
	uint16_t used;		//fifo used flag,0:空，1:非空
	uint16_t buflen;	//fifo len
	uint16_t nextlen;	//触发后保留长度
	int16_t trigpos;	//触发位置	-1：未触发
	float *buf;			//数据缓存指针
}struWaveFifo;

#define MAX_WAVE_FIFO	10

typedef enum
{
	ePosCurrWave,
	eNegCurrWave,
	eVoltWave,
	eTempWave
}eWaveType;



//#define EXHIBITION_MODE	//展机模式
/********************************************************************************
* Variable declaration                              *
********************************************************************************/
Common_EXT uint16_t Rs485Addr;

#ifdef Common_GLOBALS
Common_EXT struWaveFifo sWaveFifo[MAX_WAVE_FIFO]={0};
#else
Common_EXT struWaveFifo sWaveFifo[MAX_WAVE_FIFO];
#endif
/********************************************************************************
* function declaration                              *
********************************************************************************/
Common_EXT uint16_t Get485Addr(void);

Common_EXT void SetStatus485A(int status);

Common_EXT void SetStatus485B(int status);

Common_EXT void SetStatus485C(int status);

Common_EXT void waveFifoInit(eWaveType wavetype, float *wavebuf, uint16_t buflen, uint16_t nextlen);

Common_EXT void waveRecord(eWaveType wavetype, uint16_t trig, float data);

#endif /* APP_COMMON_H_ */
