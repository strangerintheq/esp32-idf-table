#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h> 

#include <nvs_manager.h>
#include <storage.h>
#include <network.h>
#include <network_settings.h>
#include <broadcaster.h>
#include <ws.h>
#include <fsm.h>

void delay() {
    vTaskDelay(pdMS_TO_TICKS(50));
}

void state_machine_init();
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

    state_machine_init();

   
}
