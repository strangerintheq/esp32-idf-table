#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h> 

#include <fsm.h>
#include <nvs_manager.h>
#include <storage.h>
#include <network.h>
#include <network_settings.h>
#include <server.h>
#include <server_api.h>
#include <broadcaster.h>
#include <ws.h>

static void delay() {
    vTaskDelay(pdMS_TO_TICKS(50));
}

static void fsm_state_changed(fsm_state_t state) {
    broadcaster_publish("systemState", fsm_state_to_str(state));
}

static esp_err_t set_network_settings(httpd_req_t *req) {
    char *body = server_api_get_post_body(req);
    if (body == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read body");
        return ESP_FAIL;
    }
    //ESP_LOGI(TAG, "Received: %s", body);
    network_settings_update_from_json_str(body);
    server_api_post_ok(req);
    free(body);
    return ESP_OK;
}

static esp_err_t get_network_settings(httpd_req_t *req) {
    char* json = network_settings_get_json_str();
    server_api_post_json(req, json);
    free(json);  
    return ESP_OK;
}

static esp_err_t signal(httpd_req_t *req) {
    const char *uri = req->uri;
    const char *signal = strstr(uri, "/signal/");
    if (!signal) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No signal");
        return ESP_FAIL;
    }
    signal += 8;
    // ESP_LOGI(TAG, "Fsm user event received: %s", signal);
    const fsm_user_event_t evt = fsm_user_event_from_str(signal);
    fsm_post_user_event(evt, NULL);
    server_api_post_ok(req);
    return ESP_OK;  
}

void app_main(void) {

    nvs_manager_init();

    static network_init_t ni = {
        .read_value_by_key = nvs_manager_get_str,
        .save_key_value_pair = nvs_manager_set_str,
    };
    network_init(&ni);

    storage_init();

    httpd_handle_t* server_handle = server_init();
    server_api_register_method(server_handle, HTTP_POST, "/api/network/get", get_network_settings);
    server_api_register_method(server_handle, HTTP_POST, "/api/network/get", set_network_settings);
    server_api_register_method(server_handle, HTTP_POST, "/api/signal/*", signal);

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
