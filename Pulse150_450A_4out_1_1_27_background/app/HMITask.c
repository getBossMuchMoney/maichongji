/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : HMITask.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define HMITask_GLOBALS  1
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
#include "EEpromTask.h"
#include "HMITask.h"
#include "OsConfig.h"
#include "sciframe.h"
#include "scimodbusreg.h"
#include "Common.h"
#include "Constant.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Version.h"
#include "eeprommodule.h"
#include "Initial.h"
#include "Rs485RegDef.h"
#include "SyncProc.h"

/********************************************************************************
*const define                               *
********************************************************************************/
#define HMISCI             (SciType)1
#define INTERNALSCI        (SciType)0
#define REMOTESCI		   (SciType)2

#define HMIISHOST          0
#define INTERNALISHOST     1
#define REMOTEISHOST	   0

#define FIFOSIZE           20

/********************************************************************************
* Variable declaration                              *
********************************************************************************/
uint16_t hmiSendData[60] = {0};
//uint32_t rs485timeout[4] = {0};
uint32_t rs485statussendcnt[4] = {0};
uint32_t rs485statusrecvcnt[4] = {0};
uint32_t disptestbuf[4][2] = {0};
extern uint32_t recvErrCnt[4];
extern uint32_t recvTimeoutCnt[4];
typedef struct CMD_FIFO
{
  uint16_t inidx;
  uint16_t outidx;
  uint16_t used;
  Cmdstruct fifo[FIFOSIZE];
} CmdFifo;

typedef struct SLAVE_STATUS
{
  uint16_t errcnt;
  uint16_t sendsuccess;
  uint16_t losedcnt;
} SlaveStatus;

typedef struct INTERNAL_SCI
{
  CmdFifo InternalCmdFifo;
  SlaveStatus slavestatus[SLAVEMAX];
  uint16_t forallslave;
  uint16_t loopaddress;
  uint16_t loopcount;
}InternalSciInfor;

InternalSciInfor internalsci;
Cmdstruct Scisend[SCI_MAX];

typedef struct 
{
	uint16_t rwctrl;		//0:write,1:read
	uint16_t unitAddr;
	void *writeAddr;
	uint16_t readregAddr;
	uint16_t regnum;
}sHostLoopReg;


sHostLoopReg interRegRead[] = {
//								{0,	BADDRESS,	&Ctrldata1,	0,		sizeof(sModuleCtrlregion)},
								{1,	8,			NULL,		1280,	sizeof(sUnitStatusRegion)},
								{1,	9,			NULL,		2304,	sizeof(sUnitStatusRegion)},
								{1,	10,			NULL,		3328,	sizeof(sUnitStatusRegion)},
								{1,	11,			NULL,		4352,	sizeof(sUnitStatusRegion)}};

#define INTERNAL_FIFO                  internalsci.InternalCmdFifo
#define INTERNAL_SSTATUS               internalsci.slavestatus

#define CTRL_ADDRESS                  (uint16_t)0
#define CTRL_UNIT1_ADDRESS            (uint16_t)440

#define ERRORCNTMAX                   (uint16_t)5

typedef struct{
	uint16_t used;
	uint32_t Time;
}sStatusUpdate;
sStatusUpdate sStatusUpdateTime[4];


/********************************************************************************
* function declaration                              *
********************************************************************************/
static void datacopy(uint16_t* sou,uint16_t* dir,uint16_t len)
{
  uint16_t *ptrSou;
  uint16_t *ptrDir;
  uint16_t templen;
  uint16_t temp;
  templen = len;
  ptrSou = sou;
  ptrDir = dir;
  while(templen)
  {
    temp = *ptrSou;
    ptrSou++;
    temp <<= 8;
    temp |= *ptrSou;
    ptrSou++;
    templen--;
    *ptrDir = temp;
    ptrDir++;
  }
}

void addinternalCmdFifo(Cmdstruct scmd)
{
  uint16_t in;
  in = internalsci.InternalCmdFifo.inidx;

  internalsci.InternalCmdFifo.fifo[in] = scmd;
  in++;
  if(in == FIFOSIZE)
  {
    in = 0;
  }
  internalsci.InternalCmdFifo.inidx = in;
  internalsci.InternalCmdFifo.used ++;
}
//因模式改变而清空相关设置参数
void CfgParaClr()
{
	int i;
	for(i=0;i<20;i++)
	{
		Ctrldata1.SectASet[i].Current = 0;
		Ctrldata1.SectASet[i].PulseWide = 0;
		

		sUnitCtrlReg[Get485Addr()-SlaveStartAddr].SectASet[i].Current = 0;
		sUnitCtrlReg[Get485Addr()-SlaveStartAddr].SectASet[i].PulseWide = 0;
		
	}
	
	for(i=0;i<22;i++)
	{
		SideAControl.step[i].reference = 0;
		SideAControl.step[i].timecountset = 0;
	}

	Ctrldata1.PhaseShift[0] = 0;
	Ctrldata1.PhaseShift[1] = 0;
	Ctrldata1.PhaseShift[2] = 0;
	Ctrldata1.PhaseShift[3] = 0;
	Ctrldata1.SectNum = 0;
	Ctrldata1.UnitCtrl.all = 0;
	Ctrldata1.SyncPhase = 0;
	SyncPhase = 0;

	UnitPara.SumOfExtSyncPoint = 0;
	UnitPara.SlaveExtSyncStartPoint = 0;
	ModulePara.SumOfExtSyncPoint = 0;
	ModulePara.SlaveExtSyncStartPoint = 0;
	UnitPara.ExtSynclossCntSetting = 0;
	ModulePara.ExtSynclossCntSetting = 0;
}

