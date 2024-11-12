/*******************************************************************************
*  Copyright (C),  
*  FileName: Comframe.c
*  Author: VD      Version: 01       Date: 2018/11
*  Description:      
*  Function List:
*    1. 
*  History: 
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sciframe.h"
#include "Common.h"
#include "Constant.h"
#include "ParallelTask.h"
#include "OsConfig.h"


/* Private variables ---------------------------------------------------------*/
const uint16_t tblCRC[] = CRC_DATA_TAB;

/* Private variables ---------------------------------------------------------*/
Scistruct scigroup[SCI_MAX];

uint16_t SlaveStartAddr;


volatile struct SCI_REGS * const SciRegsChannal[SCI_MAX] =
{
  &SciaRegs,
  &ScibRegs,
  &ScicRegs,
};


#define		SEND_ONGOING			(BaseType_t)0x0000
#define		SEND_BUFF_E 			(BaseType_t)0x0001
#define		SEND_COMPLETE 			(BaseType_t)0x0002
#define		SEND_ERROR   			(BaseType_t)0x0003




/* Private function prototypes -----------------------------------------------*/

void SciA485Enstart(void)
{
    SetStatus485A(STATUS_485_SEND);
}

void SciA485Enstop(void)
{
    SetStatus485A(STATUS_485_RECEIVED);
}

void SciB485Enstart(void)
{
    SetStatus485B(STATUS_485_SEND);
}

void SciB485Enstop(void)
{
    SetStatus485B(STATUS_485_RECEIVED);
}

void SciC485Enstart(void)
{
    SetStatus485C(STATUS_485_SEND);
}

void SciC485Enstop(void)
{
    SetStatus485C(STATUS_485_RECEIVED);
}

basefunction const Sci485Enstart[SCI_MAX] =
{
  &SciA485Enstart,
  &SciB485Enstart,
  &SciC485Enstart,
};

 basefunction const Sci485Enstop[SCI_MAX] =
{
  &SciA485Enstop,
  &SciB485Enstop,
  &SciC485Enstop,
};
/*******************************************************************************
Function name:	CRC16
Description:  	CRC Verify
Calls:
Called By:
Parameters: 	uint16_t *,uint16_t
Return: 		uint16_t
*******************************************************************************/
uint16_t CRC16(uint16_t * puchMsg, uint16_t usDataLen)
{
	uint16_t uchCRCHi = 0xFF ;
	uint16_t uchCRCLo = 0xFF ; 
	uint16_t uIndex ; 		   
	uint16_t hi,low;

	while (usDataLen--) 			
	{
		uIndex = uchCRCLo ^ *puchMsg++ ; 
		hi = tblCRC[uIndex] >> 8;
		low = tblCRC[uIndex] & 0xff;
		uchCRCLo = uchCRCHi ^ hi;
		uchCRCHi = low;
	}
	return (uchCRCHi << 8 | uchCRCLo) ;
}

/*********************************************************************
Function name:  modbusreceivechar
Calls:
Called By:
Parameters:     SciType  ,BaseType_t *
Return:         BaseType_t
*********************************************************************/
void modbusreceivechar(SciType sci, uint16_t data)
{
//  uint16_t tmp;
//  tmp = scigroup[sci].rx.index;

  scigroup[sci].rx.data[scigroup[sci].rx.index] = data;
  //length limit protect
  scigroup[sci].rx.index++;
  scigroup[sci].rx.len++;
  
  if (scigroup[sci].rx.index >= LEN_MAX)
  {
      scigroup[sci].rx.index = 0;
	  scigroup[sci].rx.len = 0;
  }
}

/*********************************************************************
Function name:  SciReadChar
Calls:
Called By:
Parameters:     SciType  ,BaseType_t *
Return:         BaseType_t
*********************************************************************/
BaseType_t SciReadChar(SciType sci)
{
  uint16_t temp;
  BaseType_t read = pdPASS;
  if(scigroup[sci].pSciRegs->SCIFFRX.bit.RXFFST != 0)
  {
    temp = scigroup[sci].pSciRegs->SCIRXBUF.bit.SAR;
//	ScibRegs.SCITXBUF.bit.TXDT = temp;	//test
    modbusreceivechar(sci, temp);
  }
  else
  {
    read = pdFAIL;
  }
  return read;
}

/*********************************************************************
Function name:  SciSendBuf
Description:
Calls:
Called By:
Parameters:     SciType
Return:         BaseType_t
*********************************************************************/

