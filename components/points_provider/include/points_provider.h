#ifndef POINTS_PROVIDER_H
#define POINTS_PROVIDER_H

void points_provider_init(
    int capacity, 
    int batch_size, 
    QueueHandle_t queue, 
    SemaphoreHandle_t sync
);

#endif