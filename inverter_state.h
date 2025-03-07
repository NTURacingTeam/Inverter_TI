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


#endif /* INVERTER_STATE_H_ */