BaseType_t SciSendBuf(SciType sci)
{
	BaseType_t send = SEND_ONGOING;
	uint16_t temp;
	uint16_t i=0;
	if(scigroup[sci].tx.len == 0)
	{
		send = SEND_ERROR; 
	}
	if (scigroup[sci].pSciRegs->SCIFFTX.bit.TXFFST > 14)
	{
		//Current Buf have not finished, then wait
	}
	else
	{
		if (scigroup[sci].tx.index < scigroup[sci].tx.len)
		{
			scigroup[sci].txstart();
			while(scigroup[sci].pSciRegs->SCIFFTX.bit.TXFFST < 16)
			{
				temp = scigroup[sci].tx.index;
				scigroup[sci].pSciRegs->SCITXBUF.bit.TXDT = scigroup[sci].tx.data[temp];
				scigroup[sci].tx.index++;
				i++;
				if((scigroup[sci].tx.index >= scigroup[sci].tx.len)||(i>=16))
				{
					break;
				}
			}
		}
		else
		{
			//一次判断有可能有问题，可能会造成最后一字节没发出去
			if((scigroup[sci].pSciRegs->SCICTL2.bit.TXEMPTY==1)
			&&(scigroup[sci].pSciRegs->SCICTL2.bit.TXRDY  ==1)
			&&(scigroup[sci].pSciRegs->SCIFFTX.bit.TXFFST ==0))
			{
				if((scigroup[sci].pSciRegs->SCICTL2.bit.TXEMPTY==1)
				&&(scigroup[sci].pSciRegs->SCICTL2.bit.TXRDY  ==1)
				&&(scigroup[sci].pSciRegs->SCIFFTX.bit.TXFFST ==0))
				{
					send = SEND_COMPLETE;
					scigroup[sci].txstop();
				}
				else
				{
					send = SEND_BUFF_E;
				}
			}
			else
			{
				send = SEND_BUFF_E;
			}
		}
	}

	return send;
}

