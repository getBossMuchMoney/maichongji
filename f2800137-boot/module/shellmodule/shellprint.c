/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : shellprint.c
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023年12月28日       V1.0                          Created.
 *
 *============================================================================*/
#define shellprint_GLOBALS     1
/********************************************************************************
 *include header file                              *
 ********************************************************************************/
#include "shellprint.h"
#include "stdio.h"
#include "string.h"
#include "device.h"
#include "Constant.h"
#include "Common.h"

/********************************************************************************
 *const define                               *
 ********************************************************************************/


/********************************************************************************
 * Variable declaration                              *
 ********************************************************************************/
volatile struct SCI_REGS *shellSciRegs = &SciaRegs;

uint32_t printFlag = 0;
uint32_t uartRstCnt = 0;
/********************************************************************************
 * function declaration                              *
 ********************************************************************************/
/**********************************************************

 ***********************************************************/
int getch()
{
    char c = 0;

    if (shellSciRegs->SCIFFRX.bit.RXFFST != 0)
    {
        c = shellSciRegs->SCIRXBUF.bit.SAR;
        return c;
    }

    return -1;
}
/**********************************************************

 ***********************************************************/
int UartCheckErr()
{
    if ((shellSciRegs->SCIRXST.bit.RXERROR)
            || (shellSciRegs->SCIRXST.bit.FE)
            || (shellSciRegs->SCIRXST.bit.OE)
            || (shellSciRegs->SCIRXST.bit.PE)
            || (shellSciRegs->SCIRXST.bit.BRKDT)
//            || (shellSciRegs->SCIRXBUF.bit.SCIFFFE)
//            || (shellSciRegs->SCIRXBUF.bit.SCIFFPE)
            )
    {
        //Have an error occur
        shellSciRegs->SCICTL1.bit.SWRESET = 0;
        shellSciRegs->SCIFFTX.bit.TXFIFORESET = 0;
        shellSciRegs->SCIFFRX.bit.RXFIFORESET = 0;
        //DELAY_US(5);
        shellSciRegs->SCICTL1.bit.SWRESET = 1;
        shellSciRegs->SCIFFTX.bit.TXFIFORESET = 1;
        shellSciRegs->SCIFFRX.bit.RXFIFORESET = 1;

		uartRstCnt++;
		return -1;
    }
	return 0;
}

/**********************************************************

 ***********************************************************/
void printEnable()
{
    printFlag = 1;
}
/**********************************************************

 ***********************************************************/
void printDisable()
{
    printFlag = 0;
}
/**********************************************************

 ***********************************************************/
void printch(const char ch)
{
    if(!printFlag)
        return;

    while (shellSciRegs->SCIFFTX.bit.TXFFST == 16)
        ;

    shellSciRegs->SCITXBUF.bit.TXDT = ch;

    while (!((shellSciRegs->SCICTL2.bit.TXEMPTY == 1)
            && (shellSciRegs->SCICTL2.bit.TXRDY == 1)
            && (shellSciRegs->SCIFFTX.bit.TXFFST == 0)))
        ;
}
/**********************************************************

 ***********************************************************/
int printchn(const char *buf, int len)
{
    if(!printFlag)
        return 0;

    while (len--)
    {
        while (shellSciRegs->SCIFFTX.bit.TXFFST == 16)
            ;

        shellSciRegs->SCITXBUF.bit.TXDT = *buf++;
    }

    while (!((shellSciRegs->SCICTL2.bit.TXEMPTY == 1)
            && (shellSciRegs->SCICTL2.bit.TXRDY == 1)
            && (shellSciRegs->SCIFFTX.bit.TXFFST == 0)))
        ;

    return len;
}
/**********************************************************

 ***********************************************************/
int prints(const char *buf)
{
    int len = strlen(buf);

    printchn(buf, len);

    return len;
}
/**********************************************************

 ***********************************************************/
void printdec(const unsigned long long dec)
{
    if (dec < 10)
    {
        printch((char) (dec + '0'));
        return;
    }
    printdec(dec / 10);
    printch((char) (dec % 10 + '0'));
}
/**********************************************************

 ***********************************************************/
