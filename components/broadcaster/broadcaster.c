#include <cJSON.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

static const char *TAG = "[broadcaster/broadcaster.c]";

static int throttle_ms = 500;
static cJSON *root = NULL;           
static TimerHandle_t timer = NULL;
static int delay_ms = 500;
static bool timer_running = false;
static void (*sender)(const char *json) = NULL;

static void send(void) {
    if (!sender || !root) return;
    
    // Добавляем timestamp
    // cJSON_AddNumberToObject(root, "ts", esp_timer_get_time() / 1000000);
    
    // Отправляем
    char *json = cJSON_PrintUnformatted(root);
    if (json) {
        sender(json);
        free(json);
    }
    
    timer_running = false;
}

static void timer_cb(TimerHandle_t t) {
    send();
}

void broadcaster_init(void (*fn)(const char *)) {
    sender = fn;
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

void broadcaster_publish(const char *key, const char *value) {
    if (!sender || !root) return;
    
     // Обновляем или добавляем поле
    cJSON *existing = cJSON_GetObjectItem(root, key);
    if (existing) {
        cJSON_DeleteItemFromObject(root, key);
    }
    cJSON_AddStringToObject(root, key, value);
    
    if (!timer_running) {
        // send now
        timer_running = true;
        send();
    } else {
        // delay send
        if (xTimerIsTimerActive(timer)) {
            xTimerStop(timer, 0);
        }
        xTimerStart(timer, 0);
    }
}