/*********************************************************************
Function name:  isWholeCmd
Description: return -1:addr error, -2:cmd error, -3:len error, -4:crc error
Calls:
Called By:
Parameters:     SciType
Return:         BaseType_t
*********************************************************************/
int isWholeCmd(SciType sci)
{
    uint16_t address, cmd;
    uint16_t temp;
    uint16_t len;

    address = scigroup[sci].rx.data[0];
	
	if(pdFALSE == scigroup[sci].isHost)
	{
		if(sci == SCI_COM)
		{
			if((address != GADDRESS) && (address != BADDRESS) && (address != scigroup[sci].address))
//				&& ((address < SlaveStartAddr) || (address >= SlaveStartAddr + SLAVEMAX)))
				return SCIFRAM_ADDR_ERR;
		}
		else
		{
			if((address != GADDRESS) && (address != BADDRESS) && (address != scigroup[sci].address))
				return SCIFRAM_ADDR_ERR;
		}
	}
	else
	{
		if((address != GADDRESS) && (address != BADDRESS) && (address != scigroup[sci].tx.data[0]))
			return SCIFRAM_ADDR_ERR;
	}


    cmd = scigroup[sci].rx.data[1];
    if (((pdFALSE != scigroup[sci].isHost) && (scigroup[sci].tx.data[1] != cmd))
     && ((pdFALSE == scigroup[sci].isHost) && ((CMD_READ != cmd) && (CMD_WRITE_ONE != cmd) && (CMD_WRITE != cmd))))
    {
        return SCIFRAM_CMD_ERR;
    }

    if (CMD_WRITE_ONE == cmd)
    {
        temp = CRC16(&scigroup[sci].rx.data[0], 6);				 // crc verify
        if (((temp >> 8) == scigroup[sci].rx.data[7])
                && ((temp & 0xff) == scigroup[sci].rx.data[6]))
        {
            scigroup[sci].incmd.cmd = CMD_WRITE_ONE;
			scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
            scigroup[sci].incmd.regaddress = ((scigroup[sci].rx.data[2] << 8)
                    | (scigroup[sci].rx.data[3] & 0x00FF));
            scigroup[sci].incmd.pdata = &scigroup[sci].rx.data[4];
            return SCIFRAM_NORMAL;
        }
		else
			return SCIFRAM_CRC_ERR;
    }
    else if (CMD_READ == cmd)
    {
        if (pdFALSE != scigroup[sci].isHost)
        {
            len = scigroup[sci].rx.data[2] + 5;
            if (scigroup[sci].rx.index >= len - 1)
            {
                temp = CRC16(&scigroup[sci].rx.data[0], (len - 2));	// crc verify
                if (((temp >> 8) == scigroup[sci].rx.data[len - 1])
                        && ((temp & 0xff) == scigroup[sci].rx.data[len - 2]))
                {
                    scigroup[sci].incmd.cmd = CMD_READ;
                    scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
                    scigroup[sci].incmd.regaddress = ((scigroup[sci].tx.data[2]
                            << 8) | (scigroup[sci].tx.data[3] & 0x00FF));
                    scigroup[sci].incmd.len = (scigroup[sci].rx.data[2]) >> 1;
                    scigroup[sci].incmd.pdata = &scigroup[sci].rx.data[3];
                    return SCIFRAM_NORMAL;
                }
                else
                    return SCIFRAM_CRC_ERR;
            }
            else
                return SCIFRAM_LEN_ERR;
        }
        else
        {
            temp = CRC16(&scigroup[sci].rx.data[0], 6);			// crc verify
            if (((temp >> 8) == scigroup[sci].rx.data[7])
                    && ((temp & 0xff) == scigroup[sci].rx.data[6]))
            {
                scigroup[sci].incmd.cmd = CMD_READ;
				scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
                scigroup[sci].incmd.regaddress =
                        ((scigroup[sci].rx.data[2] << 8)
                                | (scigroup[sci].rx.data[3] & 0x00FF));
                scigroup[sci].incmd.len = ((scigroup[sci].rx.data[4] << 8)
                        | (scigroup[sci].rx.data[5] & 0x00FF));
                return SCIFRAM_NORMAL;
            }
            else
                return SCIFRAM_CRC_ERR;
        }
    }
	else if (CMD_STOP == cmd)
	{
		if(scigroup[sci].BoardType == UnitCB)
		{
			temp = CRC16(&scigroup[sci].rx.data[0], 4);			// crc verify
	        if (((temp >> 8) == scigroup[sci].rx.data[5])
	                && ((temp & 0xff) == scigroup[sci].rx.data[4]))
			{
	                scigroup[sci].incmd.cmd = CMD_STOP;
					scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
	                scigroup[sci].incmd.len = 1;
	                scigroup[sci].incmd.pdata = &scigroup[sci].rx.data[2];
	                return SCIFRAM_NORMAL;
	        }
	        else
	            return SCIFRAM_CRC_ERR;
		}
		else
		{
			temp = CRC16(&scigroup[sci].rx.data[0], 10);			// crc verify
	        if (((temp >> 8) == scigroup[sci].rx.data[11])
	                && ((temp & 0xff) == scigroup[sci].rx.data[10]))
			{
	                scigroup[sci].incmd.cmd = CMD_STOP;
					scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
	                scigroup[sci].incmd.len = 4;
	                scigroup[sci].incmd.pdata = &scigroup[sci].rx.data[2];
	                return SCIFRAM_NORMAL;
	        }
	        else
	            return SCIFRAM_CRC_ERR;
		}
	}
	else if (CMD_READWRITE_MULT == cmd)
	{
		if (pdFALSE == scigroup[sci].isHost)
		{
			uint16_t readlen = (scigroup[sci].rx.data[4]<<8)+scigroup[sci].rx.data[5];
			uint16_t writelen = (scigroup[sci].rx.data[8]<<8)+scigroup[sci].rx.data[9];
			if((readlen > 0) && (writelen > 0))	
			{
				len = scigroup[sci].rx.data[10]+ 13;
            	if (scigroup[sci].rx.index >= len - 1)
            	{
            		temp = CRC16(&scigroup[sci].rx.data[0], (len - 2));	// crc verify
	                if (((temp >> 8) == scigroup[sci].rx.data[len - 1])
	                	&& ((temp & 0xff) == scigroup[sci].rx.data[len - 2]))
	                {
	                	scigroup[sci].incmd.cmd = CMD_READWRITE_MULT;
						scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
		                scigroup[sci].incmd.regaddress = ((scigroup[sci].rx.data[2] << 8)
		                        | (scigroup[sci].rx.data[3] & 0x00FF));
						scigroup[sci].incmd.len = ((scigroup[sci].rx.data[4] << 8)
                            | (scigroup[sci].rx.data[5] & 0x00FF));
						scigroup[sci].incmd.regaddress2 = ((scigroup[sci].rx.data[6] << 8)
		                        | (scigroup[sci].rx.data[7] & 0x00FF));
						scigroup[sci].incmd.len2 = ((scigroup[sci].rx.data[8] << 8)
                            	| (scigroup[sci].rx.data[9] & 0x00FF));
						scigroup[sci].incmd.pdata = &scigroup[sci].rx.data[11];

						return SCIFRAM_NORMAL;
	                }
					else
						return SCIFRAM_CRC_ERR;
            	}
				else
					return SCIFRAM_LEN_ERR;
			}
			else
				return SCIFRAM_LEN_ERR;
		}
		else
		{
			len = scigroup[sci].rx.data[2] + 5;
			if (scigroup[sci].rx.index >= len - 1)
            {
                temp = CRC16(&scigroup[sci].rx.data[0], (len - 2));	// crc verify
                if (((temp >> 8) == scigroup[sci].rx.data[len - 1])
                        && ((temp & 0xff) == scigroup[sci].rx.data[len - 2]))
                {
                    scigroup[sci].incmd.cmd = CMD_READWRITE_MULT;
                    scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
                    scigroup[sci].incmd.len = (scigroup[sci].rx.data[2]) >> 1;
                    scigroup[sci].incmd.pdata = &scigroup[sci].rx.data[3];
                    return SCIFRAM_NORMAL;
                }
                else
                    return SCIFRAM_CRC_ERR;
            }
            else
                return SCIFRAM_LEN_ERR;
		}
	}
    else if (CMD_WRITE == cmd)
    {
        if ((pdFALSE == scigroup[sci].isHost) && (scigroup[sci].rx.data[6] > 0))
        {
            len = scigroup[sci].rx.data[6] + 9;
            if (scigroup[sci].rx.index >= len - 1)
            {
                temp = CRC16(&scigroup[sci].rx.data[0], (len - 2));	// crc verify
                if (((temp >> 8) == scigroup[sci].rx.data[len - 1])
                        && ((temp & 0xff) == scigroup[sci].rx.data[len - 2]))
                {
                    scigroup[sci].incmd.cmd = CMD_WRITE;
					scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
                    scigroup[sci].incmd.regaddress = ((scigroup[sci].rx.data[2]
                            << 8) | (scigroup[sci].rx.data[3] & 0x00FF));
                    scigroup[sci].incmd.len = ((scigroup[sci].rx.data[4] << 8)
                            | (scigroup[sci].rx.data[5] & 0x00FF));
                    scigroup[sci].incmd.pdata = &scigroup[sci].rx.data[7];
                    return SCIFRAM_NORMAL;
                }
                else
                    return SCIFRAM_CRC_ERR;
            }
            else
                return SCIFRAM_LEN_ERR;
        }
        else if (pdFALSE != scigroup[sci].isHost)
        {
            temp = CRC16(&scigroup[sci].rx.data[0], 6);			// crc verify
            if (((temp >> 8) == scigroup[sci].rx.data[7])
                    && ((temp & 0xff) == scigroup[sci].rx.data[6]))
            {
                scigroup[sci].incmd.cmd = CMD_WRITE;
				scigroup[sci].incmd.address = scigroup[sci].rx.data[0];
                scigroup[sci].incmd.regaddress =
                        ((scigroup[sci].rx.data[2] << 8)
                                | (scigroup[sci].rx.data[3] & 0x00FF));
                scigroup[sci].incmd.len = ((scigroup[sci].rx.data[4] << 8)
                        | (scigroup[sci].rx.data[5] & 0x00FF));
                return SCIFRAM_NORMAL;
            }
            else
                return SCIFRAM_CRC_ERR;
        }
    }
    return SCIFRAM_NODEFINE;
}