BaseType_t HmiWriteParse(uint16_t cmd, uint16_t regaddress, uint16_t len, uint16_t *pdata)
{
	BaseType_t  parse = pdFAIL;
	eHMIRegion region = (eHMIRegion)(regaddress/HMI_REGION_LEN);
	uint16_t *ptrDir;
	Cmdstruct scmd;
//	int16_t tem16;
//	uint32_t tem32;
	uint16_t offset = 0;
	uint16_t unitnum = 0;
	uint16_t boardtype;
	CtrlNum1 ctrl1;
	
	switch(region)
	{
		case eHMIMODULECTRL:
			ptrDir = (uint16_t *)&Ctrldata1;
	     	ptrDir += regaddress;
			
			switch(regaddress)
			{
				case eModuleCtrlArea_Ctrl:
				{
					datacopy(pdata,(uint16_t*)&ctrl1,1);
					if(ctrl1.bit.LocalRemote == 0)
					{
						Ctrldata1.Ctrl1.bit.LocalRemote = 0;
					}
					else
					{
						Ctrldata1.Ctrl1.bit.LocalRemote = 1;
						if(sUnitCtrlReg[0].Ctrl1.bit.OnOff || sUnitCtrlReg[1].Ctrl1.bit.OnOff
						|| sUnitCtrlReg[2].Ctrl1.bit.OnOff || sUnitCtrlReg[3].Ctrl1.bit.OnOff)
						{
							sUnitSingleStartCtrl();
						}
						else
						{
							datacopy(pdata,ptrDir,len);
							sModuleStartCtrl();
						}
					}
					
					parse = pdPASS;
//					memcpy(hmiSendData,ptrDir,len);
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = ptrDir;
					scmd.resend = pdTRUE;
					scmd.address = ALLSLAVE;//BADDRESS;//ALLSLAVE;
					addinternalCmdFifo(scmd);

				}
				break;
				case eModuleCtrlArea_Curr1:case eModuleCtrlArea_Curr2:case eModuleCtrlArea_Curr3:case eModuleCtrlArea_Curr4:case eModuleCtrlArea_Curr5:
				case eModuleCtrlArea_Curr6:case eModuleCtrlArea_Curr7:case eModuleCtrlArea_Curr8:case eModuleCtrlArea_Curr9:case eModuleCtrlArea_Curr10:
				case eModuleCtrlArea_Curr11:case eModuleCtrlArea_Curr12:case eModuleCtrlArea_Curr13:case eModuleCtrlArea_Curr14:case eModuleCtrlArea_Curr15:
				case eModuleCtrlArea_Curr16:case eModuleCtrlArea_Curr17:case eModuleCtrlArea_Curr18:case eModuleCtrlArea_Curr19:case eModuleCtrlArea_Curr20:
					datacopy(pdata,ptrDir,len);
					
					parse = pdPASS;

//					hmiSendData[0] = ptrDir[0];
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = ptrDir;
					scmd.resend = pdTRUE;
					scmd.address = ALLSLAVE;//BADDRESS;//ALLSLAVE;
					addinternalCmdFifo(scmd);
					break;
				case eModuleCtrlArea_Pulse1:case eModuleCtrlArea_Pulse2:case eModuleCtrlArea_Pulse3:case eModuleCtrlArea_Pulse4:case eModuleCtrlArea_Pulse5:
				case eModuleCtrlArea_Pulse6:case eModuleCtrlArea_Pulse7:case eModuleCtrlArea_Pulse8:case eModuleCtrlArea_Pulse9:case eModuleCtrlArea_Pulse10:
				case eModuleCtrlArea_Pulse11:case eModuleCtrlArea_Pulse12:case eModuleCtrlArea_Pulse13:case eModuleCtrlArea_Pulse14:case eModuleCtrlArea_Pulse15:
				case eModuleCtrlArea_Pulse16:case eModuleCtrlArea_Pulse17:case eModuleCtrlArea_Pulse18:case eModuleCtrlArea_Pulse19:case eModuleCtrlArea_Pulse20:
					datacopy(pdata,ptrDir,len);
					
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 1) && (sUnitCtrlReg[unitnum].Ctrl1.bit.OnOff == 0))
					{
						sSetPulseWideFromModule();
						
						sSyncPhaseShiftSet((SyncPhase%360)*ModulePara.SumOfExtSyncPoint/4/360);
					}
					
					parse = pdPASS;

//					hmiSendData[0] = ptrDir[0];
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = ptrDir;
					scmd.resend = pdTRUE;
					scmd.address = ALLSLAVE;//BADDRESS;//ALLSLAVE;
					addinternalCmdFifo(scmd);
					break;
				case eModuleCtrlArea_PhaseShift1:case eModuleCtrlArea_PhaseShift2:
				case eModuleCtrlArea_PhaseShift3:case eModuleCtrlArea_PhaseShift4:
					datacopy(pdata,ptrDir,len);
					if(ptrDir[0] > 180)
						ptrDir[0] = 180;
					parse = pdPASS;

//					hmiSendData[0] = ptrDir[0];
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = ptrDir;
					scmd.resend = pdTRUE;
					scmd.address = SlaveStartAddr+(regaddress-eModuleCtrlArea_PhaseShift1);
					addinternalCmdFifo(scmd);
					break;
				case eModuleCtrlArea_SectNum:	//电流段数
				case eModuleCtrlArea_UnitCtrl:	//子卡控制
					parse = pdPASS;		//必须回复，否则proface屏会一直报警

					if(Ctrldata1.Ctrl1.bit.OnOff)
						break;
					datacopy(pdata,ptrDir,len);

//					hmiSendData[0] = ptrDir[0];
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = ptrDir;
					scmd.resend = pdTRUE;
					scmd.address = ALLSLAVE;//BADDRESS;//ALLSLAVE;
					addinternalCmdFifo(scmd);
					break;
				case eModuleCtrlArea_Ctrl2:
					if(Ctrldata1.Ctrl1.bit.OnOff)
						break;
					if(sUnitCtrlReg[0].Ctrl1.bit.OnOff || sUnitCtrlReg[1].Ctrl1.bit.OnOff
					|| sUnitCtrlReg[2].Ctrl1.bit.OnOff || sUnitCtrlReg[3].Ctrl1.bit.OnOff)
						break;
					if(Ctrldata1.Ctrl1.bit.LocalRemote == 0)
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
//					memcpy(hmiSendData,ptrDir,1);
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = ptrDir;
					scmd.resend = pdTRUE;
					scmd.address = ALLSLAVE;//BADDRESS;//ALLSLAVE;
					addinternalCmdFifo(scmd);
					parse = pdPASS;
				break;
				
				case eModuleCtrlArea_LoadEepromDef:
					parse = pdPASS;
					UserClearModuleParamToDefault();
					sModuleParamReg = *(sModuleParamRegion *)&sModuleEepromPara.BoardType;
					break;
				case eModuleCtrlArea_SyncPhase:
					datacopy(pdata,ptrDir,len);
					parse = pdPASS;

//					hmiSendData[0] = ptrDir[0];
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = ptrDir;
					scmd.resend = pdTRUE;
					scmd.address = ALLSLAVE;//BADDRESS;//ALLSLAVE;
					addinternalCmdFifo(scmd);
					break;
				default:
					if(regaddress > sizeof(sModuleCtrlregion))
						break;
					datacopy(pdata,ptrDir,len);
					parse = pdPASS;

//					hmiSendData[0] = ptrDir[0];
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = ptrDir;
					scmd.resend = pdTRUE;
					scmd.address = ALLSLAVE;//BADDRESS;//ALLSLAVE;
					addinternalCmdFifo(scmd);
					break;
			}
			break;
		case eHMIMODULEPARA:
			
			ptrDir = (uint16_t *)&sModuleParamReg;
	     	offset = regaddress - HMI_REG_MODULE_PARA;
			ptrDir += offset;
			switch(offset)
			{
				case eModuleParaOffset_BoardType:
					boardtype = sModuleParamReg.BoardType;
					datacopy(pdata,ptrDir,len);
					if(boardtype != sModuleParamReg.BoardType)
					{
						if(sModuleParamReg.BoardType == 0)
						{
							UserClearModuleParamToDefault();
							sModuleParamReg = *(sModuleParamRegion *)&sModuleEepromPara.BoardType;
						}
						else
						{
							UserClearUnitParamToDefault();
							sModuleParamReg.BoardType = sUintEepromPara.BoardType;
							sModuleParamReg.BoardAddr = sUintEepromPara.BoardAddr;
							sModuleParamReg.WorkMode = sUintEepromPara.WorkMode;
							sModuleParamReg.UnitAddrMin = sUintEepromPara.UnitAddrMin;
							sModuleParamReg.UnitAddrMax = sUintEepromPara.UnitAddrMax;
						}
					}
					parse = pdPASS;
					break;
				case eModuleParaOffset_BoardAddr:
					datacopy(pdata,ptrDir,len);
				    sModuleEepromPara.BoardAddr = *ptrDir;
				
					RemoteCommAddr = sModuleEepromPara.BoardAddr;
					setSciAddr(REMOTESCI,RemoteCommAddr);

				    UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,(Uint16*)((uint32_t)&sModuleEepromPara + EEPROM_PARA_OFFSET + offset),1);
				    parse = pdPASS;
					break;
				case eModuleParaOffset_SynDelay:
				    datacopy(pdata,ptrDir,len);
				    sModuleEepromPara.SynDelay = *ptrDir;
				    sSyncDelaySet(sModuleEepromPara.SynDelay);
				    UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,(Uint16*)((uint32_t)&sModuleEepromPara + EEPROM_PARA_OFFSET + offset),1);
				    parse = pdPASS;
				    break;
				case eModuleParaOffset_SynId:
				    datacopy(pdata,ptrDir,len);
				    sModuleEepromPara.SynId = *ptrDir;
				    sSyncIdSet(sModuleParamReg.SynId);
				    UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,(Uint16*)((uint32_t)&sModuleEepromPara + EEPROM_PARA_OFFSET + offset),1);
				    parse = pdPASS;
				    break;
				default:
					datacopy(pdata,ptrDir,len);
					if(sModuleParamReg.BoardType == 0)
					{
						*(uint16_t*)((uint32_t)&sModuleEepromPara + EEPROM_PARA_OFFSET + offset) = *ptrDir;
						
						UserWriteEeprom((EEPROM_PARA_OFFSET+offset)*2,(Uint16*)((uint32_t)&sModuleEepromPara + EEPROM_PARA_OFFSET + offset),1);
					}
					else
					{
						sUintEepromPara.BoardType = sModuleParamReg.BoardType;
						sUintEepromPara.BoardAddr = sModuleParamReg.BoardAddr;
						sUintEepromPara.WorkMode = sModuleParamReg.WorkMode;
						offset = &sUintEepromPara.BoardType - (Uint16*)&sUintEepromPara;
						UserWriteEeprom(offset*2,(Uint16*)&sUintEepromPara.BoardType,3);
						sUintEepromPara.UnitAddrMin = sModuleParamReg.UnitAddrMin;
						sUintEepromPara.UnitAddrMax = sModuleParamReg.UnitAddrMax;
						offset = &sUintEepromPara.UnitAddrMin - (Uint16*)&sUintEepromPara;
						UserWriteEeprom(offset*2,(Uint16*)&sUintEepromPara.UnitAddrMin,2);
					}
					break;
			}
			
			break;
		case eHMIUNIT1CTRL:
		case eHMIUNIT2CTRL:
		case eHMIUNIT3CTRL:
		case eHMIUNIT4CTRL:
			parse = pdPASS;
			unitnum = (region-eHMIUNIT1CTRL)>>2;
			ptrDir = (uint16_t *)&sUnitCtrlReg[unitnum];
			offset = regaddress-(region*HMI_REGION_LEN);
	     	ptrDir += offset;
			if(offset > sizeof(sUnitCtrlRegion))
				break;

			switch(offset)
			{
				case eUnintCtrlOffset_Ctrl:					
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 0) || (Ctrldata1.Ctrl1.bit.OnOff == 1))
					{
						break;
					}
					datacopy(pdata,ptrDir,len);
					sUnitSingleStartCtrl();
						
					memcpy(hmiSendData,ptrDir,len);
					
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = hmiSendData;
					scmd.resend = pdTRUE;
					scmd.address = SlaveStartAddr+(region-eHMIUNIT1CTRL)/4;
					addinternalCmdFifo(scmd);

					scmd.used = pdPASS;		//写完后立马读取更新状态
					scmd.cmd = CMD_READ;
					scmd.regaddress = regaddress;
					scmd.len = len;
					scmd.pdata = hmiSendData;
					scmd.resend = pdTRUE;
					scmd.address = SlaveStartAddr+(region-eHMIUNIT1CTRL)/4;
					addinternalCmdFifo(scmd);
					break;
				case eUnintCtrlOffset_Curr1:case eUnintCtrlOffset_Curr2:case eUnintCtrlOffset_Curr3:case eUnintCtrlOffset_Curr4:case eUnintCtrlOffset_Curr5:
				case eUnintCtrlOffset_Curr6:case eUnintCtrlOffset_Curr7:case eUnintCtrlOffset_Curr8:case eUnintCtrlOffset_Curr9:case eUnintCtrlOffset_Curr10:
				case eUnintCtrlOffset_Curr11:case eUnintCtrlOffset_Curr12:case eUnintCtrlOffset_Curr13:case eUnintCtrlOffset_Curr14:case eUnintCtrlOffset_Curr15:
				case eUnintCtrlOffset_Curr16:case eUnintCtrlOffset_Curr17:case eUnintCtrlOffset_Curr18:case eUnintCtrlOffset_Curr19:case eUnintCtrlOffset_Curr20:
					datacopy(pdata,ptrDir,len);
					
					parse = pdPASS;

					hmiSendData[0] = ptrDir[0];
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = hmiSendData;
					scmd.resend = pdTRUE;
					scmd.address = SlaveStartAddr+unitnum;
					addinternalCmdFifo(scmd);
					break;
				case eUnintCtrlOffset_Pulse1:case eUnintCtrlOffset_Pulse2:case eUnintCtrlOffset_Pulse3:case eUnintCtrlOffset_Pulse4:case eUnintCtrlOffset_Pulse5:
				case eUnintCtrlOffset_Pulse6:case eUnintCtrlOffset_Pulse7:case eUnintCtrlOffset_Pulse8:case eUnintCtrlOffset_Pulse9:case eUnintCtrlOffset_Pulse10:
				case eUnintCtrlOffset_Pulse11:case eUnintCtrlOffset_Pulse12:case eUnintCtrlOffset_Pulse13:case eUnintCtrlOffset_Pulse14:case eUnintCtrlOffset_Pulse15:
				case eUnintCtrlOffset_Pulse16:case eUnintCtrlOffset_Pulse17:case eUnintCtrlOffset_Pulse18:case eUnintCtrlOffset_Pulse19:case eUnintCtrlOffset_Pulse20:
					datacopy(pdata,ptrDir,len);
					if((Ctrldata1.Ctrl1.bit.LocalRemote == 1) && (Ctrldata1.Ctrl1.bit.OnOff == 0))
					{
						sSetPulseWideFromUnit();
					}
					
					parse = pdPASS;

					hmiSendData[0] = ptrDir[0];
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = hmiSendData;
					scmd.resend = pdTRUE;
					scmd.address = SlaveStartAddr+unitnum;
					addinternalCmdFifo(scmd);
					break;
				case eUnintCtrlOffset_SectNum0:	//电流段数
					datacopy(pdata,ptrDir,len);
					hmiSendData[0] = ptrDir[0];
					
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = hmiSendData;
					scmd.resend = pdTRUE;
					scmd.address = SlaveStartAddr+unitnum;
					addinternalCmdFifo(scmd);
					break;
				case eUnintCtrlOffset_LoadEepromDef:
					datacopy(pdata,ptrDir,len);
					hmiSendData[0] = ptrDir[0];
					if(hmiSendData[0] == 1)
					{
						scmd.used = pdPASS;
						scmd.cmd = cmd;
						scmd.len = len;
						scmd.regaddress = regaddress;
						scmd.pdata = hmiSendData;
						scmd.resend = pdTRUE;
						scmd.address = SlaveStartAddr+unitnum;
						addinternalCmdFifo(scmd);
					}
					else
					{
						hmiSendData[0] = 0;
					}
					break;
				default:
					datacopy(pdata,ptrDir,len);
					hmiSendData[0] = ptrDir[0];
					
					scmd.used = pdPASS;
					scmd.cmd = cmd;
					scmd.len = len;
					scmd.regaddress = regaddress;
					scmd.pdata = hmiSendData;
					scmd.resend = pdTRUE;
					scmd.address = SlaveStartAddr+unitnum;
					addinternalCmdFifo(scmd);
					break;
			}
			break;
		case eHMIUNIT1PARA:
		case eHMIUNIT2PARA:
		case eHMIUNIT3PARA:
		case eHMIUNIT4PARA:
			parse = pdPASS;
			unitnum = (region-eHMIUNIT1PARA)/4;

			datacopy(pdata,hmiSendData,len);

			scmd.used = pdPASS;
			scmd.cmd = cmd;
			scmd.len = len;
			scmd.regaddress = regaddress;
			scmd.pdata = hmiSendData;
			scmd.resend = pdTRUE;
			scmd.address = SlaveStartAddr+unitnum;
			addinternalCmdFifo(scmd);
			break;
		default:

			break;
	}
	Scisend[HMISCI].cmd = cmd;
	Scisend[HMISCI].len = len;
	Scisend[HMISCI].regaddress = regaddress;
	Scisend[HMISCI].pdata = ptrDir;
	return parse;
}

