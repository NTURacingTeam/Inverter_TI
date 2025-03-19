/*
 * can_inv.h
 *
 *  Created on: 2024��蕭7嚙踝蕭嚙�27嚙踐��
 *      Author: peter
 */

#ifndef CAN_INV_H_
#define CAN_INV_H_

#include <stdint.h>
#include "F28x_Project.h"

enum MotorLocate{
    MOTOR_DEBUG = 0,
    MOTOR_FL    = 1,
    MOTOR_FR    = 2,
    MOTOR_RL    = 3,
    MOTOR_RR    = 4,
};


// Function prototypes
void InvInitCan(uint16_t _inverterX);
void ReceiveCanControl();
void SendCanStatus(uint16_t status, float32 torque);
void SendCanState(int16_t rpm, float32 torque_fb, float32 voltage, float32 current);
void SendCanTemperature(uint16_t invTemp, uint16_t motorTemp);
void SendCanHeartbeat();
void SendCanHFCurrentLog(uint16_t currentA, uint16_t currentB, uint16_t currentC, uint16_t currentDC);

#endif // CAN_INV_H_
