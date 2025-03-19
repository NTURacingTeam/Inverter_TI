/*
 * can_inv.c
 *
 *  Created on: 2024/07/30
 *      Author: peter
 */
#include "can_inv.h"
#include "can.h"
#include "inc/hw_memmap.h"
#include "motorVars.h"
#include "MonoMtrServo-Settings.h"
#include "systick.h"
#include "inverter_state.h"


// extern volatile struct CAN_REGS CanaRegs;



#define TICK_PER_MS 200000 // systick : 200,000 tick / ms
#define CLEAR_TX_BUF do{memset(txData, 0, 8)}while(0)
#define TX_MIN_PERIOD 1 // ms
#define MIN_PERIOD_CHECK do{if(last_can_tx - SysTickValueGet() >= TICK_PER_MS * TX_MIN_PERIOD){}else{return;}}while(0)


// CAN message object structure
static tCANMsgObject canMessage, canRxMessage;
static unsigned char txData[8] = {0};
static unsigned char rxData[8] = {0};
static uint16_t inverterX;
static uint32_t last_can_tx;


static void InitCanGpio() {
    EALLOW;
    GPIO_SetupPinMux(70, GPIO_MUX_CPU1, 5); // GPIO70 as CANA_RX
    GPIO_SetupPinMux(71, GPIO_MUX_CPU1, 5); // GPIO71 as CANA_TX
    EDIS;
}

static void CanRxRequest() {
    canRxMessage.ui32MsgID = 0x210 + inverterX; // Control command ID
    canRxMessage.ui32MsgIDMask = 0x1FFFFFFF;
    canRxMessage.ui32Flags = MSG_OBJ_FIFO | MSG_OBJ_USE_ID_FILTER;
    canRxMessage.ui32MsgLen = sizeof(rxData);
    canRxMessage.pucMsgData = rxData;
    CANMessageSet(CANA_BASE, 1, &canRxMessage, MSG_OBJ_TYPE_RX);
}

// CAN bit timing parameters
tCANBitClkParms canBitClk = {
    .uSyncPropPhase1Seg = 14,
    .uPhase2Seg = 5,
    .uSJW = 1,
    .uQuantumPrescaler = 10
};

void InvInitCan(uint16_t _inverterX) {
    inverterX = _inverterX;
    InitCanGpio();
    // Initialize CAN-A
    CANInit(CANA_BASE);
    // Set up CAN bit timing
    CANBitTimingSet(CANA_BASE, &canBitClk);
    CANRetrySet(CANA_BASE, false);
    // Enable CAN module
    CANEnable(CANA_BASE);
    CanRxRequest();
}


extern float32 torqueCMD;

void ReceiveCanControl() {
    // Get the message object from the CAN controller
    // if(CANStatusGet(CANA_BASE, CAN_STS_NEWDAT) & 2) {
    CANMessageGet(CANA_BASE, 1, &canRxMessage, true);
    // Process the received control command
    uint16_t ctrl = ((uint16_t)rxData[1] << 8) | rxData[0];
    int16_t torqueCmd = ((uint16_t)rxData[3] << 8) | rxData[2];
    // Implement your control command processing here
    torqueCMD = (float32) torqueCmd / Kt / BASE_CURRENT / 1000;
    control(ctrl);
}

void SendCanStatus(uint16_t status, float32 torque) {
    static uint32_t last_send;
    const uint32_t send_period = TICK_PER_MS * 10;
    MIN_PERIOD_CHECK;
    if(last_send - SysTickValueGet() >= send_period) {
        last_send = last_can_tx = SysTickValueGet();
        // Prepare the status data
        int16_t torqueDemand = torque * 1000 * Kt * BASE_CURRENT;
        txData[1] = (status >> 8) & 0xFF;
        txData[0] = status & 0xFF;
        txData[3] = (torqueDemand >> 8) & 0xFF;
        txData[2] = torqueDemand & 0xFF;

        // Set up a transmit message object
        canMessage.ui32MsgID = 0x190 + inverterX; // Status ID
        canMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
        canMessage.ui32MsgLen = 4;
        canMessage.pucMsgData = txData;

        // Load the message object into the CAN controller
        CANMessageSet(CANA_BASE, 2, &canMessage, MSG_OBJ_TYPE_TX);
    }
}

