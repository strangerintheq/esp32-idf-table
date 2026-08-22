#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>

void storage_init(void);

bool storage_open_file(const char* filepath);

size_t storage_read_bytes(void* out_buffer, size_t bytes_to_read);

void storage_close_file(void);

#endif // STORAGE_H
