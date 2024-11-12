/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : fault.h
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#ifndef _fault_H_
#define _fault_H_
#ifdef fault_GLOBALS
#define fault_EXT
#else
#define fault_EXT extern
#endif
/********************************************************************************
*include header file                              *
********************************************************************************/

/********************************************************************************
*const define                               *
********************************************************************************/
/*******************************************************************
	Fault code
*******************************************************************/
#define 	cFNoFault						0
#define 	cFHwLoadOVpos					1   //unused
#define 	cFHwLoadOVneg					2   //unused
#define     cFHwLoadOCpos           		3   //unused
#define     cFHwLoadOCneg           		4   //unused
#define     cFambientTempOverHigh			5   //used
#define     cFsinkTempOverHigh				6   //used
#define     cFBusOver                       7   //used
#define     cFBusUnder                      8   //used
#define     cFHwExtLoadOC                   9   //used
#define     cFHw24Vabnormal                 10  //unused
#define     cFEepromRead                    11  //used
#define     cFEepromWrite                   12  //used
#define     cFHwLoadAvgOV                   13  //unused
#define     cFHwLoadAvgOC                   14  //unused
#define     cFPWMSyncLoss                   15  //unused
#define     cFPWMLockLoss                   16  //unused
#define     cFLineSyncLoss                  17  //unused
#define     cFLineLockLoss                  18  //unused

/*******************************************************************
 	warning code
*******************************************************************/
#define     cWSinkOT                        0x00000001
#define     cWambientOT                     0x00000002
#define     cWambientNTCerr                 0x00000004
#define     cWsinkNTCerr                    0x00000008
#define     cWsinkNTCOT                     0x00000010
#define     cWHwOverLoad                    0x00000020
#define     cWHw24Vabnormal                 0x00000040
#define     cWunitPWMSyncLoss               0x00000080
#define     cWunitPWMSyncLock       	    0x00000100
#define     cWunitLineSyncLoss              0x00000200
#define     cWunitLineSyncLock              0x00000400
#define     cWbusOver                       0x00000800
#define     cWbusUnder                      0x00001000
#define     cWloadPosAvgOV                  0x00002000
#define     cWloadNegAvgOV                  0x00004000
#define     cWloadPosAvgOC                  0x00008000
#define     cWloadNegAvgOC                  0x00010000

/********************************************************************************
* Variable declaration                              *
********************************************************************************/
//fault
//HWfaultBits_1
typedef struct HWFAULT_BITS_1 {                    // bits description
    Uint16 HwLoadOVpos:1;
	Uint16 HwLoadOVneg:1;
    Uint16 HwLoadOCpos:1;
	Uint16 HwLoadOCneg:1;
	Uint16 HwExtLoadOC:1;
	Uint16 Hw24Vabnormal:1;
    Uint16 rsvd1:10;
}HWfaultBits_1;

typedef union HWFAULT_REG_1 {
    Uint16  all;
    HWfaultBits_1  bit;
}HWfaultREG_1;

//HWfaultBits_2
typedef struct HWFAULT_BITS_2 {                    // bits description
    Uint16 HwLoadAvgOV:1;
    Uint16 HwLoadAvgOC:1;
	Uint16 BusOver:1;
	Uint16 BusUnder:1;
    Uint16 rsvd1:12;
}HWfaultBits_2;

typedef union HWFAULT_REG_2 {
    Uint16  all;
    HWfaultBits_2  bit;
}HWfaultREG_2;

//HWfaultBits_3
typedef struct HWFAULT_BITS_3 {                    // bits description
    Uint16 AmbientHighOT:1;
	Uint16 SinkHighOT:1;
    Uint16 rsvd1:14;
}HWfaultBits_3;

typedef union HWFAULT_REG_3 {
    Uint16  all;
    HWfaultBits_3  bit;
}HWfaultREG_3;

//HWfaultBits_4
typedef struct HWFAULT_BITS_4 {                    // bits description
    Uint16 PWMSyncLoss:1;
	Uint16 PWMLockLoss:1;
	Uint16 LineSyncLoss:1;
	Uint16 LineLockLoss:1;
    Uint16 rsvd1:12;
}HWfaultBits_4;

typedef union HWFAULT_REG_4 {
    Uint16  all;
    HWfaultBits_4  bit;
}HWfaultREG_4;

//HWfaultBits_4
typedef struct HWFAULT_BITS_5 {                    // bits description
    Uint16 EepromWrite:1;
	Uint16 EepromRead:1;
    Uint16 rsvd1:14;
}HWfaultBits_5;

typedef union HWFAULT_REG_5 {
    Uint16  all;
    HWfaultBits_5  bit;
}HWfaultREG_5;


//FAULT_REGS
typedef struct FAULT_REGS {
    HWfaultREG_1                        HWfaultLoad;
    HWfaultREG_2                        HWfaultLoadAvg;
	HWfaultREG_3						HWfaultTemp;
	HWfaultREG_4						UnitSync;
	HWfaultREG_4						MOduleSync;
	HWfaultREG_4						CabinetSync;
	HWfaultREG_5						HwDriver;
	
}FAULTREGS;
fault_EXT FAULTREGS FaultRegs;

//warning
//WARN_BITS_1
typedef struct WARN_BITS_1 {                    // bits description
    Uint16 SinkOT:1;
    Uint16 AmbientOT:1;
    Uint16 AmbientNTCerr:1;
	Uint16 SinkNTCerr:1;
	Uint16 SinkNTCOT:1;
	Uint16 OverLoad:1;
	Uint16 Hw24Vabnormal:1;
	Uint16 rsvd1:9;
}WarnBits_1;

typedef union WARN_REG_1 {
    Uint16  all;
    WarnBits_1  bit;
}WarnREG_1;

//WARN_BITS_2
typedef struct WARN_BITS_2 {                    // bits description
    Uint16 PWMSyncLoss:1;
	Uint16 PWMLockLoss:1;
	Uint16 LineSyncLoss:1;
	Uint16 LineLockLoss:1;
	Uint16 BusOver:1;
	Uint16 BusUnder:1;
	Uint16 LoadPosAvgOV:1;
	Uint16 LoadNegAvgOV:1;
	Uint16 LoadPosAvgOC:1;
	Uint16 LoadNegAvgOC:1;
	Uint16 rsvd1:6;
}WarnBits_2;

typedef union WARN_REG_2 {
    Uint16  all;
    WarnBits_2  bit;
}WarnREG_2;



//WARN_REGS
typedef struct WARN_REGS {
    WarnREG_1                        TempWarn;
    WarnREG_2                        UnitSync;
}WARNREGS;
fault_EXT WARNREGS WarnRegs;

fault_EXT Uint16 wFaultCode;
fault_EXT Uint32 wWarningCode;


/********************************************************************************
* function declaration                              *
********************************************************************************/
fault_EXT void sSetFaultCode(Uint16 wFaultCodeTemp);
fault_EXT void sClrFaultCode(void);				
fault_EXT void sSetISRFaultCode(Uint16 wFaultCodeTemp);
fault_EXT void sClrISRFaultCode(void);											
fault_EXT Uint16 sGetFaultCode(void);
fault_EXT void sSetWarningCode(Uint32 dwWarningCodeBit);	
fault_EXT void sClrWarningCode(Uint32 dwWarningCodeBit);	
fault_EXT Uint32 swGetWarningCode(void);					
fault_EXT void sSetISRWarningCode(Uint32 dwWarningCodeBit);	
fault_EXT void sClrISRWarningCode(Uint32 dwWarningCodeBit);	
fault_EXT Uint32 swISRGetWarningCode(void);					




#endif
