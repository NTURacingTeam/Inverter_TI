//###########################################################################
//
// FILE:   can.h
//
// TITLE:  Defines and Macros for the CAN controller.
//
//###########################################################################
// $TI Release: F2837xS Support Library v210 $
// $Release Date: Tue Nov  1 15:35:23 CDT 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef __CAN_H__
#define __CAN_H__
//*****************************************************************************
//! \addtogroup can_api
//! @{
//*****************************************************************************

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


#define CAN_INDEX_TO_BASE(idx)             ((idx == 0) ? CAN_A_BASE : CAN_B_BASE)

#define CAN_INDEX_TO_MSG_RAM_BASE(idx)     ((idx == 0) ? CAN_A_MSG_RAM : CAN_B_MSG_RAM)

#define CAN_REG_WORD_MASK                  (0xFFFFU)

//****************************************************************************
//   These are the Defines to select CAN pin muxing when calling the functions
//  ConfigCanPinMuxing(), ConfigGpioCanA() & ConfigGpioCanB() in F2837x_Can.c
//****************************************************************************
#define CAN_A_GPIO4_GPIO5       1   //switch case 1
#define CAN_A_GPIO19_GPIO18     2   //switch case 2
#define CAN_A_GPIO31_GPIO30     3   //switch case 3
#define CAN_A_GPIO37_GPIO36     4   //switch case 4
#define CAN_A_GPIO63_GPIO62     5   //switch case 5
#define CAN_A_GPIO71_GPIO70     6   //switch case 6

#define CAN_B_GPIO6_GPIO7       1   //switch case 1
#define CAN_B_GPIO8_GPIO10      2   //switch case 2
#define CAN_B_GPIO12_GPIO13     3   //switch case 3
#define CAN_B_GPIO16_GPIO17     4   //switch case 4
#define CAN_B_GPIO20_GPIO21     5   //switch case 5
#define CAN_B_GPIO38_GPIO39     6   //switch case 6
#define CAN_B_GPIO72_GPIO73     7   //switch case 7

//*****************************************************************************
// Miscellaneous defines for Message ID Types
//*****************************************************************************

//*****************************************************************************
// These are the flags used by the tCANMsgObject.ui32Flags value when calling the
// CANMessageSet() and CANMessageGet() functions.
//*****************************************************************************

//! This definition is used with the tCANMsgObject ui32Flags value and indicates
//! that transmit interrupts should be enabled, or are enabled.
#define MSG_OBJ_TX_INT_ENABLE           0x00000001

//! This indicates that receive interrupts should be enabled, or are
//! enabled.
#define MSG_OBJ_RX_INT_ENABLE           0x00000002

//! This indicates that a message object will use or is using an extended
//! identifier.
#define MSG_OBJ_EXTENDED_ID             0x00000004

//! This indicates that a message object will use or is using filtering
//! based on the object's message identifier.
#define MSG_OBJ_USE_ID_FILTER           0x00000008

//! This indicates that new data was available in the message object.
#define MSG_OBJ_NEW_DATA                0x00000080

//! This indicates that data was lost since this message object was last
//! read.
#define MSG_OBJ_DATA_LOST               0x00000100

//! This indicates that a message object will use or is using filtering
//! based on the direction of the transfer.  If the direction filtering is
//! used, then ID filtering must also be enabled.
#define MSG_OBJ_USE_DIR_FILTER          (0x00000010 | MSG_OBJ_USE_ID_FILTER)

//! This indicates that a message object will use or is using message
//! identifier filtering based on the extended identifier.  If the extended
//! identifier filtering is used, then ID filtering must also be enabled.
#define MSG_OBJ_USE_EXT_FILTER          (0x00000020 | MSG_OBJ_USE_ID_FILTER)

//! This indicates that a message object is a remote frame.
#define MSG_OBJ_REMOTE_FRAME            0x00000040

//! This indicates that this message object is part of a FIFO structure and
//! not the final message object in a FIFO.
#define MSG_OBJ_FIFO                    0x00000200

//! This indicates that a message object has no flags set.
#define MSG_OBJ_NO_FLAGS                0x00000000

//*****************************************************************************
//! This define is used with the flag values to allow checking only status
//! flags and not configuration flags.
//*****************************************************************************
#define MSG_OBJ_STATUS_MASK             (MSG_OBJ_NEW_DATA | MSG_OBJ_DATA_LOST)

//*****************************************************************************
//! The structure used for encapsulating all the items associated with a CAN
//! message object in the CAN controller.
//*****************************************************************************
typedef struct
{
        //! The CAN message identifier used for 11 or 29 bit identifiers.
        uint32_t ui32MsgID;

        //! The message identifier mask used when identifier filtering is enabled.
        uint32_t ui32MsgIDMask;

        //! This value holds various status flags and settings specified by
        //! tCANObjFlags.
        uint32_t ui32Flags;

        //! This value is the number of bytes of data in the message object.
        uint32_t ui32MsgLen;

        //! This is a pointer to the message object's data.
        unsigned char *pucMsgData;
}
tCANMsgObject;

