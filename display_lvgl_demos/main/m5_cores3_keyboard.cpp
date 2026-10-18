#include "esp_log.h"
#include "m5_tab5_keyboard.h"
#include "m5_cores3_keyboard.h"

extern "C" {
#include "osd.h" //osd_getinput
}

static const char *TAG = "OSD";

//D:\work_m5stack_tab5\rv32ima_test\M5Tab5-Keyboard-UserDemo-main\main\main.cpp
static m5::M5Tab5Keyboard device_kb;
static m5::M5Tab5Keyboard *g_kb_instance = 0;
static uint8_t kb_firmware_version = 0;  // Keyboard firmware version









// Physical keyboard event callback
//static const char* TAG = "ui_app";
//see kb_event_task()
/*
I (7638) m5_tab5_keyboard: KB_EVT mode=Normal(0) trigger=irq int_sta=0x01 queue=1 normal pressed=1 row=3 col=10 raw=0xBA
I (7638) NES_FILE_BROWSER: ============physical_keyboard_callback
I (7643) NES_FILE_BROWSER: NORMAL mode: row=3, col=10, key_id=52, pressed=1
I (7821) m5_tab5_keyboard: KB_EVT mode=Normal(0) trigger=irq int_sta=0x01 queue=1 normal pressed=0 row=3 col=10 raw=0x3A
I (7821) NES_FILE_BROWSER: ============physical_keyboard_callback
I (7826) NES_FILE_BROWSER: NORMAL mode: row=3, col=10, key_id=52, pressed=0
I (8016) m5_tab5_keyboard: KB_EVT mode=Normal(0) trigger=irq int_sta=0x01 queue=1 normal pressed=1 row=3 col=10 raw=0xBA
I (8016) NES_FILE_BROWSER: ============physical_keyboard_callback
I (8021) NES_FILE_BROWSER: NORMAL mode: row=3, col=10, key_id=52, pressed=1
I (8153) m5_tab5_keyboard: KB_EVT mode=Normal(0) trigger=irq int_sta=0x01 queue=1 normal pressed=0 row=3 col=10 raw=0x3A
I (8154) NES_FILE_BROWSER: ============physical_keyboard_callback
I (8159) NES_FILE_BROWSER: NORMAL mode: row=3, col=10, key_id=52, pressed=0
*/
//see D:\work_m5stack_tab5\rv32ima_test\nes-tab5-usb-host-main\components\m5_tab5_keyboard_component\src\m5_tab5_keyboard.cpp
//see D:\work_m5stack_tab5\rv32ima_test\nes-tab5-usb-host-main\components\nes_emulator\nofrendo\nes\nes.c
//osd_getinput
//see D:\work_m5stack_tab5\rv32ima_test\nes-tab5-usb-host-main\components\nes_emulator\src\nes_osd.c
//Input state
uint32_t nes_input_state = 0xFFFFFFFF;  // All buttons released
//extern uint32_t nes_input_state;
// Map CardKeyBoard keys to NES buttons
#if 0
const int ev[8] = {
	event_joypad1_up, event_joypad1_down,
	event_joypad1_left, event_joypad1_right,
	event_joypad1_select, event_joypad1_start,
	event_joypad1_a, event_joypad1_b
};
#endif
//instead of nes_usb_hid_interface_callback
static void physical_keyboard_callback(m5_tab5_key_event_t event, void* arg)
{	
    (void)arg;
//ESP_LOGI(TAG, "============physical_keyboard_callback");
	// Handle different event types
	if (event.type == M5_TAB5_KB_MODE_NORMAL) {
//FIXME:TODO: normal run here:		
		// Normal Mode: Row/Col events
		uint16_t key_id = event.row * 14 + event.col;
		//ESP_LOGD
		ESP_LOGI(TAG, "NORMAL mode: row=%d, col=%d, key_id=%d, pressed=%d", event.row, event.col, key_id,
				 event.pressed);

#if 0				 
		// Update Log Label with firmware version (Format based on _default_key_callback)
		update_log_label("[FW:%02X] [NORMAL] %s | Row: %d | Col: %d | Raw: 0x%02X", kb_firmware_version,
						 event.pressed ? "PRESSED " : "RELEASED", event.row, event.col, event.raw_data);
#endif
		//see key_id, event.pressed
		
		
		
		uint32_t old_state = nes_input_state;
		uint16_t key = (uint8_t)key_id;
		//uint32_t state = 0xFFFFFFFF;  // All buttons released by default
		if (event.pressed == 1) {
			if (key == 53/*0xB5*/) {  // Up
				nes_input_state &= ~(1UL << 0);
			}
			if (key == 67/*0xB6*/) {  // Down
				nes_input_state &= ~(1UL << 1);
			}
			if (key == 66/*0xB4*/) {  // Left
				nes_input_state &= ~(1UL << 2);
			}
			if (key == 68/*0xB7*/) {  // Right
				nes_input_state &= ~(1UL << 3);
			}
			
			// Control keys
			if (key == 69/*0x20*/) {  // Space = Select
				nes_input_state &= ~(1UL << 4);
			}
			if (key == 55/*0x0D*/) {  // Enter = Start
				nes_input_state &= ~(1UL << 5);
			}
			
			// Symbol keys
			if (key == 58/*0x2F*/) {  // Z = A button = Z
				nes_input_state &= ~(1UL << 6);
			}
			if (key == 59/*0x2E*/) {  // X = B button = X
				nes_input_state &= ~(1UL << 7);
			}
		} else {
			if (key == 53/*0xB5*/) {  // Up
				nes_input_state |= (1UL << 0);
			}
			if (key == 67/*0xB6*/) {  // Down
				nes_input_state |= (1UL << 1);
			}
			if (key == 66/*0xB4*/) {  // Left
				nes_input_state |= (1UL << 2);
			}
			if (key == 68/*0xB7*/) {  // Right
				nes_input_state |= (1UL << 3);
			}
			
			// Control keys
			if (key == 69/*0x20*/) {  // Space = Select
				ESP_LOGI(TAG, "============physical_keyboard_callback, select"); 
				nes_input_state |= (1UL << 4);
			}
			if (key == 55/*0x0D*/) {  // Enter = Start
				ESP_LOGI(TAG, "============physical_keyboard_callback, start"); 
				nes_input_state |= (1UL << 5);
			}
			
			// Symbol keys
			if (key == 58/*0x2F*/) {  // Z = A button = Z
				nes_input_state |= (1UL << 6);
			}
			if (key == 59/*0x2E*/) {  // X = B button = X
				nes_input_state |= (1UL << 7);
			}
		}
		
		// Update nes_input_state
		//nes_input_state = state;
ESP_LOGI(TAG, "============physical_keyboard_callback, nes_input_state == %02X", (uint8_t)nes_input_state);
		
#if 1		
		if (0) { //if (showBrowser) {
			//skip, don't crash in event_get()
		} else {
			// Send events for changed buttons
			uint32_t changed = nes_input_state ^ old_state;
			for (int i = 0; i < 8; i++) {
				if (changed & (1UL << i)) {
#if 0
					event_t evh = event_get(ev[i]);
					if (evh) {
						bool pressed = (nes_input_state & (1UL << i)) == 0;
						evh(pressed ? INP_STATE_MAKE : INP_STATE_BREAK);
					}
#endif
				}
			}
		}
		
         app_gamepad.btn_up = (((nes_input_state >> 0) & 1) ? false : true);
         app_gamepad.btn_down = (((nes_input_state >> 1) & 1) ? false : true);
         app_gamepad.btn_left = (((nes_input_state >> 2) & 1) ? false : true);
         app_gamepad.btn_right = (((nes_input_state >> 3) & 1) ? false : true);

         app_gamepad.btn_select = (((nes_input_state >> 4) & 1) ? false : true);
         app_gamepad.btn_start = (((nes_input_state >> 5) & 1) ? false : true);
         app_gamepad.btn_a = (((nes_input_state >> 6) & 1) ? false : true);
         app_gamepad.btn_b = (((nes_input_state >> 7) & 1) ? false : true);

         osd_getinput();
#endif	
		
		
		
		
		
		
		
		
		
		
	} else if (event.type == M5_TAB5_KB_MODE_STRING) {
		// String/Char Mode: String data events
		ESP_LOGI(TAG, "CHAR mode: len=%d, data='%s'", event.str_len, event.str_data);

#if 0
		// Update Log Label with firmware version (Format based on _default_key_callback)
		update_log_label("[FW:%02X] [STRING] Mod: 0x%02X | Len: %d | Data: %s", kb_firmware_version,
						 event.str_modifier, event.str_len, event.str_data);
#endif
						 
		ESP_LOGI(TAG, "[FW:%02X] [STRING] Mod: 0x%02X | Len: %d | Data: %s",
									kb_firmware_version, event.str_modifier, event.str_len, event.str_data);
	} else if (event.type == M5_TAB5_KB_MODE_HID) {
		// HID Mode: HID keycode events
		ESP_LOGI(TAG, "HID mode: modifier=0x%02X, keycode=0x%02X", event.hid_modifier, event.hid_key_code);
#if 0
		// Update Log Label with firmware version (Format based on _default_key_callback)
		update_log_label("[FW:%02X] [HID] Mod: 0x%02X | KeyCode: 0x%02X", kb_firmware_version, event.hid_modifier,
						 event.hid_key_code);
#endif
		//see event.hid_modifier, event.hid_key_code
	}
}
































