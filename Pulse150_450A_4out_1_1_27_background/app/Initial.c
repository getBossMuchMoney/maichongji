/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  :  Pulse 150A/450A
 *
 *  FILENAME : c.c
 *  PURPOSE  :
 *  AUTHOR   : Jiangbiguang
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023-11-27       V1.0                          Created.
 *
 *============================================================================*/
#define Initial_GLOBALS  1
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
#include "ParallelTask.h"
#include "sciframe.h"

#include "HMITask.h"
#include "OsConfig.h"
#include "Constant.h"
#include "Common.h"
//#include "scimodule.h"
#include "Version.h"
#include "sciremap.h"
#include "f280013x_device.h"
#include "f280013x_epwm_defines.h"
#include "f280013x_adc_defines.h"
#include "string.h"
#include "eeprommodule.h"
//#include "f280013x_adc.h"




#include "Initial.h"
#include "SyncProc.h"


/********************************************************************************
*const define                               *
********************************************************************************/
const int SwitchPeriod[4] = {SwitchPeriod25u,SwitchPeriod12u5,SwitchPeriod8u33,SwitchPeriod6u25};
int CompMode = CBCmode;


/********************************************************************************
* Variable declaration                              *
********************************************************************************/
extern const stXparallel UnitParaInit;
extern const stOverCheck LoadVoltPosAvgInit;
extern const stOverCheck LoadVoltNegAvgInit;
extern const stOverCheck LoadCurrPosAvgInit;
extern const stOverCheck LoadCurrNegAvgInit;	

//float fPosCurrBuf[800];
//float fNegCurrBuf[800];

/********************************************************************************
* function declaration                              *
********************************************************************************/
extern void SetVREF(int module, int mode, int ref);
void configCPUTimer(uint32_t cpuTimer, uint32_t period);


/*********************************************************************
Function name:  configCPUTimer
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void configCPUTimer(uint32_t cpuTimer, uint32_t period)
{
	uint32_t temp, freq = 12;

	//
	// Initialize timer period:
	//
	temp = (freq  * period);
	CPUTimer_setPeriod(cpuTimer, temp);

	//
	// Set pre-scale counter to divide by 1 (SYSCLKOUT):
	//
	CPUTimer_setPreScaler(cpuTimer, 0);

	//
	// Initializes timer control register. The timer is stopped, reloaded,
	// free run disabled, and interrupt enabled.
	// Additionally, the free and soft bits are set
	//
	CPUTimer_stopTimer(cpuTimer);
	CPUTimer_reloadTimerCounter(cpuTimer);
	CPUTimer_setEmulationMode(cpuTimer,
							  CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
	CPUTimer_enableInterrupt(cpuTimer);

}


//-------------------------------------------------------------------------------------------------
/*********************************************************************
Function name:  setupTimer1
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
//-------------------------------------------------------------------------------------------------
#if configGENERATE_RUN_TIME_STATS
void configRunTimerTick( void )
{
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 119);
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER0_BASE);
}

uint32_t GetRunTimerTick(void)
{
  return (0xffffffff - CPUTimer_getTimerCount(CPUTIMER0_BASE));
}
#endif

void Timer0Init( uint32_t ns100)
{
	Interrupt_register(INT_TIMER0, &Timer0Isr);

    configCPUTimer(CPUTIMER0_BASE, ns100);

    Interrupt_enable(INT_TIMER0);
    CPUTimer_stopTimer(CPUTIMER0_BASE);
}
void Timer0ReSet(uint32_t period)
{	
	uint32_t temp, freq = 12;

    //
    // Initialize timer period:
    //
    
    temp = (freq * period);
	CpuTimer0Regs.PRD.all = temp;
	CpuTimer0Regs.TCR.bit.TSS = 1;
	CpuTimer0Regs.TCR.bit.TRB = 1;
	CpuTimer0Regs.TCR.bit.TSS = 0;
}

void Timer0Stop()
{
//	CPUTimer_stopTimer(CPUTIMER0_BASE);
//	CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
	CpuTimer0Regs.TCR.bit.TSS = 1;
	CpuTimer0Regs.TCR.bit.TRB = 1;
}

void Timer1Init( uint32_t ns100)
{

//    Interrupt_register(INT_TIMER1, &TaskTick_ISR);
	Interrupt_register(INT_TIMER1, &Timer1Isr);

    configCPUTimer(CPUTIMER1_BASE, ns100);

    Interrupt_enable(INT_TIMER1);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
}

void Timer1ReSet(uint32_t period)
{	
	uint32_t temp, freq = 12;

    //
    // Initialize timer period:
    //
    
    temp = (freq * period);
	CpuTimer1Regs.PRD.all = temp;
	CpuTimer1Regs.TCR.bit.TSS = 1;
	CpuTimer1Regs.TCR.bit.TRB = 1;
	CpuTimer1Regs.TCR.bit.TSS = 0;
}

void Timer1Stop()
{
//	CPUTimer_stopTimer(CPUTIMER1_BASE);
//	CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
	CpuTimer1Regs.TCR.bit.TSS = 1;
	CpuTimer1Regs.TCR.bit.TRB = 1;
}
//uint16_t Timer2StopFlag = 0;
//void Timer2Stop()
//{
//	if(CpuTimer2Regs.TCR.bit.TIE == 0)
//		return;
////	CPUTimer_stopTimer(CPUTIMER2_BASE);
////	CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);
//	CpuTimer2Regs.TCR.bit.TSS = 1;
//	CpuTimer2Regs.TCR.bit.TRB = 1;
//	CpuTimer2Regs.TCR.bit.TIE = 0;
//	CpuTimer2Regs.TCR.bit.TIF = 1;
//	CpuTimer2Regs.TCR.bit.TSS = 0;
//	
//
//	IFR &= ~M_INT14;
//	Timer2StopFlag = 1;
//}
//
//void Timer2Resume()
//{
//	if(Timer2StopFlag)
//	{
//	//	CpuTimer2Regs.TCR.bit.TSS = 0;
//		CpuTimer2Regs.TCR.bit.TIE = 1;
//		Timer2StopFlag = 0;
//	}
//}

/*********************************************************************
Function name:  Timer2Init
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
//void Timer2Init( void )
//{
//    //
//    // Initialize timer period:
//    //
//    CPUTimer_setPeriod(CPUTIMER2_BASE, configCPU_CLOCK_HZ / Background_RATE_HZ);
//
//    //
//    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
//    //
//    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);
//
//    //
//    // Initializes timer control register. The timer is stopped, reloaded,
//    // free run disabled, and interrupt enabled.
//    // Additionally, the free and soft bits are set
//    //
//    CPUTimer_stopTimer(CPUTIMER2_BASE);
//    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);
//    CPUTimer_setEmulationMode(CPUTIMER2_BASE,
//                              CPUTIMER_EMULATIONMODE_RUNFREE);
//
//    //
//    // Enable interrupt and start timer
//    //
//    CPUTimer_enableInterrupt(CPUTIMER2_BASE);
//    Interrupt_register(INT_TIMER2, &Timer2Isr);
//    Interrupt_enable(INT_TIMER2);
//    CPUTimer_startTimer(CPUTIMER2_BASE);
//}

void Timer2Init( uint32_t ns100)
{

//    Interrupt_register(INT_TIMER1, &TaskTick_ISR);
	Interrupt_register(INT_TIMER2, &Timer2Isr);

    configCPUTimer(CPUTIMER2_BASE, ns100);

    Interrupt_enable(INT_TIMER2);
    CPUTimer_stopTimer(CPUTIMER2_BASE);
}
void Timer2ReSet(uint32_t period)
{	
	uint32_t temp, freq = 12;

    //
    // Initialize timer period:
    //
    
    temp = (freq * period);
	CpuTimer2Regs.PRD.all = temp;
	CpuTimer2Regs.TCR.bit.TSS = 1;
	CpuTimer2Regs.TCR.bit.TRB = 1;
	CpuTimer2Regs.TCR.bit.TSS = 0;
}

void Timer2Stop()
{
//	CPUTimer_stopTimer(CPUTIMER1_BASE);
//	CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
	CpuTimer2Regs.TCR.bit.TSS = 1;
	CpuTimer2Regs.TCR.bit.TRB = 1;
}

uint16_t Timer2IsStart()
{
	if(CpuTimer2Regs.TCR.bit.TSS == 0)
		return 1;
	else
		return 0;
}
/*********************************************************************
Function name:  setepwm1
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
/*
static void setepwm1( void )
{
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBasePeriod(EPWM1_BASE, cControlperiod);
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_enableSyncOutPulseSource(EPWM1_BASE,EPWM_SYNC_OUT_PULSE_ON_CNTR_ZERO);

    EPWM_disablePhaseShiftLoad(EPWM1_BASE);
    EPWM_setPhaseShift(EPWM1_BASE, 0);

    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,EPWM_COUNTER_COMPARE_A,EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,EPWM_COUNTER_COMPARE_B,EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setActionQualifierAction(EPWM1_BASE,EPWM_AQ_OUTPUT_A,EPWM_AQ_OUTPUT_HIGH,EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);

    EPWM_setDeadBandCounterClock(EPWM1_BASE,EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);
    EPWM_setRisingEdgeDeadBandDelayInput(EPWM1_BASE,EPWM_DB_INPUT_EPWMA);
    EPWM_setDeadBandDelayPolarity(EPWM1_BASE,EPWM_DB_RED,EPWM_DB_POLARITY_ACTIVE_HIGH);
    //EPWM_setDeadBandDelayPolarity(EPWM1_BASE,EPWM_DB_FED,EPWM_DB_POLARITY_ACTIVE_HIGH);
    EPWM_setRisingEdgeDelayCount(EPWM1_BASE,50);
    EPWM_setFallingEdgeDelayCount(EPWM1_BASE,50);

    EPWM_enableInterrupt(EPWM1_BASE);
    //EPWM_disableInterrupt(EPWM1_BASE);
    EPWM_enableADCTrigger(EPWM1_BASE,EPWM_SOC_A);
    EPWM_setADCTriggerSource(EPWM1_BASE,EPWM_SOC_A,EPWM_SOC_TBCTR_ZERO);

    EPWM_setInterruptSource(EPWM1_BASE, EPWM_INT_TBCTR_ZERO);
    EPWM_setInterruptEventCount(EPWM1_BASE, 1);
}
*/

/*********************************************************************
Function name:  setepwm2
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
/*
static void setepwm2( void )
{

}
*/

