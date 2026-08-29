
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static QueueHandle_t out_queue = NULL;
static SemaphoreHandle_t sync_semaphore = NULL;
static int queue_capacity;
static int queue_batch_size;

static void points_provider_task(void *pvParameters) {
    while (1) {
        
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

void points_provider_init(
    int capacity,
    int batch_size,
    QueueHandle_t queue, 
    SemaphoreHandle_t sync
) {
    out_queue = queue;
    sync_semaphore = sync;
    queue_capacity = capacity;
    queue_batch_size = batch_size;

    xTaskCreatePinnedToCore(points_provider_task, "points_provider_task", 4096, NULL, 15, NULL, 1);
}

void points_provider_start() {

}

void points_provider_pause() {

}

void points_provider_unpause() {

}

void points_provider_stop() {

}