#include "squeue.h"

void sQInit(QUEUE *pq,void *start,unsigned int size,unsigned int AddrStep); 
unsigned char sQDataIn(QUEUE	*pq,void *pdata,unsigned char option); 
unsigned char sQDataOut(QUEUE *pq,void *pdata);

/************************************************************************************
*Function name:	sQInit																*
*Parameters:	pq:	pointer to queue structure to be initialized					*
*				start:start address of ring buffer									*
*				size:the size of the ring buffer                                    *
*               QueueType:Type of the address point in queue                        *
*Description:	initialize a queue structure										*
*************************************************************************************/
void sQInit(QUEUE *pq,void *start,unsigned int size,unsigned int AddrStep)
{
	pq->pIn = start;
	pq->pOut = start;
	pq->pStart = start;
	pq->length = 0;
	pq->size = size;
    pq->step = AddrStep;
}

/************************************************************************************
*Function name:	sQDataIn															*
*Parameters:	pq:	pointer to queue structure to be initialized					*
*				pdata:the data point to be inserted into the queue							*
*				option:how to deal with the data when the buffer is full			*
*				cQCoverFirst:cover the first data									*
*				cQCoverLast:cover the latest data									*
*Returns:		cQBufNormal:data has been inserted into the queue					*
*				cQBufFull:the buffer is full										*
*Description:	insert a data into the queue										*
*************************************************************************************/
unsigned char sQDataIn(QUEUE	*pq,void *pdata,unsigned char option)
{

    unsigned int uwAddrStep,uwCnt;
	unsigned char *pubTemp;

	uwAddrStep = pq->step;
    if(pq->length == pq->size)
	{
		if(option == cQCoverFirst)
		{
		    //*(pq->pIn) = data;
			for(uwCnt = 0;uwCnt < uwAddrStep;uwCnt++)
	        {
		        *((unsigned char *)pq->pIn + uwCnt) = *((unsigned char *)pdata + uwCnt);
	        }
			if((unsigned char *)pq->pIn == ((unsigned char *)pq->pStart + (pq->size-1) * uwAddrStep))
			{
				pq->pIn = pq->pStart;
			}
			else
			{
				pq->pIn = (unsigned char *)pq->pIn + uwAddrStep;
			}
			pq->pOut = pq->pIn;
		}
		else if(option == cQCoverLast)
		{
			if(pq->pIn == pq->pStart)
			{
				//*(pq->pStart + (pq->size - 1)) = data;
				pubTemp = (unsigned char *)pq->pStart + (pq->size - 1) * uwAddrStep;
				for(uwCnt = 0;uwCnt < uwAddrStep;uwCnt++)
	            {
		            *(pubTemp+ uwCnt) = *((unsigned char *)pdata + uwCnt);
	            }
			}
			else
			{
				//*(pq->pIn - 1) = data;
				pubTemp = (unsigned char *)pq->pIn - uwAddrStep;
				for(uwCnt = 0;uwCnt < uwAddrStep;uwCnt++)
	            {
		            *(pubTemp+ uwCnt) = *((unsigned char *)pdata + uwCnt);
	            }
			}
		}
		return(cQBufFull);
	}
	else
	{
		//*(pq->pIn) = data;
		for(uwCnt = 0;uwCnt < uwAddrStep;uwCnt++)
        {
	        *((unsigned char *)pq->pIn + uwCnt) = *((unsigned char *)pdata + uwCnt);
        }
		pq->length++;
		if((unsigned char *)pq->pIn == (unsigned char *)pq->pStart + (pq->size - 1) * uwAddrStep)
		{
			pq->pIn = pq->pStart;
		}
		else
		{
			pq->pIn = (unsigned char *)pq->pIn + uwAddrStep;
		}
		return(cQBufNormal);
	}
}
/************************************************************************************
*Function name:	sQDataOut															*
*Parameters:	pq:	pointer to queue structure to be initialized					*
*				pdata:the address to save the data									*
*Returns:		cQBufNormal:data has been inserted into the queue					*
*				cQBufEmpty:the buffer is empty										*
*Description:	Get a data from the queue											*
*************************************************************************************/
unsigned char sQDataOut(QUEUE *pq,void *pdata)
{
    unsigned int uwAddrStep,uwCnt;
	
	uwAddrStep = pq->step;
	if(pq->length == 0)
	{
		return(cQBufEmpty);
	}

	for(uwCnt = 0;uwCnt < uwAddrStep;uwCnt++)
	{
		*(((unsigned char *)pdata + uwCnt)) = *(((unsigned char *)(pq->pOut) + uwCnt));
	}
	
	pq->length--;
	
	if((unsigned char *)pq->pOut == ((unsigned char *)pq->pStart + (pq->size- 1) * uwAddrStep))
	{
		pq->pOut = pq->pStart;
	}
	else
	{
		pq->pOut = (unsigned char *)pq->pOut + uwAddrStep;
	}	
	return(cQBufNormal);
}

	
void sQClear(QUEUE *pq)
{
	pq->pIn = pq->pStart;
	pq->pOut = pq->pStart;
	pq->length = 0;
}