/*********************************************************************
Function name:  InitialDSP
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitialDSP( void )
{
    //
    // Initializes device clock and peripherals
    //
    Device_init();
	
	//
	// Initialize GPIO
	//
	sInitGpio();

    // Disable CPU interrupts
    DINT;
    // Initializes PIE and clears PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    //   IER = 0x0000;
    //   IFR = 0x0000;

     // Initializes the PIE vector table with pointers to the shell Interrupt
     // Service Routines (ISR).
     //
     Interrupt_initVectorTable();


	
    InitI2C();
	AddressCheck();
	LoadEeprom();

    sInitEPWM();

    sInitADC();

	sInitECAP();

    InitSCIAll();


//    InitCAN();

    InitCMPSS();

    Uart_remap();
    versionReg();

	Timer0Init(10000);
	Timer1Init(10000);
	Timer2Init(10000);

 //setepwm1();
 // Interrupt_register(INT_EPWM1, &epwm1ISR);
 // Interrupt_enable(INT_EPWM1);
 // Interrupt_enable(INT_ADCA1);

  // Map ISR functions
     //
     EALLOW;
     PieVectTable.ADCA1_INT = &ADC_A1_ISR;     // Function for ADCA interrupt 1
     PieVectTable.ECAP1_INT = &PWMsync_CAP1_ISR;     // Function for ADCA interrupt 1
     PieVectTable.ECAP2_INT = &Linesync_CAP2_ISR;     // Function for ADCA interrupt 1
 	 PieVectTable.EPWM2_TZ_INT = &EPWM2_TZ1_ISR;     //
 	 PieVectTable.EPWM1_INT = &epwm1ISR;     //
 	 PieVectTable.CANA0_INT = &canISR;     // Function for CAN0 interrupt 1
 	 PieVectTable.EPWM4_INT = &epwm4ISR;     // Function for EPWM interrupt 1
 	 PieVectTable.EPWM5_INT = &epwm5ISR;     // Function for EPWM interrupt 1
     EDIS;
     //
      // Enable global Interrupts and higher priority real-time debug events:
      //
      IER |= M_INT1 |M_INT2 | M_INT3 | M_INT4 | M_INT9;  // Enable group 1 ,4 interrupts
	  IFR = 0x0000U;
      EINT;           // Enable Global interrupt INTM
      ERTM;           // Enable Global realtime interrupt DBGM

      //
         // Enable PIE interrupt
         //
   // PieCtrlRegs.PIEIER1.bit.INTx1 = 1;//Enable PIE group 1 interrupt 1 for ADCA1	
   //  PieCtrlRegs.PIEIER4.bit.INTx1 = 1;//Enable PIE group 4 interrupt 1 for CAP1,PWM SYNC
   //  PieCtrlRegs.PIEIER4.bit.INTx2 = 1;//Enable PIE group 4 interrupt 2 for CAP2,Line SYNC
//    PieCtrlRegs.PIEIER2.bit.INTx2 = 1;//Enable PIE group 2 interrupt 2 for EPWM2_TZ
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;//Enable PIE group3 interrupt 1 for EPWM1
    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;//Enable PIE group 9 interrupt 5 for CAN0s
    
    PieCtrlRegs.PIEIER3.bit.INTx5 = 1;//Enable PIE group 3 interrupt 5 for EPWM5

	
}

/*********************************************************************
Function name:  InitEPWM
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sInitEPWM(void)
{
	   InitEPWM1();	   
	   InitEPWM2();
	   InitEPWM3();
	   InitEPWM4();
	   InitEPWM5();
	   InitEPWM6();
	   InitEPWM7();
}


/*********************************************************************
Function name:  InitADC
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sInitADC(void)
{
	//
	// Setup VREF as internal
	//
   ADC_setVREF(ADCA_BASE,ADC_REFERENCE_EXTERNAL,ADC_REFERENCE_3_3V);
   ADC_setVREF(ADCC_BASE,ADC_REFERENCE_EXTERNAL,ADC_REFERENCE_3_3V);

	EALLOW;

	//
	// Set ADCCLK divider to /4
	//

	ADC_setPrescaler(ADCA_BASE,ADC_CLK_DIV_4_0);
	ADC_setPrescaler(ADCC_BASE,ADC_CLK_DIV_4_0);

	//
	// Set pulse positions to late
	//
	ADC_setInterruptPulseMode(ADCA_BASE,ADC_PULSE_END_OF_CONV);
	ADC_setInterruptPulseMode(ADCC_BASE,ADC_PULSE_END_OF_CONV);


	//
	// Power up the ADC and then delay for 1 ms
	//
	ADC_enableConverter(ADCA_BASE);
	ADC_enableConverter(ADCC_BASE);
	EDIS;

	DEVICE_DELAY_US(1000);

    //
    // Select the channels to convert and the end of conversion flag
    //
    EALLOW;
	//ADC_setupSOC(ADCA_BASE,ADC_SOC_NUMBER0,ADC_TRIGGER_EPWM1_SOCA,ADC_CH_ADCIN1,10);
//ADCA

	AdcaRegs.ADCSOC0CTL.bit.CHSEL = 1;     //PosCurrentSample,SOC0 will convert pin A1,AdcaResult0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5;   // Trigger on ePWM1 SOCA

	AdcaRegs.ADCSOC1CTL.bit.CHSEL = 0xF;	 //PosCurrentSample2// SOC1 will convert pin A15,AdcaResult1
	AdcaRegs.ADCSOC1CTL.bit.ACQPS = 9;	   // Sample window is 10 SYSCLK cycles
	AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 5;   // Trigger on ePWM1 SOCA
	
	AdcaRegs.ADCSOC2CTL.bit.CHSEL = 4;     //SmallCurrentSample// SOC2 will convert pin A4,AdcaResult2
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 5;   // Trigger on ePWM1 SOCA

	AdcaRegs.ADCSOC3CTL.bit.CHSEL = 5;     //BusVoltSample// SOC3 will convert pin A5,AdcaResult3
    AdcaRegs.ADCSOC3CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 5;   // Trigger on ePWM1 SOCA

	AdcaRegs.ADCSOC4CTL.bit.CHSEL = 0xC;     //SinkTemperatureSample// SOC4 will convert pin A12,AdcaResult4
    AdcaRegs.ADCSOC4CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = 5;   // Trigger on ePWM1 SOCA


//ADCC

	AdccRegs.ADCSOC0CTL.bit.CHSEL = 0;     //NegCurrentSample,SOC0 will convert pin C0,AdcCResult0
    AdccRegs.ADCSOC0CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 5;   // Trigger on ePWM1 SOCA

	GpioCtrlRegs.GPHAMSEL.bit.GPIO227 = 1;  
	AnalogSubsysRegs.AGPIOCTRLH.bit.GPIO227 = 1;
	AdccRegs.ADCSOC1CTL.bit.CHSEL = 8;	   //NegCurrentSample2// SOC1 will convert pin C8,AdcCResult1
	AdccRegs.ADCSOC1CTL.bit.ACQPS = 9;	 // Sample window is 10 SYSCLK cycles
	AdccRegs.ADCSOC1CTL.bit.TRIGSEL = 5;	 // Trigger on ePWM1 SOCA

	
	AdccRegs.ADCSOC2CTL.bit.CHSEL = 0xF;	  //OutPutVoltSample// SOC2 will convert pin C15,AdcCResult2
	AdccRegs.ADCSOC2CTL.bit.ACQPS = 9;	 // Sample window is 10 SYSCLK cycles
	AdccRegs.ADCSOC2CTL.bit.TRIGSEL = 5;	 // Trigger on ePWM1 SOCA

	GpioCtrlRegs.GPHAMSEL.bit.GPIO224 = 1; 
	AnalogSubsysRegs.AGPIOCTRLH.bit.GPIO224 = 1;
	AdccRegs.ADCSOC3CTL.bit.CHSEL = 9;     //AmbientTemperatureSample, SOC3 will convert pin C9,AdcCResult3
    AdccRegs.ADCSOC3CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC3CTL.bit.TRIGSEL = 5;   // Trigger on ePWM1 SOCA


    //-------------------------------------------------
    // OutPutVoltSample 不同触发源多次触发采样
    //-------------------------------------------------
   	AdccRegs.ADCSOC4CTL.bit.CHSEL = 0xF;	    // OutPutVoltSample,SOC4 will convert pin C15
	AdccRegs.ADCSOC4CTL.bit.ACQPS = 9;	        // Sample window is 10 SYSCLK cycles
	AdccRegs.ADCSOC4CTL.bit.TRIGSEL = 0x06;     // 06h ADCTRIG6 - ePWM1, ADCSOCB

	AdccRegs.ADCSOC5CTL.bit.CHSEL = 0xF;        // OutPutVoltSample,SOC5 will convert pin C15
    AdccRegs.ADCSOC5CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC5CTL.bit.TRIGSEL = 0x10;     // 10h ADCTRIG16 - ePWM6, ADCSOCB

	AdccRegs.ADCSOC6CTL.bit.CHSEL = 0xF;        // OutPutVoltSample,SOC6 will convert pin C15
    AdccRegs.ADCSOC6CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC6CTL.bit.TRIGSEL = 0x11;     // 11h ADCTRIG17 - ePWM7, ADCSOCA

    AdcaRegs.ADCSOC5CTL.bit.CHSEL = 0;          // OutPutVoltSample,SOC5 will convert pin A0
    AdcaRegs.ADCSOC5CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = 0x08;     // 08h ADCTRIG8 - ePWM2, ADCSOCB

    AdcaRegs.ADCSOC6CTL.bit.CHSEL = 0;          // OutPutVoltSample,SOC6 will convert pin A0
    AdcaRegs.ADCSOC6CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC6CTL.bit.TRIGSEL = 0x09;     // 09h ADCTRIG9 - ePWM3, ADCSOCA

	
    //-------------------------------------------------
    // PosCurrentSample 不同触发源多次触发采样
    //-------------------------------------------------
    AdcaRegs.ADCSOC7CTL.bit.CHSEL = 1;          // PosCurrentSample,SOC7 will convert pin A1
    AdcaRegs.ADCSOC7CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC7CTL.bit.TRIGSEL = 0x07;     // 07h ADCTRIG7 - ePWM2, ADCSOCA

    AdcaRegs.ADCSOC8CTL.bit.CHSEL = 1;          // PosCurrentSample,SOC8 will convert pin A1
    AdcaRegs.ADCSOC8CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC8CTL.bit.TRIGSEL = 0x08;     // 08h ADCTRIG8 - ePWM2, ADCSOCB

    AdcaRegs.ADCSOC9CTL.bit.CHSEL = 1;          // PosCurrentSample,SOC9 will convert pin A1
    AdcaRegs.ADCSOC9CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC9CTL.bit.TRIGSEL = 0x09;     // 09h ADCTRIG9 - ePWM3, ADCSOCA

    AdcaRegs.ADCSOC10CTL.bit.CHSEL = 1;         // PosCurrentSample,SOC10 will convert pin A1
    AdcaRegs.ADCSOC10CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC10CTL.bit.TRIGSEL = 0x0A;    // 0Ah ADCTRIG10 - ePWM3, ADCSOCB

    AdcaRegs.ADCSOC11CTL.bit.CHSEL = 1;         // PosCurrentSample,SOC11 will convert pin A1
    AdcaRegs.ADCSOC11CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC11CTL.bit.TRIGSEL = 0x06;    // 06h ADCTRIG6 - ePWM1, ADCSOCB

    AdcaRegs.ADCSOC12CTL.bit.CHSEL = 1;         // PosCurrentSample,SOC12 will convert pin A1
    AdcaRegs.ADCSOC12CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC12CTL.bit.TRIGSEL = 0x0F;    // 0Fh ADCTRIG15 - ePWM6, ADCSOCA

    AdcaRegs.ADCSOC13CTL.bit.CHSEL = 1;         // PosCurrentSample,SOC13 will convert pin A1
    AdcaRegs.ADCSOC13CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC13CTL.bit.TRIGSEL = 0x10;    // 10h ADCTRIG16 - ePWM6, ADCSOCB

    AdcaRegs.ADCSOC14CTL.bit.CHSEL = 1;         // PosCurrentSample,SOC14 will convert pin A1
    AdcaRegs.ADCSOC14CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC14CTL.bit.TRIGSEL = 0x11;    // 11h ADCTRIG17 - ePWM7, ADCSOCA

    AdcaRegs.ADCSOC15CTL.bit.CHSEL = 1;         // PosCurrentSample,SOC15 will convert pin A1
    AdcaRegs.ADCSOC15CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC15CTL.bit.TRIGSEL = 0x12;    // 12h ADCTRIG18 - ePWM7, ADCSOCB


    //-------------------------------------------------
    // NegCurrentSample 不同触发源多次触发采样
    //-------------------------------------------------
    AdccRegs.ADCSOC7CTL.bit.CHSEL = 0;          // NegCurrentSample,SOC7 will convert pin C0
    AdccRegs.ADCSOC7CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC7CTL.bit.TRIGSEL = 0x07;     // 07h ADCTRIG7 - ePWM2, ADCSOCA

    AdccRegs.ADCSOC8CTL.bit.CHSEL = 0;          // NegCurrentSample,SOC8 will convert pin C0
    AdccRegs.ADCSOC8CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC8CTL.bit.TRIGSEL = 0x08;     // 08h ADCTRIG8 - ePWM2, ADCSOCB

    AdccRegs.ADCSOC9CTL.bit.CHSEL = 0;          // NegCurrentSample,SOC9 will convert pin C0
    AdccRegs.ADCSOC9CTL.bit.ACQPS = 9;          // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC9CTL.bit.TRIGSEL = 0x09;     // 09h ADCTRIG9 - ePWM3, ADCSOCA

    AdccRegs.ADCSOC10CTL.bit.CHSEL = 0;         // NegCurrentSample,SOC10 will convert pin C0
    AdccRegs.ADCSOC10CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC10CTL.bit.TRIGSEL = 0x0A;    // 0Ah ADCTRIG10 - ePWM3, ADCSOCB

    AdccRegs.ADCSOC11CTL.bit.CHSEL = 0;         // NegCurrentSample,SOC11 will convert pin C0
    AdccRegs.ADCSOC11CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC11CTL.bit.TRIGSEL = 0x06;    // 06h ADCTRIG6 - ePWM1, ADCSOCB

    AdccRegs.ADCSOC12CTL.bit.CHSEL = 0;         // NegCurrentSample,SOC12 will convert pin C0
    AdccRegs.ADCSOC12CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC12CTL.bit.TRIGSEL = 0x0F;    // 0Fh ADCTRIG15 - ePWM6, ADCSOCA

    AdccRegs.ADCSOC13CTL.bit.CHSEL = 0;         // NegCurrentSample,SOC13 will convert pin C0
    AdccRegs.ADCSOC13CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC13CTL.bit.TRIGSEL = 0x10;    // 10h ADCTRIG16 - ePWM6, ADCSOCB

    AdccRegs.ADCSOC14CTL.bit.CHSEL = 0;         // NegCurrentSample,SOC14 will convert pin C0
    AdccRegs.ADCSOC14CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC14CTL.bit.TRIGSEL = 0x11;    // 11h ADCTRIG17 - ePWM7, ADCSOCA

    AdccRegs.ADCSOC15CTL.bit.CHSEL = 0;         // NegCurrentSample,SOC15 will convert pin C0
    AdccRegs.ADCSOC15CTL.bit.ACQPS = 9;         // Sample window is 10 SYSCLK cycles
    AdccRegs.ADCSOC15CTL.bit.TRIGSEL = 0x12;    // 12h ADCTRIG18 - ePWM7, ADCSOCB


    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 4;//0; // End of SOC4 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   // Enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Make sure INT1 flag is cleared

    EDIS;




}
/*********************************************************************
Function name:  InitSCI
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitSciA(void)
{
    //
    // Initialize SCIA and its FIFO.
    //
    SCI_performSoftwareReset(SCIA_BASE);

    //
    // Configure SCIA for echoback.
    //
    SCI_setConfig(
            SCIA_BASE, DEVICE_LSPCLK_FREQ, 19200,
            (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(SCIA_BASE);
    SCI_enableModule(SCIA_BASE);
    SCI_performSoftwareReset(SCIA_BASE);
}

void InitSciB(void)
{


    //
    // Initialize SCIB and its FIFO.
    //
    SCI_performSoftwareReset(SCIB_BASE);

    //
    // Configure SCIB for echoback.
    //
    SCI_setConfig(SCIB_BASE, DEVICE_LSPCLK_FREQ, 19200, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIB_BASE);
    SCI_resetRxFIFO(SCIB_BASE);
    SCI_resetTxFIFO(SCIB_BASE);
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(SCIB_BASE);
    SCI_enableModule(SCIB_BASE);
    SCI_performSoftwareReset(SCIB_BASE);
}
void InitSciC(void)
{

    //
    // Initialize SCIC and its FIFO.
    //
    SCI_performSoftwareReset(SCIC_BASE);

    //
    // Configure SCIC for echoback.
    //
    SCI_setConfig(
            SCIC_BASE, DEVICE_LSPCLK_FREQ, 9600,
            (SCI_CONFIG_WLEN_7 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_EVEN));

    SCI_resetChannels(SCIC_BASE);
    SCI_resetRxFIFO(SCIC_BASE);
    SCI_resetTxFIFO(SCIC_BASE);
    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(SCIC_BASE);
    SCI_enableModule(SCIC_BASE);
    SCI_performSoftwareReset(SCIC_BASE);
}
void InitSciToRemote(uint16_t sci)
{
	uint32_t base = 0;
	if(sci == 0)
		base = SCIA_BASE;
	else if(sci == 1)
		base = SCIB_BASE;
	else
		base = SCIC_BASE;
    //
    // Initialize SCIC and its FIFO.
    //
    SCI_performSoftwareReset(base);

    //
    // Configure SCIC for echoback.
    //
//    SCI_setConfig(
//            SCIC_BASE, DEVICE_LSPCLK_FREQ, 9600,
//            (SCI_CONFIG_WLEN_7 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_EVEN));
	SCI_setConfig(base, DEVICE_LSPCLK_FREQ, 19200, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(base);
    SCI_resetRxFIFO(base);
    SCI_resetTxFIFO(base);
    SCI_clearInterruptStatus(base, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(base);
    SCI_enableModule(base);
    SCI_performSoftwareReset(base);
}

/*********************************************************************
Function name:  InitSCIAll
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/

void InitSCIAll()
{
	InitSciA();
	InitSciB();
	InitSciC();
}

/*********************************************************************
Function name:  InitI2C
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitI2C(void)
{
    EALLOW;
    //
    // Must put I2C into reset before configuring it
    //
    I2caRegs.I2CMDR.all &= ~(0x20U);

    //
    // I2C configuration. Use a 400kHz I2CCLK with a 50% duty cycle.
    //
    //I2C_initMaster(base, DEVICE_SYSCLK_FREQ, 400000, I2C_DUTYCYCLE_50);
    //MODULE_CLK = DEVICE_SYSCLK_FREQ/(CPSC+1)
    //BAUD = MODULE_CLK/((CLKH+d)+(CLKL+d)),其中CPSC=0时d=7，CPSC=1时d=6,其他d=5
    I2caRegs.I2CPSC.all = 0x5;        // Prescaler - need 7-12 Mhz on module clk
    I2caRegs.I2CCLKL = 0x14;          // NOTE: must be non zero
    I2caRegs.I2CCLKH = 0x14;           // NOTE: must be non zero

    //
    // Set the bit count to 8 bits per data byte
    //
    I2caRegs.I2CMDR.bit.BC = 0x0U;

    //
    // Set emulation mode to FREE
    //
    I2caRegs.I2CMDR.bit.FREE = 0x1;

    //
    //Clear all status
    //
    I2caRegs.I2CSTR.all = 0xFFFF;

    //
    // Enable I2C Interrupts- RRDY
    //
    I2caRegs.I2CIER.all = 0x08;

    //
    // Take I2C out of reset
    //
    I2caRegs.I2CMDR.all |= 0x0020;

    EDIS;
}
/*********************************************************************
Function name:  InitCAN
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitCAN(void)
{
    //
    // Initialize the CAN controller
    //
    CAN_initModule(CANA_BASE);

    //
    // Set up the CAN bus bit rate to 250kHz
    // Refer to the Driver Library User Guide for information on how to set
    // tighter timing control. Additionally, consult the device data sheet
    // for more information about the CAN module clocking.
    //
    CAN_setBitRate(CANA_BASE, DEVICE_SYSCLK_FREQ, 250000, 20);

    //
    // Enable CAN test mode
    //
//    CAN_enableTestMode(CANA_BASE, CAN_TEST_EXL);

    //
    // Enable interrupts on the CAN peripheral.
    //
    CAN_enableInterrupt(CANA_BASE, CAN_INT_IE0 | CAN_INT_ERROR |
                        CAN_INT_STATUS);

    //
    // Enable the CAN interrupt signal
    //
    CAN_enableGlobalInterrupt(CANA_BASE, CAN_GLOBAL_INT_CANINT0);

    //
    // Initialize the transmit message object used for sending CAN messages.
    // Message Object Parameters:
    //      Message Object ID Number: 1
    //      Message Identifier: 0x1
    //      Message Frame: Standard
    //      Message Type: Transmit
    //      Message ID Mask: 0x0
    //      Message Object Flags: Transmit Interrupt
    //      Message Data Length: 4 Bytes
    //
    CAN_setupMessageObject(CANA_BASE, TX_MSG_OBJ_ID, 0x1, CAN_MSG_FRAME_EXT,
                           CAN_MSG_OBJ_TYPE_TX, 0, CAN_MSG_OBJ_TX_INT_ENABLE,
                           MSG_DATA_LENGTH);

    //
    // Initialize the receive message object used for receiving CAN messages.
    // Message Object Parameters:
    //      Message Object ID Number: 2
    //      Message Identifier: 0x1
    //      Message Frame: Standard
    //      Message Type: Receive
    //      Message ID Mask: 0x0
    //      Message Object Flags: Receive Interrupt
    //      Message Data Length: 4 Bytes (Note that DLC field is a "don't care"
    //      for a Receive mailbox
    //
    CAN_setupMessageObject(CANA_BASE, RX_MSG_OBJ_ID, 0x1, CAN_MSG_FRAME_EXT,
                           CAN_MSG_OBJ_TYPE_RX, 0, CAN_MSG_OBJ_RX_INT_ENABLE,
                           MSG_DATA_LENGTH);

    			
}
/*********************************************************************
Function name:  InitCMPSS1
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitCMPSS1(void)
{
    EALLOW;

	AnalogSubsysRegs.CMPHPMXSEL.bit.CMP1HPMXSEL = 3;//A15/C7,CMP1_HP,PosCurrentSample2

     //
     //Enable CMPSS
     //
     Cmpss1Regs.COMPCTL.bit.COMPDACE = 1;

     //
     //NEG signal comes from DAC
     //
     Cmpss1Regs.COMPCTL.bit.COMPHSOURCE = 0;
     Cmpss1Regs.COMPCTL.bit.COMPLSOURCE = 0;

     // COMP-H-L output is NOT inverted
     Cmpss1Regs.COMPCTL.bit.COMPHINV = 0;
     Cmpss1Regs.COMPCTL.bit.COMPLINV = 0;
     //Disable aynch COMP-H-L ouput
     Cmpss1Regs.COMPCTL.bit.ASYNCHEN = 0;
     Cmpss1Regs.COMPCTL.bit.ASYNCLEN = 0;

     // Configure CTRIPOUT path
         // Digital filter output feeds CTRIPH and CTRIPOUTH
         //
        // High comparator CTRIPH source select.
         //0 Asynchronous comparator output drives CTRIPH
         //1 Synchronous comparator output drives CTRIPH
        // 2 Output of digital filter drives CTRIPH
         //3 Latched output of digital filter drives CTRIPH
     Cmpss1Regs.COMPCTL.bit.CTRIPHSEL = 0;//2;//JBG
     Cmpss1Regs.COMPCTL.bit.CTRIPLSEL = 0;//2;

	 Cmpss1Regs.COMPCTL.bit.CTRIPOUTHSEL = 2;
	 Cmpss1Regs.COMPCTL.bit.CTRIPOUTLSEL = 2;

	

     // Set up COMPHYSCTL register
     Cmpss1Regs.COMPHYSCTL.bit.COMPHYS   = 0; //Comparator hysteresis. Sets the amount of hysteresis on the comparator inputs.
											//0 None
											//1 Set to typical hysteresis,12
											//2 Set to 2x of typical hysteresis,24
											//3 Set to 3x of typical hysteresis,36
											//4 Set to 4x of typical hysteresis,48


    
     Cmpss1Regs.COMPDACHCTL.bit.BLANKEN = 0;//0 EPWMBLANK signal is disabled.
											//1 EPWMBLANK signal is enabled
	 Cmpss1Regs.COMPDACHCTL.bit.DACSOURCE = 0;//0 DACHVALA is updated from DACHVALS
											  //1 DACHVALA is updated from the high ramp generator
	 Cmpss1Regs.COMPDACHCTL.bit.SWLOADSEL = 0;//0 DACxVALA is updated from DACxVALS on SYSCLK
											  //1 DACxVALA is updated from DACxVALS on EPWMSYNCPER

     // Load DACs - High and Low
     Cmpss1Regs.DACHVALS.bit.DACVAL = Cmpss1DACHVALS_Pos_N450A;
     Cmpss1Regs.DACLVALS.bit.DACVAL = 0;


     //
     // Configure Digital Filter
     //Maximum CLKPRESCALE value provides the most time between samples,1MHZ
     //
     Cmpss1Regs.CTRIPHFILCLKCTL = 0;//High filter sample clock prescale. Number of system clocks between
													//samples is CLKPRESCALE+1.
                                                    

     //
     //Maximum SAMPWIN value provides largest number of samples
     //
     Cmpss1Regs.CTRIPHFILCTL.bit.SAMPWIN = 0x1F;
     Cmpss1Regs.CTRIPLFILCTL.bit.SAMPWIN = 0x1F;
     //
     //Maximum THRESH value requires static value for entire window
     //  THRESH should be GREATER than half of SAMPWIN
     //
     Cmpss1Regs.CTRIPHFILCTL.bit.THRESH = 0x1F;
     Cmpss1Regs.CTRIPLFILCTL.bit.THRESH = 0x1F;

     //
     //Reset filter logic & start filtering,Initialize all samples to the filter input value
     //
     Cmpss1Regs.CTRIPHFILCTL.bit.FILINIT = 1;
     Cmpss1Regs.CTRIPLFILCTL.bit.FILINIT = 1;
     //

     // Clear the status register for latched comparator events
     Cmpss1Regs.COMPSTSCLR.bit.HLATCHCLR = 1;
     Cmpss1Regs.COMPSTSCLR.bit.LLATCHCLR = 1;
	 
	 // Low comparator latch EPWMSYNCPER clear. Enable EPWMSYNCPER reset of low comparator digital filter output latch
	 Cmpss1Regs.COMPSTSCLR.bit.HSYNCCLREN = 0;
	 Cmpss1Regs.COMPSTSCLR.bit.LSYNCCLREN = 0;


	OutputXbarRegs.OUTPUTLATCH.bit.OUTPUT5 = 0;
	OutputXbarRegs.OUTPUTLATCHCLR.bit.OUTPUT5 = 1;
	OutputXbarRegs.OUTPUTLATCHENABLE.bit.OUTPUT5 = 0;

    //
    //Enable OUTPUTXBAR1 Mux for Output
    //
    OutputXbarRegs.OUTPUT5MUX0TO15CFG.bit.MUX0 = 0;
    OutputXbarRegs.OUTPUT5MUXENABLE.bit.MUX0 = 1;

	


     EDIS;

}
/*********************************************************************
Function name:  InitCMPSS2
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitCMPSS2(void)
{
    EALLOW;

	AnalogSubsysRegs.CMPHPMXSEL.bit.CMP2HPMXSEL = 2;//A9/C8,CMP2_HP,NegCurrentSample2

     //
     //Enable CMPSS
     //
     CmpssLite2Regs.COMPCTL.bit.COMPDACE = 1;

     //
     //NEG signal comes from DAC
     //
     CmpssLite2Regs.COMPCTL.bit.COMPHSOURCE = 0;
     CmpssLite2Regs.COMPCTL.bit.COMPLSOURCE = 0;

     // COMP-H-L output is NOT inverted
     CmpssLite2Regs.COMPCTL.bit.COMPHINV = 0;
     CmpssLite2Regs.COMPCTL.bit.COMPLINV = 0;
     //Disable aynch COMP-H-L ouput
     CmpssLite2Regs.COMPCTL.bit.ASYNCHEN = 0;
     CmpssLite2Regs.COMPCTL.bit.ASYNCLEN = 0;

     // Configure CTRIPOUT path
         // Digital filter output feeds CTRIPH and CTRIPOUTH
         //
        // High comparator CTRIPH source select.
         //0 Asynchronous comparator output drives CTRIPH
         //1 Synchronous comparator output drives CTRIPH
        // 2 Output of digital filter drives CTRIPH
         //3 Latched output of digital filter drives CTRIPH
     CmpssLite2Regs.COMPCTL.bit.CTRIPHSEL = 2;
     CmpssLite2Regs.COMPCTL.bit.CTRIPLSEL = 2;

	 CmpssLite2Regs.COMPCTL.bit.CTRIPOUTHSEL = 2;
	 CmpssLite2Regs.COMPCTL.bit.CTRIPOUTLSEL = 2;

	

     // Set up COMPHYSCTL register
     CmpssLite2Regs.COMPHYSCTL.bit.COMPHYS   = 1; //Comparator hysteresis. Sets the amount of hysteresis on the comparator inputs.
											//0 None
											//1 Set to typical hysteresis,12
											//2 Set to 2x of typical hysteresis,24
											//3 Set to 3x of typical hysteresis,36
											//4 Set to 4x of typical hysteresis,48


    
     CmpssLite2Regs.COMPDACHCTL.bit.BLANKEN = 0;//0 EPWMBLANK signal is disabled.
											//1 EPWMBLANK signal is enabled

	 CmpssLite2Regs.COMPDACHCTL.bit.SWLOADSEL = 0;//0 DACxVALA is updated from DACxVALS on SYSCLK
											  //1 DACxVALA is updated from DACxVALS on EPWMSYNCPER

     // Load DACs - High and Low
     CmpssLite2Regs.DACHVALS.bit.DACVAL = Cmpss2DACHVALS_Neg_N450A;
     CmpssLite2Regs.DACLVALS.bit.DACVAL = 0;


     //
     // Configure Digital Filter
     //Maximum CLKPRESCALE value provides the most time between samples,1MHZ
     //
     CmpssLite2Regs.CTRIPHFILCLKCTL = 0;//High filter sample clock prescale. Number of system clocks between
													//samples is CLKPRESCALE+1.
                                                    

     //
     //Maximum SAMPWIN value provides largest number of samples
     //
     CmpssLite2Regs.CTRIPHFILCTL.bit.SAMPWIN = 0x1F;
     CmpssLite2Regs.CTRIPLFILCTL.bit.SAMPWIN = 0x1F;
     //
     //Maximum THRESH value requires static value for entire window
     //  THRESH should be GREATER than half of SAMPWIN
     //
     CmpssLite2Regs.CTRIPHFILCTL.bit.THRESH = 0x1F;
     CmpssLite2Regs.CTRIPLFILCTL.bit.THRESH = 0x1F;

     //
     //Reset filter logic & start filtering,Initialize all samples to the filter input value
     //
     CmpssLite2Regs.CTRIPHFILCTL.bit.FILINIT = 1;
     CmpssLite2Regs.CTRIPLFILCTL.bit.FILINIT = 1;
     //

     // Clear the status register for latched comparator events
     CmpssLite2Regs.COMPSTSCLR.bit.HLATCHCLR = 1;
     CmpssLite2Regs.COMPSTSCLR.bit.LLATCHCLR = 1;
	 
	 // Low comparator latch EPWMSYNCPER clear. Enable EPWMSYNCPER reset of low comparator digital filter output latch
	 CmpssLite2Regs.COMPSTSCLR.bit.HSYNCCLREN = 0;
	 CmpssLite2Regs.COMPSTSCLR.bit.LSYNCCLREN = 0;


//	OutputXbarRegs.OUTPUTLATCH.bit.OUTPUT1 = 0;
//	OutputXbarRegs.OUTPUTLATCHCLR.bit.OUTPUT1 = 1;
//	OutputXbarRegs.OUTPUTLATCHENABLE.bit.OUTPUT1 = 0;

//    //
//    //Enable OUTPUTXBAR1 Mux for Output
//    //
//    OutputXbarRegs.OUTPUT1MUX0TO15CFG.bit.MUX2 = 0;
//    OutputXbarRegs.OUTPUT1MUXENABLE.bit.MUX2 = 1;

	


     EDIS;

}


/*********************************************************************
Function name:  InitCMPSS3
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitCMPSS3(void)
{
    EALLOW;

     //
     //Enable CMPSS
     //
     CmpssLite3Regs.COMPCTL.bit.COMPDACE = 1;

     //
     //NEG signal comes from DAC
     //
     CmpssLite3Regs.COMPCTL.bit.COMPHSOURCE = 0;
     CmpssLite3Regs.COMPCTL.bit.COMPLSOURCE = 0;

     // COMP-H-L output is NOT inverted
     CmpssLite3Regs.COMPCTL.bit.COMPHINV = 0;
     CmpssLite3Regs.COMPCTL.bit.COMPLINV = 0;
     //Disable aynch COMP-H-L ouput
     CmpssLite3Regs.COMPCTL.bit.ASYNCHEN = 0;
     CmpssLite3Regs.COMPCTL.bit.ASYNCLEN = 0;

     // Configure CTRIPOUT path
         // Digital filter output feeds CTRIPH and CTRIPOUTH
         //
        // High comparator CTRIPH source select.
         //0 Asynchronous comparator output drives CTRIPH
         //1 Synchronous comparator output drives CTRIPH
        // 2 Output of digital filter drives CTRIPH
         //3 Latched output of digital filter drives CTRIPH
     CmpssLite3Regs.COMPCTL.bit.CTRIPHSEL = 2;
     CmpssLite3Regs.COMPCTL.bit.CTRIPLSEL = 2;

	 CmpssLite3Regs.COMPCTL.bit.CTRIPOUTHSEL = 2;
	 CmpssLite3Regs.COMPCTL.bit.CTRIPOUTLSEL = 2;

	

     // Set up COMPHYSCTL register
     CmpssLite3Regs.COMPHYSCTL.bit.COMPHYS   = 1; //Comparator hysteresis. Sets the amount of hysteresis on the comparator inputs.
											//0 None
											//1 Set to typical hysteresis,12
											//2 Set to 2x of typical hysteresis,24
											//3 Set to 3x of typical hysteresis,36
											//4 Set to 4x of typical hysteresis,48


     //Use VDDA as the reference for DAC
//     CmpssLite3Regs.COMPDACCTL.bit.SELREF = 0;

//     CmpssLite3Regs.COMPDACCTL.bit.SWLOADSEL = 1; //Software load select. Determines whether DACxVALA is updated
//												//from DACxVALS on SYSCLK or EPWMSYNCPER.
//												//0 DACxVALA is updated from DACxVALS on SYSCLK
//												//1 DACxVALA is updated from DACxVALS on EPWMSYNCPER
//												//Reset type: SYSRSn
//     CmpssLite3Regs.COMPDACCTL.bit.DACSOURCE = 0; // Ramp ENABLE 
//	 CmpssLite3Regs.COMPDACCTL.bit.RAMPSOURCE = 1;//Ramp generator source select. Determines which EPWMSYNCPER,EPWM2SYNCPER
//	 CmpssLite3Regs.COMPDACCTL.bit.RAMPLOADSEL = 1;//Ramp load select. Determines whether RAMPSTS is updated from
//												//RAMPMAXREFA or RAMPMAXREFS when
//												//COMPSTS[COMPHSTS] is triggered.
//												//0 RAMPSTS is loaded from RAMPMAXREFA
//												//1 RAMPSTS is loaded from RAMPMAXREFS
//												//Reset type: SYSRSn

//	CmpssLite3Regs.RAMPDECVALS = 2;


     // Load DACs - High and Low
     CmpssLite3Regs.DACHVALS.bit.DACVAL = 0;
     CmpssLite3Regs.DACLVALS.bit.DACVAL = 0;



     //
     // Configure Digital Filter
     //Maximum CLKPRESCALE value provides the most time between samples,1MHZ
     //
//     CmpssLite3Regs.CTRIPHFILCLKCTL.bit.CLKPRESCALE = 0;//High filter sample clock prescale. Number of system clocks between
//													//samples is CLKPRESCALE+1.
                                                    
//     CmpssLite3Regs.CTRIPLFILCLKCTL.bit.CLKPRESCALE = 0;
     //
     //Maximum SAMPWIN value provides largest number of samples
     //
     CmpssLite3Regs.CTRIPHFILCTL.bit.SAMPWIN = 0x1F;
     CmpssLite3Regs.CTRIPLFILCTL.bit.SAMPWIN = 0x1F;
     //
     //Maximum THRESH value requires static value for entire window
     //  THRESH should be GREATER than half of SAMPWIN
     //
     CmpssLite3Regs.CTRIPHFILCTL.bit.THRESH = 0x1F;
     CmpssLite3Regs.CTRIPLFILCTL.bit.THRESH = 0x1F;

     //
     //Reset filter logic & start filtering,Initialize all samples to the filter input value
     //
     CmpssLite3Regs.CTRIPHFILCTL.bit.FILINIT = 1;
     CmpssLite3Regs.CTRIPLFILCTL.bit.FILINIT = 1;
     //

     // Clear the status register for latched comparator events
     CmpssLite3Regs.COMPSTSCLR.bit.HLATCHCLR = 1;
     CmpssLite3Regs.COMPSTSCLR.bit.LLATCHCLR = 1;
	 
	 // Low comparator latch EPWMSYNCPER clear. Enable EPWMSYNCPER reset of low comparator digital filter output latch
	 CmpssLite3Regs.COMPSTSCLR.bit.HSYNCCLREN = 1;
	 CmpssLite3Regs.COMPSTSCLR.bit.LSYNCCLREN = 1;

	   // Configure CTRIPOUTH output pin
    // Configure OUTPUTXBAR1 to be CTRIPOUT1H
    //
  //  OutputXbarRegs.OUTPUT1MUX0TO15CFG.bit.MUX0 = 0;

    //
    //Enable OUTPUTXBAR1 Mux for Output
    //
  //  OutputXbarRegs.OUTPUT1MUXENABLE.bit.MUX0 = 1;
	


     EDIS;

}
/*********************************************************************
Function name:  InitCMPSS4
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitCMPSS4(void)
{
    EALLOW;

     //
     //Enable CMPSS
     //
     CmpssLite4Regs.COMPCTL.bit.COMPDACE = 1;

     //
     //NEG signal comes from DAC
     //
     CmpssLite4Regs.COMPCTL.bit.COMPHSOURCE = 0;
     CmpssLite4Regs.COMPCTL.bit.COMPLSOURCE = 0;

     // COMP-H-L output is NOT inverted
     CmpssLite4Regs.COMPCTL.bit.COMPHINV = 0;
     CmpssLite4Regs.COMPCTL.bit.COMPLINV = 0;
     //Disable aynch COMP-H-L ouput
     CmpssLite4Regs.COMPCTL.bit.ASYNCHEN = 0;
     CmpssLite4Regs.COMPCTL.bit.ASYNCLEN = 0;

     // Configure CTRIPOUT path
         // Digital filter output feeds CTRIPH and CTRIPOUTH
         //
        // High comparator CTRIPH source select.
         //0 Asynchronous comparator output drives CTRIPH
         //1 Synchronous comparator output drives CTRIPH
        // 2 Output of digital filter drives CTRIPH
         //3 Latched output of digital filter drives CTRIPH
     CmpssLite4Regs.COMPCTL.bit.CTRIPHSEL = 2;
     CmpssLite4Regs.COMPCTL.bit.CTRIPLSEL = 2;

	 CmpssLite4Regs.COMPCTL.bit.CTRIPOUTHSEL = 2;
	 CmpssLite4Regs.COMPCTL.bit.CTRIPOUTLSEL = 2;

	

     // Set up COMPHYSCTL register
     CmpssLite4Regs.COMPHYSCTL.bit.COMPHYS   = 1; //Comparator hysteresis. Sets the amount of hysteresis on the comparator inputs.
											//0 None
											//1 Set to typical hysteresis,12
											//2 Set to 2x of typical hysteresis,24
											//3 Set to 3x of typical hysteresis,36
											//4 Set to 4x of typical hysteresis,48


     //Use VDDA as the reference for DAC
//     CmpssLite4Regs.COMPDACCTL.bit.SELREF = 0;

//     CmpssLite4Regs.COMPDACCTL.bit.SWLOADSEL = 1; //Software load select. Determines whether DACxVALA is updated
//												//from DACxVALS on SYSCLK or EPWMSYNCPER.
//												//0 DACxVALA is updated from DACxVALS on SYSCLK
//												//1 DACxVALA is updated from DACxVALS on EPWMSYNCPER
//												//Reset type: SYSRSn
//     CmpssLite4Regs.COMPDACCTL.bit.DACSOURCE = 0; // Ramp ENABLE 
//	 CmpssLite4Regs.COMPDACCTL.bit.RAMPSOURCE = 1;//Ramp generator source select. Determines which EPWMSYNCPER,EPWM2SYNCPER
//	 CmpssLite4Regs.COMPDACCTL.bit.RAMPLOADSEL = 1;//Ramp load select. Determines whether RAMPSTS is updated from
//												//RAMPMAXREFA or RAMPMAXREFS when
//												//COMPSTS[COMPHSTS] is triggered.
//												//0 RAMPSTS is loaded from RAMPMAXREFA
//												//1 RAMPSTS is loaded from RAMPMAXREFS
//												//Reset type: SYSRSn

//	CmpssLite4Regs.RAMPDECVALS = 2;


     // Load DACs - High and Low
     CmpssLite4Regs.DACHVALS.bit.DACVAL = 0;
     CmpssLite4Regs.DACLVALS.bit.DACVAL = 0;



     //
     // Configure Digital Filter
     //Maximum CLKPRESCALE value provides the most time between samples,1MHZ
     //
//     CmpssLite4Regs.CTRIPHFILCLKCTL.bit.CLKPRESCALE = 0;//High filter sample clock prescale. Number of system clocks between
//													//samples is CLKPRESCALE+1.
                                                    
//     CmpssLite4Regs.CTRIPLFILCLKCTL.bit.CLKPRESCALE = 0;
     //
     //Maximum SAMPWIN value provides largest number of samples
     //
     CmpssLite4Regs.CTRIPHFILCTL.bit.SAMPWIN = 0x1F;
     CmpssLite4Regs.CTRIPLFILCTL.bit.SAMPWIN = 0x1F;
     //
     //Maximum THRESH value requires static value for entire window
     //  THRESH should be GREATER than half of SAMPWIN
     //
     CmpssLite4Regs.CTRIPHFILCTL.bit.THRESH = 0x1F;
     CmpssLite4Regs.CTRIPLFILCTL.bit.THRESH = 0x1F;

     //
     //Reset filter logic & start filtering,Initialize all samples to the filter input value
     //
     CmpssLite4Regs.CTRIPHFILCTL.bit.FILINIT = 1;
     CmpssLite4Regs.CTRIPLFILCTL.bit.FILINIT = 1;
     //

     // Clear the status register for latched comparator events
     CmpssLite4Regs.COMPSTSCLR.bit.HLATCHCLR = 1;
     CmpssLite4Regs.COMPSTSCLR.bit.LLATCHCLR = 1;
	 
	 // Low comparator latch EPWMSYNCPER clear. Enable EPWMSYNCPER reset of low comparator digital filter output latch
	 CmpssLite4Regs.COMPSTSCLR.bit.HSYNCCLREN = 1;
	 CmpssLite4Regs.COMPSTSCLR.bit.LSYNCCLREN = 1;

	   // Configure CTRIPOUTH output pin
    // Configure OUTPUTXBAR1 to be CTRIPOUT1H
    //
  //  OutputXbarRegs.OUTPUT1MUX0TO15CFG.bit.MUX0 = 0;

    //
    //Enable OUTPUTXBAR1 Mux for Output
    //
  //  OutputXbarRegs.OUTPUT1MUXENABLE.bit.MUX0 = 1;
	


     EDIS;

}

/*********************************************************************
Function name:  InitCMPSS
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void InitCMPSS(void)
{
	InitCMPSS1();
	InitCMPSS2();
	InitCMPSS3();
	InitCMPSS4();
	


}


/*********************************************************************
Function name:  sInitGpio
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sInitGpio(void)
{
    volatile Uint32 *gpioBaseAddr;
    Uint16 regOffset;

    //
    // Disable pin locks
    //
    EALLOW;
    GpioCtrlRegs.GPALOCK.all = 0x00000000;
    GpioCtrlRegs.GPBLOCK.all = 0x00000000;
    GpioCtrlRegs.GPHLOCK.all = 0x00000000;

    //
    // Fill all registers with zeros. Writing to each register separately
    // for three GPIO modules would make this function *very* long.
    // Fortunately, we'd be writing them all with zeros anyway,
    // so this saves a lot of space.
    //
    gpioBaseAddr = (Uint32 *)&GpioCtrlRegs;
    for (regOffset = 0; regOffset < sizeof(GpioCtrlRegs)/2; regOffset++)
    {
        //
        // Must avoid enabling pull-ups on all pins. GPyPUD is offset
        // 0x0C in each register group of 0x40 words. Since this is a
        // 32-bit pointer, the addresses must be divided by 2.
        //
        // Also, to avoid changing pin muxing of the emulator pins to regular
        // GPIOs, skip GPBMUX1 (0x46) and GPBGMUX1 (0x60).
        //
        if ((regOffset % (0x40/2) != (0x0C/2)) && (regOffset != (0x46/2)) &&
            (regOffset != (0x60/2)))
        {
            gpioBaseAddr[regOffset] = 0x00000000;
        }
    }

    gpioBaseAddr = (Uint32 *)&GpioDataRegs;
    for (regOffset = 0; regOffset < sizeof(GpioDataRegs)/2; regOffset++)
    {
        gpioBaseAddr[regOffset] = 0x00000000;
    }

    EDIS;

	GPIO_setPinConfig(GPIO_0_EPWM1_A);				// GPIO0 = PWM1A,PWM sync output

	
	GPIO_setPinConfig(GPIO_2_EPWM2_A);				// GPIO2 = PWM2A,positive current
	//GPIO_setPinConfig(GPIO_2_OUTPUTXBAR1);				// GPIO2 = PWM2A,positive current
	GPIO_setPinConfig(GPIO_3_EPWM2_B);				// GPIO3 = PWM2B
	GPIO_setPinConfig(GPIO_4_EPWM3_A);				// GPIO4 = PWM3A,negtive current;
	GPIO_setPinConfig(GPIO_5_EPWM3_B);				// GPIO5 = PWM3B

	//GPIO7,GPIO7 =DO1
	GPIO_setPadConfig(7, GPIO_PIN_TYPE_PULLUP); 	// Enable pullup on GPIO7
	GPIO_writePin(7, 0);							// Load output latch
	GPIO_setPinConfig(GPIO_7_GPIO7);				//GPIO7 =DO1
	
	GPIO_setDirectionMode(7, GPIO_DIR_MODE_OUT);	


	//GPIO41,GPIO41 =DO2
	GPIO_setPadConfig(41, GPIO_PIN_TYPE_PULLUP); 	// Enable pullup on GPIO7
	GPIO_writePin(41, 1);							// Load output latch
	GPIO_setPinConfig(GPIO_41_GPIO41);				//GPIO41 =DO2
	GPIO_setDirectionMode(41, GPIO_DIR_MODE_OUT);

	//GPIO245,GPIO245 =DI1
	EALLOW;
	GpioCtrlRegs.GPHAMSEL.bit.GPIO245 = 0;			
    EDIS;
	GPIO_setPadConfig(245, GPIO_PIN_TYPE_PULLUP); 	// Enable pullup on GPIO7
	GPIO_writePin(245, 1);							// Load output latch
	GPIO_setPinConfig(GPIO_245_GPIO245);			//GPIO245 =DI1
	GPIO_setDirectionMode(245, GPIO_DIR_MODE_IN);

	//GPIO241,GPIO241 =DI2
	EALLOW;
	GpioCtrlRegs.GPHAMSEL.bit.GPIO241 = 0;			
    EDIS;
	GPIO_setPadConfig(241, GPIO_PIN_TYPE_PULLUP); 	// Enable pullup on GPIO7
	GPIO_writePin(241, 1);							// Load output latch
	GPIO_setPinConfig(GPIO_241_GPIO241);			//GPIO241 =DI2
	GPIO_setDirectionMode(241, GPIO_DIR_MODE_IN);

//--------------------------------SCIA begin---------------------------------------------------//

    // GPIO28 is the SCI Rx pin.
    GPIO_setPinConfig(GPIO_28_SCIA_RX);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    // GPIO29 is the SCI Tx pin.
    GPIO_setPinConfig(GPIO_29_SCIA_TX);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

	//GPIO242 = SCIA_Enable,
	EALLOW;
	GpioCtrlRegs.GPHAMSEL.bit.GPIO242 = 0;	
	AnalogSubsysRegs.AGPIOCTRLH.bit.GPIO242 = 0;		
    EDIS;
	GPIO_setPadConfig(242, GPIO_PIN_TYPE_PULLUP);	// Enable pullup on GPIO242
	GPIO_writePin(242, 0); 						// Load output latch
	GPIO_setPinConfig(GPIO_242_GPIO242); 			//GPIO242 = SCIA_Enable,,
	GPIO_setDirectionMode(242, GPIO_DIR_MODE_OUT);

//--------------------------------SCIA end---------------------------------------------------//

//--------------------------------SCIB begin---------------------------------------------------//

    // GPIO11 is the SCI Rx pin.
    GPIO_setPinConfig(GPIO_11_SCIB_RX);
    GPIO_setDirectionMode(11, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(11, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(11, GPIO_QUAL_ASYNC);
//	GPIO_setPinConfig(GPIO_11_GPIO11);
//	GPIO_writePin(11, 1);
//	GPIO_setDirectionMode(11, GPIO_DIR_MODE_OUT);
//    GPIO_setPadConfig(11, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(11, GPIO_QUAL_ASYNC);

    // GPIO9 is the SCI Tx pin.
    GPIO_setPinConfig(GPIO_9_SCIB_TX);
//	GPIO_setPinConfig(GPIO_9_GPIO9);
//	GPIO_writePin(9, 1);

    GPIO_setDirectionMode(9, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(9, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(9, GPIO_QUAL_ASYNC);

    //GPIO16 = SCIB_Enable,
    GPIO_setPadConfig(16, GPIO_PIN_TYPE_PULLUP);	// Enable pullup on GPIO20
    GPIO_writePin(16, 1);						// Load output latch
    GPIO_setPinConfig(GPIO_16_GPIO16);			//GPIO16 = SCIB_Enable,
    GPIO_setDirectionMode(16, GPIO_DIR_MODE_OUT);

//--------------------------------SCIB end---------------------------------------------------//

    //--------------------------------SCIC begin---------------------------------------------------//
    // GPIO23 is the SCI Rx pin.
    GPIO_setPinConfig(GPIO_23_SCIC_RX);
    GPIO_setDirectionMode(23, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(23, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(23, GPIO_QUAL_ASYNC);

    // GPIO32 is the SCI Tx pin.
    GPIO_setPinConfig(GPIO_32_SCIC_TX);
    GPIO_setDirectionMode(32, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(32, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(32, GPIO_QUAL_ASYNC);

    //GPIO20 = SCIC_Enable,
    GPIO_setPadConfig(20, GPIO_PIN_TYPE_PULLUP);	// Enable pullup on GPIO16
    GPIO_writePin(20, 1);						// Load output latch
    GPIO_setPinConfig(GPIO_20_GPIO20);			//GPIO20 = SCIC_Enable,,
    GPIO_setDirectionMode(20, GPIO_DIR_MODE_OUT);

    //--------------------------------SCIC end---------------------------------------------------//

    //-------------------------------- Sync signal ---------------------------------------------------//

			//GPIO6
			//GPIO_setPadConfig(6, GPIO_PIN_TYPE_PULLUP); 	// Enable pullup on GPIO6
			//GPIO_writePin(6, 1);							// Load output latch
			GPIO_setPinConfig(GPIO_6_GPIO6);				//GPIO6 =ECAP1,Line sync input
			GPIO_setDirectionMode(6, GPIO_DIR_MODE_IN); 	
		
			//GPIO22
			//GPIO_setPadConfig(22, GPIO_PIN_TYPE_PULLUP);	// Enable pullup on GPIO22
			//GPIO_writePin(22, 1);							// Load output latch
			GPIO_setPinConfig(GPIO_22_GPIO22);				//GPIO22 =ECAP2,PWM sync input
			GPIO_setDirectionMode(22, GPIO_DIR_MODE_IN);	




	
		//GPIO39 = Line Sync TX,output
		//GPIO_setPadConfig(39, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO39
		//GPIO_writePin(39, 1);						// Load output latch
		SetLineSyncHigh;
		GPIO_setPinConfig(GPIO_39_GPIO39);			//GPIO39 = Line Sync TX,output
		
		GPIO_setDirectionMode(39, GPIO_DIR_MODE_OUT);
		
		SetExtLineSyncHigh;
		GPIO_setPinConfig(GPIO_17_GPIO17);			//GPIO17 = Line Sync TX,output
		GPIO_setDirectionMode(17, GPIO_DIR_MODE_OUT);

		EALLOW;
		GpioCtrlRegs.GPHAMSEL.bit.GPIO230 = 0;	
		AnalogSubsysRegs.AGPIOCTRLH.bit.GPIO230 = 0;		
	    EDIS;
		GPIO_setPadConfig(230, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO230
		GPIO_setPinConfig(GPIO_230_GPIO230);			//GPIO230
		GPIO_setDirectionMode(230, GPIO_DIR_MODE_IN);

	//-------------------------------- LED signal ---------------------------------------------------//
	//GPIO12 = PWM shut //----------------------Power LED
		GPIO_setPadConfig(12, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO12
		GPIO_writePin(12, 1);						// Load output latch
		GPIO_setPinConfig(GPIO_12_GPIO12);			//GPIO12 = PWM shut
		GPIO_setDirectionMode(12, GPIO_DIR_MODE_OUT);
		PWMdrive_OFF();

	//GPIO13 = FAN,reserved ------------------run/stop LED
		GPIO_setPadConfig(13, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO13
		GPIO_writePin(13, 1);						// Load output latch
		GPIO_setPinConfig(GPIO_13_GPIO13);			//GPIO13 = run/stop LED
		GPIO_setDirectionMode(13, GPIO_DIR_MODE_OUT);


	//GPIO37= OH LED
		GPIO_setPadConfig(37, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO37
		GPIO_writePin(37, 1);						// Load output latch
		GPIO_setPinConfig(GPIO_37_GPIO37);			//GPIO37= OH LED
		GPIO_setDirectionMode(37, GPIO_DIR_MODE_OUT);


//	//GPIO35= fault LED
		GPIO_setPadConfig(35, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO35
		GPIO_writePin(35, 1);						// Load output latch
		GPIO_setPinConfig(GPIO_35_GPIO35);			//GPIO35= fault LED
		GPIO_setDirectionMode(35, GPIO_DIR_MODE_OUT);

//-------------------------------- CAN signal ---------------------------------------------------//
    //GPIO17= CANA_TX
//    GPIO_setPinConfig(GPIO_17_CANA_TX);			//GPIO17= CANA_TX
//
//    //GPIO18= CANA_RX
//    GPIO_setAnalogMode(230, GPIO_ANALOG_DISABLED);
//    GPIO_setPinConfig(GPIO_230_CANA_RX);			//GPIO18= CANA_RX
//-------------------------------- I2C signal ---------------------------------------------------//
    //GPIO33= I2CA_SCL
    GPIO_setPadConfig(33, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO33
    GPIO_setQualificationMode(33, GPIO_QUAL_ASYNC);
    //GPIO_writePin(33, 1);						// Load output latch
    GPIO_setPinConfig(GPIO_33_I2CA_SCL);			//GPIO33= I2CA_SCL
    //GPIO_setDirectionMode(18, GPIO_DIR_MODE_OUT);

    //GPIO21= I2CA_SDA
    GPIO_setPadConfig(21, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO21
    GPIO_setQualificationMode(21, GPIO_QUAL_ASYNC);
    //GPIO_writePin(21, 1);						// Load output latch
    GPIO_setPinConfig(GPIO_21_I2CA_SDA);			//GPIO21= I2CA_SDA
    //GPIO_setDirectionMode(18, GPIO_DIR_MODE_OUT);

    //-------------------------------- BOOT MODE signal ---------------------------------------------------//
    //GPIO24= BOOT MODE
    GPIO_setPadConfig(24, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO24
    //GPIO_writePin(24, 1);						// Load output latch
    GPIO_setPinConfig(GPIO_24_GPIO24);			//GPIO24= BOOT MODE
    GPIO_setDirectionMode(24, GPIO_DIR_MODE_IN);

    //-------------------------------- 24V fault signal ---------------------------------------------------//
		EALLOW;
		GpioCtrlRegs.GPHAMSEL.bit.GPIO228 = 0;	
		AnalogSubsysRegs.AGPIOCTRLH.bit.GPIO228 = 0;		
	    EDIS;
	//GPIO228= 24V fault, low active
		GPIO_setPadConfig(228, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO228
		//GPIO_writePin(24, 1);						// Load output latch
		GPIO_setPinConfig(GPIO_228_GPIO228);			//GPIO228= 24V fault, low active
		GPIO_setDirectionMode(228, GPIO_DIR_MODE_IN);
		
	
//--------------------------------FAN DETECT---------------------------------------------------//

	//GPIO10= FAN DETECT
		GPIO_setPadConfig(10, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO10
		//GPIO_writePin(10, 1);						// Load output latch
		GPIO_setPinConfig(GPIO_10_GPIO10);			//GPIO10= FAN DETECT
		GPIO_setDirectionMode(10, GPIO_DIR_MODE_IN);

//-------------------------------- DIP switch---------------------------------------------------/
	//GPIO1= DIP switch4
	GPIO_setPadConfig(1, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO1
	//GPIO_writePin(1, 1);						// Load output latch
	GPIO_setPinConfig(GPIO_1_GPIO1);			//GPIO1= DIP switch4
	GPIO_setDirectionMode(1, GPIO_DIR_MODE_IN);
	GPIO_setQualificationMode(1, GPIO_QUAL_6SAMPLE);

	//GPIO8= DIP switch3
	GPIO_setPadConfig(8, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO8
    //GPIO_writePin(8, 1);                      // Load output latch
	GPIO_setPinConfig(GPIO_8_GPIO8);            // GPIO8= DIP switch3
    GPIO_setDirectionMode(8, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(8, GPIO_QUAL_6SAMPLE);
    
	//GPIO40= DIP switch2
	GPIO_setPadConfig(40, GPIO_PIN_TYPE_PULLUP);// Enable pullup on GPIO7
	//GPIO_writePin(40, 1);			            // Load output latch
	GPIO_setPinConfig(GPIO_40_GPIO40);		    // GPIO40= DIP switch2
	GPIO_setDirectionMode(40, GPIO_DIR_MODE_IN);
	GPIO_setQualificationMode(40, GPIO_QUAL_6SAMPLE);

	//GPIO239= DIP switch1
	EALLOW;
	GpioCtrlRegs.GPHAMSEL.bit.GPIO239 = 0;
	//AnalogSubsysRegs.AGPIOCTRLH.bit.GPIO239 = 0;
	EDIS;
	GPIO_setPadConfig(239, GPIO_PIN_TYPE_PULLUP);   // Enable pullup on GPIO226
	//GPIO_setPinConfig(GPIO_239_GPIO239);            // GPIO239= DIP switch1
	//GPIO_setDirectionMode(239, GPIO_DIR_MODE_IN);
	GPIO_setQualificationMode(239, GPIO_QUAL_6SAMPLE);


	//-------------------------------- load current P/N over protect TZ ----------------------------------------
	//GPIO226= Tz
		EALLOW;
		GpioCtrlRegs.GPHAMSEL.bit.GPIO226 = 0;	
		AnalogSubsysRegs.AGPIOCTRLH.bit.GPIO226 = 0;		
		//InputXbarRegs.INPUT1SELECT = 226;//TZ1,TRIP1
	    EDIS;
		GPIO_setPadConfig(226, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO226
		GPIO_setPinConfig(GPIO_226_GPIO226);			//GPIO226= Tz
		GPIO_setDirectionMode(226, GPIO_DIR_MODE_IN);

//		EALLOW;		
//		InputXbarRegs.INPUT1SELECT = 40;//TZ1,TRIP1
//		EDIS;

//		GPIO_setPadConfig(40, GPIO_PIN_TYPE_PULLUP);	// Enable pullup on GPIO7
//		//GPIO_writePin(40, 1);							// Load output latch
//		GPIO_setPinConfig(GPIO_40_GPIO40);				//GPIO40 =DO2
//		GPIO_setDirectionMode(40, GPIO_DIR_MODE_IN);


}

/*********************************************************************
Function name:  sInitECAP
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
void sInitECAP(void)
{
    EALLOW;

//	if(GetBoardType() == UnitCB)
//		InputXbarRegs.INPUT7SELECT = 22;//33;//22;         // Set eCAP1 = PWM sync capture;
//	else
		InputXbarRegs.INPUT7SELECT = 230;//22;//33;//22;         // Set eCAP1 = PWM sync capture;
    InputXbarRegs.INPUT8SELECT = 6;//21;//6;          // Set eCAP2 = Line sync capture;

//    EDIS;

    // ECap module1
    ECap1Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
    ECap1Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped
    // Configure peripheral registers
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;      // continuous mode
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 0;        // Stop at 2 event
    ECap1Regs.ECCTL1.bit.CAP1POL = 1;         // 0h (R/W) = Capture Event 1 triggered on a rising edge (RE)
												//1h (R/W) = Capture Event 1 triggered on a falling edge (FE)falling edge												
	ECap1Regs.ECCTL1.bit.CAP2POL = 0;		  // 0h (R/W) = Capture Event 1 triggered on a rising edge (RE)
												//1h (R/W) = Capture Event 1 triggered on a falling edge (FE)falling edge
    ECap1Regs.ECCTL1.bit.CTRRST1 = 0;          // Configure counter reset on events
    ECap1Regs.ECCTL2.bit.SYNCI_EN = 0;         // ENable sync in
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;        // Pass through
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units
    //ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    if(GetBoardType() == UnitCB)
    {
	    ECap1Regs.ECEINT.bit.CEVT1 = 1;            // 1 event = interrupt
	    //ECap1Regs.ECEINT.bit.CEVT2 = 1;            // 2 event = interrupt
    }
	else
	{
		ECap1Regs.ECEINT.bit.CEVT1 = 0;            // 1 event = interrupt
	    //ECap1Regs.ECEINT.bit.CEVT2 = 1;            // 2 event = interrupt
	}


	 // ECap module2
    ECap2Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
    ECap2Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
    ECap2Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped
    // Configure peripheral registers
    ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;      // continuous mode
    ECap2Regs.ECCTL2.bit.STOP_WRAP = 0;        // Stop at 1 event
    ECap2Regs.ECCTL1.bit.CAP1POL = 1;         // 0h (R/W) = Capture Event 1 triggered on a rising edge (RE)
													//1h (R/W) = Capture Event 1 triggered on a falling edge (FE)falling edge
    ECap2Regs.ECCTL1.bit.CTRRST1 = 0;          // absolute time stamp
    ECap2Regs.ECCTL2.bit.SYNCI_EN = 0;         // Disable sync in
    ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0;        // Pass through
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units
    //ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    ECap2Regs.ECEINT.bit.CEVT1 = 1;            // 1 event = interrupt

	// start counter at the same time
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1; 	   // Start Counter
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1; 	   // Start Counter

	EDIS;


}
void LoadHwCurrPara(void)
{
	PosCurrentBias = sUintEepromPara.PosCurrent.Bias/10.0;
	NegCurrentBias = sUintEepromPara.NegCurrent.Bias/10.0;
	PosCurrentBias2 = sUintEepromPara.PosCurrent2.Bias/10.0;
	NegCurrentBias2 = sUintEepromPara.NegCurrent2.Bias/10.0;
	SmallCurrentBias = sUintEepromPara.SmallCurrent.Bias/10.0;

	PosCurrentRatio = sUintEepromPara.PosCurrent.Ratio/10000.0;        
	NegCurrentRatio = sUintEepromPara.NegCurrent.Ratio/10000.0;       
	PosCurrentRatio2 = sUintEepromPara.PosCurrent2.Ratio/10000.0;        
	NegCurrentRatio2 = sUintEepromPara.NegCurrent2.Ratio/10000.0;       
	SmallCurrentRatio = sUintEepromPara.SmallCurrent.Ratio/10000.0;  
}
void LoadAdjustPara(void)
{
	int temp;
	for(temp = 0; temp < ADJUST_SECTOR_NUM; temp++)
	{
		PosCurrAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor[temp].Ratio / 10000.0f;
		NegCurrAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.NegCurrAdjustFactor[temp].Ratio / 10000.0f;
		PosCmpCurrAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.PosCmpCurrAdjustFactor[temp].Ratio / 10000.0f;
		NegCmpCurrAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.NegCmpCurrAdjustFactor[temp].Ratio / 10000.0f;
		PosVoltAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.PosVoltAdjustFactor[temp].Ratio / 10000.0f;
		NegVoltAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.NegVoltAdjustFactor[temp].Ratio / 10000.0f;
		PosCmpVoltAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.PosCmpVoltAdjustFactor[temp].Ratio / 10000.0f;
		NegCmpVoltAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.NegCmpVoltAdjustFactor[temp].Ratio / 10000.0f;
		PosCmpDispCurrAdjustFactorArray[temp].ratio = (float)sUintEepromAdjustPara.AdjustFactor.PosCmpDispCurrAdjustFactor[temp].Ratio / 10000.0f;
		NegCmpDispCurrAdjustFactorArray[temp].ratio= (float)sUintEepromAdjustPara.AdjustFactor.NegCmpDispCurrAdjustFactor[temp].Ratio / 10000.0f;

		PosCurrAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.PosCurrAdjustFactor[temp].Bias / 10.0f;
		NegCurrAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.NegCurrAdjustFactor[temp].Bias / 10.0f;
		PosCmpCurrAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.PosCmpCurrAdjustFactor[temp].Bias / 10.0f;
		NegCmpCurrAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.NegCmpCurrAdjustFactor[temp].Bias / 10.0f;
		PosVoltAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.PosVoltAdjustFactor[temp].Bias / 10.0f;
		NegVoltAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.NegVoltAdjustFactor[temp].Bias / 10.0f;
		PosCmpVoltAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.PosCmpVoltAdjustFactor[temp].Bias / 10.0f;
		NegCmpVoltAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.NegCmpVoltAdjustFactor[temp].Bias / 10.0f;
		PosCmpDispCurrAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.PosCmpDispCurrAdjustFactor[temp].Bias / 10.0f;
		NegCmpDispCurrAdjustFactorArray[temp].bias = (float)sUintEepromAdjustPara.AdjustFactor.NegCmpDispCurrAdjustFactor[temp].Bias / 10.0f;

		if(PosCurrAdjustFactorArray[temp].ratio == 0.0f)
		{
			PosCurrAdjustFactorArray[temp].ratio = 1.0f;
			PosCurrAdjustFactorArray[temp].bias = 0.0f;
		}
		if(NegCurrAdjustFactorArray[temp].ratio == 0.0f)
		{
			NegCurrAdjustFactorArray[temp].ratio = 1.0f;
			NegCurrAdjustFactorArray[temp].bias = 0.0f;
		}
		if(PosCmpCurrAdjustFactorArray[temp].ratio == 0.0f)
		{
			PosCmpCurrAdjustFactorArray[temp].ratio = 1.0f;
			PosCmpCurrAdjustFactorArray[temp].bias = 0.0f;
		}
		if(NegCmpCurrAdjustFactorArray[temp].ratio == 0.0f)
		{
			NegCmpCurrAdjustFactorArray[temp].ratio = 1.0f;
			NegCmpCurrAdjustFactorArray[temp].bias = 0.0f;
		}
		if(PosVoltAdjustFactorArray[temp].ratio == 0.0f)
		{
			PosVoltAdjustFactorArray[temp].ratio = 1.0f;
			PosVoltAdjustFactorArray[temp].bias = 0.0f;
		}
		if(NegVoltAdjustFactorArray[temp].ratio == 0.0f)
		{
			NegVoltAdjustFactorArray[temp].ratio = 1.0f;
			NegVoltAdjustFactorArray[temp].bias = 0.0f;
		}
		if(PosCmpVoltAdjustFactorArray[temp].ratio == 0.0f)
		{
			PosCmpVoltAdjustFactorArray[temp].ratio = 1.0f;
			PosCmpVoltAdjustFactorArray[temp].bias = 0.0f;
		}
		if(NegCmpVoltAdjustFactorArray[temp].ratio == 0.0f)
		{
			NegCmpVoltAdjustFactorArray[temp].ratio = 1.0f;
			NegCmpVoltAdjustFactorArray[temp].bias = 0.0f;
		}
		if(PosCmpDispCurrAdjustFactorArray[temp].ratio == 0.0f)
		{
			PosCmpDispCurrAdjustFactorArray[temp].ratio = 1.0f;
			PosCmpDispCurrAdjustFactorArray[temp].bias = 0.0f;
		}
		if(NegCmpDispCurrAdjustFactorArray[temp].ratio == 0.0f)
		{
			NegCmpDispCurrAdjustFactorArray[temp].ratio = 1.0f;
			NegCmpDispCurrAdjustFactorArray[temp].bias = 0.0f;
		}
	}
}
void DataInit(void)
{
	int temp,i;
//
//	waveFifoInit(ePosCurrWave,fPosCurrBuf,800,400);
//	waveFifoInit(eNegCurrWave,fNegCurrBuf,800,400);

	for(temp=0;temp<4;temp++)
	{
		UnitCommTimeout[temp] = 0;
		CommTimeoutToRemoteFlag[temp] = 0;
	}
	
	PosPWMdutyOpenLoop = 10;    //占空比 10%
	NegPWMdutyOpenLoop = 10;    //占空比 10%
	PosCMPDACOpenLoop = 300;    //DAC值
	NegCMPDACOpenLoop = 300;    //DAC值
	PwmOpenLoopEnable = 0;      //默认禁止
	CmpOpenLoopEnable = 0;      //默认禁止
	AdjustRatio1 = 0;           //校正系数
	AdjustRatio2 = 0;           //校正系数

	IavgPosDCRMSLoopErr = 0.0f;
	IavgNegDCRMSLoopErr = 0.0f;
	IavgPosDCRMSLoopInt = 0.0f;
	IavgNegDCRMSLoopInt = 0.0f;
	IavgPosDCRMSLoopOut = 0.0f;
	IavgNegDCRMSLoopOut = 0.0f;
	DCModeRMSLoopFreqCnt = 0;

	IavgPosRMSLoopErr = 0.0f;
	IavgNegRMSLoopErr = 0.0f;
	IavgPosRMSLoopInt = 0.0f;
	IavgNegRMSLoopInt = 0.0f;
	IavgPosRMSLoopOut = 0.0f;
	IavgNegRMSLoopOut = 0.0f;
	PulseModeRMSLoopFreqCnt = 0;
		
    memset(&SideAControl.Vavg_Max,0,sizeof(SideAControl));
	SideAControl.IPosNominal = 150.0;
	SideAControl.INegNominal = -450.0;
	PWMorCMP = cPWMmode;
	PWMstatus = PWMwaitSts;
	SideAControl.PhaseTimeSetNoZero = PulsePhase1;

	PulsePhaseNum = 0;

	SwitchFreq = Fsw120KHz;
	SwitchPeriodReg = SwitchPeriod[SwitchFreq];
	
	
//	BoardType = InvalidCB;
//	if (BoardType == InvalidCB)
//	{
//		BoardRole = InvalidRole;
//		UnitPara.Role = BoardRole;
//		ModulePara.Role = BoardRole;
//	}
	
	memset(&UnitPara.ExtSync,0,sizeof(UnitPara));
	UnitPara = UnitParaInit;	
	ModulePara = UnitPara;

//	ModulePara.SumOfExtSyncPoint = 600;
	if(BoardType == ModuleCB)
	{
//		BoardType = ModuleCB;
		UnitPara.Role = InvalidRole;		
		ModulePara.Role = MasterCB;	
		sModuleParamReg = *(sModuleParamRegion *)&sModuleEepromPara.BoardType;
	}
	else
	{
//		BoardType = UnitCB;
		UnitPara.Role = SlaveCB;		
		ModulePara.Role = InvalidRole;
	}
	
	
	if((BoardType == ModuleCB) || (BoardType == CabinetCB))
	{		
		if(ModulePara.Role == MasterCB)
		{
			PwmSyncON_TZ();	
		}
		else
		{
			PwmSyncOff_TZ();		
		}

		PieCtrlRegs.PIEIER4.bit.INTx1 = 1;//DISable PIE group 4 interrupt 1 for CAP1,PWM SYNC
        PieCtrlRegs.PIEIER4.bit.INTx2 = 0;//DISable PIE group 4 interrupt 2 for CAP2,Line SYNC
		PieCtrlRegs.PIEIER3.bit.INTx1 = 1;//Enable PIE group3 interrupt 1 for EPWM1
		PieCtrlRegs.PIEIER3.bit.INTx4 = 1;//Enable PIE group 3 interrupt 4 for EPWM4
		
		RemoteCommAddr = sModuleEepromPara.BoardAddr;
		SlaveStartAddr = sModuleEepromPara.UnitAddrMin;
		MachineWorkMode = sModuleEepromPara.WorkMode;

		sSyncCommInit(sModuleEepromPara.SynId,sModuleEepromPara.SynDelay);
	}
	else
	{
		PwmSyncOff_TZ();

		PieCtrlRegs.PIEIER4.bit.INTx1 = 0;//Enable PIE group 4 interrupt 1 for CAP1,PWM SYNC
        PieCtrlRegs.PIEIER4.bit.INTx2 = 1;//Enable PIE group 4 interrupt 2 for CAP2,Line SYNC
        PieCtrlRegs.PIEIER3.bit.INTx1 = 1;//Enable PIE group3 interrupt 1 for EPWM1

		RemoteCommAddr = sUintEepromPara.BoardAddr;
		SlaveStartAddr = sUintEepromPara.UnitAddrMin;
		MachineWorkMode = sUintEepromPara.WorkMode;

		sSyncCommInit(RemoteCommAddr,sUintEepromPara.SynDelay);
	}

	
	
	
	ModulePara.Mode = Parallel;

	UnitPara.ExtSync = (xExtSync)sUintEepromPara.LineSyncEnable;
	UnitPara.PWMSync = (xPWMSync)sUintEepromPara.PWMSyncEnable;
	UnitPara.PWMSyncLock = (xPWMSync)sUintEepromPara.PWMSyncLockEnable;

	memset(&VBus.Sum,0,sizeof(VBus));

	memset(&PosI.Overlevel,0,sizeof(PosI));
	memset(&NegI.Overlevel,0,sizeof(NegI));
	memset(&LoadV.Overlevel,0,sizeof(LoadV));
	memset(&LoadVoltPosAvg.Overlevel,0,sizeof(LoadVoltPosAvg));
	memset(&LoadVoltNegAvg.Overlevel,0,sizeof(LoadVoltNegAvg));
	memset(&LoadCurrPosAvg.Overlevel,0,sizeof(LoadCurrPosAvg));
	memset(&LoadCurrNegAvg.Overlevel,0,sizeof(LoadCurrNegAvg));

	IavgPosFilter = 0.0f;
	IavgNegFilter = 0.0f;
	VavgPosFilter = 0.0f;
	VavgNegFilter = 0.0f;

	IavgPosDCFilter = 0.0f;
	IavgNegDCFilter = 0.0f;
	VavgPosDCFilter = 0.0f;
	VavgNegDCFilter = 0.0f;

	for(temp = 0; temp <= 20; temp++)
	{
	    IavgPhaseFilter[temp] = 0.0f;
	}

	IavgPosFilterDisPlay = 0.0f;
	IavgNegFilterDisPlay = 0.0f;
	IavgPosDCFilterDisPlay = 0.0f;
	IavgNegDCFilterDisPlay = 0.0f;

	PulsePosAverageValueSet = 0.0f;
	PulseNegAverageValueSet = 0.0f;

	memset(&Power24Vcheck.Overlevel,0,sizeof(Power24Vcheck));
	Power24Vcheck.Overlevel = sUintEepromPara.Power24VUnderThreshold;
	Power24Vcheck.Overback = sUintEepromPara.Power24VUnderBackThreshold;
	Power24Vcheck.Overcountset = cLoadTimer1S;
	Power24Vcheck.Overbackcountset = cLoadTimer1S;
	Power24Vcheck.Enable = sUintEepromPara.Power24VUnderEnable;
	Power24Vcheck.compvalue = 1.0;

	DisplayMode = (eDISPLAYMODE)sUintEepromPara.DisplayMode;
	
	if(sUintEepromPara.PwmCmpCtrl)
	{
		PWMorCMP = cCMPmode;
	}
	else
	{
		PWMorCMP = cPWMmode;
	}
	MixModeEnable = sUintEepromPara.MixModeEnable;
	SwitchFreq = (SwitchFreqency)sUintEepromPara.SwFreq;
	SwitchPeriodReg = SwitchPeriod[SwitchFreq];

	HwOCCountSet = sUintEepromPara.HwOCCountSet;
	HwOCRecoveryTimeSet = sUintEepromPara.HwOCRecoveryTimeSet;
	HwOCCheckEnable = sUintEepromPara.HwOCCheckEnable;
	HwOCFaultTime = (Uint32)sUintEepromPara.HwOCFaultTime * 1000000UL / 25;
	HwOCFaultRecoverySet = sUintEepromPara.HwOCFaultRecoverySet;
	
	Moduletype = (eMODULE_TYPE)sUintEepromPara.Moduletype;
	if(Moduletype == Module_P100A_N300A)
	{
	    Cmpss1DACHVALS_Pos = Cmpss1DACHVALS_Pos_N300A;
	    Cmpss2DACHVALS_Neg = Cmpss2DACHVALS_Neg_N300A;
		SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*100/100*10;
		if(SmallCurrModeThreshold < 90)
		{
		    SmallCurrModeThreshold = 90;
		}

		SideAControl.IPosNominal = 100.0;
		SideAControl.INegNominal = -300.0;
	}
	else if(Moduletype == Module_P150A_N450A)
    {
        Cmpss1DACHVALS_Pos = Cmpss1DACHVALS_Pos_N450A;
        Cmpss2DACHVALS_Neg = Cmpss2DACHVALS_Neg_N450A;
		SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*150/100*10;
		
		SideAControl.IPosNominal = 150.0;
		SideAControl.INegNominal = -450.0;
    }
	else if(Moduletype == Module_P200A_N600A)
    {
        Cmpss1DACHVALS_Pos = Cmpss1DACHVALS_Pos_N600A;
        Cmpss2DACHVALS_Neg = Cmpss2DACHVALS_Neg_N600A;
		SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*200/100*10;
		if(SmallCurrModeThreshold < 150)
		{
		    SmallCurrModeThreshold = 150;
		}

		SideAControl.IPosNominal = 200.0;
		SideAControl.INegNominal = -600.0;
    }
	else if(Moduletype == Module_P150A_N750A)
    {
        Cmpss1DACHVALS_Pos = Cmpss1DACHVALS_Pos_N750A;
        Cmpss2DACHVALS_Neg = Cmpss2DACHVALS_Neg_N750A;
		SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*150/100*10;
        if(SmallCurrModeThreshold < 120)
        {
            SmallCurrModeThreshold = 120;
        }

		SideAControl.IPosNominal = 150.0;
		SideAControl.INegNominal = -750.0;
    }
	else if(Moduletype == Module_P50A_N150A)
    {
        Cmpss1DACHVALS_Pos = Cmpss1DACHVALS_Pos_N150A;
        Cmpss2DACHVALS_Neg = Cmpss2DACHVALS_Neg_N150A;
		SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*50/100*10;

		SideAControl.IPosNominal = 50.0;
		SideAControl.INegNominal = -150.0;
    }
	else
	{
	    Cmpss1DACHVALS_Pos = Cmpss1DACHVALS_Pos_N150A;
	    Cmpss2DACHVALS_Neg = Cmpss2DACHVALS_Neg_N150A;
		SmallCurrModeThreshold = sUintEepromPara.MixModeRatio*50/100*10;

		SideAControl.IPosNominal = 50.0;
		SideAControl.INegNominal = -150.0;
	}
	if(SmallCurrModeThreshold >= 450)
		SmallCurrModeThreshold = 450;

	CMPmodePWMCountMode = sUintEepromPara.CMPmodePWMCountMode;
	if(CMPmodePWMCountMode == 1)
	{
        MixModeEnable = 0;

        if(PWMorCMP == cCMPmode)
        {
            SwitchPeriodReg = SwitchPeriod[SwitchFreq]*2-1;
        }
    }
	
	PowerSetValue = sUintEepromPara.PowerSetValue;
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].PowerSetValue = PowerSetValue;
	
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].PosPWMdutyOpenLoop = PosPWMdutyOpenLoop;
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].NegPWMdutyOpenLoop = NegPWMdutyOpenLoop;
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].PosCMPDACOpenLoop = PosCMPDACOpenLoop;
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].NegCMPDACOpenLoop = NegCMPDACOpenLoop;
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].AdjustRatio1 = AdjustRatio1;
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].AdjustRatio2 = AdjustRatio2;
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].PwmOpenLoopEnable = PwmOpenLoopEnable;
	sUnitCtrlReg[Get485Addr()-SlaveStartAddr].CmpOpenLoopEnable = CmpOpenLoopEnable;

    CMPmodeSlopePos = (float)sUintEepromPara.CMPmodeSlopePos * 25.0f / 10.0f; //每25us变化的电流（扩大10倍）
    if(CMPmodeSlopePos > 1250.0f)   //1250/25=50 5A/us
    {
        CMPmodeSlopePos = 1250.0f;
    }
    else if(CMPmodeSlopePos < 75.0f)//75/25=3 0.3A/us
    {
        CMPmodeSlopePos = 75.0f;
    }

    CMPmodeSlopeNeg = (float)sUintEepromPara.CMPmodeSlopeNeg * 25.0f / 10.0f; //每25us变化的电流（扩大10倍）
    if(CMPmodeSlopeNeg > 1250.0f) //1250/25=50 5A/us
    {
        CMPmodeSlopeNeg = 1250.0f;
    }
    else if(CMPmodeSlopeNeg < 250.0f) //250/25=10 1A/us
    {
        CMPmodeSlopeNeg = 250.0f;
    }

    if((PWMorCMP == cCMPmode) || (MixModeEnable == 1))
    {
        CMPmodeSlopeEnable = sUintEepromPara.CMPmodeSlopeEnable;
    }
    else
    {
        CMPmodeSlopeEnable = 0;
    }

    CMPmodeReferenceTemp = 0.0f;

    MixModeFlag = 0;
    MixModeFlagBefore = 0;

    HwOCCount = 0;
    HwOCRecoveryTimeCount = 0;
    HwOCFaultTimeCnt = 0;
    HwOCFaultRecoverySetCnt = 0;
    HwOCFaultRecoveryFlag = 0;

	LoadHwCurrPara();
	
	OutPutVoltBias = sUintEepromPara.OutPutVolt.Bias/10.0;
	BusVolttBias = sUintEepromPara.BusVolt.Bias/10.0;
	AmbientTemperatureBias = sUintEepromPara.AmbientTemperature.Bias/10.0;
	SinkTemperatureBias = sUintEepromPara.SinkTemperature.Bias/10.0;
	SwitchDIPBias = sUintEepromPara.SwitchDIP.Bias/10.0;
 
	OutPutVoltRatio = sUintEepromPara.OutPutVolt.Ratio/10000.0;        
	BusVoltRatio = sUintEepromPara.BusVolt.Ratio/10000.0;            
	AmbientTemperatureRatio = sUintEepromPara.AmbientTemperature.Ratio/10000.0;  
	SinkTemperatureRatio = sUintEepromPara.SinkTemperature.Ratio/10000.0;     
	SwitchDIPRatio = sUintEepromPara.SwitchDIP.Ratio/10000.0;           

	WaveForm = Pulse;
	ControlMode = CloseLoop;

	
	EPwm2Regs.TBPRD = SwitchPeriodReg;	
	EPwm3Regs.TBPRD = SwitchPeriodReg;

	if((CMPmodePWMCountMode == 1) && (PWMorCMP == cCMPmode))
    {
        EALLOW;

        //PWM3
        EPwm3Regs.TZSEL.bit.DCAEVT2 = 1;
        EPwm3Regs.TZSEL.bit.DCBEVT2 = 1;

        EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
        EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
        EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;

        EPwm3Regs.TZCTL.bit.TZA = 0x1; //0-High impedance;1-force to high state;2-force to low state;3-no change
        EPwm3Regs.TZCTL.bit.TZB = 0x1;

        EPwm3Regs.TZCTL.bit.DCAEVT1 = 0x0;
        EPwm3Regs.TZCTL.bit.DCBEVT1 = 0x0;

        EPwm3Regs.TZCTL.bit.DCAEVT2 = 0x0;
        EPwm3Regs.TZCTL.bit.DCBEVT2 = 0x0;

        //PWM2
        EPwm2Regs.TZSEL.bit.DCAEVT2 = 1;
        EPwm2Regs.TZSEL.bit.DCBEVT2 = 1;

        EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
        EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
        EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;

        EPwm2Regs.TZCTL.bit.TZA = 0x1; //0-High impedance;1-force to high state;2-force to low state;3-no change
        EPwm2Regs.TZCTL.bit.TZB = 0x1;

        EPwm2Regs.TZCTL.bit.DCAEVT1 = 0x0;
        EPwm2Regs.TZCTL.bit.DCBEVT1 = 0x0;

        EPwm2Regs.TZCTL.bit.DCAEVT2 = 0x0;
        EPwm2Regs.TZCTL.bit.DCBEVT2 = 0x0;

        EDIS;
    }

	InitPulsePhase = PulsePhase1;
	InitTimeCount = 0;

	

//    OverLoadLevel = 150;//100:10A 150:15A
	OverLoadLevel = sUintEepromPara.OverLoadThreshold;
	OverLoadCheckEnable = sUintEepromPara.OverLoadCheckEnable;
    OverLoadCntSet = 30; //10
    OverLoadBackCntSet = 100;
	
		
	LoadAdjustPara();
	
    if(Moduletype == Module_P100A_N300A)
    {
        /*for (temp = 0; temp < 10; temp++)
        {
            AdjustFactorSectionDivide[temp] = 100.0f * (temp + 1); //10A 20A 。。。 100A
        }*/
        AdjustFactorSectionDivide[0] = 1000.0f * 0.1f;      //10%
        AdjustFactorSectionDivide[1] = 1000.0f * 0.3f;      //30%
        AdjustFactorSectionDivide[2] = 1000.0f * 0.6f;      //60%
        AdjustFactorSectionDivide[3] = 1000.0f * 1.0f;      //100%
    }
    else if(Moduletype == Module_P150A_N450A)
    {
        /*for (temp = 0; temp < 10; temp++)
        {
            AdjustFactorSectionDivide[temp] = 150.0f * (temp + 1); //15A 30A 。。。 150A
        }*/
        AdjustFactorSectionDivide[0] = 1500.0f * 0.1f;      //10%
        AdjustFactorSectionDivide[1] = 1500.0f * 0.3f;      //30%
        AdjustFactorSectionDivide[2] = 1500.0f * 0.6f;      //60%
        AdjustFactorSectionDivide[3] = 1500.0f * 1.0f;      //100%
    }
    else if(Moduletype == Module_P200A_N600A)
    {
        /*for (temp = 0; temp < 10; temp++)
        {
            AdjustFactorSectionDivide[temp] = 200.0f * (temp + 1); //20A 40A 。。。 200A
        }*/
        AdjustFactorSectionDivide[0] = 2000.0f * 0.1f;      //10%
        AdjustFactorSectionDivide[1] = 2000.0f * 0.3f;      //30%
        AdjustFactorSectionDivide[2] = 2000.0f * 0.6f;      //60%
        AdjustFactorSectionDivide[3] = 2000.0f * 1.0f;      //100%
    }
    else if(Moduletype == Module_P150A_N750A)
    {
        /*for (temp = 0; temp < 10; temp++)
        {
            AdjustFactorSectionDivide[temp] = 150.0f * (temp + 1); //15A 30A 。。。 150A
        }*/
        AdjustFactorSectionDivide[0] = 1500.0f * 0.1f;      //10%
        AdjustFactorSectionDivide[1] = 1500.0f * 0.3f;      //30%
        AdjustFactorSectionDivide[2] = 1500.0f * 0.6f;      //60%
        AdjustFactorSectionDivide[3] = 1500.0f * 1.0f;      //100%
    }
    else if(Moduletype == Module_P50A_N150A)
    {
        /*for (temp = 0; temp < 10; temp++)
        {
            AdjustFactorSectionDivide[temp] = 50.0f * (temp + 1); //5A 10A 。。。 50A
        }*/
        AdjustFactorSectionDivide[0] = 500.0f * 0.1f;      //10%
        AdjustFactorSectionDivide[1] = 500.0f * 0.3f;      //30%
        AdjustFactorSectionDivide[2] = 500.0f * 0.6f;      //60%
        AdjustFactorSectionDivide[3] = 500.0f * 1.0f;      //100%
    }
    else
    {
        //do nothing
    }

	for(i=0;i<ADJUST_SECTOR_NUM;i++)
	{
		sUnitCtrlReg[Get485Addr() - SlaveStartAddr].PosAdjustPara[i].setValue = (uint16_t)(AdjustFactorSectionDivide[i]*10.0f);
		sUnitCtrlReg[Get485Addr() - SlaveStartAddr].PosAdjustPara[i].actualValue = 0;
		sUnitCtrlReg[Get485Addr() - SlaveStartAddr].NegAdjustPara[i].setValue = (uint16_t)(AdjustFactorSectionDivide[i]*10.0f);
		sUnitCtrlReg[Get485Addr() - SlaveStartAddr].NegAdjustPara[i].actualValue = 0;
	}

	for (temp = 0; temp < 22; temp++)
	{
	
		SideAControl.step[temp].cali = 1.0;
		SideAControl.step[temp].gain = 1.0;
		
		SideAControl.step[temp].A2 = sUintEepromPara.PID[SwitchFreq].A2/10000.0f;
		SideAControl.step[temp].A1 = sUintEepromPara.PID[SwitchFreq].A1/10000.0f;
		SideAControl.step[temp].A0 = sUintEepromPara.PID[SwitchFreq].A0/10000.0f;
			
//		if(SwitchFreq == Fsw160KHz)
//		{
//			SideAControl.step[temp].A2 = 0.12;
//			SideAControl.step[temp].A1 = 0.2;//0.80;//0.22;
//			SideAControl.step[temp].A0 = 0.0006;//0.0006;//0.022;
//		}
//		else if(SwitchFreq == Fsw120KHz)
//		{
//			SideAControl.step[temp].A2 = 0.12;
//			SideAControl.step[temp].A1 = 0.28;//0.80;//0.22;
//			SideAControl.step[temp].A0 = 0.0004;//0.0006;//0.022;
//		}
//		else if(SwitchFreq == Fsw80KHz)
//		{
//			SideAControl.step[temp].A2 = 0.12;
//			SideAControl.step[temp].A1 = 0.24;//0.80;//0.22;
//			SideAControl.step[temp].A0 = 0.0003;//0.0006;//0.022;
//		}
//		else
//		{
//			SideAControl.step[temp].A2 = 0.12;
//			SideAControl.step[temp].A1 = 0.30;//0.80;//0.22;
//			SideAControl.step[temp].A0 = 0.0002;//0.0006;//0.022;
//		}
		
		
		SideAControl.step[temp].B1 = 0.0;
		SideAControl.step[temp].B2 = 0.0;

		SideAControl.step[temp].errorUplimit = 600.0;
		SideAControl.step[temp].errorDwlimit = -600.0;
		SideAControl.step[temp].timecountLimit = 1;

//			SideAControl.step[temp].cali = 10.0/0.6;//14.11;
//			SideAControl.step[temp].gain = 6.0;
//			SideAControl.step[temp].A0 = 0.6;//0.141;
//		    SideAControl.step[temp].A1 = 0.0;//-0.004063;
//		    SideAControl.step[temp].A2 = 0.0;//-0.003717;
//		    SideAControl.step[temp].B1 = 0.0;//0.4301;
//		    SideAControl.step[temp].B2 = 0.0;//0.3959;
			
	}


	SetModuleWorkStatus(eMODULE_IDLE);
	SetUnitWorkStatus(eMODULE_IDLE);
	SyncPhase = 0;
}


