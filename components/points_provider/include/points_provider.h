#ifndef POINTS_PROVIDER_H
#define POINTS_PROVIDER_H

void points_provider_init(int capacity, QueueHandle_t queue, SemaphoreHandle_t sync);

#endif