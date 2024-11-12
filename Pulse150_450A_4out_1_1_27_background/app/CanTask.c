/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : CanTask.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define CanTask_GLOBALS     1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "SuperTask.h"
#include "LoadTask.h"
#include "Interrupt.h"
#include "CanTask.h"
#include "InterfaceTask.h"
#include "OsConfig.h"
#include "Constant.h"
#include "Common.h"
#include "canmodule.h"
#include "sciframe.h"
#include "HMITask.h"
/********************************************************************************
*const define                               *
********************************************************************************/

#if ACDC_BOARD_TEST

#define TEST_SCI    2

#define ACDC_ADDR   1

#define TEST_CMD_START  '@'
#define TEST_CMD_END1   '*'
#define TEST_CMD_END2   '\r'
#endif

/********************************************************************************
* Variable declaration                              *
********************************************************************************/
#if ACDC_BOARD_TEST
extern Scistruct scigroup[SCI_MAX];

typedef enum
{
    eSTATUS_PREPARE_TXDATA,
    eSTATUS_TX,
    eSTATUS_TX_END,
    eSTATUS_RX,
    eSTATUS_PROCESS_DATA,
}ePOWER_CTRL_STATUS;
	

typedef enum
{
    ACDC_PRO_WD,
    ACDC_PRO_RD,
    ACDC_PRO_MB
}eADDC_PRO;

eADDC_PRO acdcPro = ACDC_PRO_WD;

uint32_t RecTimeDis = 0;

uint16_t PowerOnOff = 0;
uint16_t PowerOnFinish = 0;

#endif

/********************************************************************************
* function declaration                              *
********************************************************************************/
#if ACDC_BOARD_TEST
char decToAscii(uint16_t dec)
{
    if(dec >= 10)
        return '0';
    return '0'+dec;
}

char hexToAscii(uint16_t dec)
{
    if(dec > 0xf)
        return '0';
    if(dec >= 10)
        return 'A'+dec-10;
    else
        return '0'+dec;
}

uint16_t asciiToDec(char c)
{
    if((c >= '0')&&(c <= '9'))
        return c - '0';
    return 0;
}

uint16_t asciiToHex(char c)
{
    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    return 0;
}
/*********************************************************************
Function name:  writeTxbuf_WD
Description:
Calls:
Called By:
Parameters:     uint16_t,uint16_t,uint16_t
Return:         uint16_t
*********************************************************************/
uint16_t writeTxbuf_WD(uint16_t power,uint16_t vol,uint16_t value)
{
    uint16_t i=0,index=0;
    uint16_t fcs=0;

    scigroup[TEST_SCI].tx.data[index++] = TEST_CMD_START;

    scigroup[TEST_SCI].tx.data[index++] = decToAscii(ACDC_ADDR/10);
    scigroup[TEST_SCI].tx.data[index++] = decToAscii(ACDC_ADDR%10);

    scigroup[TEST_SCI].tx.data[index++] = 'W';
    scigroup[TEST_SCI].tx.data[index++] = 'D';

    scigroup[TEST_SCI].tx.data[index++] = decToAscii(power);
    scigroup[TEST_SCI].tx.data[index++] = decToAscii(vol);

    scigroup[TEST_SCI].tx.data[index++] = decToAscii(value/1000);
    scigroup[TEST_SCI].tx.data[index++] = decToAscii(value/100%10);
    scigroup[TEST_SCI].tx.data[index++] = decToAscii(value/10%10);
    scigroup[TEST_SCI].tx.data[index++] = decToAscii(value%10);

    for(i=0; i<index; i++)
    {
        fcs ^= scigroup[TEST_SCI].tx.data[i];
    }
    scigroup[TEST_SCI].tx.data[index++] = hexToAscii((fcs>>4)&0xf);
    scigroup[TEST_SCI].tx.data[index++] = hexToAscii(fcs&0xf);

    scigroup[TEST_SCI].tx.data[index++] = TEST_CMD_END1;
    scigroup[TEST_SCI].tx.data[index++] = TEST_CMD_END2;

    scigroup[TEST_SCI].tx.len = index;
    return index;
}

