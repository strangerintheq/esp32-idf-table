#include "esp_log.h"
#include "esp_http_server.h"
#include "storage.h"

static const char *TAG = "SERVER_STATIC";

#define FILE_CHUNK_SIZE 1024
#define FILE_PATH_SIZE 522 

static esp_err_t set_content_type_from_file(
    httpd_req_t *req, 
    const char *filename
) {
    if (strstr(filename, ".html")) 
        return httpd_resp_set_type(req, "text/html");
    if (strstr(filename, ".css"))  
        return httpd_resp_set_type(req, "text/css");
    if (strstr(filename, ".js"))   
        return httpd_resp_set_type(req, "application/javascript");
    if (strstr(filename, ".png"))  
        return httpd_resp_set_type(req, "image/png");
    if (strstr(filename, ".ico"))  
        return httpd_resp_set_type(req, "image/x-icon");
    return httpd_resp_set_type(req, "text/plain");
}

esp_err_t static_file_get_handler(httpd_req_t *req) {

    if (strcmp(req->uri, "/ws") == 0) {
        ESP_LOGI(TAG, "Skipping /ws in static handler (WebSocket)");
        return ESP_ERR_NOT_FOUND; // Возвращаем ошибку, чтобы эстафета пошла дальше
    }
    
    char filepath[FILE_PATH_SIZE];

    if (strcmp(req->uri, "/") == 0) {
        snprintf(filepath, sizeof(filepath), "/littlefs/index.html");
    } else {
        snprintf(filepath, sizeof(filepath), "/littlefs%s", req->uri);
    }

    int opened_file_handle = storage_open(filepath, "rb");

    // Вызываем абстрактное открытие файла через storage
    if (opened_file_handle == -1) {
        ESP_LOGE(TAG, "Ресурс не найден через storage API: %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = malloc(FILE_CHUNK_SIZE);
    if (!chunk) {
        storage_close(opened_file_handle); // Закрываем поток через storage
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory error");
        return ESP_FAIL;
    }

    size_t read_bytes;
    // Высокоскоростное бинарное блочное чтение без побайтового перебора '\n'
    do {
        read_bytes = storage_read(opened_file_handle, chunk, FILE_CHUNK_SIZE);
        if (read_bytes > 0) {
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                free(chunk);
                storage_close(opened_file_handle); // Защита: закрываем поток через storage
                ESP_LOGE(TAG, "Сетевой сбой при отправке чанка");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);

    // Завершаем HTTP-сессию
    httpd_resp_send_chunk(req, NULL, 0);
    free(chunk);
    storage_close(opened_file_handle); // Освобождаем дескриптор через storage
    return ESP_OK;
}

void static_init(httpd_handle_t server_handle) {
    httpd_uri_t static_files_uri = { 
        .uri = "/*", 
        .method = HTTP_GET, 
        .handler = static_file_get_handler,
        .user_ctx = NULL,
    };
    httpd_register_uri_handler(server_handle, &static_files_uri);
}
