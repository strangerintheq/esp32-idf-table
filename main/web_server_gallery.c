#include <server.h>
#include <server_api.h>
#include <gallery.h>


#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

esp_err_t web_server__get_gallery_list(httpd_req_t *req) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send_chunk(req, "[\n", 2);
    gallery_iterator_t it = gallery_iterator_start();
    char record[512];
    bool is_first = true;
    while (gallery_iterator_next(&it, record, sizeof(record))) {
         record[sizeof(record) - 1] = '\0'; 
        if (!is_first) 
            httpd_resp_send_chunk(req, ",\n", 2);
        is_first = false;
        httpd_resp_send_chunk(req, record, strlen(record)); 
    }
    gallery_iterator_close(&it);
    httpd_resp_send_chunk(req, "]\n", 2);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

#define SCRATCH_BUFSIZE 1024

esp_err_t web_server__gallery_upload(httpd_req_t *req) {
    char buf[SCRATCH_BUFSIZE];
    int received;
    size_t total_len = req->content_len;
    size_t remaining = total_len;
    gallery_write_stream_t stream = gallery_upload_start();
    while (remaining > 0) {
        received = httpd_req_recv(req, buf, MIN(remaining, SCRATCH_BUFSIZE));
        if (received <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive post data");
            return ESP_FAIL;
        }
        gallery_upload_write(&stream, buf, received);
        remaining -= received;
    }
    gallery_upload_finish(&stream);
    httpd_resp_set_status(req, "201 Created");
    httpd_resp_sendstr(req, "File uploaded successfully");
    return ESP_OK;
}


#define FILE_CHUNK_SIZE 1024

esp_err_t web_server__get_gallery_item(httpd_req_t *req) {
    char item_id[64];
    
    // 1. Извлекаем ID или имя элемента из URI (например, ?id=123)
    if (httpd_query_key_value(req->uri, "id", item_id, sizeof(item_id)) != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing 'id' parameter");
        return ESP_FAIL;
    }

    // 2. Открываем элемент через абстракцию галереи
    gallery_item_t item;
    if (!gallery_item_open(&item, item_id)) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Gallery item not found");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg"); 

    char buffer[FILE_CHUNK_SIZE];
    int read_bytes;
    esp_err_t res = ESP_OK;

    // Предполагаем, что gallery_item_read возвращает количество прочитанных байт или 0 в конце
    while ((read_bytes = gallery_item_read(&item, buffer, sizeof(buffer))) > 0) {
        res = httpd_resp_send_chunk(req, buffer, read_bytes);
        if (res != ESP_OK) {
            break; 
        }
    }

    // 5. Обязательно закрываем абстракцию для освобождения дескрипторов/памяти
    gallery_item_close(&item);

    // 6. Завершаем chunked-ответ
    httpd_resp_send_chunk(req, NULL, 0);

    return res == ESP_OK ? ESP_OK : ESP_FAIL;
}


