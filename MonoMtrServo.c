/* ============================================================================
System Name:    Mono Motor Servo Control using F28377s-XL and BOOSTXL-DRV830x

File Name:      MonoMtrServo.c

Target:         F28377s Launch Pad

Author:         C2000 Systems Lab, 30th September 2015

Description:    Motor ISR
                Coded within ADCB1INT ISR @ 10Khz,
                  --> triggered by ADCA SOC6
                      --> set up by EPWM1_SOCA tied to EPWM1 PRD

//----------------------------------------------------------------------------------
//  Copyright Texas Instruments ��蕭 2004-2015
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date      | Description / Status
//----------------------------------------------------------------------------------
// 4 Nov 2015 - Field Oriented Control of a PMSM with QEP feedback using F28377s-XL
 *              and BOOSTXL-DRV8301 or BOOSTXL-DRV8305EVM
//----------------------------------------------------------------------------------
 *
 *
 *
Peripheral Assignments:
   MOTOR 1:
     - EPWMs ==>> A, B, C -> EPWM1, EPWM2, EPWM3
     - QEP   ==>> EQep1
     - SPI   ==>> Spia

     Analog signals - Motor 1
     Va   ADC (A)14
     Vb   ADC C3
     Vc   ADC B3
     Vdc  ADC A3
     Ia   ADC C2
     Ib   ADC B2
     Ic   ADC A2

    PWMDACs -> EPWM7, EPWM8
    DACs    -> DACA, DACB

    DAC-C  ---> General purpose display (??) // wait to add

===========================================================================  */

// Include header files used in the main function
// define float maths and then include IQmath library

#include "MonoMtrServo.h"
#include "MonoMtrServo-Settings.h"
#include "inverter_state.h"
#include "can_inv.h"
#include "systick.h"

//#include "driverlib.h"
//#include "device.h"

// **********************************************************
// Prototypes for local functions within this file
// **********************************************************

// INTERRUPT FUNCTIONS
// ---------------------
#ifdef _FLASH
#pragma CODE_SECTION(MotorControlISR, "ramfuncs");
#pragma CODE_SECTION(LogISR, "ramfuncs");
#endif

#pragma DATA_SECTION(HFCurrentALog, "ramgs0");
#pragma DATA_SECTION(HFCurrentBLog, "ramgs1");
#pragma DATA_SECTION(HFCurrentCLog, "ramgs2");
#pragma DATA_SECTION(HFCurrentDCLog, "ramgs3");
#pragma DATA_SECTION(HFSinPLog, "ramgs4");
#pragma DATA_SECTION(HFSinNLog, "ramgs5");
#pragma DATA_SECTION(HFCosPLog, "ramgs6");
#pragma DATA_SECTION(HFCosNLog, "ramgs7");

#pragma INTERRUPT(MotorControlISR, HPI)

// Prototype statements for functions found within this file.
interrupt void MotorControlISR(void);
interrupt void LogISR(void);
// Core Motor Control Functions
// ------------------------------

inline void motorCurrentSense(void);
inline void motorVoltageSense(void);
inline void motorTempSense(void);
//inline void posEncoder(MOTOR_VARS *motor);


void PwmTripConfig(volatile struct EPWM_REGS *PwmRegs, Uint16 TripNum); // not used
void DMC1_Protection(void); // not used
void DMC2_Protection(void); // not used

//void InitMotor1HALL1Gpio(void);
//void InitMotor1HALL2Gpio(void);
// Miscellaneous functions
// -------------------------
_iq refPosGen(_iq out); // not used
_iq ramper(_iq in, _iq out, _iq rampDelta);
_iq ramper_speed(_iq in, _iq out, _iq rampDelta);
void GPIO_TogglePin(Uint16 pin);

// State Machine function prototypes
//------------------------------------
// Alpha states
void A0(void); // state A0
void B0(void); // state B0
void C0(void); // state C0

// A branch states
void A1(void); // state A1
void A2(void); // state A2
void A3(void); // state A3

// B branch states
void B1(void); // state B1
void B2(void); // state B2
void B3(void); // state B3

// C branch states
void C1(void); // state C1
void C2(void); // state C2
void C3(void); // state C3

// Variable declarations
void (*Alpha_State_Ptr)(void); // Base States pointer
void (*A_Task_Ptr)(void);      // State pointer A branch
void (*B_Task_Ptr)(void);      // State pointer B branch
void (*C_Task_Ptr)(void);      // State pointer C branch

// Instance a speed calculator based on sliding-mode position observer
SPEED_ESTIMATION speed3 = SPEED_ESTIMATION_DEFAULTS; // not used

SPEED_MEAS_CAP speed_hall = SPEED_MEAS_CAP_DEFAULTS;

// Hall based FOC//
//Uint16 HallSum = 0;
//Uint16 FaultEmulationSignal_Accepted_transformation2 = 0;

// Encoder variables
_iq cal_offset = _IQ(0.0);
int cos_neg = 0;
int cos_pos = 0;
int sin_pos = 0;
int sin_neg = 0;

// Enable Gate driver variables
Uint16 EnableDRV = 0;
Uint16 ocp_cnt = 0;
Uint16 EnaDRV_FBK = 0;
Uint16 EnaDRV_FBK_last = 1;
float32 torqueCMD = 0.0;

// Speed caculator variavles
float32 deltaTheta = 0.0;
float32 Electheta_last = 0.0;
float32 Fischer_PP = 4.0;
float32 speed_rpm = 0.0;
int16 speed_cnt = 0;

// Function Prototypes
__interrupt void canaISR(void);     // Receive interrupt for CAN-A (not used)

// boost select
_iq boost_gain = 1; // not used
Uint16 boost_mode = 0; // not used

Uint32 cycleTicks = 0;

// Instance PID regulators to regulate the d and q  axis currents, and speed
//PID_CONTROLLER pid1_Hall_pll = {PID_TERM_DEFAULTS, PID_PARAM_DEFAULTS, PID_DATA_DEFAULTS};
PID_CONTROLLER pid1_speed = {PID_TERM_DEFAULTS, PID_PARAM_DEFAULTS, PID_DATA_DEFAULTS};

// ****************************************************************************
// Variables for CPU control
// ****************************************************************************
// adc static cal
//int *adc_cal;

int16 VTimer0[4]; // Virtual Timers slaved off CPU Timer 0 (A events)
int16 VTimer1[4]; // Virtual Timers slaved off CPU Timer 1 (B events)
int16 VTimer2[4]; // Virtual Timers slaved off CPU Timer 2 (C events)
//int16 SerialCommsTimer;

//*********************** USER Variables *************************************

//****************************************************************************
// Global variables used in this system
//****************************************************************************
//_iq tempHall = _IQ(0);
// ****************************************************************************
// Flag variables
// ****************************************************************************
volatile Uint16 EnableFlag = FALSE;

Uint32 IsrTicker = 0;

//Uint16 BackTicker = 0;

//int LedCnt = 500;

int16 OffsetCalCounter;

