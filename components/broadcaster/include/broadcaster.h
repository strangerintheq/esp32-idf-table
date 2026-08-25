#ifndef BROADCASTER_H
#define BROADCASTER_H

typedef struct {
    void (*broadcast)(const char *);
} broadcaster_init_t;

void broadcaster_init(broadcaster_init_t*);

void broadcaster_publish(const char* key, const char* value);

#endif //BROADCASTER_H