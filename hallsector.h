//#############################################################################
//
//! \file   hallsector.h
//!
//! \brief  hall sensor sector selector & for fault tolerant
//!         Hertz Lyu
//
//  Group:          MSP430
//  Target Device:  MSP430FR5969
//#############################################################################
// $Release Date: 10/19/2022 $
//#############################################################################
// Sector1: 101 (5)
// Sector2: 100 (4)
// Sector3: 110 (6)
// Sector4: 010 (2)
// Sector5: 011 (3)
// Sector6: 001 (1)
//############################################################################
typedef struct {
    int Hall; // Hall sensor total 3-bit
    int Hall_last; //Hall sensor total 3-bit (last step)
	int Hall_A; // Hall sensor phase A input
	int Hall_A_last; // Hall sensor phase A input (last step)
	int Hall_B; // Hall sensor phase B input
	int Hall_B_last; // Hall sensor phase B input (last step)
	int Hall_C; // Hall sensor phase C input
	int Hall_C_last; // Hall sensor phase C input (last step)
	int Hall_Sector; // Hall sensor sector
	int Hall_Sector_last; // Hall sensor sector (last step)
	int Theta_out; // Hall sensor electric angle
	int Hall_flag; // Hall state update flag
	unsigned int Hall_time_out; // Hall state update flag
	unsigned int Hall_time_internal; // Hall state update flag
} HALLSECTOR;

inline void hallsector(HALLSECTOR * hallsectordata);

#define HALLSECTOR_DEFAULTS {  0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
                        0, \
             		  }
             		  
inline void hallsector(HALLSECTOR * hallsectordata)
{
    //hallsectordata->Hall = (((P2IN & BIT3)>>1)|(P1IN & BIT0)|(P1IN & BIT1));
    hallsectordata->Hall = (((P1IN & BIT1)>>1)|((P1IN & BIT0)<<1)|((P2IN & BIT3)>>1)); //8 H2(V) + H1(U) + H3(W)
    switch(hallsectordata->Hall)
    {
        case 1: // 001
            hallsectordata->Hall_Sector = 6;
            hallsectordata->Theta_out = 427;
            break;
        case 2: // 010
            hallsectordata->Hall_Sector = 4;
            hallsectordata->Theta_out = 256;
            break;
        case 3: // 011
            hallsectordata->Hall_Sector = 5;
            hallsectordata->Theta_out = 341;
            break;
        case 4: // 100
            hallsectordata->Hall_Sector = 2;
            hallsectordata->Theta_out = 85;
            break;
        case 5: // 101
            hallsectordata->Hall_Sector = 1;
            hallsectordata->Theta_out = 0;
            break;
        case 6: // 110
            hallsectordata->Hall_Sector = 3;
            hallsectordata->Theta_out = 171;
            break;
        default: // 000 or 111
            hallsectordata->Hall_Sector = 0;
            hallsectordata->Theta_out = 0;
            break;
    } //switch(hallsectordata->Hall)
    if (hallsectordata->Hall != hallsectordata->Hall_last)
    {
        hallsectordata->Hall_flag = 1; // raise flag for new speed calculation
        hallsectordata->Hall_time_out = hallsectordata->Hall_time_internal; // Update sector changing time output
        hallsectordata->Hall_time_internal = 1; // reset internal Hall timer
        //P1OUT |= BIT7; // GPIO 1.7 ON
    } //end if
    else // hall state is not updated
    {
        if (hallsectordata->Hall_time_internal > 65534) hallsectordata->Hall_time_internal = 0;
        hallsectordata->Hall_time_internal += 1;
        //P1OUT &= ~BIT7; // GPIO 1.7 OFF
    }
    hallsectordata->Hall_last = hallsectordata->Hall;
    /*
    // Hall sector change?
    if ((hallsectordata->Hall_A != hallsectordata->Hall_A_last)||(hallsectordata->Hall_B != hallsectordata->Hall_B_last)||(hallsectordata->Hall_C != hallsectordata->Hall_C_last))
    {
        // Decide Hall Sector 0~5
        if(hallsectordata->Hall_A == 1)
        {
            if(hallsectordata->Hall_B == 1) //Sector3: 1,1,0
            {
                hallsectordata->Hall_Sector = 3;
                hallsectordata->Theta_out = 171;
            }
            else
            {
                if(hallsectordata->Hall_C == 1) //Sector1: 1,0,1
                {
                    hallsectordata->Hall_Sector = 1;
                    hallsectordata->Theta_out = 0;
                }
                else //Sector2: 1,0,0
                {
                    hallsectordata->Hall_Sector = 2;
                    hallsectordata->Theta_out = 85;
                }
            }

        } //if: Hall_A == 1
        else //(Hall_A == 0)
        {
            if(hallsectordata->Hall_B == 1)
            {
                if(hallsectordata->Hall_C == 1) //Sector5: 0,1,1
                {
                    hallsectordata->Hall_Sector = 5;
                    hallsectordata->Theta_out = 341;
                }
                else //Sector4: 0,1,0
                {
                    hallsectordata->Hall_Sector = 4;
                    hallsectordata->Theta_out =256;
                }
            }
            else //Sector6: 0,0,1
            {
                hallsectordata->Hall_Sector = 6;
                hallsectordata->Theta_out = 427;
            }
        }// else: Hall_A == 0)
        //hallsectordata->Hall_flag = 1; // Will be reset by another function
    } // if ((Hall_A != Hall_A_last)||(Hall_C != Hall_C_last)||(Hall_C != Hall_C_last))
    // no else
    hallsectordata->Hall_A_last = hallsectordata->Hall_A;
    hallsectordata->Hall_B_last = hallsectordata->Hall_B;
    hallsectordata->Hall_C_last = hallsectordata->Hall_C;
    */
}

