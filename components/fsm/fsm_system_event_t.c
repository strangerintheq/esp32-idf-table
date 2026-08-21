#include <fsm_system_event_t.h>

const char* fsm_system_event_to_str(fsm_system_event_t e) {
    switch (e) {
        case FSM_SYSTEM_EVENT_BOOT_INIT_OK:   return "FSM_SYSTEM_EVENT_BOOT_INIT_OK";
        case FSM_SYSTEM_EVENT_HOMING_DONE:    return "FSM_SYSTEM_EVENT_HOMING_DONE";
        case FSM_SYSTEM_EVENT_RAMP_UP_DONE:   return "FSM_SYSTEM_EVENT_RAMP_UP_DONE";
        case FSM_SYSTEM_EVENT_RAMP_DOWN_DONE: return "FSM_SYSTEM_EVENT_RAMP_DOWN_DONE";
        case FSM_SYSTEM_EVENT_TRACK_END:      return "FSM_SYSTEM_EVENT_TRACK_END";
        case FSM_SYSTEM_EVENT_ERROR:          return "FSM_SYSTEM_EVENT_ERROR";
        default:                              return "FSM_SYSTEM_EVENT_UNKNOWN";
    }
}