void PwmSyncOff_TZ(void)
{
	PwmSyncStatus = cPwmSyncOff;

    OS_ENTER_CRITICAL();
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;

    OS_EXIT_CRITICAL();

}

void PwmSyncON_TZ(void)
{
	PwmSyncStatus = cPwmSyncOn;
	
	OS_ENTER_CRITICAL();
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;
	OS_EXIT_CRITICAL();

}

void PwmSyncOff_Isr(void)
{
	PwmSyncStatus = cPwmSyncOff;
	
	EPwm1Regs.TBCTR = 0;
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;
}

void PwmSyncON_Isr(void)
{
	PwmSyncStatus = cPwmSyncOn;

	EPwm1Regs.TBCTR = 0;
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;
}



void SideALargePWMON(void)
{
	SideALargePWMstatus  = cPWMrunSts;
	PWMdrive_ON();
	OS_ENTER_CRITICAL();
	EALLOW;
	EPwm2Regs.DBCTL.bit.POLSEL = 1;
	EPwm3Regs.DBCTL.bit.POLSEL = 1;
	
	EPwm2Regs.AQCSFRC.bit.CSFA = 0;
	EPwm2Regs.AQCSFRC.bit.CSFB = 0;
	EPwm3Regs.AQCSFRC.bit.CSFA = 0;
	EPwm3Regs.AQCSFRC.bit.CSFB = 0;
	EDIS;
	OS_EXIT_CRITICAL();

}
void SideALargePWMON_TZ(void)
{
	SideALargePWMstatus  = cPWMrunSts;
	PWMdrive_ON();
	OS_ENTER_CRITICAL();
	EALLOW;
	EPwm2Regs.TZCLR.bit.OST = 0x1;
	EPwm3Regs.TZCLR.bit.OST = 0x1;

	EPwm2Regs.TZCTL.bit.TZA = 0x3;
	EPwm2Regs.TZCTL.bit.TZB = 0x3;

	EPwm3Regs.TZCTL.bit.TZA = 0x3;
	EPwm3Regs.TZCTL.bit.TZB = 0x3;
	EDIS;
	OS_EXIT_CRITICAL();

}

