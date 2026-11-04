#include <M5Faces.h>
//#include "M5FacesBase.hpp"

#include "m5_cores3_keyboard3.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"

extern "C" {
#include "osd.h" //osd_getinput
//extern void osd_getinput(void);

//===========FIXME:added
//from osd.h
typedef struct
{    
    bool    btn_select:1;
    bool    btn_start:1;
    bool    btn_up:1;
    bool    btn_down:1;
    bool    btn_right:1;
    bool    btn_left:1;

    bool    btn_a:1;
    bool    btn_b:1;
} gamepad_t;
extern /*static*/ gamepad_t app_gamepad;
//===========
}

uint32_t nes_input_state = 0xFFFFFFFF;  // All buttons released

//------------------------------

static const char* kTag = "faces3_keyboard3";

constexpr uint32_t kI2CFreq = 400000;
//#if defined(M5FACES_HOST_CORES3)
constexpr int kFacesSda = 12;
constexpr int kFacesScl = 11;
//#else
//constexpr int kFacesSda = 21;
//constexpr int kFacesScl = 22;
//#endif
constexpr uint8_t kAppAddress  = M5FACES_BOTTOM3_ADDR;

//#if defined(M5FACES_HOST_CORE2) || defined(M5FACES_HOST_CORES3)
	constexpr i2c_port_t faces_i2c_port = I2C_NUM_1;
//#else
//	constexpr i2c_port_t faces_i2c_port = I2C_NUM_0;
//#endif

static inline void delay(uint32_t milliseconds)
{
   vTaskDelay(pdMS_TO_TICKS(milliseconds));
}

void uint32_to_bin(uint32_t val, char *buf, bool use8) {
    for (int i = use8 ? 9 : 31; i >= 0; i--) {
        *buf++ = (val & (1u << i)) ? '1' : '0';
    }
    *buf = '\0';
}

constexpr bool USE_DIRECT = true;
static i2c_master_bus_handle_t _i2c_master_bus;
static M5Faces_Keyboard3 keyboard;
static M5FacesPoller poller;
static char old_label[256] = {0};
int old_row = -1;
int old_bit = -1;
//static uint32_t nes_input_state = 0xFFFFFFFF;

