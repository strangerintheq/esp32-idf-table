#ifndef GALLERY_H
#define GALLERY_H

#include <stdio.h>

void gallery_init();

typedef struct {
    void *dir_ptr; 
    bool is_valid; 
} gallery_iterator_t;

gallery_iterator_t gallery_iterator_start(void);

bool gallery_iterator_next(gallery_iterator_t *it, char *buffer, size_t max_len);

void gallery_iterator_close(gallery_iterator_t *it);

#endif