void SendCanState(int16_t rpm, float32 torque_fb, float32 voltage, float32 current) {
    static uint32_t last_send;
    const uint32_t send_period = TICK_PER_MS * 10;
    MIN_PERIOD_CHECK;
    if(last_send - SysTickValueGet() >= send_period) {
        last_send = last_can_tx = SysTickValueGet();
        // Prepare the temperature data
        txData[1] = (rpm >> 8) & 0xFF;
        txData[0] = rpm & 0xFF;
        int16_t torqueFB = torque_fb * 1000 * Kt * BASE_CURRENT;
        txData[3] = (torqueFB >> 8) & 0xFF;
        txData[2] = torqueFB & 0xFF;
        uint16_t DCvoltage = voltage *100 * BASE_VOLTAGE;
        txData[5] = (DCvoltage >> 8) & 0xFF;
        txData[4] = DCvoltage & 0xFF;
        int16_t DCcurrent = current * 100 * BASE_CURRENT;
        txData[7] = (DCcurrent >> 8) & 0xFF;
        txData[6] = DCcurrent & 0xFF;

        // Set up a transmit message object
        canMessage.ui32MsgID = 0x290 + inverterX; // Temperature ID
        canMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
        canMessage.ui32MsgLen = 8;
        canMessage.pucMsgData = txData;

        // Load the message object into the CAN controller
        CANMessageSet(CANA_BASE, 3, &canMessage, MSG_OBJ_TYPE_TX);
    }
}

void SendCanTemperature(uint16_t invTemp, uint16_t motorTemp) {
    static uint32_t last_send;
    const uint32_t send_period = TICK_PER_MS * 100;
    MIN_PERIOD_CHECK;
    if(last_send - SysTickValueGet() >= send_period) {
        last_send = last_can_tx = SysTickValueGet();
        // Prepare the state data
        invTemp = invTemp << 1;
        txData[1] = (invTemp >> 8) & 0xFF;
        txData[0] = invTemp & 0xFF;
        motorTemp = motorTemp << 1;
        txData[3] = (motorTemp >> 8) & 0xFF;
        txData[2] = motorTemp & 0xFF;

        // Set up a transmit message object
        canMessage.ui32MsgID = 0x390 + inverterX; // State ID
        canMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
        canMessage.ui32MsgLen = 4;
        canMessage.pucMsgData = txData;

        // Load the message object into the CAN controller
        CANMessageSet(CANA_BASE, 4, &canMessage, MSG_OBJ_TYPE_TX);
    }
}

void SendCanHeartbeat() {
    static uint32_t last_send;
    const uint32_t send_period = TICK_PER_MS * 100;
    MIN_PERIOD_CHECK;
    if(last_send - SysTickValueGet() >= send_period) {
        last_send = last_can_tx = SysTickValueGet();
        // Prepare the heartbeat data
        txData[0] = 0x7F;
        // Set up a transmit message object
        canMessage.ui32MsgID = 0x710 + inverterX; // Heartbeat ID
        canMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
        canMessage.ui32MsgLen = 1;
        canMessage.pucMsgData = txData;
        // Load the message object into the CAN controller
        CANMessageSet(CANA_BASE, 5, &canMessage, MSG_OBJ_TYPE_TX);
    }
}

