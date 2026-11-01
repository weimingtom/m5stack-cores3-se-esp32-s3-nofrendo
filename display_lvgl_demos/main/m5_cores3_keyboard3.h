#ifndef M5_CORES3_KEYBOARD 
#define M5_CORES3_KEYBOARD

#include <stdint.h>
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t nes_input_state;
extern void app_i2c_keyboard_init(i2c_master_bus_handle_t bus);


#ifdef __cplusplus
}
#endif

#endif
