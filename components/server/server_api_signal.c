#include "esp_http_server.h" 
#include "esp_log.h"
#include "fsm.h" 
#include "server_api.h" 

static const char *TAG = "[server/server_api_signal.c]";

esp_err_t server_api_signal(httpd_req_t *req) {
    const char *uri = req->uri;
    const char *signal = strstr(uri, "/signal/");
    if (!signal) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No signal");
        return ESP_FAIL;
    }
    signal += 8;
    ESP_LOGI(TAG, "Fsm user event received: %s", signal);
    const fsm_user_event_t evt = fsm_user_event_from_str(signal);
    fsm_post_user_event(evt, NULL);
    server_api_post_ok(req);
    return ESP_OK;  
}