/*********************************************************************
Function name:  ReadUartBuf
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void ReadUartBuf(void)
{
    int temp;
    while (scigroup[TEST_SCI].pSciRegs->SCIFFRX.bit.RXFFST != 0)
    {
        temp = scigroup[TEST_SCI].pSciRegs->SCIRXBUF.bit.SAR;                    // 接收数据

        scigroup[TEST_SCI].rx.data[scigroup[TEST_SCI].rx.index] = temp;
        //length limit protect
        scigroup[TEST_SCI].rx.index++;
        scigroup[TEST_SCI].rx.len++;

        if (scigroup[TEST_SCI].rx.index >= LEN_MAX)
        {
            scigroup[TEST_SCI].rx.index = 0;
        }

    }
}

/*********************************************************************
Function name:  JudgeWholeCmd
Description:
Calls:
Called By:
Parameters:     int
Return:         void
*********************************************************************/
int JudgeWholeCmd(void)
{
    uint16_t len = scigroup[TEST_SCI].rx.len;
    uint16_t i = 0, fcs = 0;
    if(len == 0)
        return -3;
    if ((scigroup[TEST_SCI].rx.data[0] != TEST_CMD_START)
            || (scigroup[TEST_SCI].rx.data[len - 2] != TEST_CMD_END1)
            || (scigroup[TEST_SCI].rx.data[len - 1] != TEST_CMD_END2))
        return -1;

    if ((scigroup[TEST_SCI].rx.data[3] == 'W')
            && (scigroup[TEST_SCI].rx.data[4] == 'D'))
        acdcPro = ACDC_PRO_WD;
    else if ((scigroup[TEST_SCI].rx.data[3] == 'R')
            && (scigroup[TEST_SCI].rx.data[4] == 'D'))
        acdcPro = ACDC_PRO_WD;
    else if ((scigroup[TEST_SCI].rx.data[3] == 'M')
            && (scigroup[TEST_SCI].rx.data[4] == 'B'))
        acdcPro = ACDC_PRO_MB;
    else
        acdcPro = ACDC_PRO_WD;

    for (i = 0; i < len - 4; i++)
    {
        fcs ^= scigroup[TEST_SCI].rx.data[i];
    }

    if ((hexToAscii((fcs >> 4) & 0xf) != scigroup[TEST_SCI].rx.data[len - 4])
            || (hexToAscii(fcs & 0xf) != scigroup[TEST_SCI].rx.data[len - 3]))
    {
        return -2;
    }

    return 0;
}

