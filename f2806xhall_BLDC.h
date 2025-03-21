/* ==================================================================================
File name:        F2806XHALL3_GPIO.H                     
                    
Originator:	 Digital Control Systems Group
			 Texas Instruments
Description: Header file containing data type and macro definitions and 
			 initializers. 

Target: TMS320F2806x family
              
=====================================================================================
History:
-------------------------------------------------------------------------------------
 Hall logics are always read, not only when Hall edges are detected. 
 All Hall bits are also read at once instead of individual Hall bit be read. 
 03-01-2010 Version 1.0: Modified to use GPIOs only for Piccolo instead of eCAP
-----------------------------------------------------------------------------------*/

#ifndef __F2806X_HALL3_GPIO_H__
#define __F2806X_HALL3_GPIO_H__

#include "f2837xbmsk.h"


/*-----------------------------------------------------------------------------
 Initialization states for ECAP Control Registers 1 and 2 for ECAP1/2/3
------------------------------------------------------------------------------*/


#define HALL3_ECCTL2_INIT_STATE   ( CONTINUOUS_MODE + \
                                    TSCNTSTP_FREE + \
                                    SYNCI_DISABLE + \
                                    SYNCO_DISABLE + \
                                    CAPTURE_MODE )

/*-----------------------------------------------------------------------------
Define the structure of the Capture Driver Object 
------------------------------------------------------------------------------*/
typedef struct { Uint16 CmtnTrigHall;    // Output: Commutation trigger for Mod6cnt input (0 or 0x7FFF)
                 Uint16 CapCounter;      // Variable: Running count of detected edges on ECAP1,2,3
                 Uint16 DebounceCount;   // Variable: Counter/debounce delay current value
                 Uint16 DebounceAmount;  // Parameter: Counter delay amount to validate/debounce GPIO readings
                 Uint16 HallGpio;        // Variable: Most recent logic level on ECAP/GPIO
                 Uint16 HallGpioBuffer;  // Variable: Buffer of last logic level on ECAP/GPIO while being debounced
                 Uint16 HallGpioAccepted; // Variable: Debounced logic level on ECAP/GPIO
                 Uint16 EdgeDebounced;   // Variable: Trigger from Debounce function to Hall_Drv, if = 0x7FFF edge is debounced
                 Uint16 HallMap[6];      // Variable: ECAP/GPIO logic levels for HallMapPointer = 0-5
                 Uint16 CapFlag;          // Variable: ECAP flags, indicating which ECAP detected the edge
                 Uint16 StallCount;       // Variable: If motor stalls, this counter overflow triggers 
                                          //           commutation to start rotation. Rotation is defined as
                                          //           an edge detection of a hall signal.
                 Uint16 HallMapPointer;   // Input/Output (see note below): During hall map creation, this variable points to the 
                                          //            current commutation state.  After map creation, it 
                                          //            points to the next commutation state.
                 int16 Revolutions;       // Parameter: Running counter, with a revolution defined as 1-cycle 
                                          //            of the 6 hall states
                 Uint16 HallBitA;
                 Uint16 HallBitB;
                 Uint16 HallBitC;
                 Uint16 counterHall;
               } HALL3;

/* Note:
- When HallMapPointer is as an input, it is defined by MOD6_CNT.
- When HallMapPointer is as an output, during hall map creation, 
  this variable points to the current commutation state.  
  After map creation, it points to the next commutation state.
*/
/*-----------------------------------------------------------------------------
Define a handle for the HALL3 object
------------------------------------------------------------------------------*/
typedef HALL3 *HALL3_handle;

/*------------------------------------------------------------------------------
Default Initializer for the F2806X HALL3 Object
------------------------------------------------------------------------------*/
#define F2806XHALL3      {0,  \
                          0,  \
                          0,  \
                         10,  \
                          0,  \
                          0,  \
                          0,  \
                          0,  \
              {0,0,0,0,0,0},  \
                          0,  \
                     0xFFFF,  \
                          0,  \
                        -10,  \
						  0,  \
						  0,  \
						  0,  \
                          0,  \
					    }

/*------------------------------------------------------------------------------
Target Independent Default Initializer HALL3 Object
------------------------------------------------------------------------------*/
#define HALL3_DEFAULTS F2806XHALL3

