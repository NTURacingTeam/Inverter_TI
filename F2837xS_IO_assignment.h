//----------------------------------------------------------------------------------
//	FILE:			F2837xS_IO_assignment.h
//
//	Description:	Contains IO assignments for the project
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
//----------------the high dynamic response for the fast mirror rotation. Because ------------------------------------------------------------------
// 4 Nov 2015 - CPU IO assignments
//----------------------------------------------------------------------------------


#ifndef F2837xS_IO_ASSIGNMENT_H_
#define F2837xS_IO_ASSIGNMENT_H_


/******************************************************************************
Peripheral Assignments:
   MOTOR 1:
		 - EPWMs ==>> EPWM7, EPWM8,  EPWM9  ---> A, B, C
		 - QEP   ==>> EQep1
		 - SPI   ==>> Spia

		 Analog signals - Motor 1
		 Vdc  ADC 14
		 Va   ADC B1
		 Vb   ADC B4
		 Vc   ADC B2
		 Ia   ADC A0
		 Ib   ADC B0
		 Ic   ADC A1

******************************************************************************/

// *************************************************
// ************ GPIO pin assignments ***************
// *************************************************

// General purpose useage (used by QEP1-I)
//#define  BLUE_LED_GPIO    13
//#define  BLUE_LED_MUX      0
//
//#define  TEMP_GPIO        78
//#define  TEMP_MUX          0

// ***************************************************************
// MOTOR 1 EPWMDAC selections
// ========================

#define  MOTOR1_EPWMDAC_6A_GPIO     10
#define  MOTOR1_EPWMDAC_6A_MUX      1

#define  MOTOR1_EPWMDAC_8A_GPIO     14
#define  MOTOR1_EPWMDAC_8A_MUX      1

#define  MOTOR1_EPWMDAC_9A_GPIO     16
#define  MOTOR1_EPWMDAC_9A_MUX      5

#define  MOTOR1_EPWMDAC_10A_GPIO    18
#define  MOTOR1_EPWMDAC_10A_MUX     5
/*
#define  MOTOR1_EPWMDAC_A_GPIO     10
#define  MOTOR1_EPWMDAC_A_MUX      1

#define  MOTOR1_EPWMDAC_Ab_GPIO    14
#define  MOTOR1_EPWMDAC_Ab_MUX     1

#define  MOTOR1_EPWMDAC_B_GPIO     16
#define  MOTOR1_EPWMDAC_B_MUX      1

#define  MOTOR1_EPWMDAC_Bb_GPIO    18
#define  MOTOR1_EPWMDAC_Bb_MUX     1
*/



// MOTOR 1 EPWM selections
// ========================
#define  MOTOR1_EPWM_AH_GPIO    0
#define  MOTOR1_EPWM_AH_MUX     1

#define  MOTOR1_EPWM_BH_GPIO    2
#define  MOTOR1_EPWM_BH_MUX     1

#define  MOTOR1_EPWM_CH_GPIO    4
#define  MOTOR1_EPWM_CH_MUX     1

#define  MOTOR1_EPWM_AL_GPIO    1
#define  MOTOR1_EPWM_AL_MUX     1

#define  MOTOR1_EPWM_BL_GPIO    3
#define  MOTOR1_EPWM_BL_MUX     1

#define  MOTOR1_EPWM_CL_GPIO    5
#define  MOTOR1_EPWM_CL_MUX     1



// ***************************************************************
// MOTOR 1 EQEP selections
// ========================
#define  MOTOR1_QEPA_GPIO       62
#define  MOTOR1_QEPA_MUX         5

#define  MOTOR1_QEPB_GPIO       63
#define  MOTOR1_QEPB_MUX         5

#define  MOTOR1_QEPI_GPIO       65
#define  MOTOR1_QEPI_MUX         5

// ***************************************************************
// MOTOR 1 HALL_1 selections
// ========================
#define  MOTOR1_HALLA1_GPIO       20
#define  MOTOR1_HALLA1_MUX         0

#define  MOTOR1_HALLB1_GPIO       21
#define  MOTOR1_HALLB1_MUX         0

#define  MOTOR1_HALLC1_GPIO       54
#define  MOTOR1_HALLC1_MUX         0

// ***************************************************************
// MOTOR 1 HALL_2 selections
// ========================
#define  MOTOR1_HALLA2_GPIO       55
#define  MOTOR1_HALLA2_MUX        0

#define  MOTOR1_HALLB2_GPIO       56
#define  MOTOR1_HALLB2_MUX        0

#define  MOTOR1_HALLC2_GPIO       99
#define  MOTOR1_HALLC2_MUX        0

