#include <fsm_user_event_t.h>

const char* fsm_user_event_to_str(fsm_user_event_t e) {
    switch (e) {
        case FSM_USER_EVENT_START:  
            return "FSM_USER_EVENT_START";
        case FSM_USER_EVENT_PAUSE:  
            return "FSM_USER_EVENT_PAUSE";
        case FSM_USER_EVENT_RESUME: 
            return "FSM_USER_EVENT_RESUME";
        case FSM_USER_EVENT_STOP:   
            return "FSM_USER_EVENT_STOP";
        case FSM_USER_EVENT_START_HOMIG:   
            return "FSM_USER_EVENT_START_HOMIG";
        case FSM_USER_EVENT_REBOOT: 
            return "FSM_USER_EVENT_REBOOT";
        default:                    
            return "FSM_USER_EVENT_UNKNOWN";
    }
}
