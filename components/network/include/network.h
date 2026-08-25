#ifndef NETWORK_H
#define NETWORK_H

#include <string.h>
 
typedef struct {

    // function to save one network setting
    bool (*save_key_value_pair)(
        const char* namespace, 
        const char* key, 
        const char* value
    );

    // function to read one network setting
    bool (*read_value_by_key)(
        const char* namespace, 
        const char* key,
        char* value,
        size_t len
    );
    
} network_init_t;

void network_init(network_init_t*);

#endif