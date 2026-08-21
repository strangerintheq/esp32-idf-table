#ifndef FSM_H
#define FSM_H

#include <fsm_state_t.h>
#include <fsm_user_event_t.h>
#include <fsm_system_event_t.h>

void fsm_init(void);

void fsm_post_user_event(fsm_user_event_t event, void* arg);

void fsm_post_system_event(fsm_system_event_t event, void* arg);

fsm_state_t fsm_get_current_state(void);

#endif // FSM_H
