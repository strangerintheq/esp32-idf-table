#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h> 

#include <fsm.h>
#include <nvs_manager.h>
#include <storage.h>
#include <network.h>
#include <server.h>
#include <broadcaster.h>
#include <ws.h>

static void delay(){
    vTaskDelay(pdMS_TO_TICKS(50));
}

void app_main(void) {
    fsm_init();
    delay();
    nvs_manager_init();

    network_init_t ni = {
        .read_value_by_key = nvs_manager_get_str,
        .save_key_value_pair = nvs_manager_set_str,
    };
    network_init(&ni);
    
    storage_init();
    server_init();
    broadcaster_init(ws_send);
    delay();
    fsm_post_system_event(FSM_SYSTEM_EVENT_BOOT_INIT_OK, NULL);
    delay();
    fsm_post_user_event(FSM_USER_EVENT_START_HOMING, NULL);
    delay();
    fsm_post_system_event(FSM_SYSTEM_EVENT_HOMING_DONE, NULL);
}
