#include <fsm.h>
#include <broadcaster.h>
#include <stdio.h>

static void fsm_state_changed(fsm_state_t state) {
    broadcaster_publish("systemState", fsm_state_to_str(state));
}

void delay();

void state_machine_init() {

    fsm_init(fsm_state_changed);

    delay();
    fsm_post_system_event(FSM_SYSTEM_EVENT_BOOT_INIT_OK, NULL);
    delay();
    fsm_post_user_event(FSM_USER_EVENT_START_HOMING, NULL);
    delay();
    fsm_post_system_event(FSM_SYSTEM_EVENT_HOMING_DONE, NULL);
}