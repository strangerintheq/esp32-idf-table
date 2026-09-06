#include "gallery.h"
#include "storage.h"
#include "stdio.h"
#include "esp_random.h" 
#include "string.h"

//static const char *TAG = "[gallery/gallery.c]";

void gallery_init() {
    storage_ensure_directory("/gallery");
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
gallery_iterator_t gallery_iterator_start() {
    gallery_iterator_t it = { 
        .dir_ptr = NULL, 
        .is_valid = false 
    };
    return it;
}
bool gallery_iterator_next(gallery_iterator_t *it, char *buffer, size_t max_len) {
    return false; 
}
void gallery_iterator_close(gallery_iterator_t *it) {
  
}


// upload
bool gallery_upload_start(gallery_upload_t* upload) {
    generate_random_id(upload->id, 8);
    char path[128];
    snprintf(path, sizeof(path), "/gallery/%s.bin", upload->id);
    upload->file = storage_open(path, "wb");
    return true;
}
void gallery_upload_write(gallery_upload_t* stream, char* buffer, size_t received) {
    storage_write(stream->file, buffer, received);
}   
void gallery_upload_finish(gallery_upload_t* stream) {
    storage_close(stream->file);
    char path[128];
    snprintf(path, sizeof(path), "/gallery/%s.json", stream->id);

    // save metdata
    int descr = storage_open(path, "wb");
    char json[128];
    snprintf(json, sizeof(json), "{\"id\":\"%s\"}", stream->id);
    storage_write(descr, json, strlen(json));
    storage_close(descr);
}


// get item chunked
bool gallery_item_open(gallery_item_t* item) {
    char path[128];
    snprintf(path, sizeof(path), "/gallery/%s.bin", item->id);
    item->file = storage_open("/gallery/", "rb");
    return true;
}
size_t gallery_item_read(gallery_item_t* item, char* buffer, size_t len) {
    return storage_read(item->file, buffer, len);
}
void gallery_item_close(gallery_item_t* item) {
    storage_close(item->file);
}
