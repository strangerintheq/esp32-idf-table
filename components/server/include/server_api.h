#ifndef SERVER_API_H
#define SERVER_API_H

#include "esp_http_server.h" 

void server_api_post_ok(httpd_req_t *req);
void server_api_post_json(httpd_req_t *req, char*);
char* server_api_get_post_body(httpd_req_t *req);

void server_api_register_method(
    httpd_handle_t server_handle, 
    httpd_method_t mathod,
    char* uri, 
    esp_err_t (*handler)(httpd_req_t *r)
);

void server_api_serve_static(httpd_handle_t*);

#endif