_iq K1 = _IQ(0.998),    // Offset filter coefficient K1: 0.05/(T+0.05);
    K2 = _IQ(0.001999); // Offset filter coefficient K2: T/(T+0.05);

MOTOR_VARS motor1 = DRV830x_MOTOR_DEFAULTS;

// ****************************************************************************
// Miscellaneous Variables
// ****************************************************************************
//_iq IdRef_start = _IQ(0.1), // _IQ(0.1)
//    IdRef_run = _IQ(0.0);

// Variables for position reference generation and control
// =========================================================
_iq posArray[8] = {_IQ(1.5), _IQ(-1.5), _IQ(2.5), _IQ(-2.5)},
    cntr1 = 0,
    posSlewRate = _IQ(0.001);

int16 ptrMax = 2,
      ptr1 = 0;

Uint16 DRV_RESET = 0;

// ****************************************************************************
// Variables for Datalog module
// ****************************************************************************
uint16_t HFCurrentALog[3000],
    HFCurrentBLog[3000],
    HFCurrentCLog[3000],
    HFCurrentDCLog[3000],
    HFVoltageDCLog[3000],
    HFSinPLog[3000],
    HFSinNLog[3000],
    HFCosPLog[3000],
    HFCosNLog[3000];

uint16_t LogPos = 0;

// Create an instance of DATALOG Module
//DLOG_4CH_F dlog_4ch1;

// ****************************************************************************
// Variables for PWMDAC  module
// ****************************************************************************
int16 PwmDacCh1 = 0;
int16 PwmDacCh2 = 0;
int16 PwmDacCh3 = 0;
int16 PwmDacCh4 = 0;

// Instance a PWM DAC driver instance
PWMDAC pwmdac1 = PWMDAC_DEFAULTS;

//*******************************************************************************

#if BUILDLEVEL != LEVEL1
// ******************************************************************************
// CURRENT SENSOR SUITE
// - Reads motor currents from inverter bottom leg SHUNTs
// ******************************************************************************
inline void motorCurrentSense()
{
    motor1.currentAs = (float)IFB_A1_PPB * ADC_PU_PPB_SCALE_FACTOR/61.0;
    motor1.currentBs = (float)IFB_B1_PPB * ADC_PU_PPB_SCALE_FACTOR/61.0;
    motor1.currentCs = (-motor1.currentAs - motor1.currentBs)/61.0;
    motor1.currentDC = (float)IFB_DC_PPB * ADC_PU_PPB_SCALE_FACTOR/61.0;

    return;
}

// ******************************************************************************
// Temperature SENSOR SUITE
// - Reads motor voltages from inverter half-bridge outputs (phases A, B, and C)
//   and DC bus
// ******************************************************************************
inline void motorTempSense()
{
    /*
    Power module temperature measured by NTC thermistor.
    The NTC was placed at the lower part of a voltage divider with Vcc=3.3V, and a upper resistor of 5k ohm
    Characteristics of NTC are, R25 = 5k ohm, B25/50 = 3375, B25/80 = 3441, B25/100 = 3433
    Here we use a approximated model of degC = -32.332*ln(R) + 298.94
    */
    cycleTicks = CpuTimer0Regs.TIM.all;
    motor1.TempA = (float)TFB_A*Temp_ADC_PU_SCALE_FACTOR;
    motor1.TempB = (float)TFB_B*Temp_ADC_PU_SCALE_FACTOR;
//    motor1.TempC = -32.332*log(5000.0*(4095.0/TFB_C - 1)) + 298.94;

    /*
    Motor temperature measured by TMC PT1000 thermistor.
    The thermistor was placed at the lower part of a voltage divider with Vcc=3.34V, and a upper resistor of 1k ohm
    TMC PT1000 is a thermistor with a constant resistance to temperature coefficient. R = 3.7913*degC + 1004
    ADC resolution is 12 bits, from 0-4095.

    !! Somehow the Vrefhi(3.30V) for adc is different from the Vcc(3.34) of voltage divider,
    so it's corrected by adjust the max value of adc reading, 4095*3.34/3.3 = 4144.6
    */
//    motor1.TempMotor = 263.7/(4144.6/(float)TFB_motor - 1) - 264.76;
    cycleTicks -= CpuTimer0Regs.TIM.all;
    return;
}

// ******************************************************************************
// VOLTAGE SENSOR SUITE
// - Reads motor voltages from inverter half-bridge outputs (phases A, B, and C)
//   and DC bus
// ******************************************************************************
inline void motorVoltageSense()
{
    motor1.voltageAs = (float)VFB_U1*Voltage_ADC_PU_SCALE_FACTOR;
    motor1.voltageBs = (float)VFB_V1*Voltage_ADC_PU_SCALE_FACTOR;
    motor1.voltageCs = (float)VFB_W1*Voltage_ADC_PU_SCALE_FACTOR;
    motor1.voltageDC = (float)VFB_DC1*Voltage_ADC_PU_SCALE_FACTOR;

    return;
}

inline _iq EncoderVaricoder(int sin_pos, int sin_neg, int cos_pos, int cos_neg)
{
    int sin_singleEnd = 0;
    int cos_singleEnd = 0;
    sin_singleEnd = sin_pos - sin_neg;
    cos_singleEnd = cos_pos - cos_neg;
    _iq sinPU = _IQ(0.0);
    _iq cosPU = _IQ(0.0);
    sinPU = _IQdiv(_IQ(sin_singleEnd),2048);
    cosPU = _IQdiv(_IQ(cos_singleEnd),2048);
    _iq Elec_Angle= _IQ(0.0);
    Elec_Angle = _IQatan2PU(sinPU,cosPU); // 0~1.0: 0~360 deg
    return Elec_Angle;
}
#endif

// ****************************************************************************
// ****************************************************************************
// TODO  DMC Protection Against Over Current Protection
// ****************************************************************************
// ****************************************************************************

//void PwmTripConfig(volatile struct EPWM_REGS *PwmRegs, Uint16 TripNum)
//{
//    EALLOW;
//
//    PwmRegs->DCTRIPSEL.bit.DCAHCOMPSEL = TripNum; // TripNum is input to DCAHCOMPSEL
//    PwmRegs->TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;
//    PwmRegs->DCACTL.bit.EVT1SRCSEL = DC_EVT1;
//    PwmRegs->DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
//    PwmRegs->TZSEL.bit.DCAEVT1 = 1;
//    PwmRegs->TZSEL.bit.CBC6 = 0x1;        // Emulator Stop
//    PwmRegs->TZCTL.bit.TZA = TZ_FORCE_LO; // TZA event force EPWMxA go low
//    PwmRegs->TZCTL.bit.TZB = TZ_FORCE_LO; // TZB event force EPWMxB go low
//    PwmRegs->TZCLR.bit.DCAEVT1 = 1;       // Clear any spurious OV trip
//    PwmRegs->TZCLR.bit.OST = 1;           // clear any spurious OST set early
//
//    EDIS;
//}

// ****************************************************************************
// ****************************************************************************
// GENERAL PURPOSE UTILITY FUNCTIONS
// ****************************************************************************
// ****************************************************************************