BaseType_t HmiReadParse(uint16_t regaddress, uint16_t len)
{
	BaseType_t  parse = pdFAIL;
	eHMIRegion region = (eHMIRegion)((regaddress+len-1)/HMI_REGION_LEN);
	Cmdstruct scmd;
	uint16_t *ptrDir;
	uint16_t i,warning;
	uint16_t offset;
	uint16_t temp;
//	if(0x2000 == regaddress)
//	{
//		parse = pdPASS;
//		hmiSendData[0] = ulTaskGetIdleRunTimePercent();
//		temp = ulTaskGetIdleRunTimeCounter();
//		hmiSendData[1] = (uint16_t)(temp>>16);
//		hmiSendData[2] = (uint16_t)(temp);
//		ptrDir = (uint16_t *)&hmiSendData;
//	}
	switch(region)
	{
		case eHMIMODULECTRL:
//			if(len>20)
//				break;
			parse = pdPASS;
			ptrDir = (uint16_t *)&Ctrldata1;
			ptrDir += regaddress;
			break;
		case eHMIMODULESTATUS:
			parse = pdPASS;
			sModuleStatusReg.workStatus.all = Ctrldata1.Ctrl1.all;
			sModuleStatusReg.workStatus.bit.Online = 1;
			if(sSyncRoleGet() == cSynRole_Slave)
				sModuleStatusReg.workStatus.bit.SynRole = 0;
			else
				sModuleStatusReg.workStatus.bit.SynRole = 1;

			if(scigroup[SCI_COM].recTimeout)
			{
				sModuleStatusReg.faultInfo.bit.remoteCommErr = 1;
			}
			else
			{
				sModuleStatusReg.faultInfo.bit.remoteCommErr = 0;
			}

			sModuleStatusReg.ver[0] = APP_BVER;
			sModuleStatusReg.ver[1] = APP_MVER;
			sModuleStatusReg.ver[2] = APP_SVER;
			
			if(sUintEepromPara.Moduletype == 0)
				sModuleStatusReg.Machine = 5;
			else if(sUintEepromPara.Moduletype == 1)
				sModuleStatusReg.Machine = 1;
			else if(sUintEepromPara.Moduletype == 2)
				sModuleStatusReg.Machine = 3;
			else if(sUintEepromPara.Moduletype == 3)
				sModuleStatusReg.Machine = 4;
			else if(sUintEepromPara.Moduletype == 4)
				sModuleStatusReg.Machine = 6;
//			sModuleStatusReg.Machine = 1;
			ptrDir = (uint16_t *)&sModuleStatusReg;
			ptrDir += regaddress-HMI_REG_MODULE_STATUS;


			if(((regaddress-HMI_REG_MODULE_STATUS) <= eModuleStatusArea_MachineType) && 
			((regaddress+len-HMI_REG_MODULE_STATUS) > eModuleStatusArea_MachineType))
			{
				regaddress = (eHMIUNIT1PARA*HMI_REGION_LEN)+((uint32_t)&sUintEepromPara.Moduletype - (uint32_t)&sUintEepromPara.OutPutVolt);
				
				scmd.used = pdPASS;
				scmd.cmd = CMD_READ;
				scmd.regaddress = regaddress;
				scmd.len = 1;
				scmd.pdata = ptrDir;
				scmd.resend = pdTRUE;
				scmd.address = SlaveStartAddr;
				addinternalCmdFifo(scmd);
			}
			break;
		case eHMIMODULEREMOTE:
			parse = pdPASS;
			ptrDir = (uint16_t *)&sSynRemoteCtrlReg;
			ptrDir += regaddress - HMI_REG_MODULE_REMOTE;
			break;
		case eHMIMODULEPARA:
			parse = pdPASS;
			ptrDir = (uint16_t *)&sModuleParamReg;
			ptrDir += regaddress-HMI_REG_MODULE_PARA;
			break;
		case eHMIUNIT1CTRL:
		case eHMIUNIT2CTRL:
		case eHMIUNIT3CTRL:
		case eHMIUNIT4CTRL:
			parse = pdPASS;
			ptrDir = (uint16_t *)&sUnitCtrlReg[(region-eHMIUNIT1CTRL)/4];
			ptrDir += regaddress-(region*HMI_REGION_LEN);

			scmd.used = pdPASS;
			scmd.cmd = CMD_READ;
			scmd.regaddress = regaddress;
			scmd.len = len;
			scmd.pdata = ptrDir;
			scmd.resend = pdTRUE;
			scmd.address = SlaveStartAddr+(region-eHMIUNIT1CTRL)/4;
			addinternalCmdFifo(scmd);
			break;
		case eHMIUNIT1STATUS:
		case eHMIUNIT2STATUS:
		case eHMIUNIT3STATUS:
		case eHMIUNIT4STATUS:
			parse = pdPASS;
			warning = 0;
			for(i=0; i<4; i++)
			{
				temp = ~((1<<eUnitWarnInfo1_BusOver) 		| (1<<eUnitWarnInfo1_BusUnder) 
					  | (1<<eUnitWarnInfo1_AmbientNTCerr) 	| (1<<eUnitWarnInfo1_SinkNTCerr)
					  | (1<<eUnitWarnInfo1_SinkNTCOT) 		| (1<<eUnitWarnInfo1_PWMSyncLoss)
					  | (1<<eUnitWarnInfo1_PWMLockLoss) 	| (1<<eUnitWarnInfo1_LineSyncLoss)
					  | (1<<eUnitWarnInfo1_LineLockLoss));
				if(sUnitStatusReg[i].warnInfo1.all & temp)
				{
					warning = 1;
					break;
				}
				temp = ~((1<<eUnitFaultInfo1_BusOver) 		| (1<<eUnitFaultInfo1_BusUnder) 
					  | (1<<eUnitFaultInfo1_HwExtLoadOC) 	| (1<<eUnitFaultInfo1_Hw24Vabnormal)
					  | (1<<eUnitFaultInfo1_PWMSyncLoss) 	| (1<<eUnitFaultInfo1_PWMLockLoss)
					  | (1<<eUnitFaultInfo1_LineSyncLoss) 	| (1<<eUnitFaultInfo1_LineLockLoss));
				if(sUnitStatusReg[i].faultInfo1.all & temp)
				{
					warning = 1;
					break;
				}

				temp = ~((1<<eUnitFaultInfo2_communicationFault) | (1<<eUnitFaultInfo2_EepromRead) | (1<<eUnitFaultInfo1_EepromWrite));
				if(sUnitStatusReg[i].faultInfo2.all & temp)
				{
					warning = 1;
					break;
				}
			}
			if((sModuleStatusReg.workStatus.bit.LocalRemote == 0) && (sModuleStatusReg.faultInfo.bit.remoteCommErr))
				warning = 1;
			
			if(warning)
			{
				DO1_ON();
			}
			else
			{
				DO1_OFF();
			}
			ptrDir = (uint16_t *)&sUnitStatusReg[(region-eHMIUNIT1STATUS)/4];
			ptrDir += regaddress-(region*HMI_REGION_LEN);
			if(sStatusUpdateTime[(region-eHMIUNIT1STATUS)/4].used == 0)
			{
				sStatusUpdateTime[(region-eHMIUNIT1STATUS)/4].Time = BackgroundGetTickCount();
				sStatusUpdateTime[(region-eHMIUNIT1STATUS)/4].used = 1;
			}
			
//			scmd.used = pdPASS;
//			scmd.cmd = CMD_READ;
//			scmd.regaddress = regaddress;
//			scmd.len = len;
//			scmd.pdata = ptrDir;
//			scmd.address = SlaveStartAddr+(region-eHMIUNIT1STATUS)/4;
//			addinternalCmdFifo(scmd);
			break;
		case eHMIUNIT1PARA:
		case eHMIUNIT2PARA:
		case eHMIUNIT3PARA:
		case eHMIUNIT4PARA:
		    parse = pdPASS;
            offset = regaddress-(region*HMI_REGION_LEN);

            if(offset <= eUintParaOffset_resd)
            {
                ptrDir = (uint16_t *)&sUintEepromPara.OutPutVolt;
                ptrDir += offset;
            }
            else if(offset <= eUintParaOffset_NegCmpDispCurrAdjustBias4)
            {
                ptrDir = (uint16_t *)sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor;
                ptrDir += offset - eUintParaOffset_PosCurrAdjustRatio1;
            }

			
			scmd.used = pdPASS;
			scmd.cmd = CMD_READ;
			scmd.regaddress = regaddress;
			scmd.len = len;
			scmd.pdata = ptrDir;
			scmd.resend = pdTRUE;
			scmd.address = SlaveStartAddr+(region-eHMIUNIT1PARA)/4;
			addinternalCmdFifo(scmd);
			break;
		default:
			parse = pdPASS;
			ptrDir = (uint16_t *)&Ctrldata1;
			ptrDir += regaddress;
			break;
	}
	
	Scisend[HMISCI].cmd = CMD_READ;
	Scisend[HMISCI].len = len;
	Scisend[HMISCI].regaddress = regaddress;
	Scisend[HMISCI].pdata = ptrDir;
	return parse;
}

