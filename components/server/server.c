#include "esp_log.h"
#include "esp_http_server.h"

static const char *TAG = "[server/server.c]";

static httpd_handle_t server_handle = NULL;

extern void ws_init(httpd_handle_t);
extern void api_init(httpd_handle_t);
extern void static_init(httpd_handle_t);

void server_init(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.uri_match_fn = httpd_uri_match_wildcard; 
    config.lru_purge_enable = true; 

    ESP_LOGI(TAG, "Starting...");

    if (httpd_start(&server_handle, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Httpd start error");
        return;
    }
    
    ws_init(server_handle);
    api_init(server_handle);
    static_init(server_handle);

    ESP_LOGI(TAG, "Started.");
}
