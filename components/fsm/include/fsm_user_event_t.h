#ifndef FSM_USER_EVENT_H
#define FSM_USER_EVENT_H

typedef enum {
    FSM_USER_EVENT_START,
    FSM_USER_EVENT_PAUSE,
    FSM_USER_EVENT_RESUME,
    FSM_USER_EVENT_STOP,
    FSM_USER_EVENT_REBOOT
} fsm_user_event_t;

const char* fsm_user_event_to_str(fsm_user_event_t);

#endif // FSM_USER_EVENT_H
