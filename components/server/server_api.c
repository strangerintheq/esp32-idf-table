#include "esp_http_server.h" 
#include "esp_log.h"

#define API_MAX_POST_SIZE 1024

static const char *TAG = "[server/server_api.c]";

void server_api_register_method(
    httpd_handle_t server_handle, 
    httpd_method_t mathod,
    char* uri, 
    esp_err_t (*handler)(httpd_req_t *r)
) {
    httpd_uri_t static_files_uri = { 
        .uri = uri, 
        .method = mathod, 
        .handler = handler,
        .user_ctx = NULL,
    };
    esp_err_t ret = httpd_register_uri_handler(server_handle, &static_files_uri);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register %s: %d", uri, ret);
    }
}

char* server_api_get_post_body(httpd_req_t *req) {
    if (req->content_len > API_MAX_POST_SIZE) {
        return NULL;
    }
    char *buffer = malloc(req->content_len + 1);
    if (!buffer) {
        return NULL;
    }
    int remaining = req->content_len;
    int offset = 0;
    while (remaining > 0) {
        int recv_len = httpd_req_recv(req, buffer + offset, remaining);
        if (recv_len <= 0) {
            free(buffer);
            return NULL;
        }
        remaining -= recv_len;
        offset += recv_len;
    }
    buffer[req->content_len] = '\0';
    return buffer;
}

void server_api_post_json(httpd_req_t *req, char* json) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));
}

void server_api_post_ok(httpd_req_t *req) {
    server_api_post_json(req, "{\"status\":\"ok\"}");
}