//#pragma CODE_SECTION(SideALargePWMON_ISR,ramFuncSection);
void SideALargePWMON_ISR(void)
{
	SideALargePWMstatus  = cPWMrunSts;
	PWMdrive_ON();
	EALLOW;
	EPwm2Regs.DBCTL.bit.POLSEL = 1;
	EPwm3Regs.DBCTL.bit.POLSEL = 1;
	
	EPwm2Regs.AQCSFRC.bit.CSFA = 0;
	EPwm2Regs.AQCSFRC.bit.CSFB = 0;
	EPwm3Regs.AQCSFRC.bit.CSFA = 0;
	EPwm3Regs.AQCSFRC.bit.CSFB = 0;
	EDIS;

}


//#pragma CODE_SECTION(SideALargePWMON_TZ_ISR,ramFuncSection);

void SideALargePWMON_TZ_ISR(void)
{
	SideALargePWMstatus  = cPWMrunSts;
	
	PWMdrive_ON();

	EALLOW;

	
	EPwm2Regs.TZCLR.bit.OST = 0x1;
	EPwm3Regs.TZCLR.bit.OST = 0x1;

	EPwm2Regs.TZCTL.bit.TZA = 0x3;
	EPwm2Regs.TZCTL.bit.TZB = 0x3;

	EPwm3Regs.TZCTL.bit.TZA = 0x3;
	EPwm3Regs.TZCTL.bit.TZB = 0x3;
	EDIS;


}










