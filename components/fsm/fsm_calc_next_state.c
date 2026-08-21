#include "fsm_state_t.h"
#include "fsm_event_t.h"

fsm_state_t handle_system_event(
    const fsm_state_t state, 
    const fsm_system_event_t event
) {
    if (event == FSM_SYSTEM_EVENT_ERROR) {
        return FSM_STATE_ERROR;
    }
    switch (state) {

        case FSM_STATE_INITIALIZING:
            if (event == FSM_SYSTEM_EVENT_BOOT_INIT_OK)
                return FSM_STATE_UNHOMED_IDLE;
            break;   

        case FSM_STATE_HOMING:
            if (event == FSM_SYSTEM_EVENT_HOMING_DONE)
                return FSM_STATE_IDLE;
            break;
        
        case FSM_STATE_STARTING:
            if (event == FSM_SYSTEM_EVENT_RAMP_UP_DONE)
                return FSM_STATE_RUNNING;
            break;    
        
        case FSM_STATE_RESUMING:
            if (event == FSM_SYSTEM_EVENT_RAMP_UP_DONE)
                return FSM_STATE_RUNNING;
            break;     

        case FSM_STATE_PAUSING:
            if (event == FSM_SYSTEM_EVENT_RAMP_DOWN_DONE)
                return FSM_STATE_PAUSED;
            break;

        case FSM_STATE_STOPPING:
            if (event == FSM_SYSTEM_EVENT_RAMP_DOWN_DONE)
                return FSM_STATE_UNHOMED_IDLE;
            break; 

        case FSM_STATE_RUNNING:
            if (event == FSM_SYSTEM_EVENT_TRACK_END)
                return FSM_STATE_STOPPING;
            break;     

        default: break;
    }

    return state;
}

fsm_state_t handle_user_event(
    const fsm_state_t state, 
    const fsm_user_event_t event
) {
    if (event == FSM_USER_EVENT_REBOOT) {
        return FSM_STATE_REBOOTING;
    }
    switch (state) {

        case FSM_STATE_RUNNING:
            if (event == FSM_USER_EVENT_STOP)
                return FSM_STATE_STOPPING;
            if (event == FSM_USER_EVENT_PAUSE)
                return FSM_STATE_PAUSING;    
            break;  

        case FSM_STATE_IDLE:
            if (event == FSM_USER_EVENT_START)
                return FSM_STATE_STARTING;
            if (event == FSM_USER_EVENT_START_HOMIG)   
                return FSM_STATE_HOMING; 
            break;
        
        case FSM_STATE_UNHOMED_IDLE:
            if (event == FSM_USER_EVENT_START_HOMIG)
                return FSM_STATE_HOMING;
            break;    

        case FSM_STATE_PAUSED:
            if (event == FSM_USER_EVENT_RESUME)
                return FSM_STATE_RESUMING;
            break;  

        default: break;
    }   

    return state;
}

fsm_state_t fsm_calc_next_state(
    const fsm_state_t state, 
    const fsm_event_t* event
) {
    if (event->type == FSM_EVENT_TYPE_SYSTEM_REPORT) {
        return handle_system_event(state, event->system_event);    
    } else {
        return handle_user_event(state, event->user_event);       
    }
}
