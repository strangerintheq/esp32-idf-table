#include "gallery.h"
#include "storage.h"
#include "stdio.h"
#include "esp_random.h" 
#include "string.h"
#include "esp_log.h"

//static const char *TAG = "[gallery/gallery.c]";

#define GALLERY_DIR "/gallery"

void gallery_init() {
    storage_ensure_directory(GALLERY_DIR);
}

static void generate_random_id(char *output, size_t length) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t charset_size = sizeof(charset) - 1;
    for (size_t i = 0; i < length; i++) {
        uint32_t random_num = esp_random();
        output[i] = charset[random_num % charset_size];
    }
    output[length] = '\0'; 
}

// get gallery list
bool gallery_iterator_start(gallery_iterator_t* it) {
    it->dir = storage_dir_open(GALLERY_DIR);
    return true;
}
bool gallery_iterator_next(gallery_iterator_t* it, char *buffer, size_t max_len) {
    if (it == NULL || buffer == NULL || max_len == 0) return false;
    char filename[128];
    while (storage_dir_next(it->dir, filename, sizeof(filename))) {
        char *ext = strrchr(filename, '.');
        if (ext != NULL && strcmp(ext, ".json") == 0) {
            char file_path[140];
            snprintf(file_path, sizeof(file_path), "%s/%s", GALLERY_DIR, filename);
            int json_file = storage_open(file_path, "rb");
            if (json_file >= 0) {
                size_t read_bytes = storage_read(json_file, buffer, max_len - 1);
                storage_close(json_file);
                if (read_bytes > 0) {
                    buffer[read_bytes] = '\0'; 
                    return true; 
                }
            }
        }
    }
    return false;
}

void gallery_iterator_close(gallery_iterator_t* it) {
    storage_dir_close(it->dir);
}


// upload
bool gallery_upload_start(gallery_upload_t* upload) {
    generate_random_id(upload->id, 8);
    char path[128];
    snprintf(path, sizeof(path), "%s/%s.bin", GALLERY_DIR, upload->id);
    upload->file = storage_open(path, "wb");
    return true;
}
void gallery_upload_write(gallery_upload_t* stream, char* buffer, size_t received) {
    storage_write(stream->file, buffer, received);
}   
void gallery_upload_finish(gallery_upload_t* stream) {
    storage_close(stream->file);
    char path[128];
    snprintf(path, sizeof(path), "%s/%s.json", GALLERY_DIR, stream->id);

    // save metdata
    int descr = storage_open(path, "wb");
    char json[128];
    snprintf(json, sizeof(json), "{\"id\":\"%s\"}", stream->id);
    storage_write(descr, json, strlen(json));
    storage_close(descr);


    int test_descr = storage_open(path, "rb");
    if (test_descr >= 0) {
        char read_buf[128] = {0};
        storage_read(test_descr, read_buf, sizeof(read_buf) - 1);
        storage_close(test_descr);
        
        ESP_LOGW("GALLERY_TEST", "Содержимое файла: %s", read_buf);
    } else {
        ESP_LOGE("GALLERY_TEST", "КРИТИЧЕСКАЯ ОШИБКА: Файл %s не удалось открыть после записи!", path);
    }
}


// get item chunked
bool gallery_item_open(gallery_item_t* item) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s.bin", GALLERY_DIR, item->id);
    item->file = storage_open(GALLERY_DIR, "rb");
    return true;
}
size_t gallery_item_read(gallery_item_t* item, char* buffer, size_t len) {
    return storage_read(item->file, buffer, len);
}
void gallery_item_close(gallery_item_t* item) {
    storage_close(item->file);
}
