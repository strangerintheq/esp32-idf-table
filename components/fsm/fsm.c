#include "fsm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "fsm_event_t.h"

static const char *TAG = "[fsm/fsm.c]";
static fsm_state_t current_state;
static QueueHandle_t xFsmQueue = NULL;

extern fsm_state_t fsm_calc_next_state(
    const fsm_state_t state, 
    const fsm_event_t* event
);

static void (*fsm_publish_state)(fsm_state_t);

static void fsm_process_message(const fsm_event_t* event) {
    
    const char* evt = event->type == FSM_EVENT_TYPE_USER_COMMAND ? 
        fsm_user_event_to_str(event->user_event) : 
        fsm_system_event_to_str(event->system_event);

    ESP_LOGI(TAG, "processing event: %s", evt);

    fsm_state_t next_state = fsm_calc_next_state(current_state, event);

    if (next_state != current_state) {

        const char* next_state_str = fsm_state_to_str(next_state);
        ESP_LOGI(TAG, "%s -> %s", fsm_state_to_str(current_state), next_state_str);

        current_state = next_state;
        
        fsm_publish_state(current_state);
    }
}

static void fsm_task(void *pvParameters) {
    fsm_event_t event;
    while (1) {
        if (xQueueReceive(xFsmQueue, &event, portMAX_DELAY) == pdTRUE) {
            fsm_process_message(&event);
        }
    }
}

void fsm_init(void (*publish_state)(fsm_state_t)) {
    fsm_publish_state = publish_state;
    xFsmQueue = xQueueCreate(10, sizeof(fsm_event_t));
    current_state = FSM_STATE_INITIALIZING;
    xTaskCreatePinnedToCore(fsm_task, "fsm_task", 4096, NULL, 15, NULL, 1);
    ESP_LOGI(TAG, "initialized");
}

void fsm_post_user_event(fsm_user_event_t event, void* arg) {
    if (xFsmQueue == NULL) 
        return;
    fsm_event_t msg = { 
        .type = FSM_EVENT_TYPE_USER_COMMAND, 
        .user_event = event, 
        .arg = arg 
    };
    xQueueSend(xFsmQueue, &msg, 0);
}

void fsm_post_system_event(fsm_system_event_t event, void* arg) {
    if (xFsmQueue == NULL) 
        return;
    fsm_event_t msg = { 
        .type = FSM_EVENT_TYPE_SYSTEM_REPORT, 
        .system_event = event, 
        .arg = arg 
    };
    xQueueSend(xFsmQueue, &msg, 0);
}