// slew programmable ramper
_iq ramper(_iq in, _iq out, _iq rampDelta)
{
    _iq err;

    err = in - out;
    if (err > rampDelta)
        return (out + rampDelta);
    else if (err < -rampDelta)
        return (out - rampDelta);
    else
        return (in);
}

//*****************************************************************************
// Ramp Controller for speed reference (Not currently used)
_iq ramper_speed(_iq in, _iq out, _iq rampDelta)
{
    _iq err;

    err = in - out;
    if (err > rampDelta)
    {
        if ((out + rampDelta) > 1.0)
            return (1.0);
        else
            return (out + rampDelta);
    }
    else if (err < -rampDelta)
    {
        if (out - rampDelta <= 0.0)
            return (0.0);
        else
            return (out - rampDelta);
    }
    else
        return (in);
}

//*****************************************************************************
// Reference Position Generator for position loop
_iq refPosGen(_iq out)
{
    _iq in = posArray[ptr1];

    out = ramper(in, out, posSlewRate);

    if (in == out)
        if (++cntr1 > 1000)
        {
            cntr1 = 0;
            if (++ptr1 >= ptrMax)
                ptr1 = 0;
        }
    return (out);
}

//*****************************************************************************
// Toggle a GPIO pin
void GPIO_TogglePin(Uint16 pin)
{
    volatile Uint32 *gpioDataReg;
    Uint32 pinMask;

    gpioDataReg = (volatile Uint32 *)&GpioDataRegs + (pin / 32) * GPY_DATA_OFFSET;
    pinMask = 1UL << (pin % 32);

    gpioDataReg[GPYTOGGLE] = pinMask;

    return;

}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************





