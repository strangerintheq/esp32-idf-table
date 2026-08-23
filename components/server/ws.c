#include "esp_http_server.h" 
#include "esp_log.h"

static const char *TAG = "WEB_SOCKET";

#define MAX_WS_CLIENTS 10

typedef struct {
    httpd_handle_t server;      
    int fd;                     // client socket descriptor
    bool active;                
} ws_client_t;

// clients array
static ws_client_t ws_clients[MAX_WS_CLIENTS];

static SemaphoreHandle_t ws_clients_mutex = NULL;

static void lock() {
    xSemaphoreTake(ws_clients_mutex, portMAX_DELAY);
}

static void unlock() {
    xSemaphoreGive(ws_clients_mutex);
}

static esp_err_t add_ws_client(httpd_req_t *req) {
    lock();
    int fd = httpd_req_to_sockfd(req); 
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (!ws_clients[i].active) {
            ws_clients[i].active = true;
            ws_clients[i].fd = fd;
            ws_clients[i].server = req->handle;
            ESP_LOGI(TAG, "client #%d connected, descriptor=%d", i, fd);
            unlock();
            return ESP_OK;
        }
    }
    unlock();
    return ESP_FAIL;
}

static void remove_ws_client(httpd_req_t *req) {
    lock();
    int fd = httpd_req_to_sockfd(req);
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_clients[i].active && ws_clients[i].fd == fd) {
            ws_clients[i].active = false;
            ws_clients[i].fd = -1;
            ws_clients[i].server = NULL;
            ESP_LOGI(TAG, "client, fd=%d", fd);
            break;
        }
    }
    unlock();
}

static esp_err_t message_handler(httpd_req_t *req) {
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Ошибка получения длины кадра: %d", ret);
        return ret;
    }

    if (ws_pkt.len) {
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Ошибка выделения памяти");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;

        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Ошибка получения данных: %d", ret);
            free(buf);
            return ret;
        }

        ESP_LOGI(TAG, "Получено от клиента: %.*s", ws_pkt.len, ws_pkt.payload);

    }

    free(buf);
    return ESP_OK;
}

static esp_err_t websocket_handler(httpd_req_t *req) {
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    
    // Получаем фрейм
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Если есть данные - читаем их
    if (ws_pkt.len > 0) {
        uint8_t *buf = malloc(ws_pkt.len + 1);
        if (buf == NULL) return ESP_ERR_NO_MEM;
        ws_pkt.payload = buf;
        httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        buf[ws_pkt.len] = '\0';
        
        // Обработка в зависимости от типа кадра
        switch (ws_pkt.type) {
            case HTTPD_WS_TYPE_PING:
                ESP_LOGI(TAG,"Received PING\n");
                // Отвечаем PONG
                ws_pkt.type = HTTPD_WS_TYPE_PONG;
                httpd_ws_send_frame(req, &ws_pkt);
                break;
                
            case HTTPD_WS_TYPE_PONG:
                ESP_LOGI(TAG,"Received PONG\n");
                // Обычно ничего не делаем, просто логируем
                break;
                
            case HTTPD_WS_TYPE_CLOSE:
                ESP_LOGI(TAG,"Received CLOSE, closing connection\n");
                httpd_ws_send_frame(req, &ws_pkt);
                remove_ws_client(req);
                return ESP_OK;
                
            case HTTPD_WS_TYPE_TEXT:
            case HTTPD_WS_TYPE_BINARY:
                ESP_LOGI(TAG,"Received data: %s\n", buf);
                // Обработка данных...
                httpd_ws_send_frame(req, &ws_pkt);
                break;
            default:
                break;    
        }
        free(buf);
    }
    return ESP_OK;
}

void ws_init(httpd_handle_t server_handle) {
    if (ws_clients_mutex == NULL) {
        ws_clients_mutex = xSemaphoreCreateMutex();
    }
    
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        ws_clients[i].active = false;
        ws_clients[i].fd = -1;
        ws_clients[i].server = NULL;
    }

    httpd_uri_t ws_uri = {
        .uri                      = "/ws",
        .method                   = HTTP_GET,
        .handler                  = websocket_handler, 
        .ws_post_handshake_cb     = add_ws_client,
        .user_ctx                 = NULL,
        .is_websocket             = true,
        .handle_ws_control_frames = true,
    };

    esp_err_t ret = httpd_register_uri_handler(server_handle, &ws_uri);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "opened on /ws");
    } else {
        ESP_LOGE(TAG, "failed to register WebSocket: %d", ret);
    }
}
