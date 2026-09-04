#include <server.h>
#include <server_api.h>
#include <network_settings.h>

esp_err_t web_server__set_network_settings(httpd_req_t *req) {
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

esp_err_t web_server__get_network_settings(httpd_req_t *req) {
    char* json = network_settings_get_json_str();
    server_api_post_json(req, json);
    free(json);  
    return ESP_OK;
}

