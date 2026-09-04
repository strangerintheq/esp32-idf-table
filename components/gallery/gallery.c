#include "gallery.h"
#include <stdio.h>

void gallery_init() {

}


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
