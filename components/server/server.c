#include "esp_log.h"
#include "esp_http_server.h"

static const char *TAG = "[server/server.c]";

static httpd_handle_t server_handle = NULL;

void ws_init(httpd_handle_t);

httpd_handle_t server_init(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 16;
    config.server_port = 80;
    config.uri_match_fn = httpd_uri_match_wildcard; 
    config.lru_purge_enable = true; 

    ESP_LOGI(TAG, "Starting...");

    if (httpd_start(&server_handle, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Httpd start error");
        return NULL;
    }
    
    ws_init(server_handle);

    ESP_LOGI(TAG, "Started.");
    return server_handle;
}
