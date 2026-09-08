#include <cJSON.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "broadcaster.h"

static const char *TAG = "[broadcaster/broadcaster.c]";

static int throttle_ms = 500;
static cJSON *root = NULL;           
static TimerHandle_t timer = NULL;
static bool timer_running = false;
static void (*sender)(const char *json, int client_index) = NULL;

static void send(int client_id) {
    if (!sender || !root) 
        return;
    // cJSON_AddNumberToObject(root, "ts", esp_timer_get_time() / 1000000);
    char *json = cJSON_PrintUnformatted(root);
    if (json) {
        sender(json, client_id);
        free(json);
    }
}

static void timer_cb(TimerHandle_t t) {
    send(-1);
    timer_running = false;
}

void broadcaster_init(
    void (*broadcast)(const char *, int client_index)
) {
    sender = broadcast;
    root = cJSON_CreateObject();
    timer = xTimerCreate(
        "broadcaster",
        pdMS_TO_TICKS(throttle_ms),
        pdFALSE,
        NULL,
        timer_cb
    );
    ESP_LOGI(TAG, "Broadcaster init with %d ms throttle", throttle_ms);
}

void broadcaster_on_client_connected_callback(int client_index) {
    send(client_index);
}

void broadcaster_publish(const char *key, const char *value) {
    if (!sender || !root) return;
    
    cJSON *existing = cJSON_GetObjectItem(root, key);
    if (existing) {
        cJSON_DeleteItemFromObject(root, key);
    }
    cJSON_AddStringToObject(root, key, value);
    
    if (!timer_running) {
        // send now
        send(-1);
        timer_running = true;
    } else {
        // delay send
        if (xTimerIsTimerActive(timer)) {
            xTimerStop(timer, 0);
        }
        xTimerStart(timer, 0);
    }
}