void SideALargePWMOFF(void)
{
	PWMstatus = PWMwaitSts;

	SideALargePWMstatus  = cPWMwaitSts;
	PWMdrive_OFF();
	OS_ENTER_CRITICAL();
	EALLOW;
	EPwm2Regs.DBCTL.bit.POLSEL = 0;
	EPwm3Regs.DBCTL.bit.POLSEL = 0;
	
	EPwm2Regs.AQCSFRC.bit.CSFA = 2;
	EPwm2Regs.AQCSFRC.bit.CSFB = 2;
	EPwm3Regs.AQCSFRC.bit.CSFA = 2;
	EPwm3Regs.AQCSFRC.bit.CSFB = 2;
	EDIS;
	OS_EXIT_CRITICAL();

}

//#pragma CODE_SECTION(SideALargePWMOFF_TZ,ramFuncSection);

void SideALargePWMOFF_TZ(void)
{
    SideALargePWMstatus  = cPWMwaitSts;
	PWMdrive_OFF();
	OS_ENTER_CRITICAL();
	EALLOW;
	EPwm2Regs.TZCTL.bit.TZA = 0x2;
	EPwm2Regs.TZCTL.bit.TZB = 0x2;

	EPwm3Regs.TZCTL.bit.TZA = 0x2;
	EPwm3Regs.TZCTL.bit.TZB = 0x2;
	
	EPwm2Regs.TZFRC.bit.OST = 0x1;
	EPwm3Regs.TZFRC.bit.OST = 0x1;
	
	EDIS;
	OS_EXIT_CRITICAL();

}


