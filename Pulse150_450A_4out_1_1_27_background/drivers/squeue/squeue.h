#ifndef DRIVERS_SQUEUE_QUEUE_H
#define DRIVERS_SQUEUE_QUEUE_H

//#include "F2837xS_device.h"

#define	cQBufNormal			0
#define	cQBufFull			1
#define	cQBufEmpty			2

#define	cQDiscard			0
#define	cQCoverFirst		1
#define	cQCoverLast			2



typedef	struct
{
	void* pIn;
	void* pOut;
	void* pStart;
	unsigned int length;
	unsigned int size;
    unsigned int step;
}QUEUE;

extern	void sQInit(QUEUE *pq,void *start,unsigned int size,unsigned int AddrStep); 
extern	unsigned char sQDataIn(QUEUE	*pq,void *pdata,unsigned char option); 
extern	unsigned char sQDataOut(QUEUE *pq,void *pdata);
extern	void sQClear(QUEUE *pq);
#endif
