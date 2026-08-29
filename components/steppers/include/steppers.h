#ifndef STEPPERS_H
#define STEPPERS_H

typedef struct {
    float angle;
    float radius;
    uint32_t speed;
} polar_point_t;

void steppers_init(
    int capacity,
    int batch_size,
    QueueHandle_t in, 
    SemaphoreHandle_t sync
);

#endif