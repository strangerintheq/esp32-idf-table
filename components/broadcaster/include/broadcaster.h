#ifndef BROADCASTER_H
#define BROADCASTER_H

void broadcaster_init(void);
void broadcaster_publish(const char* key, const char* value);
void broadcaster_register_sender(void (*fn)(const char *));

#endif //BROADCASTER_H