/*********************************************************************
Function name:  SciGroupManage
Description:
Calls:
Called By:
Parameters:     SciType
Return:         void
*********************************************************************/
uint32_t recvErrCnt[4] = {0};
uint32_t recvTimeoutCnt[4] = {0};
void SciGroupManage(SciType sci)
{
    BaseType_t action = pdFAIL;
    switch (scigroup[sci].phase)
    {
    case SCI_PHASE_TX:
        if (SEND_ONGOING != SciSendBuf(sci))
        {
            scigroup[sci].phase = SCI_PHASE_TX_W_COMPLETE;
        }
        break;

    case SCI_PHASE_TX_W_COMPLETE:
        if (SEND_COMPLETE == SciSendBuf(sci))
        {			
            scigroup[sci].tx.index = 0;
            scigroup[sci].tx.len = 0;
            scigroup[sci].rx.index = 0;
            scigroup[sci].rx.len = 0;
            scigroup[sci].rx.data[0] = 0;
            scigroup[sci].rx.data[1] = 0;
            scigroup[sci].rx.data[2] = 0;
            scigroup[sci].phase = SCI_PHASE_RX;
            scigroup[sci].Timestart = BackgroundGetTickCount();
        }
        break;

    case SCI_PHASE_RX:
        action = SciReadChar(sci);
        if (pdFAIL != action)
        {
            while (pdFAIL != action)
            {
                action = SciReadChar(sci);
            }
            scigroup[sci].phase = SCI_PHASE_RX_W_COMPLETE;
        }
        else if (((BackgroundGetTickCount() - scigroup[sci].Timestart) > 50)
                && (pdFAIL != scigroup[sci].isHost))
        {
            scigroup[sci].phase = SCI_PHASE_RX_ERROR;
        }
		else 
		{
			if(scigroup[sci].recIdleCnt >= scigroup[sci].recTimeoutThrold)
			{
				scigroup[sci].recTimeout = 1;
			}
			else
			{
				scigroup[sci].recIdleCnt++;
			}
		}
        break;

    case SCI_PHASE_RX_W_COMPLETE:
        action = SciReadChar(sci);
        if (pdFAIL == action)
        {
            scigroup[sci].phase = SCI_PHASE_RX_COMPLETE;
            scigroup[sci].Timestart = BackgroundGetTickCount();
        }
        else
        {
            while (pdFAIL != action)
            {
                action = SciReadChar(sci);
            }
        }
        break;

    case SCI_PHASE_RX_COMPLETE:
//        if ((BackgroundGetTickCount() - scigroup[sci].Timestart) > 5)
        {
        	scigroup[sci].recvFramCnt++;
			scigroup[sci].outcmd.used = pdFAIL;

			int16_t ret = isWholeCmd(sci);
            if (ret == SCIFRAM_NORMAL)
            {
				if((sci == 0)&&(scigroup[0].isHost))
				{
					if((scigroup[sci].tx.data[0] >= SlaveStartAddr) && (scigroup[sci].tx.data[0] < SlaveStartAddr + SLAVEMAX))
				    {
						recvErrCnt[scigroup[sci].tx.data[0]-SlaveStartAddr] = 0;
						recvTimeoutCnt[scigroup[sci].tx.data[0]-SlaveStartAddr] = 0;
					}
				}
            	scigroup[sci].recIdleCnt = 0;
				scigroup[sci].recTimeout = 0;
				
                scigroup[sci].recvNormal(&scigroup[sci].incmd,
                                         &scigroup[sci].outcmd);
                if (pdFAIL != scigroup[sci].isHost)
                {
                    if (scigroup[sci].outcmd.used)
                    {
                        switch (scigroup[sci].outcmd.cmd)
                        {
                        case CMD_WRITE:
                            setHostWriteReport(sci,
                                               scigroup[sci].outcmd.address,
                                               scigroup[sci].outcmd.regaddress,
                                               scigroup[sci].outcmd.len,
                                               scigroup[sci].outcmd.pdata);
                            break;
                        case CMD_WRITE_ONE:
                            setHostWriteOneReport(
                                    sci, scigroup[sci].outcmd.address,
                                    scigroup[sci].outcmd.regaddress,
                                    *(scigroup[sci].outcmd.pdata));
                            break;
						case CMD_STOP:
							setHostStopCmdReport(sci,
												scigroup[sci].outcmd.address,
												scigroup[sci].outcmd.len,
                                               scigroup[sci].outcmd.pdata);
							break;
						case CMD_READWRITE_MULT:
							setHostRWMultReport(sci,scigroup[sci].outcmd);
							break;
                        default:
                            setHostreadReport(sci, scigroup[sci].outcmd.address,
                                              scigroup[sci].outcmd.regaddress,
                                              scigroup[sci].outcmd.len);
                            break;
                        }
                    }
                }
                else
                {
                    if (scigroup[sci].outcmd.used)
                    {
                        switch (scigroup[sci].outcmd.cmd)
                        {
                        case CMD_WRITE:
                            setDeviceWriteReport(
                                    sci, scigroup[sci].outcmd.regaddress,
                                    scigroup[sci].outcmd.len);
                            break;
                        case CMD_WRITE_ONE:
                            setDeviceWriteOneReport(
                                    sci, scigroup[sci].outcmd.regaddress,
                                    *(scigroup[sci].outcmd.pdata));
                            break;
						case CMD_READWRITE_MULT:
							setDeviceRWMultReport(sci,scigroup[sci].outcmd.len,
												(scigroup[sci].outcmd.pdata));
							break;
                        default:
                            setDevicereadReport(sci, scigroup[sci].outcmd.len,
                                                (scigroup[sci].outcmd.pdata));
                            break;
                        }
                    }
                }
            }
            else if(ret != SCIFRAM_ADDR_ERR)
            {
                scigroup[sci].recvError(&scigroup[sci].outcmd);
				scigroup[sci].recvErrorCnt++;
				if((sci == 0)&&(scigroup[0].isHost))
				{
					if((scigroup[sci].tx.data[0] >= SlaveStartAddr) && (scigroup[sci].tx.data[0] < SlaveStartAddr + SLAVEMAX))
				    {
					    if(recvErrCnt[scigroup[sci].tx.data[0]-SlaveStartAddr] < 10000)
					        recvErrCnt[scigroup[sci].tx.data[0]-SlaveStartAddr]++;
					}
				}
            }

            scigroup[sci].rx.index = 0;
            scigroup[sci].rx.len = 0;
            //if(scigroup[sci].tx.len == 0)
            if (pdFAIL == scigroup[sci].outcmd.used)
            {
                scigroup[sci].phase = SCI_PHASE_RX;
            }
            else
            {
                scigroup[sci].phase = SCI_PHASE_TX;
            }
        }
        break;

    case SCI_PHASE_RX_ERROR:
		scigroup[sci].outcmd.used = pdFAIL;
		scigroup[sci].recvTimeoutCnt++;
		if((sci == 0)&&(scigroup[0].isHost))
        {
			if((scigroup[sci].tx.data[0] >= SlaveStartAddr) && (scigroup[sci].tx.data[0] < SlaveStartAddr + SLAVEMAX))
		    {
			    if(recvTimeoutCnt[scigroup[sci].tx.data[0]-SlaveStartAddr] < 10000)
			        recvTimeoutCnt[scigroup[sci].tx.data[0]-SlaveStartAddr]++;
			}
        }
        scigroup[sci].recvError(&scigroup[sci].outcmd);
        if (pdFAIL != scigroup[sci].isHost)
        {
            if (scigroup[sci].outcmd.used)
            {
                switch (scigroup[sci].outcmd.cmd)
                {
                case CMD_WRITE:
                    setHostWriteReport(sci, scigroup[sci].outcmd.address,
                                       scigroup[sci].outcmd.regaddress,
                                       scigroup[sci].outcmd.len,
                                       scigroup[sci].outcmd.pdata);
                    break;
                case CMD_WRITE_ONE:
                    setHostWriteOneReport(sci, scigroup[sci].outcmd.address,
                                          scigroup[sci].outcmd.regaddress,
                                          *(scigroup[sci].outcmd.pdata));
                    break;
				case CMD_STOP:
					setHostStopCmdReport(sci,
										scigroup[sci].outcmd.address,
										scigroup[sci].outcmd.len,
                                       scigroup[sci].outcmd.pdata);
					break;
				case CMD_READWRITE_MULT:
					setHostRWMultReport(sci,scigroup[sci].outcmd);
					break;
                default:
                    setHostreadReport(sci, scigroup[sci].outcmd.address,
                                      scigroup[sci].outcmd.regaddress,
                                      scigroup[sci].outcmd.len);
                    break;
                }
            }
        }
        scigroup[sci].rx.index = 0;
        scigroup[sci].rx.len = 0;
        if (pdFAIL == scigroup[sci].outcmd.used)
        {
            scigroup[sci].phase = SCI_PHASE_RX;
        }
        else
        {
            scigroup[sci].phase = SCI_PHASE_TX;
        }
        break;

    }
    if ((scigroup[sci].pSciRegs->SCIRXST.bit.RXERROR)
            || (scigroup[sci].pSciRegs->SCIRXST.bit.FE)
            || (scigroup[sci].pSciRegs->SCIRXST.bit.OE)
            || (scigroup[sci].pSciRegs->SCIRXST.bit.PE)
            || (scigroup[sci].pSciRegs->SCIRXST.bit.BRKDT)
//            || (scigroup[sci].pSciRegs->SCIRXBUF.bit.SCIFFFE)
//            || (scigroup[sci].pSciRegs->SCIRXBUF.bit.SCIFFPE)
            )
    {
        //Have an error occur
        scigroup[sci].pSciRegs->SCICTL1.bit.SWRESET = 0;
        scigroup[sci].pSciRegs->SCIFFTX.bit.TXFIFORESET = 0;
        scigroup[sci].pSciRegs->SCIFFRX.bit.RXFIFORESET = 0;
        //DELAY_US(5);
        scigroup[sci].pSciRegs->SCICTL1.bit.SWRESET = 1;
        scigroup[sci].pSciRegs->SCIFFTX.bit.TXFIFORESET = 1;
        scigroup[sci].pSciRegs->SCIFFRX.bit.RXFIFORESET = 1;

		scigroup[sci].rstCnt++;
        scigroup[sci].rx.len = 0;
        scigroup[sci].rx.index = 0;
        scigroup[sci].tx.index = 0;
        scigroup[sci].tx.len = 0;
        scigroup[sci].txstop();
        scigroup[sci].phase = SCI_PHASE_TX;
    }
}

