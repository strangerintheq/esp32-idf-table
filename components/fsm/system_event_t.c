#include <system_event_t.h>

const char* system_event_to_str(system_event_t e) {
    switch (e) {
        case SYSTEM_EVENT_BOOT_INIT_OK:   return "SYSTEM_EVENT_BOOT_INIT_OK";
        case SYSTEM_EVENT_HOMING_DONE:    return "SYSTEM_EVENT_HOMING_DONE";
        case SYSTEM_EVENT_RAMP_UP_DONE:   return "SYSTEM_EVENT_RAMP_UP_DONE";
        case SYSTEM_EVENT_RAMP_DOWN_DONE: return "SYSTEM_EVENT_RAMP_DOWN_DONE";
        case SYSTEM_EVENT_TRACK_END:      return "SYSTEM_EVENT_TRACK_END";
        case SYSTEM_EVENT_ERROR:          return "SYSTEM_EVENT_ERROR";
        default:                          return "SYSTEM_EVENT_UNKNOWN";
    }
}