void HmiSciRecvNormal(Cmdstruct *incmd,Cmdstruct *outcmd)
{
	BaseType_t  parse = pdFAIL;
	// incmd->used = pdFAIL;
	outcmd->used = pdFAIL;
	switch(incmd->cmd)
	{
		case CMD_WRITE:
			parse = HmiWriteParse(CMD_WRITE,incmd->regaddress,incmd->len,incmd->pdata);
		break;
		case CMD_WRITE_ONE:
			parse = HmiWriteParse(CMD_WRITE_ONE,incmd->regaddress,1,incmd->pdata);
		break;
		case CMD_READ:
			parse = HmiReadParse(incmd->regaddress,incmd->len);
		break;
	}
	if((pdFAIL != parse)&&(incmd->address != BADDRESS))
	{
		*outcmd = Scisend[HMISCI];
		outcmd->used = pdTRUE;
	}
}

void HmiSciRecvError(Cmdstruct *outcmd)
{

}

/********************************************************************************
* internal Sci                             *
********************************************************************************/
BaseType_t InternalSciWriteOneParse(uint16_t regaddress, uint16_t *pdata)
{
  BaseType_t  parse = pdFAIL;
  if((CMD_WRITE_ONE == Scisend[INTERNALSCI].cmd)&&(regaddress == Scisend[INTERNALSCI].regaddress))
  {
    parse = pdPASS;
  }
  return parse;
}

