/*=============================================================================*
 *         Copyright(c) 2010-2014, Invt Co., Led.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : eeprommodule.c
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-12-24       V1.0                          Created.
 *
 *============================================================================*/
#define	 EEPROMMODULE_GLOBALS  1
/********************************************************************************
*include header file                              *
********************************************************************************/

#include "eeprommodule.h"
#include "f280013x_device.h"
#include "EepromParamDefault.h"
#include "scimodbusreg.h"
#include "fault.h"
#include "device.h"
#include "Common.h"
#include "ParallelTask.h"

/********************************************************************************
*const define                               *
********************************************************************************/
#define AT24C16_PAGE_LEN    16
#define AT24C16_TOTAL_LEN   2048

/********************************************************************************
* Variable declaration                              *
********************************************************************************/

/********************************************************************************
* function declaration                              *
********************************************************************************/

void ClearUnitParamToDefault(void);

void I2CWrite(uint16_t slaveAddr, uint16_t memAddr, uint16_t *srcAddr,uint16_t byteCount)
{
    uint16_t index = 0;
    uint16_t startPage = memAddr>>4;
    uint16_t endPage = (memAddr+byteCount)>>4;
    uint16_t startPagelen = (startPage==endPage)?byteCount:((startPage+1)*AT24C16_PAGE_LEN-memAddr);
    uint16_t endPagelen = (startPage==endPage)?0:(memAddr+byteCount-endPage*AT24C16_PAGE_LEN);
    uint16_t addr = memAddr,len=startPagelen;

    if(AT24C16_TOTAL_LEN < memAddr+byteCount)
        return;

    do{
        if(addr == memAddr)
        {
            len=startPagelen;
        }
        else if(addr == endPage*AT24C16_PAGE_LEN)
        {
            len = endPagelen;
        }
        else
        {
            len = AT24C16_PAGE_LEN;
        }
        I2caRegs.I2CSAR.all = slaveAddr | (addr>>8);      // Slave address

        // Configure I2C as Master Transmitter
        I2caRegs.I2CMDR.bit.MST = 0x1;
        I2caRegs.I2CMDR.bit.TRX = 0x1;

        I2caRegs.I2CCNT = len+1;

        // send Start condition
        I2caRegs.I2CMDR.bit.STT = 0x1;

        //transmit the memAddr
        I2caRegs.I2CDXR.all= addr&0xff;
        while(I2caRegs.I2CSTR.bit.BYTESENT != 0x1);
        I2caRegs.I2CSTR.bit.BYTESENT = 0x1;

        //transmit the bytes
        for(index=0; index < len; index++)
        {
            I2caRegs.I2CDXR.all= srcAddr[index];

            //wait till byte is sent
            while(I2caRegs.I2CSTR.bit.BYTESENT != 0x1);

            //clear the byte sent
            I2caRegs.I2CSTR.bit.BYTESENT = 0x1;
        }

        I2caRegs.I2CMDR.bit.STP = 0x1;
        while(I2caRegs.I2CMDR.bit.STP != 0x0);

        I2caRegs.I2CSTR.bit.BYTESENT = 0x1;
//      vTaskDelay(5);
		DEVICE_DELAY_US(5000);

        addr += len;
    }while(addr < memAddr+byteCount);
}

uint16_t I2CRead(uint16_t slaveAddr, uint16_t memAddr, uint16_t *dstAddr, uint16_t byteCount)
{
    if(AT24C16_TOTAL_LEN < memAddr+byteCount)
        return 0;

    // Configure slave address
    I2caRegs.I2CSAR.all = slaveAddr | (memAddr>>8);

    // Configure I2C as Master Transmitter
    I2caRegs.I2CMDR.bit.MST = 0x1;
    I2caRegs.I2CMDR.bit.TRX = 0x1;

    //Set Data Count
    I2caRegs.I2CCNT = 1;


    // send Start condition
    I2caRegs.I2CMDR.bit.STT = 0x1;

    //
    //transmit the memAddr
    //
    I2caRegs.I2CDXR.all= memAddr&0xff;
    while(I2caRegs.I2CSTR.bit.BYTESENT != 0x1);
    I2caRegs.I2CSTR.bit.BYTESENT = 0x1;

    // Configure I2C in Master Receiver mode
    I2caRegs.I2CMDR.bit.MST = 0x1;
    I2caRegs.I2CMDR.bit.TRX = 0x0;

    //Set Data Count
    I2caRegs.I2CCNT = byteCount;

    // send Start condition
    I2caRegs.I2CMDR.bit.STT = 0x1;

    uint16_t count = 0;

    // Read the received data into RX buffer
    while(count < byteCount)
    {
        if(I2caRegs.I2CSTR.bit.RRDY ==0x1)
        {
            dstAddr[count] = I2caRegs.I2CDRR.all;
            count++;
        }
    }

    // Send STOP condition
    I2caRegs.I2CMDR.bit.STP = 0x1;
    while(I2caRegs.I2CMDR.bit.STP != 0x0);
    I2caRegs.I2CSTR.bit.BYTESENT = 0x1;


    return count;
}
/**********************************************************************
函数名:EepromReadAword
功  能: 从eeprom读一个字
输  入: address:读EEPROM的地址
     dat:数据指针
返  回: 0:失败，1：成功
**********************************************************************/
Uint16	EepromReadAword(Uint16 EepromAddr,Uint16 *dat)
{
    Uint16 ui8Temp[2] = {0};

	if(I2CRead(EEPROM_SLAVE_ADDR,EepromAddr,ui8Temp,2) == 2)
	{
		*dat = (ui8Temp[1] << 8) + ui8Temp[0];
		return 1;
	}
	else
		return 0;

	
}

