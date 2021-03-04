#ifndef HEAP_H
#define HEAP_H

typedef struct heap_node heap_node_t;

struct heap_node {
    void *value;
    uint32_t priority;
};

typedef struct heap {
    heap_node_t *nodes;
    uint32_t length;
    uint32_t size;
} heap_t;

int heap_init(heap_t *h);
int heap_delete(heap_t *h);
int heap_add(heap_t *h, void *value, uint32_t priority);
int heap_decrease_priority_vertex(heap_t *h, void *value, uint32_t priority);
int heap_peek(heap_t *h, void **value, uint32_t *priority);
int heap_extract_min(heap_t *h, void **value);
int heap_size(heap_t *h);

#endif
