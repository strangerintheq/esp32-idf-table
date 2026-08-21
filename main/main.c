#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h> 

#include <fsm.h>
#include <nvs.h>
#include <network.h>
#include <server.h>

void app_main(void) {
    fsm_init();
    vTaskDelay(pdMS_TO_TICKS(50));
    nvs_init();
    network_init();
    server_init();

    // if (server_start() == true) {
    //     fsm_post_system_event(FSM_SYSTEM_EVENT_BOOT_INIT_OK, NULL);
    // } else {
    //     fsm_post_system_event(FSM_SYSTEM_EVENT_ERROR, NULL);
    // }
}