/*********************************************************************
Function name:  testRxProcess
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void testRxProcess(void)
{
	if(dif32(BackgroundGetTickCount(), RecTimeDis) > 20000)
	{
		acdcWdResp.communicationFault = 1;
		RecTimeDis = BackgroundGetTickCount();
	}
    if (scigroup[TEST_SCI].pSciRegs->SCIFFRX.bit.RXFFST > 0)
    {
        ReadUartBuf();
        scigroup[TEST_SCI].Timestart = BackgroundGetTickCount();
    }
    else if ((scigroup[TEST_SCI].rx.len > 0) && (dif32(BackgroundGetTickCount(), scigroup[TEST_SCI].Timestart) > 10))
    {
        if (JudgeWholeCmd() == 0)
        {
        	scigroup[TEST_SCI].recvFramCnt++;
            if (acdcPro == ACDC_PRO_WD)
            {
                acdcWdResp.communicationFault = asciiToDec(
                        scigroup[TEST_SCI].rx.data[5]) * 10
                        + asciiToDec(scigroup[TEST_SCI].rx.data[6]);
                acdcWdResp.isPowerOn = asciiToDec(
                        scigroup[TEST_SCI].rx.data[7]);
                acdcWdResp.CCOrCV = asciiToDec(scigroup[TEST_SCI].rx.data[8]);
                acdcWdResp.autoOrManual = asciiToDec(
                        scigroup[TEST_SCI].rx.data[9]);
                acdcWdResp.localMachineFault = asciiToDec(
                        scigroup[TEST_SCI].rx.data[10]) * 10
                        + asciiToDec(scigroup[TEST_SCI].rx.data[11]);
                acdcWdResp.actualCurrent = asciiToDec(
                        scigroup[TEST_SCI].rx.data[12]) * 1000
                        + asciiToDec(scigroup[TEST_SCI].rx.data[13]) * 100
                        + asciiToDec(scigroup[TEST_SCI].rx.data[14]) * 10
                        + asciiToDec(scigroup[TEST_SCI].rx.data[15]);
                acdcWdResp.actualVoltage = asciiToDec(
                        scigroup[TEST_SCI].rx.data[16]) * 1000
                        + asciiToDec(scigroup[TEST_SCI].rx.data[17]) * 100
                        + asciiToDec(scigroup[TEST_SCI].rx.data[18]) * 10
                        + asciiToDec(scigroup[TEST_SCI].rx.data[19]);
            }
			
        }
		else
		{
			scigroup[TEST_SCI].recvErrorCnt++;
			if ((scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.RXERROR)
	            || (scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.FE)
	            || (scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.OE)
	            || (scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.PE)
	            || (scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.BRKDT)
	//            || (scigroup[sci].pSciRegs->SCIRXBUF.bit.SCIFFFE)
	//            || (scigroup[sci].pSciRegs->SCIRXBUF.bit.SCIFFPE)
	            )
		    {
		        //Have an error occur
		        scigroup[TEST_SCI].pSciRegs->SCICTL1.bit.SWRESET = 0;
		        scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFIFORESET = 0;
		        scigroup[TEST_SCI].pSciRegs->SCIFFRX.bit.RXFIFORESET = 0;
		        //DELAY_US(5);
		        scigroup[TEST_SCI].pSciRegs->SCICTL1.bit.SWRESET = 1;
		        scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFIFORESET = 1;
		        scigroup[TEST_SCI].pSciRegs->SCIFFRX.bit.RXFIFORESET = 1;

				scigroup[TEST_SCI].rstCnt++;
		        scigroup[TEST_SCI].rx.len = 0;
		        scigroup[TEST_SCI].rx.index = 0;
		        scigroup[TEST_SCI].tx.index = 0;
		        scigroup[TEST_SCI].tx.len = 0;
		        scigroup[TEST_SCI].txstop();
		    }
		}
        scigroup[TEST_SCI].rx.index = 0;
        scigroup[TEST_SCI].rx.len = 0;
		RecTimeDis = BackgroundGetTickCount();
    }
	else
	{}
}

/*********************************************************************
Function name:  testTxProcess
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
//int powertest = 0;
void testTxProcess(void)
{
    uint16_t temp;
	static TickType_t distime = 0;

	if(dif32(BackgroundGetTickCount(), distime) > 500)
	{
//		writeTxbuf_WD(Ctrldata1.Ctrl1.bit.OnOff,1,PowerSetValue*10);
//		if(powertest)
//			writeTxbuf_WD(0,1,PowerSetValue*10);
//		else
		writeTxbuf_WD(1,1,PowerSetValue*10);
	}
	
    if (scigroup[TEST_SCI].tx.len == 0)
    {
        return;
    }

    if (scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST >= 16)
    {
        return;
    }
    else
    {
        if (scigroup[TEST_SCI].tx.index < scigroup[TEST_SCI].tx.len)
        {
        	scigroup[TEST_SCI].txstart();
            while (1)
            {
            	if(scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST < 16)
                {
	                temp = scigroup[TEST_SCI].tx.index;
	                scigroup[TEST_SCI].pSciRegs->SCITXBUF.bit.TXDT =
	                        scigroup[TEST_SCI].tx.data[temp];
	                scigroup[TEST_SCI].tx.index++;
            	}

                if ((scigroup[TEST_SCI].tx.index >= scigroup[TEST_SCI].tx.len))
                {
//                	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
                    break;
                }
            }
        }

		//前级电源回复大概在发送完600us后，需要及时切换收发状态才能保证不丢数据
		if(scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST <= 4)	
        {
	        while(1)
	        {
	            if ((scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXEMPTY == 1)
	                && (scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXRDY == 1)
	                && (scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST == 0))
	            {
	//            	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
					if ((scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXEMPTY == 1)
	                && (scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXRDY == 1)
	                && (scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST == 0))
					{
	//					GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
		            	scigroup[TEST_SCI].txstop();
		                scigroup[TEST_SCI].tx.index = 0;
		                scigroup[TEST_SCI].tx.len = 0;
						distime = BackgroundGetTickCount();
						return;
	                }
	            }
	        }
		}
    }
}

void sCheckSciTx()
{
	if ((scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXEMPTY == 1)
        && (scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXRDY == 1)
        && (scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST == 0))
    {
		if ((scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXEMPTY == 1)
        && (scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXRDY == 1)
        && (scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST == 0))
		{
        	scigroup[TEST_SCI].txstop();
        }
    }
}
ePOWER_CTRL_STATUS PowerCtrlStatus = eSTATUS_PREPARE_TXDATA;
void PowerComm()
{
	static uint16_t distime = 0;
	uint16_t temp;
	
	switch(PowerCtrlStatus)
	{
		case eSTATUS_PREPARE_TXDATA:
			if(distime++ >= 500)
			{
				writeTxbuf_WD(PowerOnOff,1,PowerSetValue*10);
				scigroup[TEST_SCI].tx.index = 0;
				PowerCtrlStatus = eSTATUS_TX;
			}
			break;
		case eSTATUS_TX:
			if (scigroup[TEST_SCI].tx.len == 0)
		    {
		        PowerCtrlStatus = eSTATUS_PREPARE_TXDATA;
				distime = 0;
				break;
		    }

		    if (scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST >= 16)
		    {
		        break;
		    }
		    else
		    {
		        if (scigroup[TEST_SCI].tx.index < scigroup[TEST_SCI].tx.len)
		        {
		        	scigroup[TEST_SCI].txstart();
		            while ((scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST < 16) && (scigroup[TEST_SCI].tx.index < scigroup[TEST_SCI].tx.len))
		            {
		            	
		                temp = scigroup[TEST_SCI].tx.index;
		                scigroup[TEST_SCI].pSciRegs->SCITXBUF.bit.TXDT =
		                        scigroup[TEST_SCI].tx.data[temp];
		                scigroup[TEST_SCI].tx.index++;
		            }
		        }

				if(scigroup[TEST_SCI].tx.index >= scigroup[TEST_SCI].tx.len)
				{
					PowerCtrlStatus = eSTATUS_TX_END;
				}
		    }
			break;
		case eSTATUS_TX_END:
			if ((scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXEMPTY == 1)
                && (scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXRDY == 1)
                && (scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST == 0))
            {
				if ((scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXEMPTY == 1)
                && (scigroup[TEST_SCI].pSciRegs->SCICTL2.bit.TXRDY == 1)
                && (scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFFST == 0))
				{
	            	scigroup[TEST_SCI].txstop();
	                scigroup[TEST_SCI].tx.index = 0;
	                scigroup[TEST_SCI].tx.len = 0;
					scigroup[TEST_SCI].rx.index = 0;
        			scigroup[TEST_SCI].rx.len = 0;
					
					PowerCtrlStatus = eSTATUS_RX;
					scigroup[TEST_SCI].Timestart = BackgroundGetTickCount();
                }
            }
			break;
		case eSTATUS_RX:
			if (scigroup[TEST_SCI].pSciRegs->SCIFFRX.bit.RXFFST > 0)
		    {
		        ReadUartBuf();
		        scigroup[TEST_SCI].Timestart = BackgroundGetTickCount();
		    }
			else if (dif32(BackgroundGetTickCount(), scigroup[TEST_SCI].Timestart) > 100)
			{
				if(scigroup[TEST_SCI].recTimeout < 10000)
					scigroup[TEST_SCI].recTimeout++;
				PowerCtrlStatus = eSTATUS_PREPARE_TXDATA;
				distime = 0;
			}
		    else if ((scigroup[TEST_SCI].rx.len > 0) && (dif32(BackgroundGetTickCount(), scigroup[TEST_SCI].Timestart) > 5))
		    {
		        PowerCtrlStatus = eSTATUS_PROCESS_DATA;
		    }
			break;
		case eSTATUS_PROCESS_DATA:
			if (JudgeWholeCmd() == 0)
	        {
	        	scigroup[TEST_SCI].recTimeout = 0;
	        	scigroup[TEST_SCI].recvErrorCnt = 0;
	        	scigroup[TEST_SCI].recvFramCnt++;
	            if (acdcPro == ACDC_PRO_WD)
	            {
	                acdcWdResp.communicationFault = asciiToDec(
	                        scigroup[TEST_SCI].rx.data[5]) * 10
	                        + asciiToDec(scigroup[TEST_SCI].rx.data[6]);
	                acdcWdResp.isPowerOn = asciiToDec(
	                        scigroup[TEST_SCI].rx.data[7]);
	                acdcWdResp.CCOrCV = asciiToDec(scigroup[TEST_SCI].rx.data[8]);
	                acdcWdResp.autoOrManual = asciiToDec(
	                        scigroup[TEST_SCI].rx.data[9]);
	                acdcWdResp.localMachineFault = asciiToDec(
	                        scigroup[TEST_SCI].rx.data[10]) * 10
	                        + asciiToDec(scigroup[TEST_SCI].rx.data[11]);
	                acdcWdResp.actualCurrent = asciiToDec(
	                        scigroup[TEST_SCI].rx.data[12]) * 1000
	                        + asciiToDec(scigroup[TEST_SCI].rx.data[13]) * 100
	                        + asciiToDec(scigroup[TEST_SCI].rx.data[14]) * 10
	                        + asciiToDec(scigroup[TEST_SCI].rx.data[15]);
	                acdcWdResp.actualVoltage = asciiToDec(
	                        scigroup[TEST_SCI].rx.data[16]) * 1000
	                        + asciiToDec(scigroup[TEST_SCI].rx.data[17]) * 100
	                        + asciiToDec(scigroup[TEST_SCI].rx.data[18]) * 10
	                        + asciiToDec(scigroup[TEST_SCI].rx.data[19]);
	            }
	        }
			else
			{
				if(scigroup[TEST_SCI].recvErrorCnt < 10000)
					scigroup[TEST_SCI].recvErrorCnt++;
			}
			PowerCtrlStatus = eSTATUS_PREPARE_TXDATA;
			distime = 0;
			break;
		default:
			PowerCtrlStatus = eSTATUS_PREPARE_TXDATA;
			distime = 0;
			break;
	}
	if ((scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.RXERROR)
    || (scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.FE)
    || (scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.OE)
    || (scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.PE)
    || (scigroup[TEST_SCI].pSciRegs->SCIRXST.bit.BRKDT)
//            || (scigroup[sci].pSciRegs->SCIRXBUF.bit.SCIFFFE)
//            || (scigroup[sci].pSciRegs->SCIRXBUF.bit.SCIFFPE)
    )
	{
	    //Have an error occur
	    scigroup[TEST_SCI].pSciRegs->SCICTL1.bit.SWRESET = 0;
	    scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFIFORESET = 0;
	    scigroup[TEST_SCI].pSciRegs->SCIFFRX.bit.RXFIFORESET = 0;
	    //DELAY_US(5);
	    scigroup[TEST_SCI].pSciRegs->SCICTL1.bit.SWRESET = 1;
	    scigroup[TEST_SCI].pSciRegs->SCIFFTX.bit.TXFIFORESET = 1;
	    scigroup[TEST_SCI].pSciRegs->SCIFFRX.bit.RXFIFORESET = 1;

		scigroup[TEST_SCI].rstCnt++;
	    scigroup[TEST_SCI].rx.len = 0;
	    scigroup[TEST_SCI].rx.index = 0;
	    scigroup[TEST_SCI].tx.index = 0;
	    scigroup[TEST_SCI].tx.len = 0;
	    scigroup[TEST_SCI].txstop();
	}

	if((scigroup[TEST_SCI].recTimeout + scigroup[TEST_SCI].recvErrorCnt) > 40)
	{
		acdcWdResp.communicationFault = 1;
	}
}

void PowerOnOffCtrl(uint16_t curstatus)
{
	static uint16_t distime = 0;
	static uint16_t poweronProc = 0;
	static uint16_t poweroffProc = 0;

	if(curstatus == ePowerOn)
	{
		poweroffProc = 0;
		
		switch(poweronProc)
		{
			case 0:
				poweronProc = 1;
				distime = 0;
				PowerOnOff = ePowerOn;
				break;
			case 1:
				if(acdcWdResp.isPowerOn == 1)
				{
					poweronProc = 2;
				}
				break;
			case 2:
				if(distime++ > 5000)
				{
					poweronProc = 3;
				}
				break;
			case 3:
				PowerOnFinish = 1;
				
				OSEventSend(cPrioSuper,eSuperTurnOn);
				poweronProc = 0;
			
				break;
			default:
				poweronProc = 0;
				break;
		}
	}
	else
	{
		PowerOnFinish = 0;
		poweronProc = 0;

		switch(poweroffProc)
		{
			case 0:
				poweroffProc = 1;
				distime = 0;
				break;
			case 1:
				if(distime++ > 500)
				{
					PowerOnOff = ePowerOff;
					poweroffProc = 0;
				}
				break;
			default:
				poweroffProc = 0;
				break;
		}
	}
}

int16_t PowerOnFinished()
{
	if(PowerOnFinish == 1)
	{
		return pdTRUE;
	}
	
	return pdFALSE;
}
#endif

/*********************************************************************
Function name:  CanTaskInit
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void CanTaskInit(void)
{
#if (ACDC_BOARD_TEST == 0)
    uint16_t txMsgData[8] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
    uint16_t count=0;

    CanDataInit();
    CAN_startModule(CANA_BASE);
#else
    RecTimeDis = BackgroundGetTickCount();
	sciframeinit(TEST_SCI, Get485Addr(), 1, GetBoardType(), NULL, NULL,500);
    writeTxbuf_WD(PowerOnOff,1,PowerSetValue);
#endif
}

/*********************************************************************
Function name:  CanTask
Description:
Calls:
Called By:
Parameters:     void*
Return:         void
*********************************************************************/
void CanTask(void * pvParameters)
{
	static uint16_t curStatus = ePowerOff;
    uint32_t event;
    //BaseType_t err;
/*
#if (ACDC_BOARD_TEST == 0)
    uint16_t txMsgData[8] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
    uint16_t count=0;

    CanDataInit();
    CAN_startModule(CANA_BASE);
#else
	RecTimeDis = BackgroundGetTickCount();
	sciframeinit(TEST_SCI, Get485Addr(), 1, GetBoardType(), NULL, NULL);
	writeTxbuf_WD(1,1,PowerSetValue);
#endif
*/
    //while(pdTRUE)
    {

        //err = xSemaphoreTake( xSemaphoreCan, (TickType_t)Tick4mS );
        event = OSEventPend(cPrioCan);
        if(event != 0)
        {

			if(OSEventConfirm(event,eCanPowerOff))
			{
				curStatus = ePowerOff;
			}
			else if(OSEventConfirm(event,eCanPowerOn))
			{
				curStatus = ePowerOn;
			}
			
           	if(OSEventConfirm(event,eCanTest))
			{

			}

        }
        else//eCanTimer
        {
#if ACDC_BOARD_TEST
//            testTxProcess();
//            testRxProcess();
			PowerComm();
#else
            count++;
            if(count&1)
            {
                CAN_sendMessage(CANA_BASE, TX_MSG_OBJ_ID, MSG_DATA_LENGTH, txMsgData);
            }
            else
                CanTxRx();
#endif
        }
#if ACDC_BOARD_TEST	
		PowerOnOffCtrl(curStatus);
#endif
    }
}




