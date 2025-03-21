
MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */

   BEGIN           	: origin = 0x000000, length = 0x000002
   RAMM0           	: origin = 0x000122, length = 0x0002DE
//;   RAMD0           	: origin = 0x00B000, length = 0x000800
   RAMLS0LS1LS2LS3LS4LS5     : origin = 0x008000, length = 0x003000
   /* Modified for IQ_Math Table */


   RAMGS456789		: origin = 0x010000, length = 0x006000
//;   RAMLS0          	: origin = 0x008000, length = 0x000800
//;   RAMLS1          	: origin = 0x008800, length = 0x000800
//;   RAMLS2      		: origin = 0x009000, length = 0x000800
//;   RAMLS3      		: origin = 0x009800, length = 0x000800
//;   RAMLS4      		: origin = 0x00A000, length = 0x000800
   RESET           	: origin = 0x3FFFC0, length = 0x000002

PAGE 1 :

   BOOT_RSVD       : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
//;   RAMD1           : origin = 0x00B800, length = 0x000800

   RAMD0D1     : origin = 0x00B000, length = 0x001000

//;   RAMLS5      : origin = 0x00A800, length = 0x000800
   RAMGS0GS1      : origin = 0x00C000, length = 0x002000

//;   RAMGS0      : origin = 0x00C000, length = 0x001000
//;   RAMGS1      : origin = 0x00D000, length = 0x001000
   RAMGS2      : origin = 0x00E000, length = 0x001000
   RAMGS3      : origin = 0x00F000, length = 0x001000
/*   RAMGS4      : origin = 0x010000, length = 0x001000
   RAMGS5      : origin = 0x011000, length = 0x001000
   RAMGS6      : origin = 0x012000, length = 0x001000
   RAMGS7      : origin = 0x013000, length = 0x001000
   RAMGS8      : origin = 0x014000, length = 0x001000
   RAMGS9      : origin = 0x015000, length = 0x001000
*/   RAMGS10     : origin = 0x016000, length = 0x001000
   RAMGS11     : origin = 0x017000, length = 0x001000
}


SECTIONS
{
   codestart        : > BEGIN,     PAGE = 0
   ramfuncs         : > RAMM0      PAGE = 0
   .text            : >>RAMLS0LS1LS2LS3LS4LS5	| RAMGS456789,   PAGE = 0
   .cinit           : > RAMGS456789,     PAGE = 0
   .pinit           : > RAMM0,     PAGE = 0
   .switch          : > RAMM0,     PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */

   .stack           : > RAMM1,     PAGE = 1
   .ebss            : > RAMD0D1|RAMGS0GS1,     PAGE = 1
   .econst          : > RAMD0D1|RAMGS2,     PAGE = 1
   .esysmem         : > RAMD0D1,     PAGE = 1
   Filter_RegsFile  : > RAMGS0GS1,	   PAGE = 1

   /* Allocate IQ math areas: */
    IQmath			: > RAMLS0LS1LS2LS3LS4LS5 ,     PAGE = 0            /* Math Code */
	IQmathTables	: > RAMGS456789, PAGE = 0
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
