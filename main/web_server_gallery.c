#include <server.h>
#include <server_api.h>
#include <gallery.h>

esp_err_t web_server__get_gallery_list(httpd_req_t *req) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send_chunk(req, "[\n", 2);
    gallery_iterator_t it = gallery_iterator_start();
    char record[512];
    bool is_first = true;
    while (gallery_iterator_next(&it, record, sizeof(record))) {
        if (!is_first) 
            httpd_resp_send_chunk(req, ",\n", 2);
        is_first = false;
        httpd_resp_send_chunk(req, record, sizeof(record)); 
    }
    gallery_iterator_close(&it);
    httpd_resp_send_chunk(req, "]\n", 2);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t web_server__gallery_upload(httpd_req_t *req){
    return ESP_OK;
}
