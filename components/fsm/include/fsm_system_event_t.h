#ifndef FSM_SYSTEM_EVENT_H
#define FSM_SYSTEM_EVENT_H

typedef enum {
    FSM_SYSTEM_EVENT_BOOT_INIT_OK,      
    FSM_SYSTEM_EVENT_HOMING_DONE,   
    FSM_SYSTEM_EVENT_RAMP_UP_DONE,  
    FSM_SYSTEM_EVENT_RAMP_DOWN_DONE,    
    FSM_SYSTEM_EVENT_TRACK_END,
    FSM_SYSTEM_EVENT_ERROR
} fsm_system_event_t;

const char* fsm_system_event_to_str(fsm_system_event_t);

#endif // FSM_SYSTEM_EVENT_H