/*********************************************************************
Function name:  setHostreadReport
Description:
Calls:
Called By:
Parameters:
Return:         void
*********************************************************************/
void setHostreadReport(SciType sci, uint16_t address, uint16_t reg, uint16_t regnum)
{
  uint16_t temp;
  temp = reg;
  scigroup[sci].tx.data[0] = address;
  scigroup[sci].tx.data[1] = 0x03;
  scigroup[sci].tx.data[2] = (temp>>8);  //start adress
  scigroup[sci].tx.data[3] = (temp&0x00FF);  //start adress
  temp = regnum;
  scigroup[sci].tx.data[4] = (temp>>8);      //register number
  scigroup[sci].tx.data[5] = (temp&0x00FF);  //register number
  temp = CRC16(&scigroup[sci].tx.data[0],6);
  scigroup[sci].tx.data[6] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[7] = (temp>>8);      //crc
  scigroup[sci].tx.len = 8;
}

void setDevicereadReport(SciType sci, uint16_t number,uint16_t *pdata)
{
  uint16_t temp;
  uint16_t len;
  uint16_t num;
  uint16_t *ptrdata;
  num = number;
  len = 0;
  ptrdata = pdata;
  scigroup[sci].tx.data[len++] = scigroup[sci].address;
  scigroup[sci].tx.data[len++] = 0x03;
  scigroup[sci].tx.data[len++] = num<<1;
  while(num > 0)
  {
    temp = *ptrdata++;
    scigroup[sci].tx.data[len++] = (temp>>8);      //register number
    scigroup[sci].tx.data[len++] = (temp&0x00FF);  //register number
    num--;
  }
  temp = CRC16(&scigroup[sci].tx.data[0],len);
  scigroup[sci].tx.data[len++] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[len++] = (temp>>8);      //crc
  scigroup[sci].tx.len = len;
}

