#include "storage.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include "esp_littlefs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "[STORAGE]";
#define MAX_OPEN_FILES 10

typedef struct {
    FILE *file;
    char path[128];
    bool is_open;
    size_t size;
} file_entry_t;

static file_entry_t open_files[MAX_OPEN_FILES];
static SemaphoreHandle_t files_mutex = NULL;

static int find_free_slot(void) {
    for (int i = 0; i < MAX_OPEN_FILES; i++)
        if (!open_files[i].is_open) return i;
    return -1;
}

static bool is_handle_valid(int handle) {
    return (handle >= 0 && handle < MAX_OPEN_FILES && open_files[handle].is_open);
}

static size_t get_file_size(FILE *file) {
    long pos = ftell(file);
    if (pos == -1) return 0;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, pos, SEEK_SET);
    return (size == -1) ? 0 : (size_t)size;
}

void storage_init(void) {
    if (files_mutex == NULL) {
        files_mutex = xSemaphoreCreateMutex();
        if (files_mutex == NULL) {
            ESP_LOGE(TAG, "Mutex create failed");
            return;
        }
    }
    memset(open_files, 0, sizeof(open_files));

    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = "storage",
        .format_if_mount_failed = true,
        .dont_mount = false
    };
    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Mount failed: %s", esp_err_to_name(ret));
        return;
    }
    size_t total = 0, used = 0;
    if (esp_littlefs_info(conf.partition_label, &total, &used) == ESP_OK)
        ESP_LOGI(TAG, "Mounted. Free: %u KB", (unsigned int)((total - used) / 1024));
}

int storage_open(const char* filepath, const char* mode) {
    if (filepath == NULL || mode == NULL || files_mutex == NULL) return -1;
    if (xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return -1;

    int slot = find_free_slot();
    if (slot == -1) {
        ESP_LOGE(TAG, "Max files (%d)", MAX_OPEN_FILES);
        xSemaphoreGive(files_mutex);
        return -1;
    }
    FILE *file = fopen(filepath, mode);
    if (file == NULL) {
        ESP_LOGE(TAG, "Can't open: %s", filepath);
        xSemaphoreGive(files_mutex);
        return -1;
    }
    open_files[slot].file = file;
    strncpy(open_files[slot].path, filepath, sizeof(open_files[slot].path) - 1);
    open_files[slot].path[sizeof(open_files[slot].path) - 1] = '\0';
    open_files[slot].is_open = true;
    open_files[slot].size = get_file_size(file);
    ESP_LOGI(TAG, "Opened: %s (handle: %d)", filepath, slot);
    xSemaphoreGive(files_mutex);
    return slot;
}

size_t storage_read(int handle, void* buffer, size_t size) {
    if (buffer == NULL || size == 0 || files_mutex == NULL) return 0;
    if (xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return 0;
    if (!is_handle_valid(handle)) {
        ESP_LOGE(TAG, "Invalid handle: %d", handle);
        xSemaphoreGive(files_mutex);
        return 0;
    }
    size_t read = fread(buffer, 1, size, open_files[handle].file);
    xSemaphoreGive(files_mutex);
    return read;
}

size_t storage_write(int handle, const void* buffer, size_t size) {
    if (buffer == NULL || size == 0 || files_mutex == NULL) return 0;
    if (xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return 0;
    if (!is_handle_valid(handle)) {
        ESP_LOGE(TAG, "Invalid handle: %d", handle);
        xSemaphoreGive(files_mutex);
        return 0;
    }
    size_t written = fwrite(buffer, 1, size, open_files[handle].file);
    if (written > 0) open_files[handle].size = get_file_size(open_files[handle].file);
    xSemaphoreGive(files_mutex);
    return written;
}

bool storage_seek(int handle, long offset, int whence) {
    if (files_mutex == NULL || xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return false;
    if (!is_handle_valid(handle)) {
        ESP_LOGE(TAG, "Invalid handle: %d", handle);
        xSemaphoreGive(files_mutex);
        return false;
    }
    int res = fseek(open_files[handle].file, offset, whence);
    xSemaphoreGive(files_mutex);
    return (res == 0);
}

long storage_tell(int handle) {
    if (files_mutex == NULL || xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return -1;
    if (!is_handle_valid(handle)) {
        ESP_LOGE(TAG, "Invalid handle: %d", handle);
        xSemaphoreGive(files_mutex);
        return -1;
    }
    long pos = ftell(open_files[handle].file);
    xSemaphoreGive(files_mutex);
    return pos;
}

size_t storage_get_size(int handle) {
    if (files_mutex == NULL || xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return 0;
    if (!is_handle_valid(handle)) {
        ESP_LOGE(TAG, "Invalid handle: %d", handle);
        xSemaphoreGive(files_mutex);
        return 0;
    }
    size_t size = open_files[handle].size;
    xSemaphoreGive(files_mutex);
    return size;
}

bool storage_is_open(int handle){
    if (files_mutex == NULL || xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return false;
    bool open = is_handle_valid(handle);
    xSemaphoreGive(files_mutex);
    return open;
}

void storage_close(int handle) {
    if (files_mutex == NULL || xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return;
    if (!is_handle_valid(handle)) {
        ESP_LOGW(TAG, "Invalid handle: %d", handle);
        xSemaphoreGive(files_mutex);
        return;
    }
    if (open_files[handle].file != NULL) {
        fclose(open_files[handle].file);
        ESP_LOGI(TAG, "Closed: %s (handle: %d)", open_files[handle].path, handle);
    }
    open_files[handle].file = NULL;
    open_files[handle].is_open = false;
    open_files[handle].size = 0;
    open_files[handle].path[0] = '\0';
    xSemaphoreGive(files_mutex);
}

void storage_close_all(void) {
    if (files_mutex == NULL || xSemaphoreTake(files_mutex, portMAX_DELAY) != pdTRUE) return;
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i].is_open && open_files[i].file != NULL) {
            fclose(open_files[i].file);
            ESP_LOGI(TAG, "Force closed: %s", open_files[i].path);
            open_files[i].is_open = false;
            open_files[i].file = NULL;
            open_files[i].size = 0;
            open_files[i].path[0] = '\0';
        }
    }
    xSemaphoreGive(files_mutex);
}