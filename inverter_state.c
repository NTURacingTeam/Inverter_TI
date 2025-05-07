// inverter_control.c

#include "inverter_state.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


void control(uint16_t ctrl) {
    // enable or disable the inverter from the command received
    if ((ctrl & CTRL_ENABLE) && (SYSTEM_STATE(READY))) {
        enable_inverter();
        SET_SYSTEM_STATE(RUNNING);
    } else if (!(ctrl & CTRL_ENABLE) && (SYSTEM_STATE(RUNNING))) {
        disable_inverter();
        SET_SYSTEM_STATE(READY);
    }

    // fault reset
    if ((ctrl_prev & CTRL_FAULT_RESET) && !(ctrl & CTRL_FAULT_RESET) && SYSTEM_STATE(ERROR)) {
        reset_fault();
        SET_SYSTEM_STATE(READY);
    }
    ctrl_prev = ctrl;
}


uint16_t get_status() {
  uint16_t status = 0;

  if (there_is_hv()) {
    status |= STATUS_HV;
  }

  if (there_is_error()) {
    SET_SYSTEM_STATE(ERROR);
    status |= STATUS_FAULT;

    return status;
  }

  if(everything_is_ok() && SYSTEM_STATE(INIT)) {
    SET_SYSTEM_STATE(STATE_READY);
  }

  switch(_system_state) {
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
   SET_SYSTEM_STATE(ERROR);
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
