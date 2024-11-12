/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : canmodule.c
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023年12月28日       V1.0                          Created.
 *
 *============================================================================*/
#define canmodule_GLOBALS     1
/********************************************************************************
*include header file                              *
********************************************************************************/
#include "canmodule.h"
#include <stdio.h>

/********************************************************************************
*const define                               *
********************************************************************************/

/********************************************************************************
* Variable declaration                              *
********************************************************************************/

/********************************************************************************
* function declaration                              *
********************************************************************************/

void CanDataInit()
{
    sQInit(&queCanRxMsg,ucCanRXMsgBuffer,MAXRXMSGCNT,8);
}

void CanTxRx(void)
{
//    uint16_t TxDelay = 250;
    uint16_t ucQData[8]={0};

    /****从队列中取出所有的接收数据****/
    while (sQDataOut(&queCanRxMsg,ucQData) != cQBufEmpty)
    {
//        ParseSyncData(ucQData);   //TODO msg process
//        shellPrintf("can rx:%02X,%02X,%02X,%02X, %02X,%02X,%02X,%02X\n",
//                    ucQData[0],ucQData[1],ucQData[2],ucQData[3],
//                    ucQData[4],ucQData[5],ucQData[6],ucQData[7]);
    }
#if 0
    if (para_cmd.f_code.FaultInfo.FaultBit.ComErr_Sync == 1)
    {
        return;
    }

    if (ParaSyncOK == 0)
    {
        TxDelay = 25;
    }

    TxDelayCount++;
    if(TxDelayCount >= TxDelay)  // 4*50 = 200 ms
    {
        TxDelayCount = 0;
        if (para_cmd.f_code.CANAddr == 1) // 主机
        {
            para_cmd.f_code.SlaveFlag1 = 0;
            para_cmd.f_code.SlaveFlag2 = 0;

            if ((CanStep == CMDSTATE) || (CanStep == CMDFAULT))
            {
                ucTXMsgData[0] = 0x01;
            }
            else
            {
                ucTXMsgData[0] = 0x03;
            }

            ucTXMsgData[1] = 0xFE;
            ucTXMsgData[2] = 0xFF;
        }
        else
        {
            if ((CanStep == CMDSTATE) || (CanStep == CMDFAULT))
            {
                ucTXMsgData[0] = 0x41;
            }
            else
            {
                ucTXMsgData[0] = 0x43;
            }

            ucTXMsgData[1] = 0x00;
            ucTXMsgData[2] = 0x01;
        }

        ucTXMsgData[3] = CanStep & 0xFF;
        if (CanStep < CMDPULSE1)
        {
            if ((para_cmd.f_code.CANAddr > 1) && (CanStep == CMDSTATE))
            {
                ucTXMsgData[4] = (para_cmd.f_code.StateInfo.all >> 24) & 0xFF;
                ucTXMsgData[5] = (para_cmd.f_code.StateInfo.all >> 16) & 0xFF;
                ucTXMsgData[6] = (para_cmd.f_code.StateInfo.all >> 8) & 0xFF;
                ucTXMsgData[7] = (para_cmd.f_code.StateInfo.all >> 0) & 0xFF;
            }
            else if ((para_cmd.f_code.CANAddr > 1) && (CanStep == CMDFAULT))
            {
                ucTXMsgData[4] = (para_cmd.f_code.FaultInfo.FaultReg[1] >> 8) & 0xFF;
                ucTXMsgData[5] = (para_cmd.f_code.FaultInfo.FaultReg[1] >> 0) & 0xFF;
                ucTXMsgData[6] = (para_cmd.f_code.FaultInfo.FaultReg[0] >> 8) & 0xFF;
                ucTXMsgData[7] = (para_cmd.f_code.FaultInfo.FaultReg[0] >> 0) & 0xFF;
            }
            else if (CanStep == CMDRUNMODE)
            {
                ucTXMsgData[4] = (para_cmd.f_code.CtrlInfo.all >> 24) & 0xFF;
                ucTXMsgData[5] = (para_cmd.f_code.CtrlInfo.all >> 16) & 0xFF;
                ucTXMsgData[6] = (para_cmd.f_code.CtrlInfo.all >> 8) & 0xFF;
                ucTXMsgData[7] = (para_cmd.f_code.CtrlInfo.all >> 0) & 0xFF;
            }
            else if (CanStep == CMDONOFF)
            {
                ucTXMsgData[4] = 0x00;
                ucTXMsgData[5] = 0x00;
                ucTXMsgData[6] = 0x00;
                ucTXMsgData[7] = (para_cmd.f_code.CtrlInfo.bit.OnOff) & 0xFF;
            }
            else
            {
                ucTXMsgData[4] = 0x00;
                ucTXMsgData[5] = 0x00;
                ucTXMsgData[6] = 0x00;
                ucTXMsgData[7] = 0x00;
            }
        }
        else
        {
            if (CanStep == CMDPHASE)
            {
                ucTXMsgData[4] = 0x00;
                ucTXMsgData[5] = 0x00;
                ucTXMsgData[6] = (para_cmd.f_code.SyncPhase >> 8) & 0xFF;
                ucTXMsgData[7] = para_cmd.f_code.SyncPhase & 0xFF;
            }
            else
            {
                ucTXMsgData[4] = (para_cmd.f_code.SectSet[0][CanStep - 5].PulseWide >> 8) & 0xFF;
                ucTXMsgData[5] = para_cmd.f_code.SectSet[0][CanStep - 5].PulseWide & 0xFF;
                ucTXMsgData[6] = (para_cmd.f_code.SectSet[1][CanStep - 5].PulseWide >> 8) & 0xFF;
                ucTXMsgData[7] = para_cmd.f_code.SectSet[1][CanStep - 5].PulseWide & 0xFF;
            }
        }

        CanStepTimes++;
        CANMessageSet(CANB_BASE, 1, &sTXCANMessage, MSG_OBJ_TYPE_TX);
    }

    CommLostCount++;
    if(CommLostCount >= 3000) // 12s
    {
        CommLostCount = 0;
        para_cmd.f_code.FaultInfo.FaultBit.ComErr_Sync = 1;
    }
#endif
}