/*----------------------------------------------------------------------------------------------
	 HALL3_INIT Macro Definition
----------------------------------------------------------------------------------------------*/
#define HALL3_INIT_MACRO(v)																	\
																							\
	EALLOW;                       /* Enable EALLOW*/										\
	/* Configure Hall snsor input pins   */                                                 \
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;  /* GPIO20 is HALL - A1 */                         \
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 0;  /* GPIO21 is HALL - B1 */                         \
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;  /* GPIO54 is HALL - C1 */  						\
    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;  /* GPIO20 is HALL - A1 */                          \
    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;  /* GPIO21 is HALL - B1 */                          \
    GpioCtrlRegs.GPDMUX1.bit.GPIO99 = 0;  /* GPIO54 is HALL - C1 */                          \
	/* Configure GPIO40/41/42 as inputs*/													\
	GpioCtrlRegs.GPADIR.bit.GPIO20 = 0;   /* GPIO20 is HALL - A1 */							\
	GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;   /* GPIO21 is HALL - B1 */							\
	GpioCtrlRegs.GPBDIR.bit.GPIO54 = 0;   /* GPIO54 is HALL - C1 */							\
    GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;   /* GPIO20 is HALL - A1 */                          \
    GpioCtrlRegs.GPBDIR.bit.GPIO56 = 0;   /* GPIO21 is HALL - B1 */                          \
    GpioCtrlRegs.GPDDIR.bit.GPIO99 = 0;   /* GPIO54 is HALL - C1 */                          \
	EDIS;                         /* Disable EALLOW*/										\
																							\
	HALL3_DETERMINE_STATE_MACRO(v)															\
	v.HallGpioBuffer = v.HallGpio;       /* Init with current ECAP/GPIO logic levels*/		\
	v.HallGpioAccepted = v.HallGpio;     /* Init with current ECAP/GPIO logic levels*/	

/*----------------------------------------------------------------------------------------------
	 HALL3_DETERMINE_STATE Macro Definition
----------------------------------------------------------------------------------------------*/
	Uint32 temp;
	Uint16 HallGpioBitA,HallGpioBitB,HallGpioBitC;

#define HALL3_DETERMINE_STATE_MACRO(v)														\
																							\
	/* temp.2-0 = GPIO42.GPIO41.GPIO40 */													\
	v.HallBitA = GpioDataRegs.GPBDAT.bit.GPIO54;	    /*(temp&0x00000001) save GPIO54 - A*/\
	v.HallBitB = GpioDataRegs.GPBDAT.bit.GPIO55;		/*(temp&0x00000002) save GPIO55 - B*/\
	v.HallBitC = GpioDataRegs.GPBDAT.bit.GPIO57;		/* (temp&0x00000004)save GPIO57 - C*/\
    HallGpioBitA = v.HallBitA;	    			/* shift GPIO40 - A*/						\
    HallGpioBitB = v.HallBitB<<1;			    /* shift GPIO41 - B*/					    \
    HallGpioBitC = v.HallBitC<<2;			    /* shift GPIO42 - C*/					    \
																							\
	v.HallGpio = HallGpioBitA + HallGpioBitB + HallGpioBitC;

/*----------------------------------------------------------------------------------------------
	 HALL3_READ Macro Definition
----------------------------------------------------------------------------------------------*/

#define HALL3_READ_MACRO(v)																					\
	v.CmtnTrigHall = 0;		/* Reset trigger, it only handshakes with calling program.*/					\
	if (v.EdgeDebounced==0)	/* Debounce current position. */												\
	{  																										\
		HALL3_DEBOUNCE_MACRO(v)																				\
		v.CmtnTrigHall = v.EdgeDebounced;      /* Set Commutation trigger here*/							\
	}																										\
	else							/* If current position is debounced, find match in table */				\
		HALL3_NEXT_STATE_MACRO(v)	/* and return pointer to current state.  Ptr to be incremented*/		\
								    /* by MOD6CNT after RET.*/												\
																											\
	v.EdgeDebounced = 0;            /* Reset trigger*/ 														\
	switch(v.HallGpio){                                                                                     \
       case 1: v.counterHall = 2; break;                                                                    \
       case 2: v.counterHall = 4; break;                                                                    \
       case 3: v.counterHall = 3; break;                                                                    \
       case 4: v.counterHall = 0; break;                                                                    \
       case 5: v.counterHall = 1; break;                                                                    \
       case 6: v.counterHall = 5; break;                                                                    \
    }


