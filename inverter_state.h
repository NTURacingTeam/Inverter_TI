/*
 * inverter_state.h
 *
 *  Created on: 2024撟�7���25�
 *      Author: peter
 */

#ifndef INVERTER_STATE_H_
#define INVERTER_STATE_H_

#include <stdint.h>
#include <stdbool.h>

#define BIT(x) (1 << (x)) // 1 followed by x zero in binary

enum state {
    STATE_INIT,
    STATE_READY,
    STATE_RUNNING,
    STATE_ERROR,
};

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

void control(uint16_t ctrl);
uint16_t get_status(void);

// state
void enable_inverter(void);
void disable_inverter(void);
void reset_fault(void);
bool there_is_hv(void);
bool there_is_error(void);
bool everything_is_ok(void);


#endif /* INVERTER_STATE_H_ */
