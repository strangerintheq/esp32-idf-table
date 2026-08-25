#ifndef FSM_H
#define FSM_H

#include <fsm_state_t.h>
#include <fsm_user_event_t.h>
#include <fsm_system_event_t.h>

typedef struct {
    void (*publish_state)(fsm_state_t);
} fsm_init_t;

void fsm_init(fsm_init_t*);

void fsm_post_user_event(fsm_user_event_t event, void* arg);

void fsm_post_system_event(fsm_system_event_t event, void* arg);

#endif // FSM_H
