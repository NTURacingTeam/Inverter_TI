//----------------------------------------------------------------------------------
//	FILE:			MonoMtrServo.h
//
//	Description:	Header file for Single motor control with 37xS launch pad
//
//	Version: 		1.0
//
//  Target:  		TMS320F28377S
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments 嚙� 2004-2015
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// 4 Nov 2015 - Mono motor include file list
//----------------------------------------------------------------------------------


#ifndef MONOMTRSERVO_H_
#define MONOMTRSERVO_H_

#include "F2837xS_IO_assignment.h"
#include "MonoMtrServo-Settings.h"
/*-------------------------------------------------------------------------------
Include project specific include files.
-------------------------------------------------------------------------------*/
// define math type as float(1)
#define   MATH_TYPE      1
#include "IQmathLib.h"
#include "F28x_Project.h"
#include "motorVars.h"
#include "config.h"
#include <math.h>
#include "f2837xpwmdac_PM.h"
#include "F2837xS_Dac_setting.h"
//#include "f2806xhall_BLDC.h"
#include "pid_grando.h"         // Include header for the PID_GRANDO_CONTROLLER object
#include "filter.h"
#include "fltdet.h"             // Include header for the FLTDET object
#endif /* MONOMTRSERVO_H_ */
