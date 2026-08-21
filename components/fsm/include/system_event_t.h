#ifndef SYSTEM_EVENT_H
#define SYSTEM_EVENT_H

typedef enum {
    SYSTEM_EVENT_BOOT_INIT_OK,      
    SYSTEM_EVENT_HOMING_DONE,   
    SYSTEM_EVENT_RAMP_UP_DONE,  
    SYSTEM_EVENT_RAMP_DOWN_DONE,    
    SYSTEM_EVENT_TRACK_END,
    SYSTEM_EVENT_ERROR
} system_event_t;

const char* system_event_to_str(system_event_t);

#endif // SYSTEM_EVENT_H
