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
#include "Interrupt.h"
#include "Initial.h"
#include "Version.h"
#include "Common.h"
#include "f280013x_device.h"
#include "f280013x_epwm_defines.h"
#include "f280013x_adc_defines.h"
#include "string.h"
//#include "f280013x_adc.h"


/********************************************************************************
*const define                               *
********************************************************************************/



/********************************************************************************
* Variable declaration                              *
********************************************************************************/
volatile uint32_t TimerTick = 0;


/********************************************************************************
* function declaration                              *
********************************************************************************/
//
// configCPUTimer - This function initializes the selected timer to the
// period specified by the "freq" and "period" variables. The "freq" is
// CPU frequency in Hz and the period in uSeconds. The timer is held in
// the stopped state after configuration.
//
void configCPUTimer(uint32_t cpuTimer, uint32_t period)
{
    uint32_t temp, freq = DEVICE_SYSCLK_FREQ;

    //
    // Initialize timer period:
    //
    temp = ((freq / 1000000) * period);
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
Function name:  TaskTick_ISR
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
#if TaskTick_ISR_Enable
interrupt void TaskTick_ISR( void )
{
    TimerTick++;
}
#endif

/*********************************************************************
Function name:  getTimerTickCur
Description:
Calls:
Called By:
Parameters:     void
Return:         uint32_t
*********************************************************************/
#if TaskTick_ISR_Enable
uint32_t getTimerTickCur( void )
{
    return TimerTick;
}
#endif
/*********************************************************************
Function name:  waitMs
Description:
Calls:
Called By:
Parameters:     uint32_t
Return:         void
*********************************************************************/
#if TaskTick_ISR_Enable
void waitMs( uint32_t ms )
{
    uint32_t cur = getTimerTickCur();

    while(1)
    {
        if(getTimerTickCur() < cur)
        {
            if(0xffffffff - cur + getTimerTickCur() > ms)
                return;
        }
        else
            if(getTimerTickCur() - cur > ms)
                return;
    }
}
#endif
//-------------------------------------------------------------------------------------------------
#if TaskTick_ISR_Enable
static void setupTimer1( void )
{

    Interrupt_register(INT_TIMER1, &TaskTick_ISR);

    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);

    configCPUTimer(CPUTIMER1_BASE, 1000);

    CPUTimer_enableInterrupt(CPUTIMER1_BASE);

    Interrupt_enable(INT_TIMER1);
    CPUTimer_startTimer(CPUTIMER1_BASE);
}
#endif
#if 0
static void setepwm1( void )
{
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBasePeriod(EPWM1_BASE, 1500);
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
/*********************************************************************
Function name:  setepwm2
Description:
Calls:
Called By:
Parameters:     void
Return:         void
*********************************************************************/
static void setepwm2( void )
{

}
#endif
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
//    IER = 0x0000;
//    IFR = 0x0000;

    //
    // Initializes the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    sInitEPWM();
    sInitADC();
    InitSCI();

    versionReg();
#if TaskTick_ISR_Enable
    setupTimer1();
#endif

  // Map ISR functions
     //
     EALLOW;
     PieVectTable.ADCA1_INT = &ADC_A1_ISR;     // Function for ADCA interrupt 1
//     PieVectTable.ECAP1_INT = &PWMsync_CAP1_ISR;     // Function for ADCA interrupt 1
//     PieVectTable.ECAP2_INT = &Linesync_CAP2_ISR;     // Function for ADCA interrupt 1
// 	 PieVectTable.EPWM2_TZ_INT = &EPWM2_TZ1_ISR;     //
 	 PieVectTable.EPWM1_INT = &epwm1ISR;     //
// 	 PieVectTable.CANA0_INT = &canISR;     // Function for CAN0 interrupt 1
     EDIS;
     //
      // Enable global Interrupts and higher priority real-time debug events:
      //
      IER |= M_INT1 |M_INT2 | M_INT3 | M_INT4 | M_INT9;  // Enable group 1 ,4 interrupts

      EINT;           // Enable Global interrupt INTM
      ERTM;           // Enable Global realtime interrupt DBGM

      //
         // Enable PIE interrupt
         //
   // PieCtrlRegs.PIEIER1.bit.INTx1 = 1;//Enable PIE group 1 interrupt 1 for ADCA1	
     //PieCtrlRegs.PIEIER4.bit.INTx1 = 1;//Enable PIE group 4 interrupt 1 for CAP1,PWM SYNC
     //PieCtrlRegs.PIEIER4.bit.INTx2 = 1;//Enable PIE group 4 interrupt 2 for CAP2,Line SYNC
//     PieCtrlRegs.PIEIER2.bit.INTx2 = 1;//Enable PIE group 2 interrupt 2 for EPWM2_TZ
     PieCtrlRegs.PIEIER3.bit.INTx1 = 1;//Enable PIE group3 interrupt 1 for EPWM1
//    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;//Enable PIE group 9 interrupt 5 for CAN0

    DEVICE_DELAY_US(1000);
    AddressCheck();
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
//	   InitEPWM2();
//	   InitEPWM3();

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


   	//AdccRegs.ADCSOC4CTL.bit.CHSEL = 4;	 //SwitchDIPSample, SOC4 will convert pin C4,AdcaResult4
	//AdccRegs.ADCSOC4CTL.bit.ACQPS = 9;	   // Sample window is 10 SYSCLK cycles
	//AdccRegs.ADCSOC4CTL.bit.TRIGSEL = 5;   // Trigger on ePWM1 SOCA
	

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
void InitSCI(void)
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
    //
    // Initialize SCIC and its FIFO.
    //
    SCI_performSoftwareReset(SCIC_BASE);

    //
    // Configure SCIC for echoback.
    //
    SCI_setConfig(
            SCIC_BASE, DEVICE_LSPCLK_FREQ, 19200,
            (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIC_BASE);
    SCI_resetRxFIFO(SCIC_BASE);
    SCI_resetTxFIFO(SCIC_BASE);
    SCI_clearInterruptStatus(SCIC_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(SCIC_BASE);
    SCI_enableModule(SCIC_BASE);
    SCI_performSoftwareReset(SCIC_BASE);
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
    gpioBaseAddr = (Uint32*) &GpioCtrlRegs;
    for (regOffset = 0; regOffset < sizeof(GpioCtrlRegs) / 2; regOffset++)
    {
        //
        // Must avoid enabling pull-ups on all pins. GPyPUD is offset
        // 0x0C in each register group of 0x40 words. Since this is a
        // 32-bit pointer, the addresses must be divided by 2.
        //
        // Also, to avoid changing pin muxing of the emulator pins to regular
        // GPIOs, skip GPBMUX1 (0x46) and GPBGMUX1 (0x60).
        //
        if ((regOffset % (0x40 / 2) != (0x0C / 2)) && (regOffset != (0x46 / 2))
                && (regOffset != (0x60 / 2)))
        {
            gpioBaseAddr[regOffset] = 0x00000000;
        }
    }

    gpioBaseAddr = (Uint32*) &GpioDataRegs;
    for (regOffset = 0; regOffset < sizeof(GpioDataRegs) / 2; regOffset++)
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

    // GPIO9 is the SCI Tx pin.
    GPIO_setPinConfig(GPIO_9_SCIB_TX);
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
		GPIO_setPadConfig(39, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO39
		GPIO_writePin(39, 1);						// Load output latch
		GPIO_setPinConfig(GPIO_39_GPIO39);			//GPIO39 = Line Sync TX,output
		GPIO_setDirectionMode(39, GPIO_DIR_MODE_OUT);

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
//		GPIO_setPadConfig(37, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO37
//		GPIO_writePin(37, 1);						// Load output latch
//		GPIO_setPinConfig(GPIO_37_GPIO37);			//GPIO37= OH LED
//		GPIO_setDirectionMode(37, GPIO_DIR_MODE_OUT);


//	//GPIO35= fault LED
//		GPIO_setPadConfig(35, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO35
//		GPIO_writePin(35, 1);						// Load output latch
//		GPIO_setPinConfig(GPIO_35_GPIO35);			//GPIO35= fault LED
//		GPIO_setDirectionMode(35, GPIO_DIR_MODE_OUT);

//-------------------------------- CAN signal ---------------------------------------------------//
    //GPIO17= CANA_TX
    //GPIO_setPadConfig(35, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO17
    //GPIO_writePin(35, 1);						// Load output latch
    GPIO_setPinConfig(GPIO_17_CANA_TX);			//GPIO17= CANA_TX
    //GPIO_setDirectionMode(35, GPIO_DIR_MODE_OUT);

    //GPIO18= CANA_RX
    //GPIO_setPadConfig(18, GPIO_PIN_TYPE_PULLUP); // Enable pullup on GPIO18
    //GPIO_writePin(18, 1);						// Load output latch
    GPIO_setAnalogMode(230, GPIO_ANALOG_DISABLED);
    GPIO_setPinConfig(GPIO_230_CANA_RX);			//GPIO18= CANA_RX
    //GPIO_setDirectionMode(18, GPIO_DIR_MODE_OUT);
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
    //GPIO_writePin(1, 1);                      // Load output latch
    GPIO_setPinConfig(GPIO_1_GPIO1);            //GPIO1= DIP switch4
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
    //GPIO_writePin(40, 1);                     // Load output latch
    GPIO_setPinConfig(GPIO_40_GPIO40);          // GPIO40= DIP switch2
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
		InputXbarRegs.INPUT1SELECT = 226;//TZ1,TRIP1
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
	
	EPwm3Regs.DCAHTRIPSEL.bit.TRIPINPUT5 = 1;
	EPwm3Regs.DCBHTRIPSEL.bit.TRIPINPUT5 = 1;

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

		EPwm3Regs.TZSEL.bit.OSHT1 = 1;//Enable TZ1 as a OST trip source for this ePWM module
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

		EPwm3Regs.TZSEL.bit.DCAEVT2         = 1;          // Digital Compare Output A Event 2 Select
														//0: Disable DCAEVT2 as a CBC trip source for this ePWM module
														//1: Enable DCAEVT2 as a CBC trip source for this ePWM module
														//Reset type: SYSRSn
		EPwm3Regs.TZSEL.bit.DCBEVT2         = 1;

        EDIS;
	
	
		EPwm3Regs.TBPRD = cPWMperiod; 						// Set timer period (12.5us)500=100M/80k/2
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

	    
		EPwm3Regs.CMPA.bit.CMPA = cPWMperiod/4;

//		EPwm3Regs.AQCTLA2.bit.T1U = AQ_NO_ACTION;			  // if limit duty
//        EPwm3Regs.AQCTLB2.bit.T1U = AQ_NO_ACTION;

//		EPwm3Regs.AQCTLA2.bit.T1U = AQ_NO_ACTION;			  // if limit duty
//        EPwm3Regs.AQCTLB2.bit.T1U = AQ_NO_ACTION;
//		EPwm3Regs.AQCTLA2.bit.T1D = AQ_NO_ACTION;			  // action disabled
//        EPwm3Regs.AQCTLB2.bit.T1D = AQ_NO_ACTION;
		
//		EPwm3Regs.AQCTLA2.bit.T2U = AQ_NO_ACTION;			  // action disabled
//        EPwm3Regs.AQCTLB2.bit.T2U = AQ_NO_ACTION;
//		EPwm3Regs.AQCTLA2.bit.T2D = AQ_NO_ACTION;			  // action disabled
//        EPwm3Regs.AQCTLB2.bit.T2D = AQ_NO_ACTION;
	
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
		EPwm3Regs.DBRED.all = 50;		   //dead time is 0.5us=50*(1/100M)
		EPwm3Regs.DBFED.all = 50;	  

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

	EPwm2Regs.DCAHTRIPSEL.bit.TRIPINPUT4 = 1;
	EPwm2Regs.DCBHTRIPSEL.bit.TRIPINPUT4 = 1;
	
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

		EPwm2Regs.TZSEL.bit.OSHT1 = 1;//Enable TZ1 as a OST trip source for this ePWM module
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

		EPwm2Regs.TZSEL.bit.DCAEVT2         = 1;          // Digital Compare Output A Event 2 Select
														//0: Disable DCAEVT2 as a CBC trip source for this ePWM module
														//1: Enable DCAEVT2 as a CBC trip source for this ePWM module
														//Reset type: SYSRSn
		EPwm2Regs.TZSEL.bit.DCBEVT2         = 1;

        EDIS;
	
	
		EPwm2Regs.TBPRD = cPWMperiod; 						// Set timer period (12.5us)500=100M/80k/2
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

	    
		EPwm2Regs.CMPA.bit.CMPA = cPWMperiod/4;

//		EPwm2Regs.AQCTLA2.bit.T1U = AQ_NO_ACTION;			  // if limit duty
//        EPwm2Regs.AQCTLB2.bit.T1U = AQ_NO_ACTION;

//		EPwm2Regs.AQCTLA2.bit.T1U = AQ_NO_ACTION;			  // if limit duty
//        EPwm2Regs.AQCTLB2.bit.T1U = AQ_NO_ACTION;
//		EPwm2Regs.AQCTLA2.bit.T1D = AQ_NO_ACTION;			  // action disabled
//        EPwm2Regs.AQCTLB2.bit.T1D = AQ_NO_ACTION;
		
//		EPwm2Regs.AQCTLA2.bit.T2U = AQ_NO_ACTION;			  // action disabled
//        EPwm2Regs.AQCTLB2.bit.T2U = AQ_NO_ACTION;
//		EPwm2Regs.AQCTLA2.bit.T2D = AQ_NO_ACTION;			  // action disabled
//        EPwm2Regs.AQCTLB2.bit.T2D = AQ_NO_ACTION;
	
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
		EPwm2Regs.DBRED.all = 50;		   //dead time is 0.5us=50*(1/100M)
		EPwm2Regs.DBFED.all = 50;	  

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
        EPwm1Regs.TBPRD = cPWMperiod;                         // Set timer period (12.5us)500=100M/80k/2  
       // EPwm1Regs.CMPA.bit.CMPA = cCMPvalue;
        EPwm1Regs.TBCTR = 0x0000;                       // Clear counter
        EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
		//sync
        EPwm1Regs.EPWMSYNCOUTEN.bit.ZEROEN = 1; //This bit enables the TBCTR = 0x0000 event to set the
												//EPWMxSYNCOUT signal.
												//0 Disabled
												//1 The EPWMxSYNCOUT signal is pulsed for one PWM clock period
												//upon the value of TBCTR changing to 0x0000
	
		EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;            // Phase is 0	
        EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
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
		EPwm1Regs.ETSEL.bit.INTSEL = 0x1;            // Enable event time-base counter equal to zero. (TBCTR = 0x00)
		
        EPwm1Regs.ETSEL.bit.SOCAEN  = 1;            // Enable SOC on A group
        EPwm1Regs.ETSEL.bit.SOCASEL = 1;            // EPWM1SOCA pulse will be generated when TBCTR = 0x00 or  TBCTR = prd
        EPwm1Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
        
		EPWM_setInterruptEventCount(EPWM1_BASE, 1);
		EPwm1Regs.ETCLR.bit.INT = 1;
		

				// Set Trip-Zone

}

