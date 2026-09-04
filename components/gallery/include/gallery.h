#ifndef GALLERY_H
#define GALLERY_H

#include <stdio.h>

void gallery_init();

// get list chunked

typedef struct {
    void *dir_ptr; 
    bool is_valid; 
} gallery_iterator_t;

gallery_iterator_t gallery_iterator_start(void);
bool gallery_iterator_next(gallery_iterator_t *it, char *buffer, size_t max_len);
void gallery_iterator_close(gallery_iterator_t *it);


// upload item chunked

typedef struct {

} gallery_write_stream_t ;

gallery_write_stream_t gallery_upload_start();
void gallery_upload_write(gallery_write_stream_t* stream, char* buffer, size_t received);
void gallery_upload_finish(gallery_write_stream_t* stream);


// get item chunked

typedef struct {

} gallery_item_t;

bool gallery_item_open(gallery_item_t*, char* item_id);
size_t gallery_item_read(gallery_item_t* item, char* buffer, size_t len);
void gallery_item_close(gallery_item_t* item);

#endif


