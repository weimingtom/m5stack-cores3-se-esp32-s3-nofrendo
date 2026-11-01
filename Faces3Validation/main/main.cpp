#include "nvs_flash.h"
#include "sdkconfig.h"

#if CONFIG_M5FACES_EXAMPLE_HOST_CORE2
#define M5FACES_HOST_CORE2 1
#elif CONFIG_M5FACES_EXAMPLE_HOST_CORES3
#define M5FACES_HOST_CORES3 1
#endif

#define M5FACES_VALIDATION_FW "IDF2.8.0"

#if 0

// Keep the Arduino and native ESP-IDF examples on the same validation logic.
// The shared source selects only ESP-IDF, FreeRTOS, and M5Unified APIs here.
//#include "../../../Arduino/Faces3Validation/Faces3Validation.cpp"
#include "../components/M5Faces/examples/Arduino/Faces3Validation/Faces3Validation.cpp"

#else
/*
from D:\Espressif\frameworks\Faces3Validation\components\M5Faces\src\M5FacesPoller.hpp

    M5FacesPoller poller;
 
    // Polling mode (no GPIO INT pin)
    poller.setPollingMode(true);
 
    //  Register callbacks
    poller.setNormalKeyCb([](M5FacesBase* f, uint8_t raw, void* ctx) {
        // handle Normal mode key
    }, nullptr);
    poller.setDirectEventCb([](m5faces_direct_event_t evt,
                               const m5faces_direct_data_t* data,
                               uint32_t hold_cnt, void* ctx) {
        // handle Direct mode event
    }, nullptr);
 
    //  Main loop (every ~20 ms)
    while (true) {
        poller.tick(faces, cur_mode, model_id);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
 */

//from https://github.com/m5stack/M5Faces

#include <M5Faces.h>
#include <M5Unified.h>

#ifdef ARDUINO
#include <Arduino.h>
#include <Wire.h>
#else
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

void log_pt(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

static inline void delay(uint32_t milliseconds)
{
   vTaskDelay(pdMS_TO_TICKS(milliseconds));
}

#endif

M5Faces_Keyboard3 keyboard;

//m5faces_mode_t g_input_mode = M5FACES_MODE_NORMAL;
void setup() {
#ifdef ARDUINO
    Serial.begin(115200);
    Wire.begin();
#endif

#ifdef ARDUINO
    if (keyboard.begin(&Wire) != M5FACES_OK) {
        Serial.println("Keyboard3 not found");
    }
#else
	
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
	//g_input_mode = M5FACES_MODE_DIRECT;
	if (!M5.In_I2C.begin(faces_i2c_port, kFacesSda, kFacesScl)) {
		log_pt("Keyboard3 not found 0");
		return;
	} else {
		log_pt("Keyboard3 0");
	}
    if (keyboard.begin(&M5.In_I2C, kAppAddress, 100000) != M5FACES_OK) {
		log_pt("Keyboard3 not found 1");
		return;
	} else {
		log_pt("Keyboard3 1");
	}
	if (keyboard.setMode(M5FACES_MODE_NORMAL/*M5FACES_MODE_DIRECT*/) != M5FACES_OK) {
        log_pt("Keyboard3 not found 2");
		return;
    } else {
		log_pt("Keyboard3 2");
	}
#endif
}

void loop() {
    if (keyboard.update() && keyboard.isPressed()) {
        char c = keyboard.getChar();
#ifdef ARDUINO
        if (c != '\0') Serial.print(c);
#else
        if (c != '\0') log_pt("keyboard.isPressed(): %c", c);
#endif
    }
    delay(5);
}

#endif


extern "C" void app_main(void)
{
    esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        result = nvs_flash_init();
    }
    ESP_ERROR_CHECK(result);

    setup();
    for (;;) {
        loop();
    }
}