BaseType_t InternalSciWriteParse(uint16_t regaddress, uint16_t len)
{
  BaseType_t  parse = pdFAIL;
  if((CMD_WRITE == Scisend[INTERNALSCI].cmd)&&(regaddress == Scisend[INTERNALSCI].regaddress))
  {
    parse = pdPASS;
  }
  return parse;
}

BaseType_t InternalSciReadParse(uint16_t regaddress, uint16_t len, uint16_t *pdata)
{
	BaseType_t	parse = pdFAIL;
	eHMIRegion region = (eHMIRegion)((regaddress+len-1)/HMI_REGION_LEN);
	uint16_t *ptrDir;
	uint16_t offset,unitNum;

	
	switch(region)
	{
		case eHMIMODULECTRL:
		case eHMIMODULESTATUS:
		case eHMIMODULEREMOTE:
		case eHMIMODULEPARA:
			break;
		case eHMIUNIT1CTRL:
		case eHMIUNIT2CTRL:
		case eHMIUNIT3CTRL:
		case eHMIUNIT4CTRL:
			parse = pdPASS;
			ptrDir = (uint16_t *)&sUnitCtrlReg[(region-eHMIUNIT1CTRL)/4];
			ptrDir += regaddress-(region*HMI_REGION_LEN);
			datacopy(pdata,ptrDir,len);
			break;
		case eHMIUNIT1STATUS:
		case eHMIUNIT2STATUS:
		case eHMIUNIT3STATUS:
		case eHMIUNIT4STATUS:
			parse = pdPASS;
			unitNum = (region-eHMIUNIT1STATUS)/4;
			sStatusUpdateTime[unitNum].used = 0;
			UnitCommTimeout[(region-eHMIUNIT1STATUS)/4] = 0;
			if(UnitCommTimeout[0] || UnitCommTimeout[1] || UnitCommTimeout[2] || UnitCommTimeout[3])
				CommTimeoutProhibitSendSyncFlag = 1;
			else
				CommTimeoutProhibitSendSyncFlag = 0;
			ptrDir = (uint16_t *)&sUnitStatusReg[unitNum];
			ptrDir += regaddress-(region*HMI_REGION_LEN);

			datacopy(pdata,ptrDir,len);
			if(Ctrldata1.Ctrl1.bit.LocalRemote == 1)
			{
				sSynRemoteStatusUpdate(unitNum);
			}
			break;
		case eHMIUNIT1PARA:
		case eHMIUNIT2PARA:
		case eHMIUNIT3PARA:
		case eHMIUNIT4PARA:
		    parse = pdPASS;
            offset = regaddress-(region*HMI_REGION_LEN);

            if(offset <= eUintParaOffset_resd)
            {
                ptrDir = (uint16_t *)&sUintEepromPara.OutPutVolt;
                ptrDir += offset;
                datacopy(pdata,ptrDir,len);

                if(sUintEepromPara.Moduletype == 0)
                    sModuleStatusReg.Machine = 5;
                else if(sUintEepromPara.Moduletype == 1)
                    sModuleStatusReg.Machine = 1;
                else if(sUintEepromPara.Moduletype == 2)
                    sModuleStatusReg.Machine = 3;
                else if(sUintEepromPara.Moduletype == 3)
                    sModuleStatusReg.Machine = 4;
                else if(sUintEepromPara.Moduletype == 4)
                    sModuleStatusReg.Machine = 6;
            }
            else if(offset <= eUintParaOffset_NegCmpDispCurrAdjustBias4)
            {
                ptrDir = (uint16_t *)sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor;
                ptrDir += offset - eUintParaOffset_PosCurrAdjustRatio1;
                datacopy(pdata,ptrDir,len);
            }

			break;
		default:

		break;
	}

	return parse;
}
BaseType_t InternalSciRWMultParse(uint16_t address, uint16_t len, uint16_t *pdata)
{
	BaseType_t  parse = pdFAIL;
	uint16_t *ptrDir;

	parse = pdPASS;
	if(Ctrldata1.Ctrl1.bit.LocalRemote == 0)
	{
		ptrDir = (uint16_t *)&sSynRemoteStatusReg[address-SlaveStartAddr];
		datacopy(pdata,ptrDir,len);
	}
	return parse;
}

