#include "network.h"
#include "nvs_manager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include <string.h>

static const char *TAG = "NETWORK";
static int s_retry_num = 0;

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

    // =================================================================
    // 2. ПРЕДВАРИТЕЛЬНАЯ ЗАГРУЗКА КОНФИГУРАЦИЙ В ПАМЯТЬ ЧИПА (ОДИН РАЗ)
    // =================================================================
    
    // Читаем и сразу скармливаем чипу настройки резервной точки доступа Dune (AP)
    wifi_config_t wifi_ap_config = {
        .ap = { 
            .channel = 1, 
            .max_connection = 4, 
            .authmode = WIFI_AUTH_WPA2_PSK 
        }
    };
    char ap_ssid[32] = {0};
    char ap_pass[64] = {0};
    nvs_manager_get_str("wifi_store", "ap_ssid", ap_ssid, sizeof(ap_ssid));
    nvs_manager_get_str("wifi_store", "ap_password", ap_pass, sizeof(ap_pass));
    
    strncpy((char*)wifi_ap_config.ap.ssid, ap_ssid, sizeof(wifi_ap_config.ap.ssid));
    wifi_ap_config.ap.ssid_len = strlen(ap_ssid);
    strncpy((char*)wifi_ap_config.ap.password, ap_pass, sizeof(wifi_ap_config.ap.password));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));

    // Читаем и сразу скармливаем чипу настройки домашнего роутера (STA)
    wifi_config_t wifi_sta_config = {0};
    char wifi_ssid[32] = {0};
    char wifi_pass[64] = {0};
    nvs_manager_get_str("wifi_store", "wifi_ssid", wifi_ssid, sizeof(wifi_ssid));
    nvs_manager_get_str("wifi_store", "wifi_password", wifi_pass, sizeof(wifi_pass));
    
    strncpy((char*)wifi_sta_config.sta.ssid, wifi_ssid, sizeof(wifi_sta_config.sta.ssid));
    strncpy((char*)wifi_sta_config.sta.password, wifi_pass, sizeof(wifi_sta_config.sta.password));
    wifi_sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config));

    // =================================================================
    // 3. ЗАПУСК ВЫБРАННОЙ СТРАТЕГИИ НА ОСНОВЕ РЕЖИМА ИЗ NVS
    // =================================================================
    char nvs_mode[8] = {0};
    nvs_manager_get_str("wifi_store", "mode", nvs_mode, sizeof(nvs_mode));

    if (strcmp(nvs_mode, "wifi") == 0) {
        network_start_sta();
    } else {
        network_start_ap();
    }
}
