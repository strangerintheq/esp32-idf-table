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

static bool (*save)(
    const char* namespace, 
    const char* key, 
    const char* value
) = NULL;

static bool (*read)(
    const char* namespace, 
    const char* key,
    char* value,
    size_t len
) = NULL;

void network_settings_init(

    bool (*read_value_by_key)(
        const char* namespace, 
        const char* key,
        char* value,
        size_t len
    ),
    
    bool (*save_key_value_pair)(
        const char* namespace, 
        const char* key, 
        const char* value
    )

) {

    save = save_key_value_pair;
    read = read_value_by_key;
}

void network_settings_read(network_settings_t *s) {
    read(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_MODE, s->mode, sizeof(s->mode));
    read(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_AP_SSID, s->ap_ssid, sizeof(s->ap_ssid));
    read(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_AP_PASSWORD, s->ap_password, sizeof(s->ap_password));
    read(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_WIFI_SSID, s->wifi_ssid, sizeof(s->wifi_ssid));
    read(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_WIFI_PASSWORD, s->wifi_password, sizeof(s->wifi_password));
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
        save(NETWORK_NVS_NAMESPACE, key, value);
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