//*****************************************************************************
//! This structure is used for encapsulating the values associated with setting
//! up the bit timing for a CAN controller.  The structure is used when calling
//! the CANGetBitTiming and CANSetBitTiming functions.
//*****************************************************************************
typedef struct
{
        //! This value holds the sum of the Synchronization, Propagation, and Phase
        //! Buffer 1 segments, measured in time quanta.  The valid values for this
        //! setting range from 2 to 16.
        uint16_t  uSyncPropPhase1Seg;

        //! This value holds the Phase Buffer 2 segment in time quanta.  The valid
        //! values for this setting range from 1 to 8.
        uint16_t  uPhase2Seg;

        //! This value holds the Resynchronization Jump Width in time quanta.  The
        //! valid values for this setting range from 1 to 4.
        uint16_t  uSJW;

        //! This value holds the CAN_CLK divider used to determine time quanta.
        //! The valid values for this setting range from 1 to 1023.
        uint16_t  uQuantumPrescaler;
}
tCANBitClkParms;

//*****************************************************************************
//! This data type is used to identify the interrupt status register.  This is
//! used when calling the CANIntStatus() function.
//*****************************************************************************
typedef enum
{
        //! Read the CAN interrupt status information.
        CAN_INT_STS_CAUSE,

        //! Read a message object's interrupt status.
        CAN_INT_STS_OBJECT
}
tCANIntStsReg;

//*****************************************************************************
//! This data type is used to identify which of several status registers to
//! read when calling the CANStatusGet() function.
//*****************************************************************************
typedef enum
{
        //! Read the full CAN controller status.
        CAN_STS_CONTROL,

        //! Read the full 32-bit mask of message objects with a transmit request
        //! set.
        CAN_STS_TXREQUEST,

        //! Read the full 32-bit mask of message objects with new data available.
        CAN_STS_NEWDAT,

        //! Read the full 32-bit mask of message objects that are enabled.
        CAN_STS_MSGVAL
}
tCANStsReg;

//*****************************************************************************
// These definitions are used to specify interrupt sources to CANIntEnable()
// and CANIntDisable().
//*****************************************************************************
//! This flag is used to allow a CAN controller to generate error
//! interrupts.
#define CAN_INT_ERROR           0x00000008

//! This flag is used to allow a CAN controller to generate status
//! interrupts.
#define CAN_INT_STATUS          0x00000004

//! This flag is used to allow a CAN controller to generate interrupts
//! on interrupt line 0
#define CAN_INT_IE0             0x00000002

//! This flag is used to allow a CAN controller to generate interrupts
//! on interrupt line 1
#define CAN_INT_IE1             0x00020000

// Defined to maintain compatibility with Stellaris Examples
#define CAN_INT_MASTER          CAN_INT_IE0

//*****************************************************************************
// These definitions are used to specify the clock source to
// CANClkSourceSelect()
//*****************************************************************************
//! This flag is used to clock the CAN controller Selected CPU SYSCLKOUT
//! (CPU1.Sysclk or CPU2.Sysclk).
#define CAN_CLK_CPU_SYSCLKOUT       0 // PERx.SYSCLK (default on reset)

//! This flag is used to clock the CAN controller with the X1/X2 oscillator
//! clock.
#define CAN_CLK_EXT_OSC             1 // External Oscillator (XTAL)

//! This flag is used to clock the CAN controller with the clock from
//! AUXCLKIN (from GPIO)
#define CAN_CLK_AUXCLKIN            2 // AUXCLKIN (from GPIO)


//*****************************************************************************
//! This definition is used to determine the type of message object that will
//! be set up via a call to the CANMessageSet() API.
//*****************************************************************************
typedef enum
{
        //! Transmit message object.
        MSG_OBJ_TYPE_TX,

        //! Transmit remote request message object
        MSG_OBJ_TYPE_TX_REMOTE,

        //! Receive message object.
        MSG_OBJ_TYPE_RX,

        //! Receive remote request message object.
        MSG_OBJ_TYPE_RX_REMOTE,

        //! Remote frame receive remote, with auto-transmit message object.
        MSG_OBJ_TYPE_RXTX_REMOTE
}
tMsgObjType;

//*****************************************************************************
// The following enumeration contains all error or status indicators that can
// be returned when calling the CANStatusGet() function.
//*****************************************************************************
//! CAN controller is in local power down mode.
#define CAN_STATUS_PDA                0x00000400

//! CAN controller has initiated a system wakeup.
#define CAN_STATUS_WAKE_UP            0x00000200