void direct_event_callback(m5faces_direct_event_t evt,
						   const m5faces_direct_data_t* data,
						   uint32_t hold_cnt, void* ctx) {
	// handle Direct mode event
	//see direct_event_callback
			
			
	// K(1,2) = Up button
	// M(2,2) = Down button
	// N(2,3) = Left button
	// $(2,1) = Right button
	// W(0,8) = Space = Select button
	// E(0,7) = Enter = Start button
	// X(2,7) = A button
	// Z(2,8) = B button
			
	const char* label = 0;
	int label_row = -1; 
	int label_bit = -1;
	bool current_pressed[3][16] = {false};
	if (data) {
		for (int row = 0; row < 3; ++row) {
			for (int bit = 0; bit < 10; ++bit) {
				if ((data->rows[row].key_bits & (1U << bit)) == 0U) { // 0 is pressed
					current_pressed[row][bit] = true;
				}
			}
		}				
		if (current_pressed[1][2] == true) {  // Up
			nes_input_state &= ~(1UL << 0);
		} else {
			nes_input_state |= (1UL << 0);
		}
		if (current_pressed[2][2] == true) {  // Down
			nes_input_state &= ~(1UL << 1);
		} else {
			nes_input_state |= (1UL << 1);
		}
		if (current_pressed[2][3] == true) {  // Left
			nes_input_state &= ~(1UL << 2);
		} else {
			nes_input_state |= (1UL << 2);
		}
		if (current_pressed[2][1] == true) {  // Right
			nes_input_state &= ~(1UL << 3);
		} else {
			nes_input_state |= (1UL << 3);
		}
		// Control keys
		if (current_pressed[0][8] == true) {  // Space = Select
			nes_input_state &= ~(1UL << 4);
		} else {
			nes_input_state |= (1UL << 4);
		}
		if (current_pressed[0][7] == true) {  // Enter = Start
			nes_input_state &= ~(1UL << 5);
		} else {
			nes_input_state |= (1UL << 5);
		}
		// Symbol keys
		if (current_pressed[2][7] == true) {  // X = A button = X
			nes_input_state &= ~(1UL << 6);
		} else {
			nes_input_state |= (1UL << 6);
		}
		if (current_pressed[2][8] == true) {  // Z = B button = Z
			nes_input_state &= ~(1UL << 7);
		} else {
			nes_input_state |= (1UL << 7);
		}
				
		if (evt == M5FACES_DIRECT_PRESSED) {
			strcpy(old_label, "");
			old_row = -1;
			old_bit = -1;
			for (int row = 0; row < 3; ++row) {
				for (int bit = 0; bit < 10; ++bit) {
					if ((data->rows[row].key_bits & (1U << bit)) == 0U) { // 0 is pressed
						label = M5Faces_Keyboard3::keyboard3_key_map(row, bit);
						strcpy(old_label, label);
						old_row = label_row = row;
						old_bit = label_bit = bit;
					}
				}
			}
		} else if (evt == M5FACES_DIRECT_RELEASED) {
			label = old_label;
			label_row = old_row;
			label_bit = old_bit;
		}
	}
			
	char nes_input_state_str[33] = {0};
	uint32_to_bin(nes_input_state, nes_input_state_str, true);
			
	//M5FACES_DIRECT_PRESSED  = 1
	//M5FACES_DIRECT_RELEASED = 2,
	if (evt == M5FACES_DIRECT_PRESSED) {
		ESP_LOGW(kTag, "handle Direct mode event pressed %s, row=%d, bit=%d, state=%s", label ? label : "(null)", label_row, label_bit, nes_input_state_str);
	} else if (evt == M5FACES_DIRECT_RELEASED) {
		ESP_LOGW(kTag, "handle Direct mode event released %s, row=%d, bit=%d, state=%s", label ? label : "(null)", label_row, label_bit, nes_input_state_str);
			
		strcpy(old_label, "");
		old_row = -1;
		old_bit = -1;
	} else {
		ESP_LOGW(kTag, "handle Direct mode event %d, %s, row=%d, bit=%d, state=%s", evt, label ? label : "(null)", label_row, label_bit, nes_input_state_str);
	}
			
	app_gamepad.btn_up = (((nes_input_state >> 0) & 1) ? false : true);
	app_gamepad.btn_down = (((nes_input_state >> 1) & 1) ? false : true);
	app_gamepad.btn_left = (((nes_input_state >> 2) & 1) ? false : true);
	app_gamepad.btn_right = (((nes_input_state >> 3) & 1) ? false : true);

	app_gamepad.btn_select = (((nes_input_state >> 4) & 1) ? false : true);
	app_gamepad.btn_start = (((nes_input_state >> 5) & 1) ? false : true);
	app_gamepad.btn_a = (((nes_input_state >> 6) & 1) ? false : true);
	app_gamepad.btn_b = (((nes_input_state >> 7) & 1) ? false : true);

	//extern void osd_getinput(void);
	osd_getinput();
}
void setup(i2c_master_bus_handle_t bus) {
	if (USE_DIRECT) {
		// Polling mode (no GPIO INT pin)
		poller.setPollingMode(true);
		//  Register callbacks
		poller.setNormalKeyCb([](M5FacesBase* f, uint8_t raw, void* ctx) {
			// handle Normal mode key
			ESP_LOGW(kTag, "handle Normal mode key");
		}, nullptr);
		poller.setDirectEventCb(direct_event_callback, nullptr);
	}

	i2c_master_bus_config_t bus_config = {
        .i2c_port          = faces_i2c_port,
        .sda_io_num        = static_cast<gpio_num_t>(kFacesSda),
        .scl_io_num        = static_cast<gpio_num_t>(kFacesScl),
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority     = 0,
        .trans_queue_depth = 0,
        .flags =
            {
                .enable_internal_pullup = true,
                .allow_pd               = false,
            },
    };

	if (bus != nullptr) {
		_i2c_master_bus = bus;
	} else {
		esp_err_t ret = i2c_new_master_bus(&bus_config, &_i2c_master_bus);
		if (ret != ESP_OK) {
			ESP_LOGE(kTag, "Keyboard3 not found 0, Failed to create I2C master bus.");
			return;
		} else {
			ESP_LOGI(kTag, "Keyboard3 init success 0");
		}
	}
	if (keyboard.begin(_i2c_master_bus, kAppAddress, 100000) != M5FACES_OK) {
		ESP_LOGE(kTag, "Keyboard3 not found 1");
		return;
	} else {
		ESP_LOGI(kTag, "Keyboard3 init success 1");
	}
	if (USE_DIRECT) {
		if (keyboard.setMode(M5FACES_MODE_DIRECT) != M5FACES_OK) {
			ESP_LOGE(kTag, "Keyboard3 not found 2");
			return;
		} else {
			ESP_LOGI(kTag, "Keyboard3 init success 2 end");
		}		
	} else {
		if (keyboard.setMode(M5FACES_MODE_NORMAL/*M5FACES_MODE_DIRECT*/) != M5FACES_OK) {
			ESP_LOGE(kTag, "Keyboard3 not found 2");
			return;
		} else {
			ESP_LOGI(kTag, "Keyboard3 init success 2 end");
		}
	}
}

void loop() {
	if (!USE_DIRECT) {
		if (keyboard.update() && keyboard.isPressed()) {
			char c = keyboard.getChar();
			if (c != '\0') ESP_LOGI(kTag, "keyboard.isPressed(): %c", c);
		}
	} else {
		//M5Faces_Calculator3::MODEL_ID
		//M5Faces_Gamepad3::MODEL_ID
		//M5Faces_Keyboard3::MODEL_ID
	    poller.tick(&keyboard, M5FACES_MODE_DIRECT, M5Faces_Keyboard3::MODEL_ID);
    }
	delay(5);
}

#define ECHO_TASK_STACK_SIZE (8192)
static void echo_task(void *arg)
{
	for (;;) {
		loop();
	}
}
void app_i2c_keyboard_init(i2c_master_bus_handle_t bus) {
	setup(bus);
	xTaskCreate(echo_task, "echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}