/*----------------------------------------------------------------------------------------------
	 HALL3_DEBOUNCE Macro Definition
----------------------------------------------------------------------------------------------*/


#define HALL3_DEBOUNCE_MACRO(v)																				\
																											\
	HALL3_DETERMINE_STATE_MACRO(v)			/* read HallGpio*/												\
																											\
	if (v.HallGpio == v.HallGpioAccepted)	/* GPIO_UNCHANGED: Current GPIO reading == debounced ..*/		\
											/*..GPIO reading, no change in state (no edge yet)	*/			\
	{																										\
		if (v.Revolutions <= 0)         	/* Only create hall map during initial Revolutions*/			\
			HALL3_CREATE_MAP(v)																				\
																											\
		v.StallCount -= 1;            		/* Decrement stall counter*/									\
		if (v.StallCount == 0)																				\
		{																									\
			v.EdgeDebounced = 0x7FFF;		/* 0x7FFF If motor has stalled, then user trigger to commutate*/\
			v.StallCount = 0xFFFF;    		/* Reset counter to starting value*/							\
		}																									\
																											\
		v.DebounceCount = 0;																				\
	}																										\
	else          							/* GPIO_CHANGED: Motor might have moved to a new position.*/	\
	{																										\
		if (v.HallGpio == v.HallGpioBuffer)	/* Current GPIO reading == previous GPIO reading?*/				\
		{																									\
			if (v.DebounceCount >= v.DebounceAmount)	/* If equal, is current GPIO reading debounced?*/	\
			{																								\
				v.HallGpioAccepted = v.HallGpioBuffer;	/* Current GPIO reading is now debounced*/			\
				v.EdgeDebounced = 0x7FFF;        			/*Edge/position debounced, trigger commutation*/\
																											\
				v.StallCount = 0xFFFF;         		    /* On new edge, reset stall counter*/				\
				v.CapCounter += 1;                   	/* Increment running edge detection counter*/		\
																											\
				v.DebounceCount = 0;             		/* Reset debounce counter*/							\
																											\
				if (v.HallMapPointer==0)																	\
					v.Revolutions += 1;          		/* Increment on every rev (HallMapPointer = 0)*/	\
			} 																								\
			else     									/* DEBOUNCE_MORE*/									\
				v.DebounceCount += 1;            		/* Increment debounce counter*/						\
		}																									\
		else       										/* NEW_READING*/									\
		{   																								\
			v.HallGpioBuffer = v.HallGpio;  		/* Save new reading and reset debounce counter*/		\
			v.DebounceCount = 0;																			\
		}																									\
	}																										\

/*----------------------------------------------------------------------------------------------
	 HALL3_NEXT Macro Definition
----------------------------------------------------------------------------------------------*/

	int16 i, HallPointer;
#define HALL3_NEXT_STATE_MACRO(v)																			\
																											\
	if (v.Revolutions>0)      			/* Only run function after map has been created.*/					\
	{																										\
		for (i=0;i<=5;i++)          	/* Search for a match of current debounced GPIO position*/			\
		{                           	/* and the table entries.*/											\
			if (v.HallMap[i] == v.HallGpioAccepted)		/* Match_Found*/									\
				HallPointer = i; 																			\
		}																									\
																											\
		v.HallMapPointer = HallPointer; /* On match, save pointer position. Pointer will be incremented */	\
	}	                            	/* by 1 since MOD6CNT will receive a positive trigger*/

/*----------------------------------------------------------------------------------------------
	 HALL3_CREATE_MAP Macro Definition
----------------------------------------------------------------------------------------------*/

#define HALL3_CREATE_MAP(v)																					\
																											\
	v.HallMap[v.HallMapPointer] = v.HallGpioAccepted;  /* Save debounced GPIO to table.*/ 					




#endif // __F2806X_HALL3_H__ 






