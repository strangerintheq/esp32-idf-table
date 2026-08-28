
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static QueueHandle_t xOutQueue = NULL;
static SemaphoreHandle_t xSyncSem = NULL;
static int queue_capacity;

static void points_provider_task(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

void points_provider_init(int capacity, QueueHandle_t queue, SemaphoreHandle_t sync) {
    xOutQueue = queue;
    xSyncSem = sync;
    queue_capacity = capacity;
    xTaskCreatePinnedToCore(points_provider_task, "points_provider_task", 4096, NULL, 15, NULL, 1);
}

