#include <server.h>
#include <server_api.h>
#include <fsm.h>

esp_err_t web_server__signal(httpd_req_t *req) {
    const char *uri = req->uri;
    const char * prefix = "/signal/";
    const char *signal = strstr(uri, prefix);
    if (!signal) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No signal");
        return ESP_FAIL;
    }
    signal += strlen(prefix);
    // ESP_LOGI(TAG, "Fsm user event received: %s", signal);
    const fsm_user_event_t evt = fsm_user_event_from_str(signal);
    fsm_post_user_event(evt, NULL);
    server_api_post_ok(req);
    return ESP_OK;  
}