void InternalSciupdateSenddata(void)
{
  uint16_t temp = 0xff;
  uint16_t loopreg;
  if(internalsci.forallslave != pdFALSE)
  {
    if(Scisend[INTERNALSCI].address < SLAVEMAX+SlaveStartAddr-1)
    {
        Scisend[INTERNALSCI].address++;
        return;
    }
    else
    {
        internalsci.forallslave = pdFALSE;
    }
  }

  if(internalsci.InternalCmdFifo.inidx != internalsci.InternalCmdFifo.outidx)
  {
    temp = internalsci.InternalCmdFifo.outidx;
    Scisend[INTERNALSCI] = internalsci.InternalCmdFifo.fifo[temp];

    if(ALLSLAVE == Scisend[INTERNALSCI].address)
    {
      internalsci.forallslave = pdTRUE;
	  Scisend[INTERNALSCI].address = SlaveStartAddr;
    }
    
    internalsci.InternalCmdFifo.outidx++;
    if(internalsci.InternalCmdFifo.outidx == FIFOSIZE)
    {
      internalsci.InternalCmdFifo.outidx = 0;
    }
	internalsci.InternalCmdFifo.used --;
  }
  else
  {
  	loopreg = internalsci.loopcount%(sizeof(interRegRead)/sizeof(sHostLoopReg));
	if(interRegRead[loopreg].regnum)
	{
		if(interRegRead[loopreg].rwctrl)
		{
			Scisend[INTERNALSCI].used = pdTRUE;
			Scisend[INTERNALSCI].address = interRegRead[loopreg].unitAddr;
			Scisend[INTERNALSCI].cmd = CMD_READ;
			Scisend[INTERNALSCI].regaddress = interRegRead[loopreg].readregAddr;
			Scisend[INTERNALSCI].len = interRegRead[loopreg].regnum;
			rs485statussendcnt[interRegRead[loopreg].unitAddr-8]++;
		}
		else
		{
			Scisend[INTERNALSCI].used = pdTRUE;
			Scisend[INTERNALSCI].address = interRegRead[loopreg].unitAddr;
			Scisend[INTERNALSCI].cmd = CMD_WRITE;
			Scisend[INTERNALSCI].regaddress = interRegRead[loopreg].readregAddr;
			Scisend[INTERNALSCI].len = interRegRead[loopreg].regnum;
			Scisend[INTERNALSCI].pdata = interRegRead[loopreg].writeAddr;
		}
	}
	internalsci.loopcount++;
		
    internalsci.forallslave = pdFALSE;
//    Scisend[INTERNALSCI].used = pdTRUE;
//    Scisend[INTERNALSCI].address = internalsci.loopaddress;
//    Scisend[INTERNALSCI].cmd = CMD_READ;
//    Scisend[INTERNALSCI].regaddress = interRegRead[internalsci.loopcount%sizeof(interRegRead)];
//    Scisend[INTERNALSCI].len = 6;
////    Scisend[INTERNALSCI].pdata = hmiSendData;
//    if(internalsci.loopaddress < SLAVEMAX+SlaveStartAddr-1)
//    {
//      internalsci.loopaddress++;
//    }
//    else
//    {
//      internalsci.loopaddress = SlaveStartAddr;
//      internalsci.loopcount ++;
//    }
  }
}


void InternalSciRecvNormal(Cmdstruct *incmd,Cmdstruct *outcmd)
{
	BaseType_t  parse = pdFAIL;
	uint16_t temp = 0xff;
	sUnitStatusReg[incmd->address-SlaveStartAddr].workStatus.bit.Online = 1;
	CommTimeoutToRemoteFlag[incmd->address-SlaveStartAddr] = 0;
	switch(incmd->cmd)
	{
		case CMD_WRITE:
			parse = InternalSciWriteParse(incmd->regaddress,incmd->len);
		break;
		case CMD_WRITE_ONE:
			parse = InternalSciWriteOneParse(incmd->regaddress,incmd->pdata);
		break;
		case CMD_READ:
			parse = InternalSciReadParse(incmd->regaddress,incmd->len,incmd->pdata);
		break;
		case CMD_READWRITE_MULT:
			parse = InternalSciRWMultParse(incmd->address,incmd->len,incmd->pdata);
		break;
	}
	temp = Scisend[INTERNALSCI].address;
	if((temp < SlaveStartAddr + SLAVEMAX)&&(temp >= SlaveStartAddr))
	{
		temp -= SlaveStartAddr;
		internalsci.slavestatus[temp].sendsuccess = pdTRUE;
		if(internalsci.slavestatus[temp].errcnt > 0)
		{
			internalsci.slavestatus[temp].errcnt = 0;
		}
	}
	InternalSciupdateSenddata();
	*outcmd = Scisend[INTERNALSCI];
	parse = parse; //avoid warning
}

