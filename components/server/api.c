#include "esp_http_server.h" 
#include "esp_log.h"
#include "nvs_manager.h"
#include "network.h"
#include "gallery.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <fcntl.h>     

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define API_MAX_POST_SIZE 1024

static const char *TAG = "[server/api.c]";

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
    esp_err_t ret = httpd_register_uri_handler(server_handle, &static_files_uri);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register %s: %d", uri, ret);
    }
}

static char * get_post_body(httpd_req_t *req) {
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

static esp_err_t set_network_settings(httpd_req_t *req) {
    char *body = get_post_body(req);
    if (body == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, 
                           "Failed to read body");
        return ESP_FAIL;
    }
    ESP_LOGI("TAG", "Received: %s", body);
    network_update_settings(body);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"status\":\"ok\"}");
    free(body);
    return ESP_OK;
}

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

#define SCRATCH_BUFSIZE 8192 // Размер буфера для приема данных

static esp_err_t gallery_upload(httpd_req_t *req) {
    
    return ESP_OK;
}


void api_init(httpd_handle_t server_handle) {
    register_post(server_handle, "/api/network/get", get_network_settings);
    register_post(server_handle, "/api/network/set", set_network_settings);
    register_post(server_handle, "/api/upload", gallery_upload);
}