void SendCanHFCurrentLog(uint16_t currentA, uint16_t currentB, uint16_t currentC, uint16_t currentDC) {
    static uint32_t last_send;
    const uint32_t send_period = TICK_PER_MS * 100;
    MIN_PERIOD_CHECK;
    if(last_send - SysTickValueGet() >= send_period) {
        last_send = last_can_tx = SysTickValueGet();
        // Prepare the temperature data
        txData[1] = (currentA >> 8) & 0xFF;
        txData[0] = currentA& 0xFF;
        txData[3] = (currentB >> 8) & 0xFF;
        txData[2] = currentB & 0xFF;
        txData[5] = (currentC >> 8) & 0xFF;
        txData[4] = currentC & 0xFF;
        txData[7] = (currentDC >> 8) & 0xFF;
        txData[6] = currentDC & 0xFF;

        // Set up a transmit message object
        canMessage.ui32MsgID = 0x610 + inverterX; // Temperature ID
        canMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
        canMessage.ui32MsgLen = 8;
        canMessage.pucMsgData = txData;

        // Load the message object into the CAN controller
        CANMessageSet(CANA_BASE, 3, &canMessage, MSG_OBJ_TYPE_TX);
    }
}


uint32_t can_new_data = 0;

void CanTestReceive() {
    uint16_t new_data_stat = CANStatusGet(CANA_BASE, CAN_STS_NEWDAT);
    if(new_data_stat) {
        can_new_data ++;
        CANMessageGet(CANA_BASE, 2, &canMessage, true);

        canMessage.ui32MsgID = 0x710; // Control command ID
        canMessage.ui32MsgIDMask = 0;
        canMessage.ui32Flags = MSG_OBJ_FIFO | MSG_OBJ_USE_ID_FILTER;
        canMessage.ui32MsgLen = sizeof(rxData);
        canMessage.pucMsgData = rxData;
        CANMessageSet(CANA_BASE, 2, &canMessage, MSG_OBJ_TYPE_RX);
    }
}



uint32_t can_debug_var = 0;

void CanTestSend() {
    can_debug_var ++;
    int i = 0;
    for(; i < 8; i++) {
        txData[i] = i;
    }
    canMessage.ui32MsgID = 0x190; // Status ID
    canMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
    canMessage.ui32MsgLen = 8;
    canMessage.pucMsgData = txData;
    CANMessageSet(CANA_BASE, 1, &canMessage, MSG_OBJ_TYPE_TX);
}


bool can_it_status = false;

__interrupt void canISR(void)
{
    uint32_t status = CANIntStatus(CANA_BASE, CAN_INT_STS_CAUSE);
    can_it_status = ! can_it_status;
    if (status == 2)
    {
        // Message object 2 has new data
        ReceiveCanControl();
    }

    // Clear the interrupt
    CANIntClear(CANA_BASE, status);

    // Acknowledge the interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}





void InvInitCanInterrupts()
{
    EALLOW;
    PieVectTable.CANA0_INT = &canISR;
    PieVectTable.CANA1_INT = &canISR;
    EDIS;

    PieCtrlRegs.PIEIER9.bit.INTx5 = 1; // Enable CAN interrupt in PIE group 9
    PieCtrlRegs.PIEIER9.bit.INTx6 = 1; // Enable CAN interrupt in PIE group 9
    IER |= M_INT9; // Enable CPU INT9
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global real-time interrupt DBGM

    CANIntEnable(CANA_BASE, CAN_INT_IE0 | CAN_INT_STATUS | CAN_INT_ERROR | CAN_INT_IE1);

    // Set up a receive message object for control commands
    canMessage.ui32MsgID = 0x710; // Control command ID
    canMessage.ui32MsgIDMask = 0x1FFFFFFF;
    canMessage.ui32MsgIDMask = 0;
    canMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    canMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER | MSG_OBJ_EXTENDED_ID;
    canMessage.ui32MsgLen = sizeof(rxData);
    canMessage.pucMsgData = rxData;
    CANMessageSet(CANA_BASE, 2, &canMessage, MSG_OBJ_TYPE_RX);
}