void SideALargePWMOFF_ISR(void)
{
	SideALargePWMstatus  = cPWMwaitSts;
	PWMdrive_OFF();

	EALLOW;
	EPwm2Regs.DBCTL.bit.POLSEL = 0;
	EPwm3Regs.DBCTL.bit.POLSEL = 0;
	
	EPwm2Regs.AQCSFRC.bit.CSFA = 2;
	EPwm2Regs.AQCSFRC.bit.CSFB = 2;
	EPwm3Regs.AQCSFRC.bit.CSFA = 2;
	EPwm3Regs.AQCSFRC.bit.CSFB = 2;
	EDIS;


}

void SideALargePWMOFF_TZ_ISR(void)
{
	SideALargePWMstatus  = cPWMwaitSts;
	PWMdrive_OFF();

	EALLOW;
	EPwm2Regs.TZCTL.bit.TZA = 0x2;
	EPwm2Regs.TZCTL.bit.TZB = 0x2;

	EPwm3Regs.TZCTL.bit.TZA = 0x2;
	EPwm3Regs.TZCTL.bit.TZB = 0x2;
	
	EPwm2Regs.TZFRC.bit.OST = 0x1;
	EPwm3Regs.TZFRC.bit.OST = 0x1;
	EDIS;


}

void EPWM2A_ZRO_PRD_SET(void)
{
	EPwm2Regs.AQCTLA.bit.PRD = 0;
	EPwm2Regs.AQCTLA.bit.ZRO = 1;
}
void EPWM2A_ZRO_PRD_CLR(void)
{
	EPwm2Regs.AQCTLA.bit.PRD = 0;
	EPwm2Regs.AQCTLA.bit.ZRO = 2;
}


void EPWM2B_ZRO_PRD_SET(void)
{
	EPwm2Regs.AQCTLB.bit.PRD = 0;
	EPwm2Regs.AQCTLB.bit.ZRO = 1;

}
void EPWM2B_ZRO_PRD_CLR(void)
{
	EPwm2Regs.AQCTLB.bit.PRD = 0;
	EPwm2Regs.AQCTLB.bit.ZRO = 2;

}


void EPWM3A_ZRO_PRD_SET(void)
{
	EPwm3Regs.AQCTLA.bit.PRD = 0;
	EPwm3Regs.AQCTLA.bit.ZRO = 1;
}
void EPWM3A_ZRO_PRD_CLR(void)
{
	EPwm3Regs.AQCTLA.bit.PRD = 0;
	EPwm3Regs.AQCTLA.bit.ZRO = 2;
}

void EPWM3B_ZRO_PRD_SET(void)
{
	EPwm3Regs.AQCTLB.bit.PRD = 0;
	EPwm3Regs.AQCTLB.bit.ZRO = 1;

}
void EPWM3B_ZRO_PRD_CLR(void)
{
	EPwm3Regs.AQCTLB.bit.PRD = 0;
	EPwm3Regs.AQCTLB.bit.ZRO = 2;

}


