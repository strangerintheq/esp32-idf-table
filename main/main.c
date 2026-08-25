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
    network_init();
    storage_init();
    server_init();
    broadcaster_init(ws_send);

    // if (server_start() == true) {
    //     fsm_post_system_event(FSM_SYSTEM_EVENT_BOOT_INIT_OK, NULL);
    // } else {
    //     fsm_post_system_event(FSM_SYSTEM_EVENT_ERROR, NULL);
    // }
}