void setHostWriteOneReport(SciType sci, uint16_t address, uint16_t reg, uint16_t value)
{
  uint16_t temp;
  temp = reg;
  scigroup[sci].tx.data[0] = address;
  scigroup[sci].tx.data[1] = 0x06;
  scigroup[sci].tx.data[2] = (temp>>8);  //start adress
  scigroup[sci].tx.data[3] = (temp&0x00FF);  //start adress
  temp = value;
  scigroup[sci].tx.data[4] = (temp>>8);      //register number
  scigroup[sci].tx.data[5] = (temp&0x00FF);  //register number
  temp = CRC16(&scigroup[sci].tx.data[0],6);
  scigroup[sci].tx.data[6] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[7] = (temp>>8);      //crc
  scigroup[sci].tx.len = 8;
}

void setDeviceWriteOneReport(SciType sci, uint16_t reg, uint16_t value)
{
  uint16_t temp;
  temp = reg;
  scigroup[sci].tx.data[0] = scigroup[sci].address;
  scigroup[sci].tx.data[1] = 0x06;
  scigroup[sci].tx.data[2] = (temp>>8);  //start adress
  scigroup[sci].tx.data[3] = (temp&0x00FF);  //start adress
  temp = value;
  scigroup[sci].tx.data[4] = (temp>>8);      //register number
  scigroup[sci].tx.data[5] = (temp&0x00FF);  //register number
  temp = CRC16(&scigroup[sci].tx.data[0],6);
  scigroup[sci].tx.data[6] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[7] = (temp>>8);      //crc
  scigroup[sci].tx.len = 8;
}

