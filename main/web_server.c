#include <server.h>
#include <server_api.h>
#include <fsm.h>
#include <network_settings.h>

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

void web_server_init() {
    httpd_handle_t server_handle = server_init();
    server_api_register_method(server_handle, HTTP_POST, "/api/network/get", get_network_settings);
    server_api_register_method(server_handle, HTTP_POST, "/api/network/set", set_network_settings);
    server_api_register_method(server_handle, HTTP_POST, "/api/signal/*", signal);
    server_api_serve_static(server_handle);
}