void main(void){


    // Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This function derived from the one found in F2837x_SysCtrl.c file
    InitSysCtrl1();
    InvInitCan(MOTOR_RR); // input which motor is
    SysTickInit();
    SysTickEnable();
    EnableFlag = FALSE;//0816 reset the enableflag

    // Waiting for enable flag set
    while (EnableFlag == FALSE)
    {
//        BackTicker++;
        ReceiveCanControl();
        SendCanHeartbeat();
        get_status();
//        uint16_t status = get_status();
//        uint16_t invTemp = (motor1.TempA + motor1.TempB +motor1.TempC)/3;
//        SendCanStatus(status, torqueCMD);
//        SendCanTemperature(speed_rpm, motor1.pi_iq.Out, motor1.voltageDC, motor1.currentDC);
//        SendCanState(invTemp, motor1.TempMotor);
//
//
//                // State machine entry & exit point
//                //===========================================================
////                (*Alpha_State_Ptr)();

    }

    // Clear all interrupts and initialize PIE vector table:

    // Disable CPU interrupts
    DINT;

    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the F28M3Xx_PieCtrl.c file.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;
    /*
    Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
    This will populate the entire table, even if the interrupt is not used in this example.
    This is useful for debug purposes.
    The shell ISR routines are found in F28M3Xx_DefaultIsr.c.
    This function is found in F28M3Xx_PieVect.c.
    */
    InitPieVectTable();

    // Timing sync for background loops
    // Timer period definitions found in device specific PeripheralHeaderIncludes.h
    CpuTimer0Regs.PRD.all = 10000; // A tasks
    CpuTimer1Regs.PRD.all = 200000; // B tasks
    CpuTimer2Regs.PRD.all = 30000; // C tasks

    CpuTimer1Regs.TCR.bit.TIE = 0x1; // enable timer 1 interrupt

    // Tasks State-machine init
    Alpha_State_Ptr = &A0;
    A_Task_Ptr = &A1;
    B_Task_Ptr = &B1;
    C_Task_Ptr = &C1;

    // ****************************************************************************
    // ****************************************************************************
    // Set up peripheral assignments for motor control
    // ****************************************************************************
    // ****************************************************************************
    motor1.PwmARegs = &EPwm1Regs;       // set up EPWM for motor 1 phase A
    motor1.PwmBRegs = &EPwm2Regs;       // set up EPWM for motor 1 phase B
    motor1.PwmCRegs = &EPwm3Regs;       // set up EPWM for motor 1 phase C

    // ****************************************************************************
    // ****************************************************************************
    // Initialize EPWM modules for inverter PWM generation
    // ****************************************************************************
    // ****************************************************************************

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    // *****************************************
    // Inverter PWM configuration for motor 1
    // ****************************************
    /* Deadband is set externally on DRV830x chip
     */
    /*
    PWM_1ch_UpDwnCnt_CNF_noDB(motor1.PwmARegs, INV_PWM_TICKS);
    PWM_1ch_UpDwnCnt_CNF_noDB(motor1.PwmBRegs, INV_PWM_TICKS);
    PWM_1ch_UpDwnCnt_CNF_noDB(motor1.PwmCRegs, INV_PWM_TICKS);
    */
    PWM_1ch_UpDwnCnt_CNF(motor1.PwmARegs,INV_PWM_TICKS,10);
    PWM_1ch_UpDwnCnt_CNF(motor1.PwmBRegs,INV_PWM_TICKS,10);
    PWM_1ch_UpDwnCnt_CNF(motor1.PwmCRegs,INV_PWM_TICKS,10);

    // configure Epwms 2 and 3 as slaves
    (motor1.PwmBRegs)->TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    (motor1.PwmBRegs)->TBCTL.bit.PHSEN = TB_ENABLE;
    (motor1.PwmBRegs)->TBPHS.bit.TBPHS = 2;
    (motor1.PwmBRegs)->TBCTL.bit.PHSDIR = TB_UP;
    (motor1.PwmCRegs)->TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    (motor1.PwmCRegs)->TBCTL.bit.PHSEN = TB_ENABLE;
    (motor1.PwmCRegs)->TBPHS.bit.TBPHS = 2;
    (motor1.PwmCRegs)->TBCTL.bit.PHSDIR = TB_UP;

    InitMotor1EPwmGpio(); // Set up GPIOs for EPWMA of 1,2,3

    //---------------------------------------------------------------------------------------

    // Setting up link from EPWM to ADC (EPwm1 is chosen)
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD; // Select SOC from counter at ctr = PRD
    EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;      // Generate pulse on 1st even
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;           // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCBEN = 1;

    // ****************************************************************************
    // ****************************************************************************
    // TODO ADC Configuration
    // ****************************************************************************
    // ****************************************************************************
    // Configure the ADC and power it up
    ConfigureADC();

    // Select the channels to convert and end of conversion flag

    EALLOW;

    // Analog signals - Motor Fischer

    // voltage signals
    // Vdc  ADC_A A0 SOC5
    // Vu   ADC_A A1 SOC2
    // Vv   ADC_A A2 SOC3
    // Vw   ADC_A A3 SOC4

    // current signals
    // Idc  ADC_D D0 SOC5
    // Iu   ADC_D D1 SOC2
    // Iv   ADC_D D2 SOC3
    // Iw   ADC_D D3 SOC4

    // temperature signals
    // Tm   ADC_C 15 SOC6
    // Tu   ADC_A A4 SOC7
    // Tv   ADC_A A5 SOC8
    // Tw   ADC_A 14 SOC9

    // encoder signals
    // sin+ ADC_B B0 SOC1
    // sin- ADC_B B1 SOC2
    // cos+ ADC_B B2 SOC3
    // cos- ADC_B B3 SOC4

    // TODO calculate the suitable ACQPS time for each ADC measurement (current value seems to be pretty low)

    // define ADC for voltage measurement
    // Motor 1: Vdc  @ A0
    // ********************************
    AdcaRegs.ADCSOC5CTL.bit.CHSEL = 0;                    // SOC6 will convert pin A0
    AdcaRegs.ADCSOC5CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA

    // Motor 1: Vu  @ A1
    // ********************************
    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 1;                    // SOC4 will convert pin A1
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA

    // Motor 1: Vv  @ A2
    // ********************************
    AdcaRegs.ADCSOC3CTL.bit.CHSEL = 2;                    // SOC3 will convert pin A2
    AdcaRegs.ADCSOC3CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA

    // Motor 1: Vw  @ A3
    // ********************************
    AdcaRegs.ADCSOC4CTL.bit.CHSEL = 3;                    // SOC2 will convert pin A3
    AdcaRegs.ADCSOC4CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA

    // Motor 1: Idc @ D0
    // ********************************
    AdcdRegs.ADCSOC5CTL.bit.CHSEL = 0;                    // SOC2 will convert pin D0
    AdcdRegs.ADCSOC5CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcdRegs.ADCSOC5CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA
    // Configure the post processing block (PPB) to eliminate subtraction related calculation
    AdcdRegs.ADCPPB4CONFIG.bit.CONFIG = 5; // PPB is associated with SOC5
    AdcdRegs.ADCPPB4OFFCAL.bit.OFFCAL = 0; // Write zero to this for now till offset ISR is run

    // Motor 1: Iu  @ D1
    // ********************************
    AdcdRegs.ADCSOC2CTL.bit.CHSEL = 1;                    // SOC0 will convert pin D1
    AdcdRegs.ADCSOC2CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcdRegs.ADCSOC2CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA
    // Configure the post processing block (PPB) to eliminate subtraction related calculation
    AdcdRegs.ADCPPB1CONFIG.bit.CONFIG = 2; // PPB is associated with SOC2
    AdcdRegs.ADCPPB1OFFCAL.bit.OFFCAL = 0; // Write zero to this for now till offset ISR is run

    // Motor 1: Iv  @ D2
    // ********************************
    AdcdRegs.ADCSOC3CTL.bit.CHSEL = 2;                    // SOC0 will convert pin D2
    AdcdRegs.ADCSOC3CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcdRegs.ADCSOC3CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA
    // Configure the post processing block (PPB) to eliminate subtraction related calculation
    AdcdRegs.ADCPPB2CONFIG.bit.CONFIG = 3; // PPB is associated with SOC3
    AdcdRegs.ADCPPB2OFFCAL.bit.OFFCAL = 0; // Write zero to this for now till offset ISR is run

    // Motor 1: Iw  @ D3
    // ********************************
    AdcdRegs.ADCSOC4CTL.bit.CHSEL = 3;                    // SOC2 will convert pin D3
    AdcdRegs.ADCSOC4CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcdRegs.ADCSOC4CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA
    // Configure the post processing block (PPB) to eliminate subtraction related calculation
    AdcdRegs.ADCPPB3CONFIG.bit.CONFIG = 4; // PPB is associated with SOC4
    AdcdRegs.ADCPPB3OFFCAL.bit.OFFCAL = 0; // Write zero to this for now till offset ISR is run

    // Motor 1: Temp motor @ 15
    // ********************************
    AdccRegs.ADCSOC6CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdccRegs.ADCSOC6CTL.bit.CHSEL = 15;                    // SOC8 will convert pin 15
    AdccRegs.ADCSOC6CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA

    // Motor 1: Temp u @ A4
    // ********************************
    AdcaRegs.ADCSOC7CTL.bit.CHSEL = 4;                    // SOC8 will convert pin A4
    AdcaRegs.ADCSOC7CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcaRegs.ADCSOC7CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA

    // Motor 1: Temp v @ A5
    // ********************************
    AdcaRegs.ADCSOC8CTL.bit.CHSEL = 5;                    // SOC8 will convert pin A5
    AdcaRegs.ADCSOC8CTL.bit.ACQPS = 30;                   // sample window in SYSCLK cycles
    AdcaRegs.ADCSOC8CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA

    // Motor 1: Temp w @ 14
    // ********************************
    AdcaRegs.ADCSOC9CTL.bit.CHSEL = 14;                    // SOC8 will convert pin 14
    AdcaRegs.ADCSOC9CTL.bit.ACQPS = 59;                   // sample window in SYSCLK cycles
    AdcaRegs.ADCSOC9CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA; // trigger on ePWM1 SOCA

    //encoder : sin+ @ B0
    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 0;
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = 30;
    AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA;

    //encoder : sin- @ B1
    AdcbRegs.ADCSOC2CTL.bit.CHSEL = 1;
    AdcbRegs.ADCSOC2CTL.bit.ACQPS = 30;
    AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA;

    //encoder : cos+ @ B2
    AdcbRegs.ADCSOC3CTL.bit.CHSEL = 2;
    AdcbRegs.ADCSOC3CTL.bit.ACQPS = 30;
    AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA;

    //encoder : cos- @ B3
    AdcbRegs.ADCSOC4CTL.bit.CHSEL = 3;
    AdcbRegs.ADCSOC4CTL.bit.ACQPS = 30;
    AdcbRegs.ADCSOC4CTL.bit.TRIGSEL = ADCTRIG5_EPWM1SOCA;




    // ****************************************************************************
    // ****************************************************************************
    // TODO ISR Mapping
    // ****************************************************************************
    // ****************************************************************************
    // ADC A EOC of SOC2 is used to trigger Motor control Interrupt
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 2;
    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;

    PieVectTable.ADCA1_INT = &MotorControlISR;
    PieVectTable.TIMER1_INT = &LogISR;
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1; // Enable ADCA1INT in PIE group 1

    IER |= M_INT1; // Enable group 1 interrupts
    IER |= M_INT13;

    // SETUP DAC-C (DACs A, B and C are already used up)

    EDIS;
    InitGateGpio();

    // ****************************************************************************
    // ****************************************************************************
    // Initialize PWMDAC module
    // ****************************************************************************
    // ****************************************************************************
    pwmdac1.PeriodMax = 500; // @200Mhz: 4500->20kHz, 3000-> 30kHz, 1500->60kHz, min: 500
    pwmdac1.PwmDacInPointer0 = &PwmDacCh1;
    pwmdac1.PwmDacInPointer1 = &PwmDacCh2;
    pwmdac1.PwmDacInPointer2 = &PwmDacCh3;
    pwmdac1.PwmDacInPointer3 = &PwmDacCh4;

    PWMDAC_INIT_MACRO(pwmdac1)
    InitMotor1EPwmDACGpio(); // Set up GPIOs for EPWMA of 7,8

    //    SPIDAC_INIT_MACRO(spidac1)//

    // ****************************************************************************
    // ****************************************************************************
    // Initialize DAC module
    // ****************************************************************************
    // ****************************************************************************

//  configureDAC(DACA);
//  configureDAC(DACB);
//    configureDAC(DACC);

    // ****************************************************************************
    // Initialise DRV830x interface GPIOs
    // ****************************************************************************
    //InitMotor1_DRV_Gpio(); // DRV init for motor 1
    InitMotor1_AFMDRV_Gpio();
    // ****************************************************************************
    // ****************************************************************************

    // Initialise DRV830x
    // ****************************************************************************
    // ****************************************************************************
    GPIO_WritePin(MOTOR1_GATE_EN_GPIO, EnableDRV); // Enable DRV
    GPIO_WritePin(MOTOR1_Gate_GPIO, EnableDRV); // Enable DRV
    DELAY_US(50000);                       // delay to allow DRV830x supplies to ramp up

    // ****************************************************************************
    // ****************************************************************************
    // Paramaeter Initialisation
    // ****************************************************************************
    // ****************************************************************************

    // Initialize the SPEED_EST module SMOPOS based speed calculation
    motor1.speed1.K1 = _IQ21(1 / (BASE_FREQ * motor1.T));
    motor1.speed1.K2 = _IQ(1 / (1 + motor1.T * 2 * PI * 5)); // Low-pass cut-off frequency
    motor1.speed1.K3 = _IQ(1) - motor1.speed1.K2;
    motor1.speed1.BaseRpm = 120 * (BASE_FREQ / POLES);

    // Init QEP parameters


    // Initialize the RAMPGEN module
    motor1.rg.StepAngleMax = _IQ(BASE_FREQ * motor1.T);



    // Initialize the PID module for speed
    pid1_speed.param.Kp = _IQ(5);
    pid1_speed.param.Ki = _IQ(0.00005);
    pid1_speed.param.Kd = _IQ(0);
    pid1_speed.param.Kr = _IQ(1.0);
    pid1_speed.param.Km = _IQ(1.0);
    pid1_speed.param.Umax = _IQ(1.0);
    pid1_speed.param.Umin = _IQ(-1.0);
    pid1_speed.term.Out = _IQ(0.0);


    // Init PI module for ID loop
    motor1.pi_id.Kp = _IQ(1.5*25/BASE_VOLTAGE); // resistance * bandwidth / dc voltage  //note the unit
    motor1.pi_id.Ki = _IQ(0.0005*25/BASE_VOLTAGE); //20k //inductance * bandwidth / dc voltage
    motor1.pi_id.Umax = _IQ(0.9);
    motor1.pi_id.Umin = _IQ(-0.9);

    // Init PI module for IQ loop
    motor1.pi_iq.Kp = _IQ(1.5*25/BASE_VOLTAGE);
    motor1.pi_iq.Ki = _IQ(0.0005*25/BASE_VOLTAGE); //20k
    motor1.pi_iq.Umax = _IQ(0.9);
    motor1.pi_iq.Umin = _IQ(-0.9);

    // Set mock REFERENCES for Speed and Iq loops
    motor1.SpeedRef = 0; // 0.005;

    motor1.IqRef = _IQ(0); //_IQ(0.1);

    // Init FLAGS
    motor1.RunMotor = 1;

    //  Note that the vectorial sum of d-q PI outputs should be less than 1.0 which
    //  refers to maximum duty cycle for SVGEN. Another duty cycle limiting factor
    //  is current sense through shunt resistors which depends on hardware/software
    //  implementation. Depending on the application requirements 3,2 or a single
    //  shunt resistor can be used for current waveform reconstruction. The higher
    //  number of shunt resistors allow the higher duty cycle operation and better
    //  dc bus utilization. The users should adjust the PI saturation levels
    //  carefully during open loop tests (i.e pi_id.Umax, pi_iq.Umax and Umins) as
    //  in project manuals. Violation of this procedure yields distorted  current
    // waveforms and unstable closed loop operations which may damage the inverter.

    // ****************************************************
    // Initialize DATALOG module
    // ****************************************************
    /*
    DLOG_4CH_F_init(&dlog_4ch1);
    dlog_4ch1.input_ptr1 = &DlogCh1; // data value
    dlog_4ch1.input_ptr2 = &DlogCh2;
    dlog_4ch1.input_ptr3 = &DlogCh3;
    dlog_4ch1.input_ptr4 = &DlogCh4;
    dlog_4ch1.output_ptr1 = &DBUFF_4CH1[0];
    dlog_4ch1.output_ptr2 = &DBUFF_4CH2[0];
    dlog_4ch1.output_ptr3 = &DBUFF_4CH3[0];
    dlog_4ch1.output_ptr4 = &DBUFF_4CH4[0];
    dlog_4ch1.size = 200;
    dlog_4ch1.pre_scalar = 5;
    dlog_4ch1.trig_value = 0.01;
    dlog_4ch1.status = 2;
    */

    // ****************************************************************************
    // ****************************************************************************
    // Call DMC Protection function
    // ****************************************************************************
    // ****************************************************************************
    //DMC1_Protection();

// TODO
// ****************************************************************************
// ****************************************************************************
// Feedbacks OFFSET Calibration Routine
// ****************************************************************************
// ****************************************************************************

    DELAY_US(5); // delay to allow DRV830x amplifiers to settle

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    motor1.offset_shntA = 0;
    motor1.offset_shntB = 0;
    motor1.offset_shntC = 0;
    motor1.offset_shntDC  = 0;

    for (OffsetCalCounter = 0; OffsetCalCounter < 20000;)
    {
        if (EPwm1Regs.ETFLG.bit.SOCA == 1)
        {
            if (OffsetCalCounter > 1000)
            {
                motor1.offset_shntA = K1 * motor1.offset_shntA + K2 * (IFB_U1)*ADC_PU_SCALE_FACTOR; // Mtr1 : Phase A offset
                motor1.offset_shntB = K1 * motor1.offset_shntB + K2 * (IFB_V1)*ADC_PU_SCALE_FACTOR; // Mtr1 : Phase B offset
                motor1.offset_shntC = K1 * motor1.offset_shntC + K2 * (IFB_W1)*ADC_PU_SCALE_FACTOR; // Mtr1 : Phase C offset
                motor1.offset_shntDC = K1 * motor1.offset_shntDC + K2 * (IFB_DC1)*ADC_PU_SCALE_FACTOR; // Mtr1 : Phase C offset
            }
            EPwm1Regs.ETCLR.bit.SOCA = 1;
            OffsetCalCounter++;
        }
    }

    // ********************************************
    // Init OFFSET regs with identified values
    // ********************************************
    EALLOW;

    AdcdRegs.ADCPPB1OFFREF = motor1.offset_shntA * 4096.0; // set shunt Iu1 offset
    AdcdRegs.ADCPPB2OFFREF = motor1.offset_shntB * 4096.0; // set shunt Iv1 offset
    AdcdRegs.ADCPPB3OFFREF = motor1.offset_shntC * 4096.0; // set shunt Iw1 offset
    AdcdRegs.ADCPPB4OFFREF = motor1.offset_shntDC* 4096.0; // set shunt Iw1 offset



    EDIS;

    // ****************************************************************************
    // ****************************************************************************
    // TODO Enable Interrupts
    // ****************************************************************************
    // ****************************************************************************
    EALLOW;
    EINT; // Enable Global interrupt INTM
    ERTM; // Enable Global realtime interrupt DBGM
    EDIS;

    // ***************************************************************************
    //  Initialisations COMPLETE
    //  - IDLE loop. Just loop forever
    // ***************************************************************************
    SysTickInit();
    SysTickEnable();
    for (;;) // infinite loop
    {
        motorTempSense();
        ReceiveCanControl();
        /* please finish this */
        uint16_t status = get_status();
        uint16_t invTemp = (motor1.TempA + motor1.TempB +motor1.TempC)/3;
        SendCanStatus(status, torqueCMD);
        SendCanTemperature(speed_rpm, motor1.pi_iq.Out, motor1.voltageDC, motor1.currentDC);
        SendCanState(invTemp, motor1.TempMotor);
        SendCanHeartbeat();
        // State machine entry & exit point
        //===========================================================
        (*Alpha_State_Ptr)(); // jump to an Alpha state (A0,B0,...)
                              //===========================================================
    }
} // END MAIN CODE

