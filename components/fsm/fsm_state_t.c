#include <fsm_state_t.h>

const char* fsm_state_to_str(fsm_state_t state) {
    switch (state) {
        case FSM_STATE_INITIALIZING:    
            return "INITIALIZING";
        case FSM_STATE_UNHOMED: 
            return "UNHOMED";
        case FSM_STATE_HOMING:       
            return "HOMING";
        case FSM_STATE_IDLE:         
            return "IDLE";
        case FSM_STATE_STARTING:     
            return "STARTING";
        case FSM_STATE_RUNNING:      
            return "RUNNING";
        case FSM_STATE_PAUSING:      
            return "PAUSING";
        case FSM_STATE_PAUSED:       
            return "PAUSED";
        case FSM_STATE_RESUMING:     
            return "RESUMING";
        case FSM_STATE_STOPPING:     
            return "STOPPING";
        case FSM_STATE_REBOOTING:
            return "REBOOTING";
        case FSM_STATE_ERROR:        
            return "ERROR";
        default:                     
            return "FSM_STATE_UNKNOWN";
    }
}
