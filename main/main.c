#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h> 

#include <fsm.h>
#include <nvs_manager.h>
#include <storage.h>
#include <network.h>
#include <network_settings.h>
#include <broadcaster.h>
#include <ws.h>

static void delay() {
    vTaskDelay(pdMS_TO_TICKS(50));
}

static void fsm_state_changed(fsm_state_t state) {
    broadcaster_publish("systemState", fsm_state_to_str(state));
}

void web_server_init();

void app_main(void) {

    nvs_manager_init();

    static network_init_t ni = {
        .read_value_by_key = nvs_manager_get_str,
        .save_key_value_pair = nvs_manager_set_str,
    };
    network_init(&ni);

    storage_init();

    web_server_init();

    static broadcaster_init_t bi = {
        .broadcast = ws_send
    };
    broadcaster_init(&bi);

    static fsm_init_t fi = {
        .publish_state = fsm_state_changed
    };
    fsm_init(&fi);

    delay();
    fsm_post_system_event(FSM_SYSTEM_EVENT_BOOT_INIT_OK, NULL);
    delay();
    fsm_post_user_event(FSM_USER_EVENT_START_HOMING, NULL);
    delay();
    fsm_post_system_event(FSM_SYSTEM_EVENT_HOMING_DONE, NULL);
}
