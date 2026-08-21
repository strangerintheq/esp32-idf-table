#include <fsm_state_t.h>

const char* fsm_state_to_str(fsm_state_t state) {
    switch (state) {
        case FSM_STATE_INITIALIZING:    
            return "FSM_STATE_INITIALIZING";
        case FSM_STATE_UNHOMED_IDLE: 
            return "FSM_STATE_UNHOMED_IDLE";
        case FSM_STATE_HOMING:       
            return "FSM_STATE_HOMING";
        case FSM_STATE_IDLE:         
            return "FSM_STATE_IDLE";
        case FSM_STATE_STARTING:     
            return "FSM_STATE_STARTING";
        case FSM_STATE_RUNNING:      
            return "FSM_STATE_RUNNING";
        case FSM_STATE_PAUSING:      
            return "FSM_STATE_PAUSING";
        case FSM_STATE_PAUSED:       
            return "FSM_STATE_PAUSED";
        case FSM_STATE_RESUMING:     
            return "FSM_STATE_RESUMING";
        case FSM_STATE_STOPPING:     
            return "FSM_STATE_STOPPING";
        case FSM_STATE_REBOOTING:
            return "FSM_STATE_REBOOTING";
        case FSM_STATE_ERROR:        
            return "FSM_STATE_ERROR";
        default:                     
            return "FSM_STATE_UNKNOWN";
    }
}