/******************************************************************************
 * ****************************************************************************
 * ****************************************************************************
 * ****************************************************************************
 */

//=================================================================================
//  STATE-MACHINE SEQUENCING AND SYNCRONIZATION FOR SLOW BACKGROUND TASKS
//=================================================================================

//--------------------------------- FRAMEWORK -------------------------------------
void A0(void)
{
    // loop rate synchronizer for A-tasks
    if (CpuTimer0Regs.TCR.bit.TIF == 1)
    {
        CpuTimer0Regs.TCR.bit.TIF = 1; // clear flag

        //-----------------------------------------------------------
        (*A_Task_Ptr)(); // jump to an A Task (A1,A2,A3,...)
        //-----------------------------------------------------------

        VTimer0[0]++; // virtual timer 0, instance 0 (spare)
//        SerialCommsTimer++;
    }

    Alpha_State_Ptr = &B0; // Comment out to allow only A tasks
}

void B0(void)
{
    // loop rate synchronizer for B-tasks
    if (CpuTimer1Regs.TCR.bit.TIF == 1)
    {
        CpuTimer1Regs.TCR.bit.TIF = 1; // clear flag

        //-----------------------------------------------------------
        (*B_Task_Ptr)(); // jump to a B Task (B1,B2,B3,...)
        //-----------------------------------------------------------
        VTimer1[0]++; // virtual timer 1, instance 0 (spare)
    }

    Alpha_State_Ptr = &C0; // Allow C state tasks
}

