#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include <stddef.h>

void nvs_manager_init(void);

bool nvs_manager_set_str(
    const char* space, 
    const char* key, 
    const char* value
);

bool nvs_manager_get_str(
    const char* space, 
    const char* key, 
    char* out_value, 
    size_t max_len
);

bool nvs_manager_erase_key(
    const char* space, 
    const char* key
);

#endif // NVS_MANAGER_H