void app_i2c_keyboard_init(i2c_master_bus_handle_t bus) {
	//------------------------
    ESP_LOGI(TAG, "Initializing M5Tab5 Keyboard...");
	g_kb_instance = &device_kb;
	
	m5_tab5_kb_mode_t driver_mode = M5_TAB5_KB_MODE_NORMAL;
	//switch (page_id) {
	//	case PAGE_ID_FACTORY:
	//	case PAGE_ID_NORMAL:
	//		driver_mode = M5_TAB5_KB_MODE_NORMAL;
	//		break;
	//	case PAGE_ID_HID:
	//		driver_mode = M5_TAB5_KB_MODE_HID;
	//		break;
	//	case PAGE_ID_STRING:
	//		driver_mode = M5_TAB5_KB_MODE_STRING;
	//		break;
	//	default:
	//		driver_mode = M5_TAB5_KB_MODE_NORMAL;
	//		break;
	//}

	g_kb_instance->setMode(driver_mode);

	//if (page_id == PAGE_ID_FACTORY) {
	//	g_kb_instance->setRGBMode(M5_TAB5_KB_RGB_MODE_CUSTOM);
	//} else {
	g_kb_instance->setRGBMode(M5_TAB5_KB_RGB_MODE_BINDING);
	//}
	
	if (g_kb_instance) {
		ESP_LOGI(TAG, "============Keyboard initialized 1");
 #if 0       
		if (g_kb_instance->isInitialized()) {
            ESP_LOGI(TAG, "============Keyboard initialized 2");
			if (g_kb_instance->getVersion(&kb_firmware_version) != M5_TAB5_KB_OK) {
                kb_firmware_version = 0x00;
            }
        }
#endif
		//FIXME: be careful, setKeyCallback() will be overwrite by enableNormalMode()
		ESP_LOGI(TAG, "============Keyboard initialized 3");
		g_kb_instance->setKeyCallback(physical_keyboard_callback, nullptr);		
	}
	
	//FIXME:NOTE: Be careful, .begin() or ->begin() must be at the end of the init code
    // Initialize keyboard using standard I2C_NUM_1 and hardware interrupt pin
	m5_tab5_kb_err_t kb_err;
	if (bus != nullptr) {
		kb_err =
        device_kb.begin(bus, M5_TAB5_KB_DEFAULT_ADDR,
                        M5_TAB5_KB_I2C_FREQ_400K, M5_TAB5_KB_DEFAULT_INT, M5_TAB5_KB_INT_MODE_HARDWARE);
	} else {
		kb_err =
        device_kb.begin(I2C_NUM_1, M5_TAB5_KB_DEFAULT_ADDR, M5_TAB5_KB_DEFAULT_SDA, M5_TAB5_KB_DEFAULT_SCL,
                        M5_TAB5_KB_I2C_FREQ_400K, M5_TAB5_KB_DEFAULT_INT, M5_TAB5_KB_INT_MODE_HARDWARE);		
	}
    uint8_t kb_version = 0x00;
    if (kb_err == M5_TAB5_KB_OK) {
        ESP_LOGI(TAG, "Keyboard initialized successfully. Default Normal Mode Enabled.");
        if (device_kb.getVersion(&kb_version) != M5_TAB5_KB_OK) {
            ESP_LOGW(TAG, "Failed to read keyboard firmware version");
            kb_version = 0x00;
        } else {
            ESP_LOGI(TAG, "Keyboard FW: 0x%02X", kb_version);
        }
    } else {
        ESP_LOGW(TAG, "Keyboard not detected at boot, hotplug monitor will handle reconnection");
    }
	
	//physical_keyboard_callback
	//setKeyCallback
#if 0	
	//D:\work_m5stack_tab5\rv32ima_test\M5Tab5-Keyboard-UserDemo-main\main\ui_app.cpp
	// --- Disconnected State ---
    //kb_set_hotplug_state(KB_HOTPLUG_STATE_RECONNECTING);
	m5_tab5_kb_err_t err =
		g_kb_instance->begin(I2C_NUM_1, M5_TAB5_KB_DEFAULT_ADDR, M5_TAB5_KB_DEFAULT_SDA, M5_TAB5_KB_DEFAULT_SCL,
							 M5_TAB5_KB_I2C_FREQ_400K, M5_TAB5_KB_DEFAULT_INT, M5_TAB5_KB_INT_MODE_HARDWARE);

	if (err == M5_TAB5_KB_OK) {
		// Initialization successful, read version and set mode
		g_kb_instance->setKeyCallback(physical_keyboard_callback, nullptr);

		uint8_t ver = 0;
		if (g_kb_instance->getVersion(&ver) == M5_TAB5_KB_OK) {
			kb_firmware_version = ver;
		} else {
			kb_firmware_version = 0;
		}

		kb_connected      = true;
		kb_i2c_fail_count = 0;
		kb_apply_mode_for_current_page();
		kb_set_hotplug_state(KB_HOTPLUG_STATE_STABLE);
		update_log_label("[FW:%02X] Keyboard Connected", kb_firmware_version);
		refresh_factory_version_if_visible();
		ESP_LOGI(TAG, "Keyboard connected! FW: 0x%02X", kb_firmware_version);
	}
#endif			
	
	
	//FIXME: be careful, setKeyCallback() will be overwrite by enableNormalMode(), so executed after .begin() 
	ESP_LOGI(TAG, "============Keyboard initialized 3.1");
	g_kb_instance->setKeyCallback(physical_keyboard_callback, nullptr);	
		
	//------------------------
}
