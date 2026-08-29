#ifndef NETWORK_H
#define NETWORK_H

#include <string.h>
 

void network_init(

    bool (*read_value_by_key)(
        const char* namespace, 
        const char* key,
        char* value,
        size_t len
    ),

    bool (*save_key_value_pair)(
        const char* namespace, 
        const char* key, 
        const char* value
    )

);

#endif