void setHostWriteReport(SciType sci, uint16_t address, uint16_t reg, uint16_t regnum,uint16_t *pdata)
{
  uint16_t temp;
  uint16_t len;
  uint16_t num;
  uint16_t *ptrdata;
  num = regnum;
  len = 0;
  ptrdata = pdata;
  scigroup[sci].tx.data[len++] = address;
  scigroup[sci].tx.data[len++] = 0x10;
  temp = reg;
  scigroup[sci].tx.data[len++] = (temp>>8);
  scigroup[sci].tx.data[len++] = (temp&0x00FF);
  scigroup[sci].tx.data[len++] = (num>>8);
  scigroup[sci].tx.data[len++] = (num&0x00FF);
  scigroup[sci].tx.data[len++] = num<<1;
  while(num > 0)
  {
    temp = *ptrdata++;
    scigroup[sci].tx.data[len++] = (temp>>8);      //register number
    scigroup[sci].tx.data[len++] = (temp&0x00FF);  //register number
    num--;
  }
  temp = CRC16(&scigroup[sci].tx.data[0],len);
  scigroup[sci].tx.data[len++] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[len++] = (temp>>8);      //crc
  scigroup[sci].tx.len = len;
}

void setDeviceWriteReport(SciType sci, uint16_t reg, uint16_t regnum)
{
  uint16_t temp;
  temp = reg;
  scigroup[sci].tx.data[0] = scigroup[sci].address;
  scigroup[sci].tx.data[1] = 0x10;
  scigroup[sci].tx.data[2] = (temp>>8);  //start adress
  scigroup[sci].tx.data[3] = (temp&0x00FF);  //start adress
  temp = regnum;
  scigroup[sci].tx.data[4] = (temp>>8);      //register number
  scigroup[sci].tx.data[5] = (temp&0x00FF);  //register number
  temp = CRC16(&scigroup[sci].tx.data[0],6);
  scigroup[sci].tx.data[6] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[7] = (temp>>8);      //crc
  scigroup[sci].tx.len = 8;
}
/*********************************************************************
Function name:  setHostRWMultReport
Description:
Calls:
Called By:
Parameters:
Return:         void
*********************************************************************/
void setHostRWMultReport(SciType sci,Cmdstruct outcmd)
{
  uint16_t temp;
  uint16_t len;
  uint16_t num;
  uint16_t *ptrdata;
  num = outcmd.len2;
  len = 0;
  ptrdata = outcmd.pdata;
  scigroup[sci].tx.data[len++] = outcmd.address;
  scigroup[sci].tx.data[len++] = 0x17;
  scigroup[sci].tx.data[len++] = outcmd.regaddress>>8;
  scigroup[sci].tx.data[len++] = outcmd.regaddress&0x00ff;
  scigroup[sci].tx.data[len++] = outcmd.len>>8;
  scigroup[sci].tx.data[len++] = outcmd.len&0x00ff;
  scigroup[sci].tx.data[len++] = outcmd.regaddress2>>8;
  scigroup[sci].tx.data[len++] = outcmd.regaddress2&0x00ff;
  scigroup[sci].tx.data[len++] = outcmd.len2>>8;
  scigroup[sci].tx.data[len++] = outcmd.len2&0x00ff;
  scigroup[sci].tx.data[len++] = num<<1;
  while(num > 0)
  {
    temp = *ptrdata++;
    scigroup[sci].tx.data[len++] = (temp>>8);      //register number
    scigroup[sci].tx.data[len++] = (temp&0x00FF);  //register number
    num--;
  }
  temp = CRC16(&scigroup[sci].tx.data[0],len);
  scigroup[sci].tx.data[len++] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[len++] = (temp>>8);      //crc
  scigroup[sci].tx.len = len;
}

