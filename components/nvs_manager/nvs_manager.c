#include "nvs_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>

static const char *TAG = "[nvs_manager/nvs_manager.c]";

void nvs_manager_init(void) {
    ESP_LOGI(TAG, "Initializing NVS...");
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "ESP_ERR_NVS_NO_FREE_PAGES or ESP_ERR_NVS_NEW_VERSION_FOUND. Clearing NVS...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing NVS: %s", esp_err_to_name(ret));
        return;
    }
    
    ESP_LOGI(TAG, "NVS initialized.");
}

bool nvs_manager_set_str(
    const char* space, 
    const char* key, 
    const char* value
) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(space, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Namespace [%s] open fail: %s", space, esp_err_to_name(err));
        return false;
    }

    err = nvs_set_str(handle, key, value);
    if (err == ESP_OK) {
        err = nvs_commit(handle); // Физический сброс данных во Flash
        ESP_LOGI(TAG, "%s.%s saved", space, key);
    } else {
        ESP_LOGE(TAG, "%s.%s write error: %s", space, key, esp_err_to_name(err));
    }

    nvs_close(handle);
    return (err == ESP_OK);
}

bool nvs_manager_get_str(
    const char* space, 
    const char* key, 
    char* out_value, 
    size_t max_len
) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(space, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return false; // Раздел еще ни разу не создавался
    }

    size_t required_size = max_len;
    err = nvs_get_str(handle, key, out_value, &required_size);
    nvs_close(handle);

    if (err != ESP_OK) {
        if (err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "%s.%s error reading: %s", space, key, esp_err_to_name(err));
        }
        return false;
    }

    return true;
}

bool nvs_manager_erase_key(
    const char* space, 
    const char* key
) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(space, NVS_READWRITE, &handle);
    if (err != ESP_OK) return false;

    err = nvs_erase_key(handle, key);
    if (err == ESP_OK) {
        nvs_commit(handle);
        ESP_LOGW(TAG, "%s.%s removed.", space, key);
    }
    
    nvs_close(handle);
    return (err == ESP_OK);
}
