#include "priority_queue.h"

int priority_queue_init(priority_queue_t *q)
{
    q->nodes = malloc(sizeof(q->nodes)*20);
    q->length = 20;
    q->size = 0;
    return 0;
}

int priority_queue_delete(priority_queue_t *q)
{
    free(q->nodes);
    return 0;
}

int priority_queue_add(priority_queue_t *q, vertex_t v, uint16_t priority)
{
    int i = q->size;
    priority_queue_node_t temp;
    q->size++;
    if (q->size >= q->length) {
        q->length *= 2;
        q->nodes = realloc(q->nodes, sizeof(q->nodes) * q->length);
    }
    q->nodes[i].value = v;
    q->nodes[i].priority = priority;
    while (i > 0 && q->nodes[i].priority < q->nodes[(i-1)/2].priority) {
        temp = q->nodes[i];
        q->nodes[i] = q->nodes[(i-1)/2];
        q->nodes[(i-1)/2] = temp;
        i = (i-1)/2;
    }
    return 0;
}

int priority_queue_extract_min(priority_queue_t *q, vertex_t *v, uint16_t *priority)
{
    int i = 0, j;
    priority_queue_node_t temp;
    if (q->size <= 0) {
        return 1;
    }
    *v = q->nodes[0].value;
    *priority = q->nodes[0].priority;
    q->size--;
    q->nodes[0] = q->nodes[q->size];
    while (2*i+1 < q->size) {
        if (2*i+2 < q->size) {
            j = q->nodes[2*i+1].priority < q->nodes[2*i+2].priority ? 2*i+1 : 2*i + 2;
        }
        else {
            j = 2*i+1;
        }
        if (q->nodes[i].priority < q->nodes[j].priority) {
            break;
        }
        temp = q->nodes[i];
        q->nodes[i] = q->nodes[j];
        q->nodes[j] = temp;
        i = j;
    }
    if (q->length > 20 && q->length > q->size*4) {
        q->length /= 2;
        q->nodes = realloc(q->nodes, sizeof(q->nodes) * q->length);
    }
    return 0;
}

int priority_queue_decrease_priority(priority_queue_t *q, vertex_t v, uint16_t priority) {
    int i;
    priority_queue_node_t temp;
    for (i = 0; i < q->size; i++) {
        if (vertex_equal(v, q->nodes[i].value))
            break;
    }
    if (i >= q->size)
        return 1;
    q->nodes[i].priority = priority;
    while (i > 0 && q->nodes[i].priority < q->nodes[(i-1)/2].priority) {
        temp = q->nodes[i];
        q->nodes[i] = q->nodes[(i-1)/2];
        q->nodes[(i-1)/2] = temp;
        i = (i-1)/2;
    }
    return 0;
}

int priority_queue_size(priority_queue_t *q)
{
    return q->size;
}
