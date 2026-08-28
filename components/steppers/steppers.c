
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static QueueHandle_t xInQueue = NULL;
static SemaphoreHandle_t xSyncSem = NULL;
static int queue_capacity;

static void steppers_task(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

void steppers_init(int capacity, QueueHandle_t queue, SemaphoreHandle_t sync) {
    xInQueue = queue;
    xSyncSem = sync;
    queue_capacity = capacity;
    xTaskCreatePinnedToCore(steppers_task, "steppers_task", 4096, NULL, 15, NULL, 1);
}