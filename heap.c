#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vertex.h"
#include "heap.h"

int heap_init(heap_t *h)
{
    h->nodes = malloc(sizeof(heap_node_t) * 20);
    h->length = 20;
    h->size = 0;
    return 0;
}

int heap_delete(heap_t *h)
{
    uint16_t i;
    for (i = 0; i < h->size; i++) {
        free(h->nodes[i].value);
    }
    free(h->nodes);
    return 0;
}

int heap_add(heap_t *h, void *value, uint16_t priority)
{
    int i = h->size;
    heap_node_t temp;
    h->size++;
    if (h->size >= h->length) {
        h->length *= 2;
        h->nodes = realloc(h->nodes, sizeof(heap_node_t) * h->length);
    }
    h->nodes[i].value = value;
    h->nodes[i].priority = priority;
    while (i > 0 && h->nodes[i].priority < h->nodes[(i-1)/2].priority) {
        temp = h->nodes[i];
        h->nodes[i] = h->nodes[(i-1)/2];
        h->nodes[(i-1)/2] = temp;
        i = (i-1)/2;
    }
    return 0;
}

int heap_decrease_priority_vertex(heap_t *h, void *value, uint16_t priority)
{
    int i;
    heap_node_t temp;
    for (i = 0; i < h->size; i++) {
        if (vertex_equal(*((vertex_t*) value), *((vertex_t*) h->nodes[i].value)))
            break;
    }
    if (i >= h->size)
        return 1;
    h->nodes[i].priority = priority;
    while (i > 0 && h->nodes[i].priority < h->nodes[(i-1)/2].priority) {
        temp = h->nodes[i];
        h->nodes[i] = h->nodes[(i-1)/2];
        h->nodes[(i-1)/2] = temp;
        i = (i-1)/2;
    }
    return 0;
}

int heap_peek(heap_t *h, void **value, uint16_t *priority)
{
    if (h->size == 0)
        return 1;
    *value = h->nodes[0].value;
    *priority = h->nodes[0].priority;
    return 0;
}

int heap_extract_min(heap_t *h, void **value)
{
    int i = 0, j;
    heap_node_t temp;
    if (h->size <= 0) {
        return 1;
    }
    *value = h->nodes[0].value;
    h->size--;
    h->nodes[0] = h->nodes[h->size];
    while (2*i+1 < h->size) {
        if (2*i+2 < h->size) {
            j = h->nodes[2*i+1].priority < h->nodes[2*i+2].priority ? 2*i+1 : 2*i + 2;
        }
        else {
            j = 2*i+1;
        }
        if (h->nodes[i].priority < h->nodes[j].priority) {
            break;
        }
        temp = h->nodes[i];
        h->nodes[i] = h->nodes[j];
        h->nodes[j] = temp;
        i = j;
    }
    if (h->length > 20 && h->length > h->size*4) {
        h->length /= 2;
        h->nodes = realloc(h->nodes, sizeof(h->nodes) * h->length);
    }
    return 0;
}

int heap_size(heap_t *h)
{
    return h->size;
}
