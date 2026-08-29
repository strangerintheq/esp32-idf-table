#ifndef POINTS_PROVIDER_H
#define POINTS_PROVIDER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

void points_provider_start();

void points_provider_pause();

void points_provider_unpause();

void points_provider_stop();

void points_provider_init(
    int capacity, 
    int batch_size, 
    QueueHandle_t queue, 
    SemaphoreHandle_t sync
);

#endif