void C0(void)
{
    // loop rate synchronizer for C-tasks
    if (CpuTimer2Regs.TCR.bit.TIF == 1)
    {
        CpuTimer2Regs.TCR.bit.TIF = 1; // clear flag

        //-----------------------------------------------------------
        (*C_Task_Ptr)(); // jump to a C Task (C1,C2,C3,...)
        //-----------------------------------------------------------
        VTimer2[0]++; // virtual timer 2, instance 0 (spare)
    }

    Alpha_State_Ptr = &A0; // Back to State A0
}

//=================================================================================
//  A - TASKS (executed in every 50 usec)
//=================================================================================
//--------------------------------------------------------
void A1(void) // SPARE (not used)
//--------------------------------------------------------
{

    //-------------------
    // the next time CpuTimer0 'counter' reaches Period value go to A2
    A_Task_Ptr = &A2;
    //-------------------
}

//-----------------------------------------------------------------
void A2(void) // SPARE (not used)
//-----------------------------------------------------------------
{

    //-------------------
    // the next time CpuTimer0 'counter' reaches Period value go to A3
    A_Task_Ptr = &A3;
    //-------------------
}

//-----------------------------------------
void A3(void) // SPARE (not used)
//-----------------------------------------
{

    //-----------------
    // the next time CpuTimer0 'counter' reaches Period value go to A1
    A_Task_Ptr = &A1;
    //-----------------
}

//=================================================================================
//  B - TASKS (executed in every 100 usec)
//=================================================================================

//----------------------------------- USER ----------------------------------------

//----------------------------------------
void B1(void) // SPARE
//----------------------------------------
{

    //-----------------
    // the next time CpuTimer1 'counter' reaches Period value go to B2
    B_Task_Ptr = &B2;
    //-----------------
}

//----------------------------------------
void B2(void) //  SPARE
//----------------------------------------
{

    //-----------------
    // the next time CpuTimer1 'counter' reaches Period value go to B3
    B_Task_Ptr = &B3;
    //-----------------
}

//----------------------------------------
void B3(void) //  SPARE
//----------------------------------------
{

    //-----------------
    // the next time CpuTimer1 'counter' reaches Period value go to B1
    B_Task_Ptr = &B1;
    //-----------------
}

//=================================================================================
//  C - TASKS (executed in every 150 usec)
//=================================================================================

//--------------------------------- USER ------------------------------------------

//----------------------------------------
void C1(void)
//----------------------------------------
{
    //-----------------
    // the next time CpuTimer2 'counter' reaches Period value go to C2
    C_Task_Ptr = &C2;
    //-----------------
}

//----------------------------------------
void C2(void)
//----------------------------------------
{

    //-----------------
    // the next time CpuTimer2 'counter' reaches Period value go to C3
    C_Task_Ptr = &C3;
    //-----------------
}

//-----------------------------------------
void C3(void) //  SPARE
//-----------------------------------------
{

    //-----------------
    // the next time CpuTimer2 'counter' reaches Period value go to C1
    C_Task_Ptr = &C1;
    //-----------------
}


