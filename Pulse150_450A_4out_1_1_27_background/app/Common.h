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
 *  2023��12��28��       V1.0                          Created.
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
	uint16_t used;		//fifo used flag,0:�գ�1:�ǿ�
	uint16_t buflen;	//fifo len
	uint16_t nextlen;	//������������
	int16_t trigpos;	//����λ��	-1��δ����
	float *buf;			//���ݻ���ָ��
}struWaveFifo;

#define MAX_WAVE_FIFO	10

typedef enum
{
	ePosCurrWave,
	eNegCurrWave,
	eVoltWave,
	eTempWave
}eWaveType;



//#define EXHIBITION_MODE	//չ��ģʽ
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
