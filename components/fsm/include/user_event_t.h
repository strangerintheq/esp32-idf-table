#ifndef USER_EVENT_H
#define USER_EVENT_H

typedef enum {
    USER_EVENT_START,
    USER_EVENT_PAUSE,
    USER_EVENT_RESUME,
    USER_EVENT_STOP,
    USER_EVENT_REBOOT
} user_event_t;

const char* user_event_to_str(user_event_t);

#endif // USER_EVENT_H