/**********************************************************************
函数名:EepromWriteAword
功  能: 写一个字到eeprom
输  入: EepromAddr:写入EEPROM的地址
     data:数据
返  回: 0:失败，1：成功
**********************************************************************/
Uint16 EepromWriteAword(Uint16 EepromAddr,Uint16 data)
{
    Uint16 tem[2];
	Uint16 tem16;
    tem[0] = (data & 0xFF);
    tem[1] = (data >> 8) & 0xFF;
    I2CWrite(EEPROM_SLAVE_ADDR,EepromAddr,&tem[0],2);
	
	if(EepromReadAword(EepromAddr,&tem16))
	{
		if(data == tem16)
			return 1;
	}

	return 0;
}

/**********************************************************************
函数名:ReadEepromDataLong
功  能: 从eeprom读多个字
输  入: EepromAddr:读EEPROM的地址
     pRAM:数据指针
     wordLength：字长度
返  回: 读取成功的字数
**********************************************************************/
Uint16 ReadEepromDataLong(Uint16 EepromAddr,Uint16* pRAM,Uint16 wordLength)
{
	Uint16 i,rep;
	
 	for(i=0;i<wordLength;i++)
    {
    	rep=0;
    	while(1)
   	 	{
   	 		if(EepromReadAword(EepromAddr+i*2,pRAM+i))
				break;
			rep++;
			if(rep >= 3)
			{
				FaultRegs.HwDriver.bit.EepromRead = 1;
				return i;
			}
    	}
    }	
	FaultRegs.HwDriver.bit.EepromRead = 0;
	return i;
}

/**********************************************************************
函数名:WriteEepromDataLong
功  能: 写多个字到eeprom
输  入: EepromAddr:写EEPROM的地址
     pRAM:数据指针
     Length：字长度
返  回: 写成功的字数
**********************************************************************/
Uint16 WriteEepromDataLong(Uint16 EepromAddr,Uint16* pRAM,Uint16 Length)
{
	Uint16 i,rep;
 	for(i=0;i<Length;i++)
    {
    	rep = 0;
		while(1)
    	{
	    	if(1 == EepromWriteAword(EepromAddr+i*2,*(pRAM+i)))
	    	{
				break;
			}
			rep++;
			if(rep >= 3)
			{
				FaultRegs.HwDriver.bit.EepromWrite = 1;
				return i;
			}
		}
    }
	FaultRegs.HwDriver.bit.EepromWrite = 0;
	return i;
}


//------------------------------------------
//Load all data parameter from eeprom
Uint16 LoadEeprom(void)
{
	Uint16 tem[2] = {0};
	ReadEepromDataLong(EEPROM_PARA_START,tem,2);
	if((tem[0] == sUintEepromParaDef.Magic) || (tem[0] == sModuleEepromParaDef.Magic))
	{
		if(tem[0] == sUintEepromParaDef.Magic)
		{
			ReadEepromDataLong(EEPROM_PARA_START,(Uint16*)&sUintEepromPara,sizeof(sUnitEEPROM));

		    VerifyUnitEeprom();

			ReadEepromDataLong(EEPROM_ADJUST_PARA_START,tem,2);
			if(tem[0] == UNIT_ADJUST_PARAM_DEFAULT_HEAD)
			{
				ReadEepromDataLong(EEPROM_ADJUST_PARA_START,(Uint16*)&sUintEepromAdjustPara,sizeof(sUnitEEPROMAdjust));
				
				VerifyUnitAdjustEeprom();
			}
			else
			{
				ClearUnitAdjustParamToDefault();
			}
			SetBoardType(UnitCB);
			return 1;	
		}
		else //if(tem[0] == sModuleEepromParaDef.Magic)
		{
			ReadEepromDataLong(EEPROM_PARA_START,(Uint16*)&sModuleEepromPara,sizeof(sModuleEEPROM));

			VerifyModuleEeprom();
			SetBoardType(ModuleCB);
			return 0;
		}
	}
	else
	{
		if( Get485Addr() < 8)
		{
			ClearModuleParamToDefault();
			SetBoardType(ModuleCB);
			return 0;
		}
		else
		{
			ClearUnitParamToDefault();
			ClearUnitAdjustParamToDefault();
			SetBoardType(UnitCB);
			return 1;
		}
	}
}

