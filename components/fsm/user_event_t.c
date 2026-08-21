#include <user_event_t.h>

const char* user_event_to_str(user_event_t e) {
    switch (e) {
        case USER_EVENT_START:  return "USER_EVENT_START";
        case USER_EVENT_PAUSE:  return "USER_EVENT_PAUSE";
        case USER_EVENT_RESUME: return "USER_EVENT_RESUME";
        case USER_EVENT_STOP:   return "USER_EVENT_STOP";
        case USER_EVENT_REBOOT: return "USER_EVENT_REBOOT";
        default:                return "USER_EVENT_UNKNOWN";
    }
}
