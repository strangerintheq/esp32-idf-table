#ifndef STEPPERS_H
#define STEPPERS_H

void steppers_init(int capacity, QueueHandle_t queue, SemaphoreHandle_t sync);

#endif