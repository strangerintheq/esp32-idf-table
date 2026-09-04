#include <server.h>
#include <server_api.h>
#include <fsm.h>
#include <network_settings.h>
#include <gallery.h>

esp_err_t web_server__set_network_settings(httpd_req_t *req);
esp_err_t web_server__get_network_settings(httpd_req_t *req);
esp_err_t web_server__get_gallery_list(httpd_req_t *req);
esp_err_t web_server__gallery_upload(httpd_req_t *req);
esp_err_t web_server__signal(httpd_req_t *req);

void web_server_init() {
    httpd_handle_t server_handle = server_init();
    server_api_register_method(server_handle, HTTP_POST, "/api/network/get", web_server__get_network_settings);
    server_api_register_method(server_handle, HTTP_POST, "/api/network/set", web_server__set_network_settings);
    server_api_register_method(server_handle, HTTP_POST, "/api/gallery/list", web_server__get_gallery_list);
    server_api_register_method(server_handle, HTTP_POST, "/api/gallery/upload", web_server__gallery_upload);
    server_api_register_method(server_handle, HTTP_POST, "/api/signal/*", web_server__signal);
    server_api_serve_static(server_handle);
}
