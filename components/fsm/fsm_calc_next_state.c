#include "fsm_state_t.h"
#include "fsm_event_t.h"

fsm_state_t handle_system_event(
    const fsm_state_t state, 
    const fsm_system_event_t event
) {
    if (event == FSM_SYSTEM_EVENT_ERROR) {
        return FSM_STATE_ERROR;
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
