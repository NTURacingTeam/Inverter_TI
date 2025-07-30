//#############################################################################
//
// FILE:   sd_fat32.c
//
// TITLE:  SD FATFS Library Example
//
//! \addtogroup driver_example_list
//! <h1> SD FATFS Library Example </h1>
//!
//! This example demonstrates how to use the FATFS library.
//!
//! \b External \b Connections \n
//!  - Connect the SPI signals identifed in the SysConfig to an SD CARD.
//!
//! \b Watch \b Variables \n
//!  - None.
//!
//
//#############################################################################
//
//
// $Copyright:
// Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "string.h"

#include <sdspi/sdspi.h>
#include <sdspi/SDFatFS.h>

uint16_t SDFatFS_config_count = 1;
SDFatFS_Object sdfatfsObject;

SDSPI_Object sdspiObject = {
        .spiHandle = mySDCardSPI_BASE,
        .spiCsGpioIndex = mySDCardCS
};

SDFatFS_Object* SDFatFS_config [] = {&sdfatfsObject};

SDSPI_Handle sdspiHandle = &sdspiObject;


/* String conversion macro */
#define STR_(n)             #n
#define STR(n)              STR_(n)

/* Drive number used for FatFs */
#define DRIVE_NUM           0

#define BUFF_SIZE   21

char inputfile[] = STR(DRIVE_NUM)":input00.txt";
char outputfile[] = STR(DRIVE_NUM)":output00.txt";
#define FILE_NUM_INDEX1 8
#define FILE_NUM_INDEX2 9
const char writeBuffer[BUFF_SIZE] = "This is a write test";
char readBuffer[BUFF_SIZE] = "";
uint16_t bytesWritten = 0;
uint16_t bytesRead = 0;
uint16_t testNumber = 0;

FIL src;
FIL dst;


unsigned int filesize;
FRESULT fresult;

//
// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Board initialization
    //
    Board_init();

    SDFatFS_init();
    SDFatFS_Handle sdFatFs_handle = SDFatFS_open(sdspiHandle, DRIVE_NUM);
    if (sdFatFs_handle == NULL)
    {
        ESTOP0;
        while(1);
    }

    for (testNumber = 0; testNumber < 100; testNumber++)
    {
        outputfile[FILE_NUM_INDEX1] = testNumber/10 + '0';
        outputfile[FILE_NUM_INDEX2] = testNumber%10 + '0';
        /* Create a new file object for the file copy */
        fresult = f_open(&dst, outputfile, FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
        if (fresult != FR_OK) {
            ESTOP0;
            while(1);
        }

        fresult = f_write(&dst, writeBuffer, strlen(writeBuffer), &bytesWritten);
        if (fresult != FR_OK) {
            ESTOP0;
            while(1);
        }

        if (bytesWritten != (BUFF_SIZE - 1))
        {
            ESTOP0;
            while(1);
        }

        fresult = f_sync(&dst);
        if (fresult != FR_OK) {
            ESTOP0;
            while(1);
        }

        fresult = f_lseek(&dst, 0);
        if (fresult != FR_OK) {
            ESTOP0;
            while(1);
        }

        fresult = f_read(&dst, readBuffer, BUFF_SIZE, &bytesRead);
        if (fresult != FR_OK) {
            ESTOP0;
            while(1);
        }

        if (bytesRead != (BUFF_SIZE - 1))
        {
            ESTOP0;
            while(1);
        }

        if (strcmp(readBuffer, writeBuffer) != 0)
        {
            ESTOP0;
            while(1);
        }

        fresult = f_sync(&dst);
        if (fresult != FR_OK) {
            ESTOP0;
            while(1);
        }
        /* Get the filesize of the source file */
        filesize = f_size(&src);

        /* Close outputfile[] */
        fresult = f_close(&dst);
        if (fresult != FR_OK) {
            ESTOP0;
            while(1);
        }
    }

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;


    SDFatFS_close(sdFatFs_handle);
    ESTOP0;
}


int32_t fatfs_getFatTime(void)
{
    return 0;
}
//
// End File
//