void InternalSciRecvError(Cmdstruct *outcmd)
{
	uint16_t temp,i;

	for(i=0;i<4;i++)
	{
//		if(sStatusUpdateTime[i].used == 1)
//		{
//			if(BackgroundGetTickCount() - sStatusUpdateTime[i].Time > 30000)
//				sUnitStatusReg[i].workStatus.bit.Online = 0;
//			if(BackgroundGetTickCount() - sStatusUpdateTime[i].Time > 10000)
//			{
//				UnitCommTimeout[i] = 1;
//				CommTimeoutProhibitSendSyncFlag = 1;
//			}
//		}
		if((recvTimeoutCnt[i] > 30) || (recvErrCnt[i] > 30))
		{
			CommTimeoutProhibitSendSyncFlag = 1;
			UnitCommTimeout[i] = 1;
			disptestbuf[i][0] = rs485statussendcnt[i];
			disptestbuf[i][1] = rs485statusrecvcnt[i];
		}
		
		if((recvTimeoutCnt[i] > 60) || (recvErrCnt[i] > 60))
		{
			sUnitStatusReg[i].workStatus.bit.Online = 0;
		}

		if((recvTimeoutCnt[i] > 120) || (recvErrCnt[i] > 120))
		{
			CommTimeoutToRemoteFlag[i] = 1;
		}
		else
		{
			CommTimeoutToRemoteFlag[i] = 0;
		}
	}
	temp = Scisend[INTERNALSCI].address;
	if((temp < SlaveStartAddr + SLAVEMAX)&&(temp >= SlaveStartAddr))
	{
		temp -= SlaveStartAddr;
		internalsci.slavestatus[temp].sendsuccess = pdFALSE;
		if((Scisend[INTERNALSCI].resend == pdTRUE) && (internalsci.slavestatus[temp].errcnt < ERRORCNTMAX) && (sUnitStatusReg[temp].workStatus.bit.Online))
		{
			internalsci.slavestatus[temp].errcnt++;
			Scisend[INTERNALSCI].used = pdPASS;	//重发
			*outcmd = Scisend[INTERNALSCI];
			return;
		}
		else
		{
			internalsci.slavestatus[temp].errcnt = 0;
			internalsci.slavestatus[temp].losedcnt++;
		}
	}
	
	InternalSciupdateSenddata();
	*outcmd = Scisend[INTERNALSCI];
}

void internalSciCmdinit(void)
{
  uint16_t i;
  internalsci.InternalCmdFifo.inidx = 0;
  internalsci.InternalCmdFifo.outidx = 0;
  internalsci.InternalCmdFifo.used = 0;
  internalsci.forallslave = pdFALSE;
  for(i=0;i<SLAVEMAX;i++)
  {
    internalsci.slavestatus[i].errcnt = 0;
    internalsci.slavestatus[i].sendsuccess = 0;
	internalsci.slavestatus[i].losedcnt = 0;
  }
  internalsci.loopaddress = SlaveStartAddr;
  internalsci.loopcount = 0;
  Scisend[INTERNALSCI].used = pdTRUE;
  Scisend[INTERNALSCI].address = SlaveStartAddr;
  Scisend[INTERNALSCI].cmd = CMD_READ;
  Scisend[INTERNALSCI].regaddress = 0xA0;
  Scisend[INTERNALSCI].len = 6;
  Scisend[INTERNALSCI].pdata = hmiSendData;
}

