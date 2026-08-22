#include "server.h"
// #include "ws_handler.h" // Наш изолированный модуль вебсокетов
#include "esp_log.h"
#include "esp_http_server.h"
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "WEB_SERVER_CORE";
static httpd_handle_t server_handle = NULL;

/* Буфер чтения файлов. 1024 байта — промышленный стандарт для экономии RAM */
#define FILE_CHUNK_SIZE 1024

// // Глобальный проброс Push-логики наружу для оркестратора
// void server_send_state_push(fsm_state_t state) {
//     ws_handler_send_state_push(server_handle, state);
// }

/* Функция автоматического определения MIME-типа файла по его расширению */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename) {
    if (strstr(filename, ".html")) return httpd_resp_set_type(req, "text/html");
    if (strstr(filename, ".css"))  return httpd_resp_set_type(req, "text/css");
    if (strstr(filename, ".js"))   return httpd_resp_set_type(req, "application/javascript");
    if (strstr(filename, ".png"))  return httpd_resp_set_type(req, "image/png");
    if (strstr(filename, ".ico"))  return httpd_resp_set_type(req, "image/x-icon");
    return httpd_resp_set_type(req, "text/plain");
}

/* Настоящий промышленный обработчик статики через системную абстракцию VFS */
static esp_err_t static_file_get_handler(httpd_req_t *req) {
    char filepath[522];

    // Формируем путь. VFS сама поймет, что префикс "/littlefs" ведет на наш диск
    if (strcmp(req->uri, "/") == 0) {
        snprintf(filepath, sizeof(filepath), "/littlefs/index.html");
    } else {
        snprintf(filepath, sizeof(filepath), "/littlefs%s", req->uri);
    }

    // Стандартное Си-открытие файла. Под капотом работает абстрактная VFS ядра
    FILE *fd = fopen(filepath, "r");
    if (!fd) {
        ESP_LOGE(TAG, "Файл не найден на диске: %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = malloc(FILE_CHUNK_SIZE);
    if (!chunk) {
        fclose(fd);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory error");
        return ESP_FAIL;
    }

    size_t read_bytes;
    // Стандартное блочное чтение из абстрактной файловой системы
    do {
        read_bytes = fread(chunk, 1, FILE_CHUNK_SIZE, fd);
        if (read_bytes > 0) {
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                free(chunk);
                fclose(fd);
                ESP_LOGE(TAG, "Сетевой сбой при отправке файла");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);

    // Завершаем передачу (чанком нулевой длины)
    httpd_resp_send_chunk(req, NULL, 0);
    free(chunk);
    fclose(fd); // Закрываем системный дескриптор
    return ESP_OK;
}

void server_init(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.uri_match_fn = httpd_uri_match_wildcard; 
    config.lru_purge_enable = true; 

    ESP_LOGI(TAG, "Запуск HTTP службы...");

    if (httpd_start(&server_handle, &config) == ESP_OK) {
        
        // 1. РЕГИСТРИРУЕМ ИЗОЛИРОВАННЫЙ ВЕБСОКЕТ-ЭНДПОИНТ
        // httpd_uri_t ws_uri = {
        //     .uri        = "/ws",
        //     .method     = HTTP_GET,
        //     .handler    = websocket_handler, 
        //     .user_ctx   = NULL,
        //     .is_websocket = true
        // };
        // httpd_register_uri_handler(server_handle, &ws_uri);

        // 2. РЕГИСТРИРУЕМ АБСТРАКТНЫЙ POSIX-РАЗДАТЧИК СТАТИКИ (GET /*)
        httpd_uri_t static_files_uri = { 
            .uri = "/*", 
            .method = HTTP_GET, 
            .handler = static_file_get_handler,
            .user_ctx = NULL 
        };
        httpd_register_uri_handler(server_handle, &static_files_uri);

        ESP_LOGI(TAG, "Сервер успешно запущен. Статика VFS и WebSockets активны.");
    }
}