/*********************************************************************
Function name:  PWM_status GetPWMstatus(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
PWM_status GetPWMstatus(void)
{
	return(PWMstatus);

}

/*********************************************************************
Function name:  SetSideALargePWMstatus
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void SetSideALargePWMstatus(PWM_status status)
{
	PWMstatus = status;

}

/*********************************************************************
Function name:  void InitEPWM3(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void InitEPWM3(void)
{

		// Disable all the muxes first
		EALLOW;


	EPwm3Regs.DCAHTRIPSEL.bit.TRIPINPUT4 = 1;
	EPwm3Regs.DCBHTRIPSEL.bit.TRIPINPUT4 = 1;
	
	//EPwm3Regs.DCAHTRIPSEL.bit.TRIPINPUT5 = 1;
	//EPwm3Regs.DCBHTRIPSEL.bit.TRIPINPUT5 = 1;

       EPwm3Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 0xF; //Trip 4 is the input to the DCAHCOMPSEL
        //EPwm3Regs.DCTRIPSEL.bit.DCALCOMPSEL = 3; //Trip 4 is the input to the DCALCOMPSEL
        EPwm3Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 0xF; //Trip 4 is the input to the DCBHCOMPSEL
		//EPwm3Regs.DCTRIPSEL.bit.DCBLCOMPSEL = 3; //Trip 4 is the input to the DCALCOMPSEL

		EPwm3Regs.TZDCSEL.bit.DCAEVT1       = 2;//Digital Compare Output A Event 1 Selection
                                                        //000: Event disabled
                                                        //001: DCAH = low, DCAL = don't care
                                                        //010: DCAH = high, DCAL = don't care
                                                        //011: DCAL = low, DCAH = don't care
                                                        //100: DCAL = high, DCAH = don't care
                                                        //101: DCAL = high, DCAH = low
                                                        //110: Reserved
                                                   //111: Reserved
        EPwm3Regs.TZDCSEL.bit.DCAEVT2       = 2;
		EPwm3Regs.TZDCSEL.bit.DCBEVT1       = 2;
		EPwm3Regs.TZDCSEL.bit.DCBEVT2       = 2;//*************************
		
//        EPwm3Regs.DCACTL.bit.EVT1SRCSEL     = DC_EVT1;//DCAEVT1 Source Signal Select
//                                                      //0: Source Is DCAEVT1 Signal
//                                                      //1: Source Is DCEVTFILT Signal
//        EPwm3Regs.DCACTL.bit.EVT2SRCSEL     = DC_EVT1;//DC_EVT2;
//		EPwm3Regs.DCBCTL.bit.EVT1SRCSEL     = DC_EVT1;
//		EPwm3Regs.DCBCTL.bit.EVT2SRCSEL 	= DC_EVT1;//DC_EVT2;

//       EPwm3Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;//DCAEVT1 Force Synchronization Signal Select
//                                                            //0: Source is synchronized with EPWMCLK
//                                                            //1: Source is passed through asynchronously
//       EPwm3Regs.DCACTL.bit.EVT2FRCSYNCSEL = DC_EVT_ASYNC;
//	   EPwm3Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
//	   EPwm3Regs.DCBCTL.bit.EVT2FRCSYNCSEL = DC_EVT_ASYNC;

//		EPwm3Regs.DCACTL.bit.EVT1SOCE = 0;
//		EPwm3Regs.DCACTL.bit.EVT1SYNCE = 0;

//		EPwm3Regs.DCBCTL.bit.EVT1SOCE = 0;
//		EPwm3Regs.DCBCTL.bit.EVT1SYNCE = 0;

//		EPwm3Regs.DCFCTL.bit.BLANKE = 1;//ENable blanking window
//		EPwm3Regs.DCFCTL.bit.PULSESEL = 2;//Pulse Select For Blanking & Capture Alignment
//											//00: Time-base counter equal to period (TBCTR = TBPRD)
//											//01: Time-base counter equal to zero (TBCTR = 0x00)
//											//10: Time-base counter equal to zero (TBCTR = 0x00) or period
//											//(TBCTR = TBPRD)
//											//11: Reserved
//		EPwm3Regs.DCFCTL.bit.SRCSEL = 1;//Filter Block Signal Source Select
//										//00: Source Is DCAEVT1 Signal
//										//01: Source Is DCAEVT2 Signal
//										//10: Source Is DCBEVT1 Signal
//										//11: Source Is DCBEVT2 Signal
		/*Blanking Window Offset These 16-bits specify the number of TBCLK cycles from the blanking
window reference to the point when the blanking window is applied. The blanking window reference is either period or zero as defined by
the DCFCTL[PULSESEL] bit. This offset register is shadowed and the active register is loaded at the reference point defined by
DCFCTL[PULSESEL]. The offset counter is also initialized and begins to count down when the active register is loaded. When the
counter expires, the blanking window is applied. If the blanking window is currently active, then the blanking window counter is
restarted.*/								
//		EPwm3Regs.DCFOFFSET = 0;
//		EPwm3Regs.DCFWINDOW = 30;

		EPwm3Regs.TZSEL.bit.CBC1 = 0;//Disable TZ1 as a CBC trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.CBC2 = 0;//Disable TZ2 as a CBC trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.CBC3 = 0;//Disable TZ3 as a CBC trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.CBC4 = 0;//Disable TZ4 as a CBC trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.CBC5 = 0;//Disable TZ5 as a CBC trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.CBC6 = 0;//Disable TZ6 as a CBC trip source for this ePWM module

		EPwm3Regs.TZSEL.bit.OSHT1 = 0;//Enable TZ1 as a OST trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.OSHT2 = 0;//Disable TZ2 as a OST trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.OSHT3 = 0;//Disable TZ3 as a OST trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.OSHT4 = 0;//Disable TZ4 as a OST trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.OSHT5 = 0;//Disable TZ5 as a OST trip source for this ePWM module
		EPwm3Regs.TZSEL.bit.OSHT6 = 0;//Disable TZ6 as a OST trip source for this ePWM module
//		
//		
        EPwm3Regs.TZSEL.bit.DCAEVT1         = 0;           //Digital Compare Output A Event 1 Select
                                                           // 0: Disable DCAEVT1 as one-shot-trip source for this ePWM module.
                                                           // 1: Enable DCAEVT1 as one-shot-trip source for this ePWM module.
        EPwm3Regs.TZSEL.bit.DCBEVT1         = 0;

//		if(CompMode == CBCmode)
//		{
//			EPwm3Regs.TZSEL.bit.DCAEVT2         = 1;          // Digital Compare Output A Event 2 Select
//															//0: Disable DCAEVT2 as a CBC trip source for this ePWM module
//															//1: Enable DCAEVT2 as a CBC trip source for this ePWM module
//															//Reset type: SYSRSn
//			EPwm3Regs.TZSEL.bit.DCBEVT2         = 1;		
//		}
//		else
		{
			EPwm3Regs.TZSEL.bit.DCAEVT2         = 0;//1;          // Digital Compare Output A Event 2 Select
															//0: Disable DCAEVT2 as a CBC trip source for this ePWM module
															//1: Enable DCAEVT2 as a CBC trip source for this ePWM module
															//Reset type: SYSRSn
			EPwm3Regs.TZSEL.bit.DCBEVT2         = 0;//1;			
		}


        EDIS;
	
	
		EPwm3Regs.TBPRD = SwitchPeriod[Fsw80KHz]; 						// Set timer period (12.5us)500=100M/80k/2
		EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
				//sync
        EPwm3Regs.EPWMSYNCINSEL.bit.SEL = 0x1; //EPWM1.SYNCOUT
												
		EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;        // phase loading	
        EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;             // Phase is 0
        EPwm3Regs.TBCTR = 0x0000;                       // Clear counter	
        

        EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
        EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
		EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;

        EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

        EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	    
		EPwm3Regs.CMPA.bit.CMPA = 0;

//		if(CompMode == T1mode)
//		{
//			EPwm3Regs.AQTSRCSEL.bit.T1SEL = 1;		//0000: DCAEVT1
//												//0001: DCAEVT2
//												//0010: DCBEVT1
//												//0011: DCBEVT2
//
//			EPwm3Regs.AQCTLA2.bit.T1U = AQ_SET;			  // if limit duty
//			EPwm3Regs.AQCTLA2.bit.T1D = AQ_CLEAR;	
//		}

	
		EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;			  // Clear PWM1A on CAU
		//EPwm3Regs.AQCTLB.bit.CAU = AQ_NO_ACTION;			  // 
		EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;			 //  Set PWM1A on CAD
		//EPwm3Regs.AQCTLB.bit.CAD = AQ_NO_ACTION;			  //


		EPwm3Regs.AQSFRC.bit.RLDCSF = 3;//Load immediately
		EPwm3Regs.AQCSFRC.bit.CSFA = 2;
		EPwm3Regs.AQCSFRC.bit.CSFB = 2;
		//EPwm3Regs.AQSFRC.bit.ACTSFA = 1;
		//EPwm3Regs.AQSFRC.bit.ACTSFB = 1;
		//EPwm3Regs.AQSFRC.bit.OTSFA = 1; 
		//EPwm3Regs.AQSFRC.bit.OTSFB = 1; 

//		EPwm3Regs.AQCTLA.bit.PRD = AQ_SET;			   // Set PWM1A on CAU
//		EPwm3Regs.AQCTLA.bit.ZRO = AQ_CLEAR;		   // Clear PWM1A on CAD


//		EPwm3Regs.AQCTLA.bit.PRD = 0;
//		EPwm3Regs.AQCTLA.bit.ZRO = 0;//Action When TBCTR = 0
//									//Note: By definition, in count up-down mode when the counter equals 0 the direction is defined as 1 or counting up.
//									//00: Do nothing (action disabled)
//									//01: Clear: force EPWMxA output low.
//									//10: Set: force EPWMxA output high.
//									//11: Toggle EPWMxA output: low output signal will be forced high,and a high signal will be forced low.
//									//Reset type: SYSRSn
//		EPwm3Regs.AQCTLB.bit.PRD = 0;
//		EPwm3Regs.AQCTLB.bit.ZRO = 0;

		EPwm3Regs.DBCTL.bit.IN_MODE = 0x0;
		EPwm3Regs.DBCTL.bit.OUT_MODE = 0x3; 	//ensble DBM
		EPwm3Regs.DBCTL.bit.POLSEL = 0;//0x2;//10: Active high complementary (AHC). EPWMxB is inverted..
		EPwm3Regs.DBRED.all = cDeadTime;		   //dead time is 0.5us=50*(1/100M)
		EPwm3Regs.DBFED.all = cDeadTime;

		// Set Trip-Zone
		EALLOW;
	   EPwm3Regs.TZCTL.bit.TZA = 0x2; //0-High impedance;1-force to high state;2-force to low state;3-no change
       EPwm3Regs.TZCTL.bit.TZB = 0x2;	   


//	   EPwm3Regs.TZCTL.bit.TZA = 0x3;
//	   EPwm3Regs.TZCTL.bit.TZB = 0x3;
//	   EPwm3Regs.TZCLR.bit.OST = 0x1;

	   EPwm3Regs.TZCTL.bit.DCAEVT1 = 0x2;
	   EPwm3Regs.TZCTL.bit.DCBEVT1 = 0x2;	
	   
	   EPwm3Regs.TZCTL.bit.DCAEVT2 = 0x2;
	   EPwm3Regs.TZCTL.bit.DCBEVT2 = 0x2;

	   EDIS;

	   EPwm3Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
	   EPwm3Regs.ETSEL.bit.SOCASEL = 5;            // 101: Enable event time-base counter equal to CMPA when the timer
                                                   // is decrementing or CMPC when the timer is decrementing
	                                               // 100: Enable event time-base counter equal to CMPA when the timer
                                                   // is incrementing or CMPC when the timer is incrementing
	   EPwm3Regs.ETSEL.bit.SOCASELCMP = 1;         // CMPC
	   EPwm3Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
	   EPwm3Regs.CMPC = 600;

	   EPwm3Regs.ETSEL.bit.SOCBEN  = 1;            // Enable SOC on A group
	   EPwm3Regs.ETSEL.bit.SOCBSEL = 7;            // 111: Enable event: time-base counter equal to CMPB when the timer
                                                   // is decrementing or CMPD when the timer is decrementing
	                                               // 110: Enable event: time-base counter equal to CMPB when the timer
                                                   // is incrementing or CMPD when the timer is incrementing
	   EPwm3Regs.ETSEL.bit.SOCBSELCMP = 1;         // CMPD
	   EPwm3Regs.ETPS.bit.SOCBPRD = 1;             // Generate pulse on 1st event
	   EPwm3Regs.CMPD = 300;
}


/*********************************************************************
Function name:  void InitEPWM2(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void InitEPWM2(void)
{

		// Disable all the muxes first
		EALLOW;
        EPwmXbarRegs.TRIP4MUXENABLE.all = 0x0000;
		EPwmXbarRegs.TRIP5MUXENABLE.all = 0x0000;
		EPwmXbarRegs.TRIP7MUXENABLE.all = 0x0000;
		EPwmXbarRegs.TRIP8MUXENABLE.all = 0x0000;
		 
        // Clear everything first
        EPwmXbarRegs.TRIP4MUX0TO15CFG.all  = 0x0000;
        EPwmXbarRegs.TRIP4MUX16TO31CFG.all = 0x0000;
        // Enable Muxes for ored input of CMPSS1H and 1L, i.e. .1 mux for Mux0
        EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX0  = 0;  //CMPSS1.CTRIPH
        EPwmXbarRegs.TRIP5MUX0TO15CFG.bit.MUX2  = 0;  //CMPSS2.CTRIPH
//        EPwmXbarRegs.TRIP7MUX0TO15CFG.bit.MUX4  = 0;  //CMPSS3.CTRIPH
//        EPwmXbarRegs.TRIP8MUX0TO15CFG.bit.MUX6  = 0;  //CMPSS4.CTRIPH

        // Enable Mux 0  OR Mux 4 to generate TRIP4
        EPwmXbarRegs.TRIP4MUXENABLE.bit.MUX0  = 1;
		EPwmXbarRegs.TRIP5MUXENABLE.bit.MUX2  = 1;
//		EPwmXbarRegs.TRIP7MUXENABLE.bit.MUX4  = 1;
//		EPwmXbarRegs.TRIP8MUXENABLE.bit.MUX6  = 1;

	//EPwm2Regs.DCAHTRIPSEL.bit.TRIPINPUT4 = 1;
	//EPwm2Regs.DCBHTRIPSEL.bit.TRIPINPUT4 = 1;
	
	EPwm2Regs.DCAHTRIPSEL.bit.TRIPINPUT5 = 1;
	EPwm2Regs.DCBHTRIPSEL.bit.TRIPINPUT5 = 1;

       EPwm2Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 0xF; //Trip 4 is the input to the DCAHCOMPSEL
        //EPwm2Regs.DCTRIPSEL.bit.DCALCOMPSEL = 3; //Trip 4 is the input to the DCALCOMPSEL
        EPwm2Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 0xF; //Trip 4 is the input to the DCBHCOMPSEL
		//EPwm2Regs.DCTRIPSEL.bit.DCBLCOMPSEL = 3; //Trip 4 is the input to the DCALCOMPSEL

		EPwm2Regs.TZDCSEL.bit.DCAEVT1       = 2;//Digital Compare Output A Event 1 Selection
                                                        //000: Event disabled
                                                        //001: DCAH = low, DCAL = don't care
                                                        //010: DCAH = high, DCAL = don't care
                                                        //011: DCAL = low, DCAH = don't care
                                                        //100: DCAL = high, DCAH = don't care
                                                        //101: DCAL = high, DCAH = low
                                                        //110: Reserved
                                                   //111: Reserved
        EPwm2Regs.TZDCSEL.bit.DCAEVT2       = 2;
		EPwm2Regs.TZDCSEL.bit.DCBEVT1       = 2;
		EPwm2Regs.TZDCSEL.bit.DCBEVT2       = 2;//*************************
		
//        EPwm2Regs.DCACTL.bit.EVT1SRCSEL     = DC_EVT1;//DCAEVT1 Source Signal Select
//                                                      //0: Source Is DCAEVT1 Signal
//                                                      //1: Source Is DCEVTFILT Signal
//        EPwm2Regs.DCACTL.bit.EVT2SRCSEL     = DC_EVT1;//DC_EVT2;
//		EPwm2Regs.DCBCTL.bit.EVT1SRCSEL     = DC_EVT1;
//		EPwm2Regs.DCBCTL.bit.EVT2SRCSEL 	= DC_EVT1;//DC_EVT2;

//       EPwm2Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;//DCAEVT1 Force Synchronization Signal Select
//                                                            //0: Source is synchronized with EPWMCLK
//                                                            //1: Source is passed through asynchronously
//       EPwm2Regs.DCACTL.bit.EVT2FRCSYNCSEL = DC_EVT_ASYNC;
//	   EPwm2Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
//	   EPwm2Regs.DCBCTL.bit.EVT2FRCSYNCSEL = DC_EVT_ASYNC;

//		EPwm2Regs.DCACTL.bit.EVT1SOCE = 0;
//		EPwm2Regs.DCACTL.bit.EVT1SYNCE = 0;

//		EPwm2Regs.DCBCTL.bit.EVT1SOCE = 0;
//		EPwm2Regs.DCBCTL.bit.EVT1SYNCE = 0;

//		EPwm2Regs.DCFCTL.bit.BLANKE = 1;//ENable blanking window
//		EPwm2Regs.DCFCTL.bit.PULSESEL = 2;//Pulse Select For Blanking & Capture Alignment
//											//00: Time-base counter equal to period (TBCTR = TBPRD)
//											//01: Time-base counter equal to zero (TBCTR = 0x00)
//											//10: Time-base counter equal to zero (TBCTR = 0x00) or period
//											//(TBCTR = TBPRD)
//											//11: Reserved
//		EPwm2Regs.DCFCTL.bit.SRCSEL = 1;//Filter Block Signal Source Select
//										//00: Source Is DCAEVT1 Signal
//										//01: Source Is DCAEVT2 Signal
//										//10: Source Is DCBEVT1 Signal
//										//11: Source Is DCBEVT2 Signal
		/*Blanking Window Offset These 16-bits specify the number of TBCLK cycles from the blanking
window reference to the point when the blanking window is applied. The blanking window reference is either period or zero as defined by
the DCFCTL[PULSESEL] bit. This offset register is shadowed and the active register is loaded at the reference point defined by
DCFCTL[PULSESEL]. The offset counter is also initialized and begins to count down when the active register is loaded. When the
counter expires, the blanking window is applied. If the blanking window is currently active, then the blanking window counter is
restarted.*/								
//		EPwm2Regs.DCFOFFSET = 0;
//		EPwm2Regs.DCFWINDOW = 30;

		EPwm2Regs.TZSEL.bit.CBC1 = 0;//Disable TZ1 as a CBC trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.CBC2 = 0;//Disable TZ2 as a CBC trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.CBC3 = 0;//Disable TZ3 as a CBC trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.CBC4 = 0;//Disable TZ4 as a CBC trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.CBC5 = 0;//Disable TZ5 as a CBC trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.CBC6 = 0;//Disable TZ6 as a CBC trip source for this ePWM module

		EPwm2Regs.TZSEL.bit.OSHT1 = 0;//Enable TZ1 as a OST trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.OSHT2 = 0;//Disable TZ2 as a OST trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.OSHT3 = 0;//Disable TZ3 as a OST trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.OSHT4 = 0;//Disable TZ4 as a OST trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.OSHT5 = 0;//Disable TZ5 as a OST trip source for this ePWM module
		EPwm2Regs.TZSEL.bit.OSHT6 = 0;//Disable TZ6 as a OST trip source for this ePWM module
//		
//		
        EPwm2Regs.TZSEL.bit.DCAEVT1         = 0;           //Digital Compare Output A Event 1 Select
                                                           // 0: Disable DCAEVT1 as one-shot-trip source for this ePWM module.
                                                           // 1: Enable DCAEVT1 as one-shot-trip source for this ePWM module.
        EPwm2Regs.TZSEL.bit.DCBEVT1         = 0;