Uint16 VerifyUnitEeprom(void)
{
    Uint16 result = 0;
	Uint16 *pParamDst = (Uint16 *)&sUintEepromPara;
	Uint16 *pParamSrc = (Uint16 *)&sUintEepromParaDef;

    if((sUintEepromPara.Magic != sUintEepromParaDef.Magic) || (sUintEepromPara.Len > sizeof(sUnitEEPROM)))
    {
    	ClearUnitParamToDefault();
		result = 1;
    }
	else
	{
		if(sUintEepromPara.Len < sizeof(sUnitEEPROM))
		{
			memcpy(pParamDst+sUintEepromPara.Len,pParamSrc+sUintEepromPara.Len,sizeof(sUnitEEPROM)-sUintEepromPara.Len);
			sUintEepromPara.Len = sizeof(sUnitEEPROM);
			WriteEepromDataLong(EEPROM_PARA_START,(Uint16*)&sUintEepromPara,sizeof(sUnitEEPROM));
			result = 1;
		}
	}
    return result;
}

Uint16 VerifyUnitAdjustEeprom(void)
{
    Uint16 result = 0;
	Uint16 i,item;
	sRatioBias *pRatiobias;

    if((sUintEepromAdjustPara.Magic != UNIT_ADJUST_PARAM_DEFAULT_HEAD) || (sUintEepromAdjustPara.Len != sizeof(sUnitEEPROMAdjust)))
    {
    	ClearUnitAdjustParamToDefault();
		result = 1;
    }
	else
	{
		item = sizeof(sUnitADJUSTFACTOR)/sizeof(sRatioBias);
		pRatiobias = sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor;
//		for(j=0;j<2;j++)
		{
			for(i=0;i<item;i++)
			{
				if((pRatiobias->Ratio == 0) || (pRatiobias->Ratio >= 20000))
				{
					pRatiobias->Ratio = 10000;
					pRatiobias->Bias = 0;
				}
				pRatiobias++;
			}
		}
	}
    return result;
}

Uint16 VerifyModuleEeprom(void)
{
    Uint16 result = 0;
	Uint16 *pParamDst = (Uint16 *)&sModuleEepromPara;
	Uint16 *pParamSrc = (Uint16 *)&sModuleEepromParaDef;

    if((sModuleEepromPara.Magic != sModuleEepromParaDef.Magic) || (sModuleEepromPara.Len > sizeof(sModuleEEPROM)))
    {
    	ClearModuleParamToDefault();
		result = 1;
    }
	else
	{
		if(sModuleEepromPara.Len < sizeof(sModuleEEPROM))
		{
			memcpy(pParamDst+sModuleEepromPara.Len,pParamSrc+sModuleEepromPara.Len,sizeof(sModuleEEPROM)-sModuleEepromPara.Len);
			sModuleEepromPara.Len = sizeof(sModuleEEPROM);
			WriteEepromDataLong(EEPROM_PARA_START,(Uint16*)&sModuleEepromPara,sizeof(sModuleEEPROM));
			result = 1;
		}
	}
    return result;
}


void ClearUnitParamToDefault(void)
{
	AddressCheck();
	sUintEepromParaDef.Len = sizeof(sUnitEEPROM);
	sUintEepromParaDef.BoardAddr = Get485Addr();
    WriteEepromDataLong(EEPROM_PARA_START,(Uint16*)&sUintEepromParaDef,sizeof(sUnitEEPROM));
	memcpy(&sUintEepromPara,&sUintEepromParaDef,sizeof(sUnitEEPROM));
}

void ClearUnitAdjustParamToDefault(void)
{
	uint16_t i,item;
	sRatioBias *pRatiobias;
	
	sUintEepromAdjustPara.Magic = UNIT_ADJUST_PARAM_DEFAULT_HEAD;
	sUintEepromAdjustPara.Len = sizeof(sUnitEEPROMAdjust);
	pRatiobias = sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor;
	
	item = sizeof(sUnitEEPROMAdjust)/sizeof(sRatioBias);
//	for(j=0;j<2;j++)
	{
		for(i=0;i<item;i++)
		{
			pRatiobias->Ratio = 10000;
			pRatiobias->Bias = 0;
			
			pRatiobias++;
		}
	}
    WriteEepromDataLong(EEPROM_ADJUST_PARA_START,(Uint16*)&sUintEepromAdjustPara,sizeof(sUnitEEPROMAdjust));
}

