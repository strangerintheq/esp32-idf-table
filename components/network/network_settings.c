#include "network.h"
#include "network_settings.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "cJSON.h"

#define NETWORK_NVS_NAMESPACE "wifi_store"
#define NETWORK_NVS_KEY_MODE "mode"
#define NETWORK_NVS_KEY_WIFI_SSID "wifi_ssid"
#define NETWORK_NVS_KEY_WIFI_PASSWORD "wifi_password"
#define NETWORK_NVS_KEY_AP_SSID "ap_ssid"
#define NETWORK_NVS_KEY_AP_PASSWORD "ap_password"

// static const char *TAG = "[network/network_settings.c]";
static network_init_t* ni;

void network_settings_init(network_init_t* init) {
    ni = init;
}

void network_settings_read(network_settings_t *s) {
    ni->read_value_by_key(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_MODE, s->mode, sizeof(s->mode));
    ni->read_value_by_key(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_AP_SSID, s->ap_ssid, sizeof(s->ap_ssid));
    ni->read_value_by_key(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_AP_PASSWORD, s->ap_password, sizeof(s->ap_password));
    ni->read_value_by_key(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_WIFI_SSID, s->wifi_ssid, sizeof(s->wifi_ssid));
    ni->read_value_by_key(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_WIFI_PASSWORD, s->wifi_password, sizeof(s->wifi_password));
}

char* network_settings_get_json_str(void) {
    network_settings_t s;
    network_settings_read(&s);
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, NETWORK_NVS_KEY_MODE, s.mode);
    cJSON_AddStringToObject(root, NETWORK_NVS_KEY_WIFI_SSID, s.wifi_ssid);
    cJSON_AddStringToObject(root, NETWORK_NVS_KEY_WIFI_PASSWORD, s.wifi_password);
    cJSON_AddStringToObject(root, NETWORK_NVS_KEY_AP_SSID, s.ap_ssid);
    cJSON_AddStringToObject(root, NETWORK_NVS_KEY_AP_PASSWORD, s.ap_password);
    char *json_str = cJSON_PrintUnformatted(root); 
    cJSON_Delete(root);
    return json_str; // important: call 'free()' after usage!
}

static void update_setting(cJSON* data, char* key) {
    char *value = cJSON_GetStringValue(cJSON_GetObjectItem(data, key));
    if (value != NULL) {
        ni->save_key_value_pair(NETWORK_NVS_NAMESPACE, key, value);
        ESP_LOGI("TAG", "Updated %s = %s", key, value);
    } else {
        ESP_LOGW("TAG", "Key '%s' not found or not a string", key);
    }
}

void network_settings_update_from_json_str(char* settings_str) {
    cJSON *data = cJSON_Parse(settings_str);
    update_setting(data, NETWORK_NVS_KEY_MODE);
    update_setting(data, NETWORK_NVS_KEY_WIFI_SSID);
    update_setting(data, NETWORK_NVS_KEY_WIFI_PASSWORD);
    update_setting(data, NETWORK_NVS_KEY_AP_SSID);
    update_setting(data, NETWORK_NVS_KEY_AP_PASSWORD);
    cJSON_Delete(data);
}
