#include <fsm_user_event_t.h>
#include <string.h>

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
        case FSM_USER_EVENT_START_HOMING:   
            return "FSM_USER_EVENT_START_HOMING";
        case FSM_USER_EVENT_REBOOT: 
            return "FSM_USER_EVENT_REBOOT";
        default:                    
            return "FSM_USER_EVENT_UNKNOWN";
    }
}

fsm_user_event_t fsm_user_event_from_str(const char* name) {
    // Проверяем все возможные строки
    if (strcmp(name, "start") == 0) {
        return FSM_USER_EVENT_START;
    }
    if (strcmp(name, "pause") == 0) {
        return FSM_USER_EVENT_PAUSE;
    }
    if (strcmp(name, "resume") == 0) {
        return FSM_USER_EVENT_RESUME;
    }
    if (strcmp(name, "stop") == 0) {
        return FSM_USER_EVENT_STOP;
    }
    if (strcmp(name, "home") == 0) {
        return FSM_USER_EVENT_START_HOMING;
    }
    if (strcmp(name, "reboot") == 0) {
        return FSM_USER_EVENT_REBOOT; 
    }
    
    return -1;

}