// ***************************************************************
// MOTOR 1 - SPI selections
// =========================
#define  MOTOR1_SDI_GPIO       58
#define  MOTOR1_SDI_MUX        15

#define  MOTOR1_SDO_GPIO       59
#define  MOTOR1_SDO_MUX        15

#define  MOTOR1_CLK_GPIO       60
#define  MOTOR1_CLK_MUX        15

// ***************************************************************
// MOTOR 1 DRVxx selections
// ========================
/*
#define  MOTOR1_SCS_GPIO        4
#define  MOTOR1_SCS_MUX         0

#define  MOTOR1_EN_GATE_GPIO   72
#define  MOTOR1_EN_GATE_MUX     0

#define  MOTOR1_FAULT_GPIO     90
#define  MOTOR1_FAULT_MUX       0

#if (MOTOR1_DRV == 1)
  #define  MOTOR1_DC_CAL_GPIO    73
  #define  MOTOR1_DC_CAL_MUX      0

  #define  MOTOR1_OCTW_GPIO      89
  #define  MOTOR1_OCTW_MUX        0
#else
  #define  MOTOR1_WAKE_GPIO      73
  #define  MOTOR1_WAKE_MUX        0

  #define  MOTOR1_PWRGD_GPIO     // tied to RESET# input of MCU - no use in code
#endif
*/



//define gate en
#define MOTOR1_Gate_GPIO 7
#define MOTOR1_Gate_MUX 0

// ***************************************************************
// AFM-DRV1.0 GPIO selections
// ========================


#define  MOTOR1_GATE_EN_GPIO   6
#define  MOTOR1_GATE_EN_MUX     0

#define  MOTOR1_IOinput1_GPIO   24
#define  MOTOR1_IOinput1_MUX     0

#define  MOTOR1_IOinput2_GPIO   25
#define  MOTOR1_IOinput2_MUX     0

#define  MOTOR1_IOinput3_GPIO   26
#define  MOTOR1_IOinput3_MUX     0

#define  MOTOR1_SD_IN_GPIO     27
#define  MOTOR1_SD_IN_MUX       0

// *************************************************
// ************ ADC pin assignments ***************
// *************************************************

// AFM-DRV1.0 Analog definitions
// ============================
// Ia   ADC C2     SOC2
// Ib   ADC B2     SOC2
// Ic   ADC A2     SOC2
// Idc  ADC A1     SOC1
// Va   ADC (A)14  SOC14
// Vb   ADC C3     SOC3
// Vc   ADC B3     SOC3
// Vdc  ADC A3     SOC3
// Vn   ADC A0     SOC0
// TmpA ADC A5     SOC5
// TmpB ADC A4     SOC4
// TmpC ADC C4     SOC4

// Current feedback result
#define IFB_U1       AdcdResultRegs.ADCRESULT2
#define IFB_V1       AdcdResultRegs.ADCRESULT3
#define IFB_W1       AdcdResultRegs.ADCRESULT4
#define IFB_DC1      AdcdResultRegs.ADCRESULT5
#define IFB_A1_PPB   ((signed int)AdcdResultRegs.ADCPPB1RESULT.all)
#define IFB_B1_PPB   ((signed int)AdcdResultRegs.ADCPPB2RESULT.all)
#define IFB_C1_PPB   ((signed int)AdcdResultRegs.ADCPPB3RESULT.all)
#define IFB_DC_PPB   ((signed int)AdcdResultRegs.ADCPPB4RESULT.all)

#define ADC_PU_SCALE_FACTOR        0.00024414  //125/4096
#define ADC_PU_PPB_SCALE_FACTOR    0.061035     //125/2048
// Voltage feedback result
#define VFB_U1       AdcaResultRegs.ADCRESULT2
#define VFB_V1       AdcaResultRegs.ADCRESULT3
#define VFB_W1       AdcaResultRegs.ADCRESULT4
#define VFB_DC1      AdcaResultRegs.ADCRESULT5

#define Voltage_ADC_PU_SCALE_FACTOR 0.24414

// Driver temperature feedback result
#define TFB_A        AdcaResultRegs.ADCRESULT7
#define TFB_B        AdcaResultRegs.ADCRESULT8
#define TFB_C        AdcaResultRegs.ADCRESULT9
#define TFB_motor    AdccResultRegs.ADCRESULT6

#define Temp_ADC_PU_SCALE_FACTOR 0.24414 //TempRange/4096

/*****************************************************************************
 * ***************************************************************************
 */

#endif /* F2837xS_IO_ASSIGNMENT_H_ */
