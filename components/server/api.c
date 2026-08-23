#include "esp_http_server.h" 
#include "esp_log.h"
#include "nvs_manager.h"
#include "network.h"

static esp_err_t get_network_settings(httpd_req_t *req) {
    char* json = network_get_settings();
    if (json == NULL) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
    free(json);  
    return ESP_OK;
}

static void register_post(
    httpd_handle_t server_handle, 
    char* uri, 
    esp_err_t (*handler)(httpd_req_t *r)
) {
    httpd_uri_t static_files_uri = { 
        .uri = uri, 
        .method = HTTP_POST, 
        .handler = handler,
        .user_ctx = NULL,
    };
    httpd_register_uri_handler(server_handle, &static_files_uri);
}

void api_init(httpd_handle_t server_handle) {
    register_post(server_handle, "/api/network/get", get_network_settings);
}