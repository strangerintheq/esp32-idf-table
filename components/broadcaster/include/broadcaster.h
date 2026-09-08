#ifndef BROADCASTER_H
#define BROADCASTER_H

void broadcaster_init(
    void (*broadcast)(const char *, int client_index)
);

void broadcaster_publish(
    const char* key, 
    const char* value
);

void broadcaster_on_client_connected_callback(int client_index);

#endif //BROADCASTER_H