BaseType_t RemoteRWMultParse(Cmdstruct *incmd)
{
	BaseType_t  parse = pdFAIL;
	uint16_t *ptrDir;
	Cmdstruct scmd;
	uint16_t unitNum = incmd->address;
	uint16_t readlen = incmd->len;
	uint16_t writelen = incmd->len2;
	uint16_t *pdata = incmd->pdata;
	uint16_t i;//,tem;
	

	if(MachineWorkMode == 0)
	{
		if(Ctrldata1.Ctrl1.bit.LocalRemote == 0)
		{
			ptrDir = (uint16_t *)&sSynRemoteCtrlReg;
			datacopy(pdata,ptrDir,writelen);

			sSetPulseWideFromRemote();
			
			ModulePara.Mode = Parallel;
			UnitPara.Mode = Parallel;
		
			sSynRemoteUnitCtrlReg[0].Ctrl.all = sSynRemoteCtrlReg.Ctrl.all;
			for(i=0;i<20;i++)
			{
				sSynRemoteUnitCtrlReg[0].SectASet[i].Current = sSynRemoteCtrlReg.SectASet[i].Current[0];
				sSynRemoteUnitCtrlReg[0].SectASet[i].PulseWide = sSynRemoteCtrlReg.SectASet[i].PulseWide;
				sSynRemoteUnitCtrlReg[0].SynPhase = sSynRemoteCtrlReg.SynPhase;
			}
		
			scmd.used = pdPASS;
			scmd.cmd = CMD_READWRITE_MULT;
			scmd.regaddress = incmd->regaddress;
			scmd.len = sizeof(sSynRemoteStatusRegion);//readlen;
			scmd.regaddress2 = incmd->regaddress2;
			scmd.len2 = sizeof(sSynRemoteUintCtrlRegion);
			scmd.pdata = (uint16_t*)&sSynRemoteUnitCtrlReg[0];
			scmd.resend = pdFALSE;
			scmd.address = SlaveStartAddr;
			addinternalCmdFifo(scmd);	

			sSynRemoteUnitCtrlReg[1].Ctrl.all = sSynRemoteCtrlReg.Ctrl.all;
			for(i=0;i<20;i++)
			{
				sSynRemoteUnitCtrlReg[1].SectASet[i].Current = sSynRemoteCtrlReg.SectASet[i].Current[1];
				sSynRemoteUnitCtrlReg[1].SectASet[i].PulseWide = sSynRemoteCtrlReg.SectASet[i].PulseWide;
				sSynRemoteUnitCtrlReg[1].SynPhase = sSynRemoteCtrlReg.SynPhase;
			}
			scmd.used = pdPASS;
			scmd.cmd = CMD_READWRITE_MULT;
			scmd.regaddress = incmd->regaddress;
			scmd.len = sizeof(sSynRemoteStatusRegion);//readlen;
			scmd.regaddress2 = incmd->regaddress2;
			scmd.len2 = sizeof(sSynRemoteUintCtrlRegion);
			scmd.pdata = (uint16_t*)&sSynRemoteUnitCtrlReg[1];
			scmd.resend = pdFALSE;
			scmd.address = SlaveStartAddr+1;
			addinternalCmdFifo(scmd);

			sSynRemoteUnitCtrlReg[2].Ctrl.all = sSynRemoteCtrlReg.Ctrl.all;
			for(i=0;i<20;i++)
			{
				sSynRemoteUnitCtrlReg[2].SectASet[i].Current = sSynRemoteCtrlReg.SectASet[i].Current[2];
				sSynRemoteUnitCtrlReg[2].SectASet[i].PulseWide = sSynRemoteCtrlReg.SectASet[i].PulseWide;
				sSynRemoteUnitCtrlReg[2].SynPhase = sSynRemoteCtrlReg.SynPhase;
			}
			scmd.used = pdPASS;
			scmd.cmd = CMD_READWRITE_MULT;
			scmd.regaddress = incmd->regaddress;
			scmd.len = sizeof(sSynRemoteStatusRegion);//readlen;
			scmd.regaddress2 = incmd->regaddress2;
			scmd.len2 = sizeof(sSynRemoteUintCtrlRegion);
			scmd.pdata = (uint16_t*)&sSynRemoteUnitCtrlReg[2];
			scmd.resend = pdFALSE;
			scmd.address = SlaveStartAddr+2;
			addinternalCmdFifo(scmd);

			sSynRemoteUnitCtrlReg[3].Ctrl.all = sSynRemoteCtrlReg.Ctrl.all;
			for(i=0;i<20;i++)
			{
				sSynRemoteUnitCtrlReg[3].SectASet[i].Current = sSynRemoteCtrlReg.SectASet[i].Current[3];
				sSynRemoteUnitCtrlReg[3].SectASet[i].PulseWide = sSynRemoteCtrlReg.SectASet[i].PulseWide;
				sSynRemoteUnitCtrlReg[3].SynPhase = sSynRemoteCtrlReg.SynPhase;
			}
			scmd.used = pdPASS;
			scmd.cmd = CMD_READWRITE_MULT;
			scmd.regaddress = incmd->regaddress;
			scmd.len = sizeof(sSynRemoteStatusRegion);//readlen;
			scmd.regaddress2 = incmd->regaddress2;
			scmd.len2 = sizeof(sSynRemoteUintCtrlRegion);
			scmd.pdata = (uint16_t*)&sSynRemoteUnitCtrlReg[3];
			scmd.resend = pdFALSE;
			scmd.address = SlaveStartAddr+3;
			addinternalCmdFifo(scmd);
		}
		for(i=0;i<4;i++)
		{
			if(CommTimeoutToRemoteFlag[i] == 0)//(sUnitStatusReg[i].workStatus.bit.Online)
				sSynRemoteStatusReg[i].status1.bit.local = 0;
			else
				sSynRemoteStatusReg[i].status1.bit.local = 1;
		}
	}
	parse = pdPASS;

	
	Scisend[REMOTESCI].cmd = CMD_READWRITE_MULT;
	Scisend[REMOTESCI].len = readlen;
	Scisend[REMOTESCI].address = unitNum;
	Scisend[REMOTESCI].pdata = (uint16_t*)&sSynRemoteStatusReg[0];
	return parse;
}

BaseType_t RemoteStopCmdParse(uint16_t address, uint16_t len, uint16_t *pdata)
{
	BaseType_t  parse = pdFAIL;
	Cmdstruct scmd;
	BaseType_t bitpos = ((Get485Addr()-1)%4)*4;
	
	datacopy(pdata,hmiSendData,len);
	
	hmiSendData[0] = (hmiSendData[(Get485Addr()-1)/4]&(0xf<<bitpos))>>bitpos;
	
	scmd.used = pdPASS;
	scmd.cmd = CMD_STOP;
	scmd.len = 1;
	scmd.pdata = hmiSendData;
	scmd.address = BADDRESS;//ALLSLAVE;
	addinternalCmdFifo(scmd);
	

	return parse;
}

void RemoteSciRecvNormal(Cmdstruct *incmd,Cmdstruct *outcmd)
{
	BaseType_t  parse = pdFAIL;
	// incmd->used = pdFAIL;
	outcmd->used = pdFAIL;
	switch(incmd->cmd)
	{
//		case CMD_WRITE:
//			parse = HmiWriteParse(incmd->regaddress,incmd->len,incmd->pdata);
//		break;
//		case CMD_WRITE_ONE:
//			parse = HmiWriteOneParse(incmd->regaddress,incmd->pdata);
//		break;
//		case CMD_READ:
//			parse = HmiReadParse(incmd->regaddress,incmd->len);
//		break;
		case CMD_STOP:
			parse = RemoteStopCmdParse(incmd->address,incmd->len,incmd->pdata);
			break;
		case CMD_READWRITE_MULT:
			parse = RemoteRWMultParse(incmd);
		break;
	}
	if(pdFAIL != parse)
	{
		*outcmd = Scisend[REMOTESCI];
		outcmd->used = pdTRUE;
	}
}

void RemoteSciRecvError(Cmdstruct *outcmd)
{

}

/*********************************************************************
Function name:  HMITaskInit
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void HMITaskInit(void)
{
    internalSciCmdinit();
    sciframeinit(HMISCI, Get485Addr(), HMIISHOST,GetBoardType(), HmiSciRecvNormal,
                 HmiSciRecvError,500);
    sciframeinit(INTERNALSCI, Get485Addr(), INTERNALISHOST,GetBoardType(),
                 InternalSciRecvNormal, InternalSciRecvError,500);
    InitSciToRemote(REMOTESCI);
    sciframeinit(REMOTESCI, RemoteCommAddr, REMOTEISHOST,GetBoardType(), RemoteSciRecvNormal,
                 RemoteSciRecvError,500);
}

/*********************************************************************
Function name:  HMITask
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void HMITask(void * pvParameters)
{
    uint32_t event;

    //BaseType_t err;

    /*internalSciCmdinit();
    sciframeinit(HMISCI, Get485Addr(), HMIISHOST,GetBoardType(), HmiSciRecvNormal,
                 HmiSciRecvError);
    sciframeinit(INTERNALSCI, Get485Addr(), INTERNALISHOST,GetBoardType(),
                 InternalSciRecvNormal, InternalSciRecvError);
	InitSciToRemote(REMOTESCI);
	sciframeinit(REMOTESCI, RemoteCommAddr, REMOTEISHOST,GetBoardType(), RemoteSciRecvNormal,
                 RemoteSciRecvError);*/
    //while (pdTRUE)
    {
        //err = xSemaphoreTake(xSemaphoreHMI, (TickType_t)Tick4mS);
        event = OSEventPend(cPrioHMI);
        if(event != 0)
        {
            if (OSEventConfirm(event, eHMITest))
            {

            }
        }
        else    //eHMITimer
        {
			SciGroupManage(HMISCI);
        	SciGroupManage(INTERNALSCI);
			SciGroupManage(REMOTESCI);
        }
    }

}