// ****************************************************************************
// ****************************************************************************
// TODO Motor Control ISR - - Build level 9
//  lsw=0: Open loop voltage output
//  lsw=1: close the current loop (Virtual Angle)
//  lsw=2: close the current loop (hall)
//  lsw=3: close the speed loop (hall)
//  NOTE:- Iq loop is closed using internal ramp angle as position feedback.
//         Therefore, motor speed does not race high with lighter load. User's
//         wanting to use actual rotor angle should ensure that the test value
//         for Iq reference will not race the motor to high speeds. In other
//         words, to use the actual angle, a loaded motor is needed.
// ****************************************************************************
// ****************************************************************************
inline void PM_FOC_main(MOTOR_VARS *motor)
{

    // ------------------------------------------------------------------------------
    // Alignment Routine: this routine aligns the motor to zero electrical angle
    // and in case of QEP also finds the index location and initializes the angle
    // w.r.t. the index location
    // ------------------------------------------------------------------------------
    if (!motor->RunMotor)
        motor->lsw = 0;
    else if (motor->lsw == 0)
    {
        motor->rc.TargetValue = motor->rc.SetpointValue = 0;
    }
    // ------------------------------------------------------------------------------
    //  Connect inputs of the RMP module and call the ramp control macro
    // ------------------------------------------------------------------------------
    if (motor->lsw == 0)
    {
        motor->rc.TargetValue = 0; // lock the motor
    }
    else
    {
        motor->rc.TargetValue = motor->SpeedRef;
    }
    RC_MACRO(motor->rc)

    // ------------------------------------------------------------------------------
    //  Connect inputs of the RAMP GEN module and call the ramp generator macro
    // ------------------------------------------------------------------------------
    motor->rg.Freq = motor->rc.SetpointValue;
    RG_MACRO(motor->rg)

    // ------------------------------------------------------------------------------
    //  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
    //  Connect inputs of the CLARKE module and call the clarke transformation macro
    // ------------------------------------------------------------------------------
    motor->clarke.As = motor->currentAs; // Phase A curr.
    motor->clarke.Bs = motor->currentBs; // Phase B curr.
    CLARKE_MACRO(motor->clarke)

    // ------------------------------------------------------------------------------
    //  Connect inputs of the SPEED_FR module and call the speed calculation macro
    // ------------------------------------------------------------------------------
//  motor->speed.ElecTheta = motor->ElecTheta;
//  SPEED_FR_MACRO(motor->speed)

    // ------------------------------------------------------------------------------
    //  Connect inputs of the PARK module and call the park trans. macro
    // ------------------------------------------------------------------------------
    motor->park.Alpha = motor->clarke.Alpha;
    motor->park.Beta = motor->clarke.Beta;

    if (motor->lsw == 0)
    {
        motor->park.Angle = 0;
    }
    else if (motor->lsw == 1)
    {
        motor->park.Angle = motor1.rg.Out;//Hall_thata_hat_offset;
    }
    else
    {
        _iq EleTheta_norm = _IQ(0.0);
        EleTheta_norm = motor1.ElecTheta + cal_offset;
        if(EleTheta_norm > _IQ(1.0))
        {
            EleTheta_norm -= _IQ(1.0);
        }
        if(EleTheta_norm < _IQ(0.0))
        {
            EleTheta_norm += _IQ(1.0);
        }
        motor->park.Angle = EleTheta_norm; //EleTheta_norm;
    }

    motor->park.Sine = __sinpuf32(motor->park.Angle);
    motor->park.Cosine = __cospuf32(motor->park.Angle);
    PARK_MACRO(motor->park)

    // ------------------------------------------------------------------------------
    //  Connect inputs of the PID_REG3 module and call the PID IQ controller macro
    // ------------------------------------------------------------------------------
    if (motor->lsw == 0)
    {
        motor->pi_iq.Ref = 0;
        motor->pi_iq.i1 = 0;
    }
    else if(motor->lsw == 1)
    {
        motor->pi_iq.Ref = 0;
    }
    else if(motor->lsw == 2)
    {
        motor->pi_iq.Ref = motor->IqRef;
    }
    else if(motor->lsw == 3)
    {
        motor->pi_iq.Ref = _IQmpy(pid1_speed.term.Out,0.3);
    }
    else
    {
        motor->pi_iq.Ref = 0;
    }
    motor->pi_iq.Fbk = motor->park.Qs;
    PI_MACRO(motor->pi_iq);
    if(motor->lsw < 2)
    {
        motor->pi_iq.i1 = 0;
        motor->pi_iq.Out = 0;
    }

    // ------------------------------------------------------------------------------
    //  Connect inputs of the PI module and call the PID ID controller macro
    // ------------------------------------------------------------------------------
    if (motor->lsw == 0)
    {
        motor->pi_id.Ref = motor->IdRef;
        motor->pi_id.i1 = 0;
    }
    else
    {
        motor->pi_id.Ref = motor->IdRef;
    }
    motor->pi_id.Fbk = motor->park.Ds;
    PI_MACRO(motor->pi_id);
    if(motor->lsw < 2)
    {
        motor->pi_id.i1 = 0;
        motor->pi_id.Out = 0;
    }

    // ------------------------------------------------------------------------------
    //  Connect inputs of the INV_PARK module and call the inverse park trans. macro
    // ------------------------------------------------------------------------------
//  motor->ipark.Ds = motor->pi_id.Out; // motor->VdTesting
//  motor->ipark.Qs = motor->pi_iq.Out; // motor->VqTesting
    if(motor->lsw == 0)
    {
        motor->ipark.Ds = 0;
        motor->ipark.Qs = 0;
    }
    else if(motor->lsw == 1)
    {
        motor->ipark.Ds = motor->VdTesting;
        motor->ipark.Qs = motor->VqTesting;
    }
    else
    {
        motor->ipark.Ds = motor->pi_id.Out;
        motor->ipark.Qs = motor->pi_iq.Out;
    }
    motor->ipark.Sine = motor->park.Sine;
    motor->ipark.Cosine = motor->park.Cosine;
    IPARK_MACRO(motor->ipark);

    // ------------------------------------------------------------------------------
    //  Connect inputs of the VOLT_CALC module and call the phase voltage calc. macro
    // ------------------------------------------------------------------------------
    motor->volt.DcBusVolt = motor->voltageDC;
    motor->volt.MfuncV1 = motor->svgen.Ta;
    motor->volt.MfuncV2 = motor->svgen.Tb;
    motor->volt.MfuncV3 = motor->svgen.Tc;
    PHASEVOLT_MACRO(motor->volt)
    // ------------------------------------------------------------------------------
    //  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
    // ------------------------------------------------------------------------------
    motor->svgen.Ualpha = motor->ipark.Alpha;
    motor->svgen.Ubeta = motor->ipark.Beta;
    SVGENDQ_MACRO(motor->svgen)


    // ------------------------------------------------------------------------------
    //  Computed Duty and Write to CMPA register
    // ------------------------------------------------------------------------------
    (motor->PwmARegs)->CMPA.bit.CMPA = (INV_PWM_HALF_TBPRD * motor->svgen.Ta) + INV_PWM_HALF_TBPRD;
    (motor->PwmBRegs)->CMPA.bit.CMPA = (INV_PWM_HALF_TBPRD * motor->svgen.Tb) + INV_PWM_HALF_TBPRD;
    (motor->PwmCRegs)->CMPA.bit.CMPA = (INV_PWM_HALF_TBPRD * motor->svgen.Tc) + INV_PWM_HALF_TBPRD;

    // read Hall set#1


    // ------------------------------------------------------------------------------
    //  Connect inputs of the speed pid module
    // ------------------------------------------------------------------------------
    /*
    if(motor1.lsw == 3)
    {
        if(speed_ctl_timer > 19)
        {
            speedRef_ramp = ramper_speed(speedRef, speedRef_ramp,0.00005);//0.00002
            pid1_speed.term.Ref = speedRef_ramp;
            pid1_speed.term.Fbk = _IQdiv(Mech_Speed_out,3000.0);
            PID_MACRO(pid1_speed);
            speed_ctl_timer = 0;
        }
       else
       {
           speed_ctl_timer++;
       }
    }
    else
    {
        speedRef_ramp = 0;
        pid1_speed.term.Ref = 0;
        pid1_speed.term.Fbk = 0;
        pid1_speed.term.Out = 0;
        pid1_speed.data.i1 = 0;
    }
    */

    // ------------------------------------------------------------------------------
    //  Connect inputs of the DATALOG module
    // ------------------------------------------------------------------------------
    //  DlogCh2 = motor->ElecTheta; // motor->currentAs;
    //  DlogCh1 = motor->rg.Out;
    //  DlogCh3 = motor->clarke.As; // motor->current.Bs;
    //  DlogCh4 = motor->clarke.Bs; // motor->current.Cs;

    //------------------------------------------------------------------------------
    // Variable display on PWMDAC - available
    //------------------------------------------------------------------------------
    PwmDacCh4 = _IQtoQ15(motor1.pi_iq.Ref*3.0);               // Launchpad pin DAC1
    PwmDacCh1 = _IQtoQ15(motor1.pi_iq.Fbk*3.0);               // Launchpad pin DAC2
    PwmDacCh3 = _IQtoQ15(motor1.ElecTheta); //start at 1.7v max at 3.3v //motor->clarke.As                // Launchpad pin DAC3
    PwmDacCh2 = _IQtoQ15(0.0); //start at 1.7v max at 3.3v //_IQtoQ15(motor->clarke.As);         // Launchpad pin DAC4 -8 ~ 8
//    PwmDacCh1 = _IQtoQ15(1.0);

    return;
}