void printhex(const unsigned long long hex)
{
    if (hex < 16)
    {
        if (hex < 10)
            printch((char) (hex + '0'));
        else
            printch((char) (hex - 10 + 'A'));
        return;
    }
    printhex(hex >> 4);

    if ((hex & 0xf) < 10)
        printch((char) ((hex & 0xf) + '0'));
    else
        printch((char) ((hex & 0xf) - 10 + 'A'));
}
/**********************************************************

 ***********************************************************/
void printX(const unsigned long long hex, int prtlen, int zeroflg)
{
    int len = 0;
    unsigned long long val = hex;

    do
    {
        len++;
        val = val >> 4;
    }
    while (val);

    while (len < prtlen)
    {
        if (zeroflg)
            printch('0');

        len++;
    }

    printhex(hex);
}
/**********************************************************

 ***********************************************************/
void printuint(const unsigned int dec, int prtlen, int zeroflg)
{
    int len = 0;
    unsigned int val = dec;

    do
    {
        len++;
        val = val / 10;
    }
    while (val);

    while (len < prtlen)
    {
        if (zeroflg)
            printch('0');
        else
            printch(' ');
        len++;
    }

    printdec(dec);
}
/**********************************************************

 ***********************************************************/
void printint(const int dec, int prtlen, int zeroflg)
{
    unsigned int udec;
    if (dec < 0)
    {
        printch('-');
        udec = -dec;
    }
    else
        udec = dec;

    printuint(udec, prtlen, zeroflg);
}
/**********************************************************

 ***********************************************************/
void printulong(const unsigned long dec, int prtlen, int zeroflg)
{
    int len = 0;
    unsigned long val = dec;

    do
    {
        len++;
        val = val / 10;
    }
    while (val);

    while (len < prtlen)
    {
        if (zeroflg)
            printch('0');
        else
            printch(' ');
        len++;
    }

    printdec(dec);
}
/**********************************************************

 ***********************************************************/
void printlong(const long dec, int prtlen, int zeroflg)
{
    unsigned long udec;
    if (dec < 0)
    {
        printch('-');
        udec = -dec;
    }
    else
        udec = dec;
    printulong(udec, prtlen, zeroflg);
}
/**********************************************************

 ***********************************************************/
void printstr(const char *ptr, int pos, int len)
{
    int count = (len > strlen(ptr)) ? (len - strlen(ptr)) : 0;

    if (pos == 0)
    {
        while (count > 0)
        {
            printch(' ');
            count--;
        }
    }

    prints(ptr);

    if (pos == 1)
    {
        while (count > 0)
        {
            printch(' ');
            count--;
        }
    }
}
/**********************************************************

 ***********************************************************/
void printdecimal(const unsigned long decimal, int prtlen)
{
    int len = 0;
    unsigned long val = decimal;

    do
    {
        len++;
        val = val / 10;
    }
    while (val);

    printdec(decimal);

    while (len < prtlen)
    {
        printch('0');
        len++;
    }
}
/**********************************************************

 ***********************************************************/
void printfloat(const float flt, int intlen, int fltlen, int zeroflg)
{
    long tmpint = (long) flt;
    long tem[7] = { 10000000, 100, 1000, 10000, 100000, 1000000, 10000000 }; //最多6位小数

    fltlen = (fltlen > 6) ? 6 : fltlen;
    long tmpflt = (long) (tem[fltlen] * (flt - tmpint));

    if (tmpflt % 10 >= 5)
    {
        tmpflt = tmpflt / 10 + 1;
    }
    else
    {
        tmpflt = tmpflt / 10;
    }
    printlong(tmpint, intlen, zeroflg);
    printch('.');
    printdecimal(tmpflt, fltlen);

}
/**********************************************************

 ***********************************************************/
