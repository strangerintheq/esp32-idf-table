#ifndef STORAGE_H
#define STORAGE_H

#define FILE_CHUNK_SIZE 1024

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void storage_init(void);

int storage_open(const char* filepath, const char* mode);

size_t storage_read(int handle, void* buffer, size_t size);

size_t storage_write(int handle, const void* buffer, size_t size);

bool storage_seek(int handle, long offset, int whence);

long storage_tell(int handle);

size_t storage_get_size(int handle);

bool storage_is_open(int handle);

void storage_close(int handle);

void storage_close_all();

bool storage_ensure_directory(char*);

typedef void* storage_dir_t; 
storage_dir_t storage_dir_open(const char* dirpath);
bool storage_dir_next(storage_dir_t dir, char* filename_out, size_t max_len);
void storage_dir_close(storage_dir_t dir);

#endif // STORAGE_H