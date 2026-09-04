#include "gallery.h"
#include <stdio.h>

void gallery_init() {

}


// get gallery list

gallery_iterator_t gallery_iterator_start(void) {
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

gallery_write_stream_t gallery_upload_start() {
    gallery_write_stream_t stream;
    return stream;
}
void gallery_upload_write(gallery_write_stream_t* stream, char* buffer, size_t received){

}
void gallery_upload_finish(gallery_write_stream_t* stream){

}


// get item chunked

bool gallery_item_open(gallery_item_t*, char* item_id){
    return true;
}
size_t gallery_item_read(gallery_item_t* item, char* buffer, size_t len){
    return 0;
}
void gallery_item_close(gallery_item_t* item){

}