void printdouble(const double flt, int intlen, int fltlen, int zeroflg)
{
    long tmpint = (long) flt;
    long tem[7] = { 10000000, 100, 1000, 10000, 100000, 1000000, 10000000 }; //最多6位小数

    fltlen = (fltlen > 6) ? 6 : fltlen;
    long tmpflt = (long) (tem[fltlen] * (flt - tmpint));

    if (tmpflt % 10 >= 5)
    {
        tmpflt = tmpflt / 10 + 1;
    }
    else
    {
        tmpflt = tmpflt / 10;
    }
    printlong(tmpint, intlen, zeroflg);
    printch('.');
    printlong(tmpflt, fltlen, 0);

}

int charformat(char *str, int *posflag, int *zeroflag, int *longflag,
               int *intlen, int *fltlen)
{
    int count = 0;

    *posflag = 0;
    *zeroflag = 0;
    *longflag = 0;
    *intlen = 0;
    *fltlen = 0;

    if (*str == '-')
    {
        *posflag = 1;
        count++;
        str++;
    }

    if (*str == '0')
    {
        *zeroflag = 1;
        count++;
        str++;
    }

    if ((*str > '0') && (*str <= '9'))
    {
        while ((*str > '0') && (*str <= '9'))
        {
            count++;
            *intlen = (*intlen * 10) + (*str - '0');
            str++;
        }
    }

    if (*str == '.')
    {
        count++;
        str++;
        while ((*str >= '0') && (*str <= '9'))
        {
            count++;
            *fltlen = (*fltlen * 10) + (*str - '0');
            str++;
        }
    }

    if (*str == 'l')
    {
        count++;
        str++;
        *longflag = 1;
    }

    return count;
}
/**********************************************************

 ***********************************************************/
#if 1
void shellPrintf(const char *format, ...)
{
    int posflag = 0;
    int zeroflag = 0;
    int longflag = 0;
    int intlen = 0;
    int fltlen = 0;
    int count = 0;

    if(!printFlag)
        return;

    va_list ap;
    va_start(ap, format);
    while (*format)
    {
        if (*format != '%')
        {
            printch(*format);
            format++;
        }
        else
        {
            format++;
            count = charformat((char*) format, &posflag, &zeroflag, &longflag,
                               &intlen, &fltlen);
            format += count;
            switch (*format)
            {
            case 'c':
            {
                char valch = va_arg(ap,int);
                printch(valch);
                format++;
                break;
            }
            case 'x':
            case 'X':
            {
                if (longflag)
                {
                    unsigned long valhex = va_arg(ap,unsigned long);
                    printX(valhex, intlen, zeroflag);
                }
                else
                {
                    unsigned int valhex = va_arg(ap,unsigned int);
                    printX(valhex, intlen, zeroflag);
                }
                format++;
                break;
            }
            case 'd':
            {
                if (longflag)
                {
                    long valint = va_arg(ap,long);
                    printlong(valint, intlen, zeroflag);
                }
                else
                {
                    int valint = va_arg(ap,int);
                    printint(valint, intlen, zeroflag);
                }
                format++;
                break;
            }
            case 'u':
            {
                if (longflag)
                {
                    unsigned long valint = va_arg(ap,unsigned long);
                    printulong(valint, intlen, zeroflag);
                }
                else
                {
                    unsigned int valint = va_arg(ap,unsigned int);
                    printuint(valint, intlen, zeroflag);
                }
                format++;
                break;
            }
            case 's':
            {
                char *valstr = va_arg(ap,char *);
                printstr(valstr, posflag, intlen);
                format++;
                break;
            }
            case 'f':
            {
                if (longflag)
                {
                    double valflt = va_arg(ap,double);
                    printdouble(valflt, intlen, fltlen, zeroflag);
                }
                else
                {
                    float valflt = va_arg(ap,double);
                    printfloat(valflt, intlen, fltlen, zeroflag);
                }
                format++;
                break;
            }

            default:
            {
                printch(*format);
                format++;
            }
            }
            va_end(ap);
        }
    }
}
#else
void shellPrintf(const char *format,...)
{
    char buffer[64] = {0};
    char* p = buffer;
    Uint32 len = 0UL;

    va_list ap=NULL;
    va_start(ap, format);
    len = (Uint32)vsnprintf(buffer, 64, format, ap);
    va_end(ap);

    printchn(p,len);
}
#endif