interrupt void LogISR(void){
    GPIO_WritePin(MOTOR1_Gate_GPIO, TRUE);
    HFCurrentALog[LogPos] = IFB_A1_PPB; // U phase current
    HFCurrentBLog[LogPos] = IFB_B1_PPB;
    HFCurrentCLog[LogPos] = IFB_C1_PPB;
    HFCurrentDCLog[LogPos] = IFB_DC_PPB;
    HFSinPLog[LogPos] = AdcbResultRegs.ADCRESULT1; // sin+
    HFSinNLog[LogPos] = AdcbResultRegs.ADCRESULT2;
    HFCosPLog[LogPos] = AdcbResultRegs.ADCRESULT3;
    HFCosNLog[LogPos] = AdcbResultRegs.ADCRESULT4;
    if(LogPos > 999){
        LogPos = 0;
    }
    else{
        LogPos ++;
    }
    GPIO_WritePin(MOTOR1_Gate_GPIO, FALSE);
}


// ****************************************************************************
// ****************************************************************************
// TODO Motor Control ISR
// ****************************************************************************
// ****************************************************************************
interrupt void MotorControlISR(void)
{
    // Verifying the ISR
    IsrTicker++;
    if(EnableFlag)
    {
        EnableDRV = 1;
        motor1.lsw = 2;
//        motor1.lsw = 1;
//        motor1.VdTesting = 0.0;
//        motor1.SpeedRef = 0.02;
        motor1.IqRef = torqueCMD; // torque command
    }
    else
    {
        EnableDRV = 0;
        motor1.lsw = 0;
        motor1.IqRef = 0;
    }


//    GPIO_WritePin(MOTOR1_GATE_EN_GPIO, EnableDRV); //GPIO6
//    GPIO_WritePin(MOTOR1_Gate_GPIO,EnableDRV); //GPIO7
    // check the EnaDRV feedback
//unfinish
//  EnaDRV_FBK = GPIO_ReadPin(7); //GPIO7
//  if(ocp_cnt > 20) // 1 ms
//  {
//
//        if((EnaDRV_FBK == 0)&&(EnaDRV_FBK_last == 0))
//        {
//            GPIO_WritePin(MOTOR1_GATE_EN_GPIO, 0);
//        }
//        else
//        {
//            GPIO_WritePin(MOTOR1_GATE_EN_GPIO, EnableDRV);
//        }
//        EnaDRV_FBK_last = EnaDRV_FBK;
//        ocp_cnt = 0;
//  }
//  else
//  {
//      ocp_cnt ++;
//  }


    //    Pdl_tq = (float)PDL_TQ_SEN* ADC_PU_PPB_SCALE_FACTOR;

    //    GPIO_TogglePin(TEMP_GPIO, IsrTicker%2);

    // ------------------------------------------------------------------------------
    //  Measure phase currents and obtain position encoder (QEP) feedback
    // ------------------------------------------------------------------------------
    motorCurrentSense(); //  Measure normalized phase currents (-1,+1)
    motorVoltageSense(); //  Measure normalized phase voltages (-1,+1)
    motorTempSense();    //  Measure normalized temperature (-1,+1)

    // ------------------------------------------------------------------------------
    //  Obtain position encoder (Varicoder) feedback
    // ------------------------------------------------------------------------------
    sin_pos = AdcbResultRegs.ADCRESULT1;
    sin_neg = AdcbResultRegs.ADCRESULT2;
    cos_pos = AdcbResultRegs.ADCRESULT3;
    cos_neg = AdcbResultRegs.ADCRESULT4;
    motor1.ElecTheta = EncoderVaricoder(sin_pos, sin_neg, cos_pos,cos_neg);

    // ------------------------------------------------------------------------------
    //  Obtain position encoder (Varicoder) feedback
    // ------------------------------------------------------------------------------
    if(speed_cnt > 19)
    {
        deltaTheta = motor1.ElecTheta - Electheta_last;
        Electheta_last = motor1.ElecTheta;
        if(deltaTheta > 0.8)
        {
           deltaTheta -= 1.0;
        }
        else if(deltaTheta < -0.8)
        {
            deltaTheta += 1.0;
        }
        speed_rpm = deltaTheta*1000.0/Fischer_PP*60; // [rpm]
        speed_cnt = 0;
    }
    else
    {
        speed_cnt ++;
    }

//

    // ------------------------------------------------------------------------------
    //    FOC Main functions

    PM_FOC_main(&motor1);
    // ------------------------------------------------------------------------------
    //    Call the DATALOG update function.
    // ------------------------------------------------------------------------------
    //DLOG_4CH_F_FUNC(&dlog_4ch1);

    // ------------------------------------------------------------------------------
    //    Call the PWMDAC update macro.
    // ------------------------------------------------------------------------------
    PWMDAC_MACRO(pwmdac1)

    // ------------------------------------------------------------------------------
    //    Call DAC update.
    // ------------------------------------------------------------------------------
    DAC_PTR[DACA]->DACVALS.all = dacAval;
    DAC_PTR[DACB]->DACVALS.all = dacBval;

    // clear ADCINT1 INT and ack PIE INT
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;



} // MainISR Ends Here

/****************************************************************************
 * End of Code *
 * ***************************************************************************
 */
