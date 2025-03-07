//----------------------------------------------------------------------------------
//	FILE:			config.c
//
//	Description:	Contains all peripheral init / config functions
//
//	Version: 		1.0
//
//  Target:  		TMS320F28377S
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments ��蕭 2004-2015
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// 4 Nov 2015 - Peripheral config modules
//----------------------------------------------------------------------------------


/*-------------------------------------------------------------------------------
Include project specific include files.
-------------------------------------------------------------------------------*/
#include "F28x_Project.h"
#include "F2837xS_IO_assignment.h"
#include "MonoMtrServo-Settings.h"

// ****************************************************************************
// ****************************************************************************
//TODO EPWMDAC GPIO Configuration
// ****************************************************************************
// ****************************************************************************
void InitMotor1EPwmDACGpio(void)
{
    // set up EPWM6A: PWMDAC1
    GPIO_SetupPinOptions(MOTOR1_EPWMDAC_6A_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
    GPIO_SetupPinMux(MOTOR1_EPWMDAC_6A_GPIO, 0, MOTOR1_EPWMDAC_6A_MUX);

    // set up EPWM8A: PWMDAC2
    GPIO_SetupPinOptions(MOTOR1_EPWMDAC_8A_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
    GPIO_SetupPinMux(MOTOR1_EPWMDAC_8A_GPIO, 0, MOTOR1_EPWMDAC_8A_MUX);

    // set up EPWM9A: PWMDAC3
    GPIO_SetupPinOptions(MOTOR1_EPWMDAC_9A_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
    GPIO_SetupPinMux(MOTOR1_EPWMDAC_9A_GPIO, 0, MOTOR1_EPWMDAC_9A_MUX);

    // set up EPWM10A: PWMDAC4
    GPIO_SetupPinOptions(MOTOR1_EPWMDAC_10A_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
    GPIO_SetupPinMux(MOTOR1_EPWMDAC_10A_GPIO, 0, MOTOR1_EPWMDAC_10A_MUX);

    return;
}

// ****************************************************************************
// ****************************************************************************
//TODO EPWM GPIO Configuration
// ****************************************************************************
// ****************************************************************************
void InitMotor1EPwmGpio(void)
{
	// set up EPWM-AH
	GPIO_SetupPinOptions(MOTOR1_EPWM_AH_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
	GPIO_SetupPinMux(MOTOR1_EPWM_AH_GPIO, 0, MOTOR1_EPWM_AH_MUX);

	// set up EPWM-AL
    GPIO_SetupPinOptions(MOTOR1_EPWM_AL_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
    GPIO_SetupPinMux(MOTOR1_EPWM_AL_GPIO, 0, MOTOR1_EPWM_AL_MUX);

	// set up EPWM-BH
	GPIO_SetupPinOptions(MOTOR1_EPWM_BH_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
	GPIO_SetupPinMux(MOTOR1_EPWM_BH_GPIO, 0, MOTOR1_EPWM_BH_MUX);

	// set up EPWM-BL
    GPIO_SetupPinOptions(MOTOR1_EPWM_BL_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
    GPIO_SetupPinMux(MOTOR1_EPWM_BL_GPIO, 0, MOTOR1_EPWM_BL_MUX);

	// set up EPWM-CH
	GPIO_SetupPinOptions(MOTOR1_EPWM_CH_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
	GPIO_SetupPinMux(MOTOR1_EPWM_CH_GPIO, 0, MOTOR1_EPWM_CH_MUX);

	// set up EPWM-CL
    GPIO_SetupPinOptions(MOTOR1_EPWM_CL_GPIO, GPIO_OUTPUT, GPIO_PULLUP);
    GPIO_SetupPinMux(MOTOR1_EPWM_CL_GPIO, 0, MOTOR1_EPWM_CL_MUX);

	return;
}

// ****************************************************************************
// ****************************************************************************
//CAN GPIO
// ****************************************************************************
// ****************************************************************************
// Initialize GPIO for?
//void initGPIO(void) {
////    EALLOW;
//    GPIO_SetupPinMux(91, GPIO_MUX_CPU1, 3);
//    GPIO_SetupPinMux(92, GPIO_MUX_CPU1, 3);
////    EDIS;
//}

// ****************************************************************************
// ****************************************************************************
//TODO EQEP GPIO Configuration
// ****************************************************************************
// ****************************************************************************

//QEP*************************************************
/*
void InitMotor1EQepGpio(void)
{
	// Set up QEP-A
	GPIO_SetupPinOptions(MOTOR1_QEPA_GPIO, GPIO_INPUT, GPIO_SYNC);
	GPIO_SetupPinMux(MOTOR1_QEPA_GPIO,0,MOTOR1_QEPA_MUX);

	// Set up QEP-B
	GPIO_SetupPinOptions(MOTOR1_QEPB_GPIO, GPIO_INPUT, GPIO_SYNC);
	GPIO_SetupPinMux(MOTOR1_QEPB_GPIO,0,MOTOR1_QEPB_MUX);

	// Set up QEP-I
	GPIO_SetupPinOptions(MOTOR1_QEPI_GPIO, GPIO_INPUT, GPIO_SYNC);
	GPIO_SetupPinMux(MOTOR1_QEPI_GPIO,0,MOTOR1_QEPI_MUX);

	return;
}
*/

//ini gateSink gpio*************************************************
void InitGateGpio(void)
{
    // Set up
    GPIO_SetupPinOptions(MOTOR1_Gate_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_Gate_GPIO,0,MOTOR1_Gate_MUX);
    GPIO_WritePin(MOTOR1_Gate_GPIO, 0);  // Disable Gate_gpio

    return;
}

//HALL*************************************************
//void InitMotor1HALL1Gpio(void)
//{
//    // Set up Hall-A1
//    GPIO_SetupPinOptions(MOTOR1_HALLA1_GPIO, GPIO_INPUT, GPIO_SYNC);
//    GPIO_SetupPinMux(MOTOR1_HALLA1_GPIO,0,MOTOR1_HALLA1_MUX);
//
//    // Set up Hall-B1
//    GPIO_SetupPinOptions(MOTOR1_HALLB1_GPIO, GPIO_INPUT, GPIO_SYNC);
//    GPIO_SetupPinMux(MOTOR1_HALLB1_GPIO,0,MOTOR1_HALLB1_MUX);
//
//    // Set up Hall-C1
//    GPIO_SetupPinOptions(MOTOR1_HALLC1_GPIO, GPIO_INPUT, GPIO_SYNC);
//    GPIO_SetupPinMux(MOTOR1_HALLC1_GPIO,0,MOTOR1_HALLC1_MUX);
//
//    return;
//}
//
//void InitMotor1HALL2Gpio(void)
//{
//    // Set up Hall-A2
//    GPIO_SetupPinOptions(MOTOR1_HALLA2_GPIO, GPIO_INPUT, GPIO_SYNC);
//    GPIO_SetupPinMux(MOTOR1_HALLA2_GPIO,0,MOTOR1_HALLA2_MUX);
//
//    // Set up Hall-B2
//    GPIO_SetupPinOptions(MOTOR1_HALLB2_GPIO, GPIO_INPUT, GPIO_SYNC);
//    GPIO_SetupPinMux(MOTOR1_HALLB2_GPIO,0,MOTOR1_HALLB2_MUX);
//
//    // Set up Hall-C2
//    GPIO_SetupPinOptions(MOTOR1_HALLC2_GPIO, GPIO_INPUT, GPIO_SYNC);
//    GPIO_SetupPinMux(MOTOR1_HALLC2_GPIO,0,MOTOR1_HALLC2_MUX);
//
//    return;
//}

// ****************************************************************************
// ****************************************************************************
//TODO SPI GPIO Configuration
// ****************************************************************************
// ****************************************************************************
/*
void InitMotor1SpiGpio(void)
{
	// set up SDI pin
	GPIO_SetupPinOptions(MOTOR1_SDI_GPIO, GPIO_INPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(MOTOR1_SDI_GPIO,0,MOTOR1_SDI_MUX);

	// set up SDO pin
	GPIO_SetupPinOptions(MOTOR1_SDO_GPIO, GPIO_INPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(MOTOR1_SDO_GPIO,0,MOTOR1_SDO_MUX);

	// set up CLK pin
	GPIO_SetupPinOptions(MOTOR1_CLK_GPIO, GPIO_INPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(MOTOR1_CLK_GPIO,0,MOTOR1_CLK_MUX);

	// set up SCS pin
	GPIO_SetupPinOptions(MOTOR1_SCS_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(MOTOR1_SCS_GPIO,0,MOTOR1_SCS_MUX);

	return;
}
*/

// ****************************************************************************
// ****************************************************************************
//TODO DRV83xx GPIO Configuration
// ****************************************************************************
// ****************************************************************************
void InitMotor1_AFMDRV_Gpio(void)
{
    // Configure as Output : Motor 1 - Gate_EN
    GPIO_SetupPinOptions(MOTOR1_GATE_EN_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_GATE_EN_GPIO, 0, MOTOR1_GATE_EN_MUX);
//    GPIO_WritePin(MOTOR1_GATE_EN_GPIO, 0);  // Disable Gate_EN

    // Configure as Input : Motor 1 - I/O_input1
    GPIO_SetupPinOptions(MOTOR1_IOinput1_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_IOinput1_GPIO, 0, MOTOR1_IOinput1_MUX );

    // Configure as Input : Motor 1 - I/O_input2
    GPIO_SetupPinOptions(MOTOR1_IOinput2_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_IOinput2_GPIO, 0, MOTOR1_IOinput2_MUX );

    // Configure as Input : Motor 1 - I/O_input3
    GPIO_SetupPinOptions(MOTOR1_IOinput3_GPIO, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(MOTOR1_IOinput3_GPIO, 0, MOTOR1_IOinput3_MUX );

    // Configure as Output : Motor 1 - SD_IN
   GPIO_SetupPinOptions(MOTOR1_SD_IN_GPIO, GPIO_INPUT, GPIO_ASYNC);
   GPIO_SetupPinMux(MOTOR1_SD_IN_GPIO, 0, MOTOR1_SD_IN_MUX);

}

// ****************************************************************************
// ****************************************************************************
//TODO DRV83xx GPIO Configuration
// ****************************************************************************
// ****************************************************************************
/*
void InitMotor1_DRV_Gpio(void)
{
	// Configure as Output : Motor 1 - EN-GATE
	GPIO_WritePin(MOTOR1_EN_GATE_GPIO, 0);  // Disable EN_GATE
	GPIO_SetupPinOptions(MOTOR1_EN_GATE_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(MOTOR1_EN_GATE_GPIO, 0, MOTOR1_EN_GATE_MUX);

#if (MOTOR1_DRV == DRV8301)
	// Configure as Output : Motor 1 - DC-CAL for DRV8301
	GPIO_WritePin(MOTOR1_DC_CAL_GPIO, 1);  // Set DC-CAL to 1 to disable amplifiers
	GPIO_SetupPinOptions(MOTOR1_DC_CAL_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(MOTOR1_DC_CAL_GPIO, 0, MOTOR1_DC_CAL_MUX);
#else
	// Configure as Output : Motor 1 - WAKE for DRV8305
	GPIO_WritePin(MOTOR1_WAKE_GPIO, 0);  // Set WAKE to 0 (default state)
	GPIO_SetupPinOptions(MOTOR1_WAKE_GPIO, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(MOTOR1_WAKE_GPIO, 0, MOTOR1_WAKE_MUX);
#endif
}
*/

// ****************************************************************************
// ****************************************************************************
//TODO CPU SYSTEM CONTROL Configuration
// ****************************************************************************
// ****************************************************************************
#pragma CODE_SECTION(InitFlash_Bank0, "ramfuncs");
#pragma CODE_SECTION(InitFlash_Bank1, "ramfuncs");

void InitSysCtrl1(void)
{
    // Disable the watchdog
    DisableDog();

#ifdef _FLASH
// Copy time critical code and Flash setup code to RAM
// This includes the following functions:  InitFlash();
// The  RamfuncsLoadStart, RamfuncsLoadSize, and RamfuncsRunStart
// symbols are created by the linker. Refer to the device .cmd file.
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
//    InitFlash_Bank0();
#endif

    // *IMPORTANT*
    // The Device_cal function, which copies the ADC & oscillator calibration values
    // from TI reserved OTP into the appropriate trim registers, occurs automatically
    // in the Boot ROM. If the boot ROM code is bypassed during the debug process, the
    // following function MUST be called for the ADC and oscillators to function according
    // to specification. The clocks to the ADC MUST be enabled before calling this
    // function.
    // See the device data manual and/or the ADC Reference
    // Manual for more information.

    EALLOW;

    //enable pull-ups on unbonded IOs as soon as possible to reduce power consumption.
    GPIO_EnableUnbondedIOPullups();

	CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_C = 1;
	CpuSysRegs.PCLKCR13.bit.ADC_D = 1;

    //check if device is trimmed
    if(*((Uint16 *)0x5D1B6) == 0x0000){
        //device is not trimmed, apply static calibration values
        AnalogSubsysRegs.ANAREFTRIMA.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMB.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMC.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMD.all = 31709;
    }

	CpuSysRegs.PCLKCR13.bit.ADC_A = 0;
	CpuSysRegs.PCLKCR13.bit.ADC_B = 0;
	CpuSysRegs.PCLKCR13.bit.ADC_C = 0;
	CpuSysRegs.PCLKCR13.bit.ADC_D = 0;
    EDIS;

    // Initialize the PLL control: PLLCR and CLKINDIV
    // F28_PLLCR and F28_CLKINDIV are defined in F2837xD_Examples.h
    // Note: The internal oscillator CANNOT be used as the PLL source if the
    // PLLSYSCLK is configured to frequencies above 194 MHz.
    InitSysPll(XTAL_OSC,IMULT_40,FMULT_0,PLLCLK_BY_2); 		//PLLSYSCLK = 10Mhz(OSCCLK) * 40 (IMULT) * 1 (FMULT) /  2 (PLLCLK_BY_2)

    //Turn on all peripherals
	InitPeripheralClocks();

}

// ****************************************************************************
// ****************************************************************************
//TODO ADC Configuration
// ****************************************************************************
// ****************************************************************************
//Write ADC configurations and power up the ADC for both ADC A and ADC B
void ConfigureADC(void)
{
	EALLOW;

	//write configurations
	AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
	AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
	AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcdRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4

    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCD, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

	//Set pulse positions to late
	AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
	AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
	AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcdRegs.ADCCTL1.bit.INTPULSEPOS = 1;

	//power up the ADC
	AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;

	//delay for 1ms to allow ADC time to power up
	DELAY_US(1000);

	EDIS;
}

// ****************************************************************************
// ****************************************************************************
//TODO PWM Configuration
// ****************************************************************************
// ****************************************************************************
void PWM_1ch_UpDwnCnt_CNF(volatile struct EPWM_REGS * ePWM, Uint16 period, Uint16 db)
{
	EALLOW;
	// Time Base SubModule Registers
	ePWM->TBCTL.bit.PRDLD = TB_IMMEDIATE; // set Immediate load
	ePWM->TBPRD = period / 2; // PWM frequency = 1 / period
	ePWM->TBPHS.bit.TBPHS = 0;
	ePWM->TBCTR = 0;
	ePWM->TBCTL.bit.CTRMODE   = TB_COUNT_UPDOWN;
	ePWM->TBCTL.bit.HSPCLKDIV = TB_DIV1;
	ePWM->TBCTL.bit.CLKDIV    = TB_DIV1;

	ePWM->TBCTL.bit.PHSEN    = TB_DISABLE;
	ePWM->TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // sync "down-stream"

	// Counter Compare Submodule Registers
	ePWM->CMPA.bit.CMPA = 0; // set duty 0% initially
	ePWM->CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	ePWM->CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	// Action Qualifier SubModule Registers
	ePWM->AQCTLA.bit.CAU = AQ_CLEAR;
	ePWM->AQCTLA.bit.CAD = AQ_SET;

	// Active high complementary PWMs - Set up the deadband
	ePWM->DBCTL.bit.IN_MODE  = DBA_ALL;
	ePWM->DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	ePWM->DBCTL.bit.POLSEL   = DB_ACTV_HIC;
	ePWM->DBRED.all = db;
	ePWM->DBFED.all = db;
	EDIS;
}

// ****************************************************************************
// ****************************************************************************
//TODO PWM Configuration
// ****************************************************************************
// ****************************************************************************
void PWM_1ch_UpDwnCnt_CNF_noDB(volatile struct EPWM_REGS * ePWM, Uint16 period)
{
	EALLOW;
	// Time Base SubModule Registers
	ePWM->TBCTL.bit.PRDLD = TB_IMMEDIATE; // set Immediate load
	ePWM->TBPRD = period / 2; // PWM frequency = 1 / period
	ePWM->TBPHS.bit.TBPHS = 0;
	ePWM->TBCTR = 0;
	ePWM->TBCTL.bit.CTRMODE   = TB_COUNT_UPDOWN;
	ePWM->TBCTL.bit.HSPCLKDIV = TB_DIV1;
	ePWM->TBCTL.bit.CLKDIV    = TB_DIV1;

	ePWM->TBCTL.bit.PHSEN    = TB_DISABLE;
	ePWM->TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // sync "down-stream"

	// Counter Compare Submodule Registers
	ePWM->CMPA.bit.CMPA = 0; // set duty 0% initially
	ePWM->CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	ePWM->CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

	// Action Qualifier SubModule Registers
	ePWM->AQCTLA.bit.CAU = AQ_CLEAR;
	ePWM->AQCTLA.bit.CAD = AQ_SET;

	// Active high complementary PWMs - Disable deadband
	ePWM->DBCTL.bit.OUT_MODE = DB_DISABLE;
	EDIS;
}

// ****************************************************************************
// ****************************************************************************
//TODO SPI Configuration
// ****************************************************************************
// ****************************************************************************
void InitSpiRegs_DRV830x(volatile struct SPI_REGS *s)
{
	s->SPICCR.bit.SPISWRESET = 0; 		// Put SPI in reset state
	s->SPICCR.bit.SPICHAR = 0xF;		// 16-bit character
	s->SPICCR.bit.SPILBK = 0;     		// loopback off
	s->SPICCR.bit.CLKPOLARITY = 0;  	// Rising edge without delay

	s->SPICTL.bit.SPIINTENA = 0;		// disable SPI interrupt
	s->SPICTL.bit.TALK = 1;				// enable transmission
	s->SPICTL.bit.MASTER_SLAVE = 1;		// master
	s->SPICTL.bit.CLK_PHASE = 0;    	// Rising edge without delay
	s->SPICTL.bit.OVERRUNINTENA = 0;	// disable reciever overrun interrupt

	s->SPIBRR.bit.SPI_BIT_RATE = 19;		// SPICLK = LSPCLK / 4 (max SPICLK)

	s->SPICCR.bit.SPISWRESET=1;  		// Enable SPI
}


/*****************************************************************************/
// EOF
/*****************************************************************************/
