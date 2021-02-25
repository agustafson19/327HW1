#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdlib.h>
#include <stdint.h>

#ifndef VERTEX_H
#include "vertex.c"
#endif

typedef struct priority_queue_node priority_queue_node_t;

struct priority_queue_node {
    vertex_t value;
    uint16_t priority;
};

typedef struct priority_queue {
    priority_queue_node_t *nodes;
    uint16_t length;
    uint16_t size;
} priority_queue_t;

int priority_queue_init(priority_queue_t *q);
int priority_queue_delete(priority_queue_t *q);
int priority_queue_add(priority_queue_t *q, vertex_t v, uint16_t priority);
int priority_queue_extract_min(priority_queue_t *q, vertex_t *v);
int priority_queue_decrease_priority(priority_queue_t *q, vertex_t v, uint16_t priority);
int priority_queue_size(priority_queue_t *q);

#endif