void ClearModuleParamToDefault(void)
{
	AddressCheck();
	sModuleEepromParaDef.Len = sizeof(sModuleEEPROM);
	sModuleEepromParaDef.BoardAddr = Get485Addr();
    WriteEepromDataLong(EEPROM_PARA_START,(Uint16*)&sModuleEepromParaDef,sizeof(sModuleEEPROM));
	memcpy(&sModuleEepromPara,&sModuleEepromParaDef,sizeof(sModuleEEPROM));
}


/*******************************************************************************
Function name:	UserWriteEeprom
Description:  	Save EEPROM data interface function
Calls:
Called By:
Parameters: 	eepromAddr:eeprom ADDR, pReg:register memory addr,len:register length(word)
Return: 		void
*******************************************************************************/
void UserWriteEeprom(Uint16 eepromAddr,Uint16* pReg,Uint16 len)
{
	//EepromWriteTrue(GET_ROM_ADDR(MdbAddr),len,pReg);
	EepromCmdSendBySciMdb.bit.Cmd = EEPROM_CMD_WRITE;
	EepromCmdSendBySciMdb.bit.EepromAddr=eepromAddr;
    if(len == 0)
    {
        len = 1;
    }
	EepromCmdSendBySciMdb.bit.length = len;
	EepromCmdSendBySciMdb.bit.pram = pReg;
	sQDataIn(&EepromCmdeQueue,&EepromCmdSendBySciMdb.all[0],cQCoverLast);
}

/*******************************************************************************
Function name:	UserClearModuleParamToDefault
Description:  	Save module default EEPROM data interface function
Calls:
Called By:
Parameters: 	void
Return: 		void
*******************************************************************************/
void UserClearModuleParamToDefault(void)
{
	sModuleEepromParaDef.Len = sizeof(sModuleEEPROM);
	if(sModuleEepromPara.AddrMode == 0)
	{
		AddressCheck();
		sModuleEepromParaDef.BoardAddr = Get485Addr();
	}
	memcpy(&sModuleEepromPara,&sModuleEepromParaDef,sizeof(sModuleEEPROM));
	
	EepromCmdSendBySciMdb.bit.Cmd = EEPROM_CMD_WRITE;
	EepromCmdSendBySciMdb.bit.EepromAddr=0;

	EepromCmdSendBySciMdb.bit.length = sModuleEepromParaDef.Len;
	EepromCmdSendBySciMdb.bit.pram = (Uint16*)&sModuleEepromPara;
	sQDataIn(&EepromCmdeQueue,&EepromCmdSendBySciMdb.all[0],cQCoverLast);
}

/*******************************************************************************
Function name:	UserClearUnitParamToDefault
Description:  	Save unit default EEPROM data interface function
Calls:
Called By:
Parameters: 	void
Return: 		void
*******************************************************************************/
void UserClearUnitParamToDefault(void)
{
	sUintEepromParaDef.Len = sizeof(sUnitEEPROM);
	if(sUintEepromPara.AddrMode == 0)
	{
		AddressCheck();
		sUintEepromParaDef.BoardAddr = Get485Addr();
	}
	memcpy(&sUintEepromPara,&sUintEepromParaDef,sizeof(sUnitEEPROM));
	
	EepromCmdSendBySciMdb.bit.Cmd = EEPROM_CMD_WRITE;
	EepromCmdSendBySciMdb.bit.EepromAddr=0;

	EepromCmdSendBySciMdb.bit.length = sUintEepromParaDef.Len;
	EepromCmdSendBySciMdb.bit.pram = (Uint16*)&sUintEepromPara;
	sQDataIn(&EepromCmdeQueue,&EepromCmdSendBySciMdb.all[0],cQCoverLast);
}

void UserClearUnitAdjustParamToDefault(void)
{
    uint16_t i,item;
    sRatioBias *pRatiobias;

    sUintEepromAdjustPara.Magic = UNIT_ADJUST_PARAM_DEFAULT_HEAD;
    sUintEepromAdjustPara.Len = sizeof(sUnitEEPROMAdjust);
    pRatiobias = sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor;

    item = sizeof(sUnitADJUSTFACTOR)/sizeof(sRatioBias);

    for(i=0;i<item;i++)
    {
        pRatiobias->Ratio = 10000;
        pRatiobias->Bias = 0;

        pRatiobias++;
    }
    

    EepromCmdSendBySciMdb.bit.Cmd = EEPROM_CMD_WRITE;
    EepromCmdSendBySciMdb.bit.EepromAddr=EEPROM_ADJUST_PARA_START;

    EepromCmdSendBySciMdb.bit.length = sUintEepromAdjustPara.Len;
    EepromCmdSendBySciMdb.bit.pram = (Uint16*)&sUintEepromAdjustPara;
    sQDataIn(&EepromCmdeQueue,&EepromCmdSendBySciMdb.all[0],cQCoverLast);
}
