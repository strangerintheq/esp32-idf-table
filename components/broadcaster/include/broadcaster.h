#ifndef BROADCASTER_H
#define BROADCASTER_H

void broadcaster_init(
    void (*broadcast)(const char *)
);

void broadcaster_publish(
    const char* key, 
    const char* value
);

#endif //BROADCASTER_H