//		if(CompMode == CBCmode)
//		{
//			EPwm2Regs.TZSEL.bit.DCAEVT2         = 1;          // Digital Compare Output A Event 2 Select
//															//0: Disable DCAEVT2 as a CBC trip source for this ePWM module
//															//1: Enable DCAEVT2 as a CBC trip source for this ePWM module
//															//Reset type: SYSRSn
//			EPwm2Regs.TZSEL.bit.DCBEVT2         = 1;		
//		}
//		else
		{
			EPwm2Regs.TZSEL.bit.DCAEVT2         = 0;//1;          // Digital Compare Output A Event 2 Select
															//0: Disable DCAEVT2 as a CBC trip source for this ePWM module
															//1: Enable DCAEVT2 as a CBC trip source for this ePWM module
															//Reset type: SYSRSn
			EPwm2Regs.TZSEL.bit.DCBEVT2         = 0;//1;			
		}


        EDIS;
	
	
		EPwm2Regs.TBPRD = SwitchPeriod[Fsw80KHz]; 						// Set timer period (12.5us)500=100M/80k/2
		EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
				//sync
        EPwm2Regs.EPWMSYNCINSEL.bit.SEL = 0x1; //EPWM1.SYNCOUT
												
		EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;        // Disable phase loading	
        EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;             // Phase is 0
        EPwm2Regs.TBCTR = 0x0000;                       // Clear counter	
        

        EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
        EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
		EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;

        EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

        EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	    
		EPwm2Regs.CMPA.bit.CMPA = 0;
	/*	if(CompMode == T1mode)
		{
			EPwm2Regs.AQTSRCSEL.bit.T1SEL = 1;		//0000: DCAEVT1
												//0001: DCAEVT2
												//0010: DCBEVT1
												//0011: DCBEVT2

			EPwm2Regs.AQCTLA2.bit.T1U = AQ_SET;			  // if limit duty
			EPwm2Regs.AQCTLA2.bit.T1D = AQ_CLEAR;	
		}*/



	
		EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;			  // Clear PWM1A on CAU
		//EPwm2Regs.AQCTLB.bit.CAU = AQ_NO_ACTION;			  // 
		EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;			 //  Set PWM1A on CAD
		//EPwm2Regs.AQCTLB.bit.CAD = AQ_NO_ACTION;			  //


		EPwm2Regs.AQSFRC.bit.RLDCSF = 3;//Load immediately
		EPwm2Regs.AQCSFRC.bit.CSFA = 2;
		EPwm2Regs.AQCSFRC.bit.CSFB = 2;
//		EPwm2Regs.AQSFRC.bit.ACTSFA = 1;
//		EPwm2Regs.AQSFRC.bit.ACTSFB = 1;
		//EPwm2Regs.AQSFRC.bit.OTSFA = 1; 
		//EPwm2Regs.AQSFRC.bit.OTSFB = 1; 

//	EPwm2Regs.AQCTLA.bit.PRD = AQ_SET;			   // Set PWM1A on CAU
//	EPwm2Regs.AQCTLA.bit.ZRO = AQ_CLEAR;		   // Clear PWM1A on CAD

//		EPwm2Regs.AQCTLA.bit.PRD = 0;
//		EPwm2Regs.AQCTLA.bit.ZRO = 0;//Action When TBCTR = 0
//									//Note: By definition, in count up-down mode when the counter equals 0 the direction is defined as 1 or counting up.
//									//00: Do nothing (action disabled)
//									//01: Clear: force EPWMxA output low.
//									//10: Set: force EPWMxA output high.
//									//11: Toggle EPWMxA output: low output signal will be forced high,and a high signal will be forced low.
//									//Reset type: SYSRSn
//		EPwm2Regs.AQCTLB.bit.PRD = 0;
//		EPwm2Regs.AQCTLB.bit.ZRO = 0;

		EPwm2Regs.DBCTL.bit.IN_MODE = 0x0;
		EPwm2Regs.DBCTL.bit.OUT_MODE = 0x3; 	//ensble DBM
		EPwm2Regs.DBCTL.bit.POLSEL = 0;//0x2;//10: Active high complementary (AHC). EPWMxB is inverted..
		EPwm2Regs.DBRED.all = cDeadTime;		   //dead time is 0.5us=50*(1/100M)
		EPwm2Regs.DBFED.all = cDeadTime;

		// Set Trip-Zone
		EALLOW;
	   EPwm2Regs.TZCTL.bit.TZA = 0x2; //0-High impedance;1-force to high state;2-force to low state;3-no change
       EPwm2Regs.TZCTL.bit.TZB = 0x2;	   
//       EPwm2Regs.TZFRC.bit.OST = 0x1;

//	   EPwm2Regs.TZCTL.bit.TZA = 0x3;
//	   EPwm2Regs.TZCTL.bit.TZB = 0x3;
//	   EPwm2Regs.TZCLR.bit.OST = 0x1;

	   EPwm2Regs.TZCTL.bit.DCAEVT1 = 0x2;
	   EPwm2Regs.TZCTL.bit.DCBEVT1 = 0x2;	
	   
	   EPwm2Regs.TZCTL.bit.DCAEVT2 = 0x2;
	   EPwm2Regs.TZCTL.bit.DCBEVT2 = 0x2;



//	   EPwm2Regs.TZCLR.bit.CBCPULSE = 0x0;

//	   EPwm2Regs.TZEINT.bit.CBC  = 1;
	   EPwm2Regs.TZEINT.bit.OST  = 1;
	   EDIS;

	   EPwm2Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
       EPwm2Regs.ETSEL.bit.SOCASEL = 5;            // 101: Enable event time-base counter equal to CMPA when the timer
                                                   // is decrementing or CMPC when the timer is decrementing
                                                   // 100: Enable event time-base counter equal to CMPA when the timer
                                                   // is incrementing or CMPC when the timer is incrementing
       EPwm2Regs.ETSEL.bit.SOCASELCMP = 1;         // CMPC
       EPwm2Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
       EPwm2Regs.CMPC = 1200;

       EPwm2Regs.ETSEL.bit.SOCBEN  = 1;            // Enable SOC on A group
       EPwm2Regs.ETSEL.bit.SOCBSEL = 7;            // 111: Enable event: time-base counter equal to CMPB when the timer
                                                   // is decrementing or CMPD when the timer is decrementing
                                                   // 110: Enable event: time-base counter equal to CMPB when the timer
                                                   // is incrementing or CMPD when the timer is incrementing
       EPwm2Regs.ETSEL.bit.SOCBSELCMP = 1;         // CMPD
       EPwm2Regs.ETPS.bit.SOCBPRD = 1;             // Generate pulse on 1st event
       EPwm2Regs.CMPD = 900;
}

/*********************************************************************
Function name:  void InitEPWM1(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void InitEPWM1(void)
{
//EPWM1A,EPWM1B
//--------------------------------------------------------------------------------//
        EPwm1Regs.TBPRD = cControlperiod;                         // Set timer period (12.5us)500=100M/80k/2  
       // EPwm1Regs.CMPA.bit.CMPA = cCMPvalue;
        EPwm1Regs.TBCTR = 0x0000;                       // Clear counter
        EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
		//sync
//		if(GetBoardType() == UnitCB)
//		{
//			EALLOW;
//			InputXbarRegs.INPUT5SELECT = 22;
//			EDIS;
//			EPwm1Regs.TBCTL.bit.PHSEN = TB_ENABLE;
//			EPwm1Regs.TBCTL.bit.PHSDIR = 1;
//			EPwm1Regs.EPWMSYNCINSEL.bit.SEL = 0x18;	//INPUT-XBAR.INPUT5
//		}
//		else
		{
			EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
		}
        EPwm1Regs.EPWMSYNCOUTEN.bit.ZEROEN = 1; //This bit enables the TBCTR = 0x0000 event to set the
												//EPWMxSYNCOUT signal.
												//0 Disabled
												//1 The EPWMxSYNCOUT signal is pulsed for one PWM clock period
												//upon the value of TBCTR changing to 0x0000
	
		EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;            // Phase is 0	
//        EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
        EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
        EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

        EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

        EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

        // Set actions
        EPwm1Regs.AQCTLA.bit.CAU = AQ_NO_ACTION;//AQ_CLEAR;             // Set PWM1A on CAU
        EPwm1Regs.AQCTLA.bit.CAD = AQ_NO_ACTION;//AQ_SET;           // Clear PWM1A on CAD
        EPwm1Regs.AQCTLA.bit.PRD = AQ_SET;             // Set PWM1A on CAU
        EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;           // Clear PWM1A on CAD

//		EPwm1Regs.AQCTLA.bit.PRD = AQ_SET;             // Set PWM1A on CAU
//        EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;           // Clear PWM1A on CAD

        // Set Dead-Band, set to a pair PWMAB
        EPwm1Regs.DBCTL.bit.IN_MODE = 0x0;
        EPwm1Regs.DBCTL.bit.OUT_MODE = 0x3;     //ensble DBM
        EPwm1Regs.DBCTL.bit.POLSEL = 0x2;       //active high complementary mode
        EPwm1Regs.DBRED.all = 0;             //dead time is 0.5us=50*(1/100M)
        EPwm1Regs.DBFED.all = 0;             //dead time is 0.5us


        // Set event-trigger
        EPwm1Regs.ETSEL.bit.INTEN = 0x1;            // Enable EPWMx_INT generation
		EPwm1Regs.ETSEL.bit.INTSEL = 0x1;           // Enable event time-base counter equal to zero. (TBCTR = 0x00)
		
        EPwm1Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
        EPwm1Regs.ETSEL.bit.SOCASEL = 1;            // 001: Enable event time-base counter equal to zero. (TBCTR = 0x00)
        EPwm1Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
        
        EPwm1Regs.ETSEL.bit.SOCBEN  = 1;            // Enable SOC on A group
        EPwm1Regs.ETSEL.bit.SOCBSEL = 2;            // 010: Enable event time-base counter equal to period (TBCTR = TBPRD)
        EPwm1Regs.ETPS.bit.SOCBPRD = 1;             // Generate pulse on 1st event

		EPWM_setInterruptEventCount(EPWM1_BASE, 1);
		EPwm1Regs.ETCLR.bit.INT = 1;
		

				// Set Trip-Zone

}

/*********************************************************************
Function name:  void InitEPWM4(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void InitEPWM4(void)
{
//EPWM1A,EPWM1B
//--------------------------------------------------------------------------------//
        EPwm4Regs.TBPRD = 3000;                         // Set timer period (25us)3000=25*120 
       // EPwm1Regs.CMPA.bit.CMPA = cCMPvalue;
        EPwm4Regs.TBCTR = 0x0000;                       // Clear counter
        EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_DOWN;
		//sync
        EPwm4Regs.EPWMSYNCOUTEN.bit.ZEROEN = 0; //This bit enables the TBCTR = 0x0000 event to set the
												//EPWMxSYNCOUT signal.
												//0 Disabled
												//1 The EPWMxSYNCOUT signal is pulsed for one PWM clock period
												//upon the value of TBCTR changing to 0x0000
	
		EPwm4Regs.TBPHS.bit.TBPHS = 0x0000;            // Phase is 0	
        EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
        EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
        EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;

        EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

        EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

        // Set actions
        EPwm4Regs.AQCTLA.bit.CAU = AQ_NO_ACTION;//AQ_CLEAR;             // Set PWM1A on CAU
        EPwm4Regs.AQCTLA.bit.CAD = AQ_NO_ACTION;//AQ_SET;           // Clear PWM1A on CAD
        EPwm4Regs.AQCTLA.bit.PRD = AQ_SET;             // Set PWM1A on CAU
        EPwm4Regs.AQCTLA.bit.ZRO = AQ_SET;           // Clear PWM1A on CAD

//		EPwm1Regs.AQCTLA.bit.PRD = AQ_SET;             // Set PWM1A on CAU
//        EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;           // Clear PWM1A on CAD

        // Set Dead-Band, set to a pair PWMAB
        EPwm4Regs.DBCTL.bit.IN_MODE = 0x0;
        EPwm4Regs.DBCTL.bit.OUT_MODE = 0x3;     //ensble DBM
        EPwm4Regs.DBCTL.bit.POLSEL = 0x2;       //active high complementary mode
        EPwm4Regs.DBRED.all = 0;             //dead time is 0.5us=50*(1/100M)
        EPwm4Regs.DBFED.all = 0;             //dead time is 0.5us


        // Set event-trigger
        EPwm4Regs.ETSEL.bit.INTEN = 0x1;            // Enable EPWMx_INT generation
		EPwm4Regs.ETSEL.bit.INTSEL = 0x1;            // Enable event time-base counter equal to zero. (TBCTR = 0x00)
		
        EPwm4Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
        EPwm4Regs.ETSEL.bit.SOCASEL = 1;            // EPWM1SOCA pulse will be generated when TBCTR = 0x00 or  TBCTR = prd
        EPwm4Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
        
		EPWM_setInterruptEventCount(EPWM4_BASE, 1);
		EPwm4Regs.ETCLR.bit.INT = 1;
		

				// Set Trip-Zone

}

/*********************************************************************
Function name:  void InitEPWM5(void)
Description:    
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void InitEPWM5(void)
{
//EPWM1A,EPWM1B
//--------------------------------------------------------------------------------//
        EPwm5Regs.TBPRD = 12000;                         // Set timer period (100us)12000=100*120 
       // EPwm1Regs.CMPA.bit.CMPA = cCMPvalue;
        EPwm5Regs.TBCTR = 0x0000;                       // Clear counter
        EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_DOWN;
		//sync
        EPwm5Regs.EPWMSYNCOUTEN.bit.ZEROEN = 0; //This bit enables the TBCTR = 0x0000 event to set the
												//EPWMxSYNCOUT signal.
												//0 Disabled
												//1 The EPWMxSYNCOUT signal is pulsed for one PWM clock period
												//upon the value of TBCTR changing to 0x0000
	
		EPwm5Regs.TBPHS.bit.TBPHS = 0x0000;            // Phase is 0	
        EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
        EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
        EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1;

        EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

        EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;    // Load registers every ZERO
        EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

        // Set actions
        EPwm5Regs.AQCTLA.bit.CAU = AQ_NO_ACTION;//AQ_CLEAR;             // Set PWM1A on CAU
        EPwm5Regs.AQCTLA.bit.CAD = AQ_NO_ACTION;//AQ_SET;           // Clear PWM1A on CAD
        EPwm5Regs.AQCTLA.bit.PRD = AQ_SET;             // Set PWM1A on CAU
        EPwm5Regs.AQCTLA.bit.ZRO = AQ_SET;           // Clear PWM1A on CAD

//		EPwm1Regs.AQCTLA.bit.PRD = AQ_SET;             // Set PWM1A on CAU
//        EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;           // Clear PWM1A on CAD

        // Set Dead-Band, set to a pair PWMAB
        EPwm5Regs.DBCTL.bit.IN_MODE = 0x0;
        EPwm5Regs.DBCTL.bit.OUT_MODE = 0x3;     //ensble DBM
        EPwm5Regs.DBCTL.bit.POLSEL = 0x2;       //active high complementary mode
        EPwm5Regs.DBRED.all = 0;             //dead time is 0.5us=50*(1/100M)
        EPwm5Regs.DBFED.all = 0;             //dead time is 0.5us


        // Set event-trigger
        EPwm5Regs.ETSEL.bit.INTEN = 0x0;            // Enable EPWMx_INT generation
		EPwm5Regs.ETSEL.bit.INTSEL = 0x1;            // Enable event time-base counter equal to zero. (TBCTR = 0x00)
		
        EPwm5Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
        EPwm5Regs.ETSEL.bit.SOCASEL = 1;            // EPWM1SOCA pulse will be generated when TBCTR = 0x00 or  TBCTR = prd
        EPwm4Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
        
		EPWM_setInterruptEventCount(EPWM5_BASE, 1);
		EPwm5Regs.ETCLR.bit.INT = 1;
		

				// Set Trip-Zone

}


/*********************************************************************
Function name:  void InitEPWM6(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void InitEPWM6(void)
{
    EPwm6Regs.TBPRD = cControlperiod;               // Set timer period (12.5us)500=100M/80k/2
    EPwm6Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    //sync
    EPwm6Regs.EPWMSYNCINSEL.bit.SEL = 0x1;          //EPWM1.SYNCOUT

    EPwm6Regs.TBPHS.bit.TBPHS = 0x0000;             // Phase is 0
    EPwm6Regs.TBCTL.bit.PHSEN = TB_ENABLE;
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;

    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

    EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    EPwm6Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
    EPwm6Regs.ETSEL.bit.SOCASEL = 4;            // 100: Enable event time-base counter equal to CMPA when the timer
                                                // is incrementing or CMPC when the timer is incrementing
                                                // 101: Enable event time-base counter equal to CMPA when the timer
                                                // is decrementing or CMPC when the timer is decrementing
    EPwm6Regs.ETSEL.bit.SOCASELCMP = 1;         // CMPC
    EPwm6Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
    EPwm6Regs.CMPC = 300;

    EPwm6Regs.ETSEL.bit.SOCBEN  = 1;            // Enable SOC on A group
    EPwm6Regs.ETSEL.bit.SOCBSEL = 6;            // 110: Enable event: time-base counter equal to CMPB when the timer
                                                // is incrementing or CMPD when the timer is incrementing
                                                // 111: Enable event: time-base counter equal to CMPB when the timer
                                                // is decrementing or CMPD when the timer is decrementing
    EPwm6Regs.ETSEL.bit.SOCBSELCMP = 1;         // CMPD
    EPwm6Regs.ETPS.bit.SOCBPRD = 1;             // Generate pulse on 1st event
    EPwm6Regs.CMPD = 600;
}

/*********************************************************************
Function name:  void InitEPWM7(void)
Description:
Calls:
Called By:
Parameters:     void
Return:         void
 *********************************************************************/
void InitEPWM7(void)
{
    EPwm7Regs.TBPRD = cControlperiod;               // Set timer period (12.5us)500=100M/80k/2
    EPwm7Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    //sync
    EPwm7Regs.EPWMSYNCINSEL.bit.SEL = 0x1;          //EPWM1.SYNCOUT

    EPwm7Regs.TBPHS.bit.TBPHS = 0x0000;             // Phase is 0
    EPwm7Regs.TBCTL.bit.PHSEN = TB_ENABLE;
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm7Regs.TBCTL.bit.PRDLD = TB_SHADOW;

    EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

    EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    EPwm7Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
    EPwm7Regs.ETSEL.bit.SOCASEL = 4;            // 100: Enable event time-base counter equal to CMPA when the timer
                                                // is incrementing or CMPC when the timer is incrementing
                                                // 101: Enable event time-base counter equal to CMPA when the timer
                                                // is decrementing or CMPC when the timer is decrementing
    EPwm7Regs.ETSEL.bit.SOCASELCMP = 1;         // CMPC
    EPwm7Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
    EPwm7Regs.CMPC = 900;

    EPwm7Regs.ETSEL.bit.SOCBEN  = 1;            // Enable SOC on A group
    EPwm7Regs.ETSEL.bit.SOCBSEL = 6;            // 110: Enable event: time-base counter equal to CMPB when the timer
                                                // is incrementing or CMPD when the timer is incrementing
                                                // 111: Enable event: time-base counter equal to CMPB when the timer
                                                // is decrementing or CMPD when the timer is decrementing
    EPwm7Regs.ETSEL.bit.SOCBSELCMP = 1;         // CMPD
    EPwm7Regs.ETPS.bit.SOCBPRD = 1;             // Generate pulse on 1st event
    EPwm7Regs.CMPD = 1200;
}
