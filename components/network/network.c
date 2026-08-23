#include "network.h"
#include "nvs_manager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "cJSON.h"

#define NETWORK_NVS_NAMESPACE "wifi_store"
#define NETWORK_NVS_KEY_MODE "mode"
#define NETWORK_NVS_KEY_WIFI_SSID "wifi_ssid"
#define NETWORK_NVS_KEY_WIFI_PASSWORD "wifi_password"
#define NETWORK_NVS_KEY_AP_SSID "ap_ssid"
#define NETWORK_NVS_KEY_AP_PASSWORD "ap_password"

static const char *TAG = "NETWORK";
static int s_retry_num = 0;

typedef struct {
    char mode[8];
    char wifi_ssid[32];
    char wifi_password[64];
    char ap_ssid[32];
    char ap_password[64];
} network_settings_t;

static void network_start_sta(void) {
    s_retry_num = 0;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Запущен режим STA. Ожидание подключения к роутеру...");
    esp_wifi_connect();
}

static void network_start_ap(void) {
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP)); // Включаем строго чистую раздачу, без STA
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Запущен чистый режим точки доступа (AP Mode).");
}

// Асинхронный обработчик системных событий Wi-Fi чипа
static void wifi_event_handler(
    void* arg, 
    esp_event_base_t event_base,
    int32_t event_id, 
    void* event_data
) {
    
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 5) {
            s_retry_num++;
            ESP_LOGW(TAG, "Сбой подключения к роутеру. Попытка %d/5...", s_retry_num);
            esp_wifi_connect();
        } else {
            ESP_LOGE(TAG, "Роутер не ответил за 5 попыток.");
            ESP_LOGW(TAG, "ФОЛБЕК: Наносекундное включение резервной точки доступа...");
            
            esp_wifi_stop(); // Полностью глушим радиомодуль, очищая контекст STA
            
            // Вызываем наш готовый внутренний метод включения чистой точки доступа
            network_start_ap();
        }
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGW(TAG, "STA подключен! Домашний IP стола: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
    }
}

static network_settings_t read_settings(void) {

    network_settings_t network_settings = {0};

    nvs_manager_get_str(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_MODE, 
        network_settings.mode, sizeof(network_settings.mode));

    nvs_manager_get_str(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_AP_SSID, 
        network_settings.ap_ssid, sizeof(network_settings.ap_ssid));

    nvs_manager_get_str(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_AP_PASSWORD, 
        network_settings.ap_password, sizeof(network_settings.ap_password));

    nvs_manager_get_str(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_WIFI_SSID, 
        network_settings.wifi_ssid, sizeof(network_settings.wifi_ssid));

    nvs_manager_get_str(NETWORK_NVS_NAMESPACE, NETWORK_NVS_KEY_WIFI_PASSWORD, 
        network_settings.wifi_password, sizeof(network_settings.wifi_password));

    return network_settings;
}

void network_init(void) {
    ESP_LOGI(TAG, "Старт оркестрации сетевых интерфейсов...");

    // 1. Накатываем системное ядро LwIP и виртуальные адаптеры
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Подписываем наш обработчик на прерывания Wi-Fi стека
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    
    // Читаем и сразу скармливаем чипу настройки резервной точки доступа Dune (AP)
    wifi_config_t wifi_ap_config = {
        .ap = { 
            .channel = 1, 
            .max_connection = 4, 
            .authmode = WIFI_AUTH_WPA2_PSK 
        }
    };
  
    network_settings_t network_settings = read_settings();

    strncpy((char*)wifi_ap_config.ap.ssid, network_settings.ap_ssid, sizeof(wifi_ap_config.ap.ssid));
    wifi_ap_config.ap.ssid_len = strlen(network_settings.ap_ssid);
    strncpy((char*)wifi_ap_config.ap.password, network_settings.ap_password, sizeof(wifi_ap_config.ap.password));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));

    // Читаем и сразу скармливаем чипу настройки домашнего роутера (STA)
    wifi_config_t wifi_sta_config = {0};
  
    
    strncpy((char*)wifi_sta_config.sta.ssid, network_settings.wifi_ssid, sizeof(wifi_sta_config.sta.ssid));
    strncpy((char*)wifi_sta_config.sta.password, network_settings.wifi_password, sizeof(wifi_sta_config.sta.password));
    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));


    if (strcmp(network_settings.mode, "wifi") == 0) {
        network_start_sta();
    } else {
        network_start_ap();
    }
}

char* network_get_settings() {
    network_settings_t s = read_settings();
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