//! CAN controller has detected a parity error.
#define CAN_STATUS_PERR               0x00000100

//! CAN controller has entered a Bus Off state.
#define CAN_STATUS_BUS_OFF            0x00000080

//! CAN controller error level has reached warning level.
#define CAN_STATUS_EWARN              0x00000040

//! CAN controller error level has reached error passive level.
#define CAN_STATUS_EPASS              0x00000020

//! A message was received successfully since the last read of this status.
#define CAN_STATUS_RXOK               0x00000010

//! A message was transmitted successfully since the last read of this
//! status.
#define CAN_STATUS_TXOK               0x00000008

//! This is the mask for the last error code field.
#define CAN_STATUS_LEC_MSK            0x00000007

//! There was no error.
#define CAN_STATUS_LEC_NONE           0x00000000

//! A bit stuffing error has occurred.
#define CAN_STATUS_LEC_STUFF          0x00000001

//! A formatting error has occurred.
#define CAN_STATUS_LEC_FORM           0x00000002

//! An acknowledge error has occurred.
#define CAN_STATUS_LEC_ACK            0x00000003

//! The bus remained a bit level of 1 for longer than is allowed.
#define CAN_STATUS_LEC_BIT1           0x00000004

//! The bus remained a bit level of 0 for longer than is allowed.
#define CAN_STATUS_LEC_BIT0           0x00000005

//! A CRC error has occurred.
#define CAN_STATUS_LEC_CRC            0x00000006

//*****************************************************************************
// The following macros are added for the new Global Interrupt EN/FLG/CLR
// register
//*****************************************************************************
//CANINT0 global interrupt bit
#define CAN_GLOBAL_INT_CANINT0       0x00000001

//CANINT1 global interrupt bit
#define CAN_GLOBAL_INT_CANINT1       0x00000002

//*****************************************************************************
// The following macros are missing in hw_can.h because of scripting
// but driverlib can.c needs them
//*****************************************************************************

#define CAN_INT_INT0ID_STATUS       0x8000

#define CAN_IF1ARB_STD_ID_S         18
#define CAN_IF1ARB_STD_ID_M         0x1FFC0000 // Standard Message Identifier

#define CAN_IF2ARB_STD_ID_S         18
#define CAN_IF2ARB_STD_ID_M         0x1FFC0000 // Standard Message Identifier

//*****************************************************************************
// API Function prototypes
//*****************************************************************************
extern void CANClkSourceSelect(uint32_t ui32Base, uint16_t  ucSource);
extern void CANBitTimingGet(uint32_t ui32Base, tCANBitClkParms *pClkParms);
extern void CANBitTimingSet(uint32_t ui32Base, tCANBitClkParms *pClkParms);
extern uint32_t CANBitRateSet(uint32_t ui32Base, uint32_t ui32SourceClock,
                              uint32_t ui32BitRate);
extern void CANDisable(uint32_t ui32Base);
extern void CANEnable(uint32_t ui32Base);
extern bool CANErrCntrGet(uint32_t ui32Base, uint32_t *pui32RxCount,
                          uint32_t *pui32TxCount);
extern void CANInit(uint32_t ui32Base);
extern void CANIntClear(uint32_t ui32Base, uint32_t ui32IntClr);
extern void CANIntDisable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void CANIntEnable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void CANIntRegister(uint32_t ui32Base, unsigned char ucIntNumber,
                           void (*pfnHandler)(void));
extern uint32_t CANIntStatus(uint32_t ui32Base, tCANIntStsReg eIntStsReg);
extern void CANIntUnregister(uint32_t ui32Base, unsigned char ucIntNumber);
extern void CANMessageClear(uint32_t ui32Base, uint32_t ui32ObjID);
extern void CANMessageGet(uint32_t ui32Base, uint32_t ui32ObjID,
                          tCANMsgObject *pMsgObject, bool bClrPendingInt);
extern void CANMessageSet(uint32_t ui32Base, uint32_t ui32ObjID,
                          tCANMsgObject *pMsgObject, tMsgObjType eMsgType);
extern bool CANRetryGet(uint32_t ui32Base);
extern void CANRetrySet(uint32_t ui32Base, bool bAutoRetry);
extern uint32_t CANStatusGet(uint32_t ui32Base, tCANStsReg eStatusReg);
extern void CANGlobalIntEnable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void CANGlobalIntDisable(uint32_t ui32Base, uint32_t ui32IntFlags);
extern void CANGlobalIntClear(uint32_t ui32Base, uint32_t ui32IntFlags);
extern bool CANGlobalIntstatusGet(uint32_t ui32Base, uint32_t ui32IntFlags);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
// Close the Doxygen group.
//! @}
//*****************************************************************************

#endif //  __CAN_H__


