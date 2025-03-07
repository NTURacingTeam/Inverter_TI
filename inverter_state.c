// inverter_control.c

#include "inverter_state.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

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

static enum state state = STATE_INIT;

static uint16_t ctrl_prev = 0;

extern uint16_t EnableFlag;
//extern uint16_t EnableDRV;

void control(uint16_t ctrl) {
    // enable or disable the inverter from the command received
    if ((ctrl & CTRL_ENABLE) && (state == STATE_READY)) {
        enable_inverter();
        state = STATE_RUNNING;
    } else if (!(ctrl & CTRL_ENABLE) && (state == STATE_RUNNING)) {
        disable_inverter();
        state = STATE_READY;
    }

    // fault reset
    if ((ctrl_prev & CTRL_FAULT_RESET) && !(ctrl & CTRL_FAULT_RESET) && state == STATE_ERROR) {
        reset_fault();
        state = STATE_READY;
    }
    ctrl_prev = ctrl;
}


uint16_t get_status() {
  uint16_t status = 0;

  if (there_is_hv()) {
    status |= STATUS_HV;
  }

  if (there_is_error()) {
    state = STATE_ERROR;
    status |= STATUS_FAULT;

    return status;
  }

  if(everything_is_ok() && state == STATE_INIT) {
    state = STATE_READY;
  }

  switch(state) {
    case STATE_READY:
      status |= STATUS_READY;
      break;
    case STATE_RUNNING:
      status |= STATUS_ENABLED;
      break;
    default:
      break;
  }

  return status;
}



// Function implementations
void enable_inverter(void) {
    // Code to enable the inverter
    EnableFlag = 1;
//    EnableDRV = 1;
}



void disable_inverter(void) {
    // Code to disable the inverter
    EnableFlag = FALSE;
}

void reset_fault(void) {
    // Code to reset fault
    disable_inverter();
}

bool there_is_hv(void) {
    // Code to check if there is high voltage
   // if(motor1.voltageDC >60)
    return true; // Placeholder

}

bool there_is_error(void) {
    // Code to check if there is an error
    return false; // Placeholder
}

bool everything_is_ok(void) {
    // Code to check if everything is okay
    return true; // Placeholder
}
