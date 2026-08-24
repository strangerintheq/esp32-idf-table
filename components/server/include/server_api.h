#ifndef SERVER_API_H
#define SERVER_API_H

#include "esp_http_server.h" 

void server_api_post_ok(httpd_req_t *req);
char* server_api_get_post_body(httpd_req_t *req);

#endif