/*********************************************************************
Function name:  setDeviceRWMultReport
Description:
Calls:
Called By:
Parameters:
Return:         void
*********************************************************************/
void setDeviceRWMultReport(SciType sci, uint16_t number,uint16_t *pdata)
{
  uint16_t temp;
  uint16_t len;
  uint16_t num;
  uint16_t *ptrdata;
  num = number;
  len = 0;
  ptrdata = pdata;
  scigroup[sci].tx.data[len++] = scigroup[sci].outcmd.address;
  scigroup[sci].tx.data[len++] = 0x17;
  scigroup[sci].tx.data[len++] = num<<1;
  while(num > 0)
  {
    temp = *ptrdata++;
    scigroup[sci].tx.data[len++] = (temp>>8);      //register number
    scigroup[sci].tx.data[len++] = (temp&0x00FF);  //register number
    num--;
  }
  temp = CRC16(&scigroup[sci].tx.data[0],len);
  scigroup[sci].tx.data[len++] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[len++] = (temp>>8);      //crc
  scigroup[sci].tx.len = len;
}

/*********************************************************************
Function name:  setHostStopCmdReport
Description:
Calls:
Called By:
Parameters:
Return:         void
*********************************************************************/
void setHostStopCmdReport(SciType sci, uint16_t addr,uint16_t num,uint16_t *pdata)
{
  uint16_t temp;
  uint16_t len = 0;

  scigroup[sci].tx.data[len++] = addr;
  scigroup[sci].tx.data[len++] = 0x20;
  while(num > 0)
  {
    temp = *pdata++;
    scigroup[sci].tx.data[len++] = (temp>>8);      //register number
    scigroup[sci].tx.data[len++] = (temp&0x00FF);  //register number
    num--;
  }
  temp = CRC16(&scigroup[sci].tx.data[0],len);
  scigroup[sci].tx.data[len++] = (temp&0x00FF);  //crc
  scigroup[sci].tx.data[len++] = (temp>>8);      //crc
  scigroup[sci].tx.len = len;
}
/*********************************************************************
Function name:  setSciAddr
Description:
Calls:
Called By:
Parameters:
Return:         void
*********************************************************************/
void setSciAddr(SciType sci, uint16_t addr)
{
	scigroup[sci].address = addr;
}

/*********************************************************************
Function name:  sciframeinit
Description:
Calls:
Called By:
Parameters:
Return:         void
*********************************************************************/
void sciframeinit(uint16_t sci, uint16_t address, BaseType_t host, BaseType_t boardtype, recvfuntion recvok, funtion recvnok,BaseType_t recTimeoutThrold)
{
  scigroup[sci].rx.len = 0;
  scigroup[sci].rx.index = 0;
  scigroup[sci].tx.index = 0;
  scigroup[sci].tx.len = 0;
  scigroup[sci].address = address;
  scigroup[sci].BoardType = boardtype;
  scigroup[sci].isHost = host;
  scigroup[sci].recvNormal = recvok;
  scigroup[sci].recvError = recvnok;
  scigroup[sci].recvFramCnt = 0;
  scigroup[sci].recvErrorCnt = 0;
  scigroup[sci].rstCnt = 0;
  scigroup[sci].recTimeout = 0;
  scigroup[sci].recIdleCnt = 0;
  scigroup[sci].recTimeoutThrold = recTimeoutThrold;
  scigroup[sci].pSciRegs = SciRegsChannal[sci];
  scigroup[sci].txstart = Sci485Enstart[sci];
  scigroup[sci].txstop = Sci485Enstop[sci];
  scigroup[sci].txstop();
}







