/*
 * inverter_state.h
 *
 *  Created on: 2024年7月25日
 *      Author: peter
 */

#ifndef INVERTER_STATE_H_
#define INVERTER_STATE_H_

#include <stdint.h>
#include <stdbool.h>

void control(uint16_t ctrl);
uint16_t get_status(void);

// state
void enable_inverter(void);
void disable_inverter(void);
void reset_fault(void);
bool there_is_hv(void);
bool there_is_error(void);
bool everything_is_ok(void);


#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#define BIT(x) (1 << (x)) // 1 followed by x zero in binary

enum state {
        SYSTEM_INIT,
        SYSTEM_READY,
        SYSTEM_RUNNING,
        SYSTEM_ERROR,
    };

// // Define system states using macros
// #define _SYSTEM_INIT       0
// #define _SYSTEM_READY      1
// #define _SYSTEM_RUNNING    2
// #define _SYSTEM_ERROR      3

// Global variable to track system state
volatile uint8_t _system_state = SYSTEM_INIT;  

// Macros for setting and checking system state
#define SET_SYSTEM_STATE(state) (_system_state = _SYSTEM_##state)
#define SYSTEM_STATE(state) (_system_state == _SYSTEM_##state)

enum control_bits {
        CTRL_ENABLE = BIT(3), // 0b1000
        CTRL_FAULT_RESET = BIT(5), // 0b100000
        CTRL_DIR = BIT(11), // 0b100000000000
    };

enum status_bits {
        STATUS_READY = BIT(1), // 0b10 = 2
        STATUS_ENABLED = BIT(2), // 0b100 = 4
        STATUS_FAULT = BIT(3), // 0b1000 = 8
        STATUS_HV = BIT(4), // 0b10000 = 16
    };

static uint16_t ctrl_prev = 0;

extern uint16_t EnableFlag;
//extern uint16_t EnableDRV;

#endif /* INVERTER_STATE_H_ */
