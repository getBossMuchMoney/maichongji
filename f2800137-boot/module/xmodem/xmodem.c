/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : xmodem.c
 *  PURPOSE  :
 *  AUTHOR   : liusongjie
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define Xmodem_GLOBALS 1

/********************************************************************************
*include header file                              *
********************************************************************************/
#include <stdio.h>
#include "device.h"
#include "Initial.h"
#include "xmodem.h"
#include "Constant.h"
#include "flashmodule.h"
#include "shellprint.h"
#include "Common.h"

/********************************************************************************
*const define                               *
********************************************************************************/
#define SOH 0x01  
#define STX 0x02  
#define EOT 0x04  
#define ACK 0x06  
#define NAK 0x15  
#define CAN 0x18  
#define CTRLZ 0x1A  
#define DELAY 2000
#define MAXRETRANS 25  

/********************************************************************************
* Variable declaration                              *
********************************************************************************/
static int last_error = 0;  

/********************************************************************************
* function declaration                              *
********************************************************************************/
  
void out_buff(unsigned char *buff, int size)  
{  
    SetStatus485A(STATUS_485_SEND);
    DEVICE_DELAY_US(30);

    printchn((const char*)buff,size);
}  
  
int in_buff(unsigned char *buff)
{  
    int readSize = 0;  
    char c = 0;
    uint32_t curtime = 0,distime = 0;
      
    last_error = 0;  

    DEVICE_DELAY_US(30);
    SetStatus485A(STATUS_485_RECEIVED);

    curtime = getTimerTickCur();
    while(1)
    {
        distime = (getTimerTickCur() >= curtime)?(getTimerTickCur()-curtime):(0xffffffff-curtime+getTimerTickCur());
        if(distime > DELAY)
        {
            last_error = 1;
            return 0;
        }

        c= getch();
        if(c != -1)
        {
            buff[readSize++] = c;
            break;
        }
    }

    curtime = getTimerTickCur();
    while(1)
    {
        distime = (getTimerTickCur() >= curtime)?(getTimerTickCur()-curtime):(0xffffffff-curtime+getTimerTickCur());
        if(distime > 5)
            break;

        c= getch();
        if(c == -1)
            continue;

        buff[readSize++]=c;
        curtime = getTimerTickCur();
    }
      
    if(readSize == 0)  
        last_error = 1;  
  
    return (readSize);  
}  
int calcrc(const unsigned char *ptr, int count)  
{  
    int crc;  
    char i;  
  
    crc = 0;  
    while (--count >= 0)  
    {  
        crc = crc ^ (int) *ptr++ << 8;  
        i = 8;  
        do  
        {  
            if (crc & 0x8000)  
                crc = crc << 1 ^ 0x1021;  
            else  
                crc = crc << 1;  
        } while (--i);  
    }  
  
    return (crc);  
}  
static int check(int crc, const unsigned char *buf, int sz)  
{    
    if(crc)  
    {  
        unsigned short crc = calcrc(buf, sz);  
        unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];  
  
        if (crc == tcrc)  
            return 1;  
    }  
    else  
    {  
        int i = 0;  
        unsigned char cks = 0;  
          
        for(i = 0; i < sz; i ++)  
        {  
            cks += buf[i];   
        }  
          
        if (cks == buf[sz])  
            return 1;  
    }  
  
    return 0;   
  
}  
void dataReduction(uint16_t *buf, uint32_t len)
{
    uint32_t i,index;

    for(i=0,index=0;i<len;i=i+2,index++)
    {
        buf[index] = (buf[i+4] << 8) | buf[i+3];
    }
}
//recv_buff_size == 384  
#pragma DATA_ALIGN(xbuff, 16);
unsigned char xbuff[1030];
int xmodemReceive()
{    

    int bufsz = 0;  
    int crc = 0;  
    unsigned char trychar = 'C';  
    unsigned char packetno = 1;  
    int c = 0;  
    uint32_t len = 0;
    int retry = 0;  
    int retrans = MAXRETRANS;  
    int recvSize = 0;  
    uint32_t addr = APP_Addr;
    int result = 0;

    for(;;)   
    {  
        for(retry = 0; retry < 16; retry ++)  
        {  
            if(trychar)  
            {  
                xbuff[0] = trychar;  
                out_buff(xbuff, 1);  
            }  
              
            recvSize = in_buff(xbuff);
            c = xbuff[0];  
              
            if (last_error == 0)   
            {  
                switch(c)  
                {  
                    case SOH:  
                        bufsz = 128;  
                        goto start_recv;  
                    case STX:  
                    {  
                        bufsz = 1024;
                        goto start_recv;
//                        xbuff[0] = CAN;
//                        out_buff(xbuff, 1);
                    }  
//                    return -1;
                    case EOT:  
  
                        {  
                            xbuff[0] = ACK;  
                            out_buff(xbuff, 1);
                        }  
                        waitMs(1000);
                        return len;  
                    case CAN:  
  
                        in_buff(xbuff);
                        c = xbuff[0];  
                        if(c == CAN)  
                        {  
                            {  
                                xbuff[0] = ACK;  
                                out_buff(xbuff, 1);  
                            }  
                            return -1;  
                        }  
                        break;  
                    default:  
                        break;  
                }  
            }  
        }  
  
        if (trychar == 'C')
        {  
            return -4;
//            trychar = NAK;
//            continue;
        }  
cancel:
        {  
            xbuff[0] = CAN;  
            out_buff(xbuff, 1);  
            out_buff(xbuff, 1);
            out_buff(xbuff, 1);  
        }  
  
        return -2;  
  
start_recv:  
        if(trychar == 'C')  
            crc = 1;  
  
        trychar = 0;  
  
        if(recvSize != (bufsz + (crc ? 1 : 0) + 4))  
            goto reject;  
          
        if((xbuff[1] + xbuff[2] == 0xff) &&
            (xbuff[1] == (packetno&0xff) || xbuff[1] == (packetno&0xff) - 1) &&
            check(crc, &xbuff[3], bufsz))  
        {  
            if(xbuff[1] == (packetno&0xff))
            {  
                if(packetno == 1)
                {
                    result = flashInit();
                    if(result != 0)
                    {
//                        printf("flash init failed:%d\n",result);
                        goto cancel;
                    }
                    result = flashEraseSector(APP_Addr,APP_LEN);
                    if(result != 0)
                    {
//                        printf("flash erase failed:%d\n",result);
                        goto cancel;
                    }
                }

                dataReduction((uint16_t*)xbuff,bufsz);
                result = flashProgram(addr,(uint16_t*)xbuff,bufsz/2);
                if(result != 0)
                {
//                    printf("flash program failed:%d\n",result);
                    goto cancel;
                }
                addr += bufsz/2;

                len += bufsz;

                packetno ++;  
  
                retrans = MAXRETRANS+1;  
            }  
  
            if(-- retrans <= 0)  
            {  
                {  
                    xbuff[0] = CAN;  
                    out_buff(xbuff, 1);  
                    out_buff(xbuff, 1);  
                    out_buff(xbuff, 1);  
                }  
                return -3;  
            }  
  
            {  
                    xbuff[0] = ACK;  
                    out_buff(xbuff, 1);  
            }  
              
            continue;  
        }  
  
reject:    
        {  
                xbuff[0] = NAK;  
                out_buff(xbuff, 1);  
        }  
  
    }  
}  

