#ifndef FSM_EVENT_T_H
#define FSM_EVENT_T_H

#include "fsm_user_event_t.h"
#include "fsm_system_event_t.h"

typedef enum {
    FSM_EVENT_TYPE_USER_COMMAND,
    FSM_EVENT_TYPE_SYSTEM_REPORT
} fsm_event_type_t;

typedef struct {
    fsm_event_type_t type;
    fsm_user_event_t user_event;
    fsm_system_event_t system_event;
    void* arg;
} fsm_event_t;

#endif // FSM_EVENT_T_H
