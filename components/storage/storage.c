#include "storage.h"
#include "esp_log.h"
#include <stdio.h>
#include "esp_littlefs.h" // Системный компонент VFS от Espressif

static const char *TAG = "STORAGE";
static FILE *current_file = NULL;

void storage_init(void) {
        ESP_LOGI(TAG, "Монтирование встроенного диска LittleFS...");

    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",        // Базовый префикс в виртуальном дереве VFS
        .partition_label = "storage",    // Имя раздела из partitions.csv
        .format_if_mount_failed = true,  // Автоформат чистой памяти на заводе
        .dont_mount = false
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Критическая ошибка монтирования Flash: %s", esp_err_to_name(ret));
    }

    size_t total = 0, used = 0;
    if (esp_littlefs_info(conf.partition_label, &total, &used) == ESP_OK) {
        ESP_LOGI(TAG, "Диск успешно зашит в VFS. Свободно: %u КБ", (total - used) / 1024);
    }
}



bool storage_open_file(const char* filepath) {
    if (current_file != NULL) {
        storage_close_file();
    }

    // Открываем файл в бинарном режиме чтения "rb" для корректного блочного чтения
    current_file = fopen(filepath, "rb");
    if (current_file == NULL) {
        ESP_LOGE(TAG, "Не удалось открыть файл в VFS: %s", filepath);
        return false;
    }

    ESP_LOGI(TAG, "Бинарный файловый поток открыт: %s", filepath);
    return true;
}

size_t storage_read_bytes(void* out_buffer, size_t bytes_to_read) {
    if (current_file == NULL || out_buffer == NULL || bytes_to_read == 0) {
        return 0;
    }

    // Прямое, высокоскоростное чтение блока памяти без побайтового анализа текста
    return fread(out_buffer, 1, bytes_to_read, current_file);
}

void storage_close_file(void) {
    if (current_file != NULL) {
        fclose(current_file);
        current_file = NULL;
        ESP_LOGI(TAG, "Бинарный файловый поток закрыт.");
    }
}
