#include "network_settings.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "cJSON.h"
#include "network.h"

static const char *TAG = "[network/network.c]";
static int s_retry_num = 0;

static void network_start_sta(void) {
    s_retry_num = 0;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi mode, connection to router...");
    esp_wifi_connect();
}

static void network_start_ap(void) {
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Access point mode activated.");
}

static void wifi_event_handler(
    void* arg, 
    esp_event_base_t event_base,
    int32_t event_id, 
    void* event_data
) {
    
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 5) {
            s_retry_num++;
            ESP_LOGW(TAG, "Router connection failed. Attempt %d/5...", s_retry_num);
            esp_wifi_connect();
        } else {
            ESP_LOGE(TAG, "Router connection not respond...");
            esp_wifi_stop(); 
            network_start_ap();
        }
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGW(TAG, "Wifi router connected! IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
    }
}

void network_settings_init(network_init_t*);

void network_init(network_init_t* ni) {
    ESP_LOGI(TAG, "Network starting...");

    network_settings_init(ni);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // subscribe to wifi events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    wifi_config_t wifi_ap_config = {
        .ap = { 
            .channel = 1, 
            .max_connection = 4
        }
    };
  
    network_settings_t s;
    network_settings_read(&s);

    // access point settings
    if (strlen(s.ap_password) < 8) {
        wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    } else {
        strncpy((char*)wifi_ap_config.ap.password, s.ap_password, sizeof(wifi_ap_config.ap.password));
        wifi_ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    }
    strncpy((char*)wifi_ap_config.ap.ssid, s.ap_ssid, sizeof(wifi_ap_config.ap.ssid));
    wifi_ap_config.ap.ssid_len = strlen(s.ap_ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));

    // router settings
    wifi_config_t wifi_sta_config = {0};
    strncpy((char*)wifi_sta_config.sta.ssid, s.wifi_ssid, sizeof(wifi_sta_config.sta.ssid));
    strncpy((char*)wifi_sta_config.sta.password, s.wifi_password, sizeof(wifi_sta_config.sta.password));
    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));

    if (strcmp(s.mode, "wifi") == 0) {
        network_start_sta();
    } else {
        network_start_ap();
    }
}


