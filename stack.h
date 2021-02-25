#ifndef STACK_H
#define STACK_H

#ifndef VERTEX_H
#include "vertex.c"
#endif

typedef struct stack_node stack_node_t;

struct stack_node {
    vertex_t value;
    stack_node_t *next;
};

typedef struct stack {
    stack_node_t *top;
    int size;
} stack_t;

int stack_init(stack_t *s);
int stack_delete(stack_t *s);
int stack_push(stack_t *s, vertex_t v);
int stack_peek(stack_t *s, vertex_t *v);
int stack_pop(stack_t *s, vertex_t *v);
int stack_size(stack_t *s);

#endif
