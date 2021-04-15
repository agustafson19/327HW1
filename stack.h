#ifndef STACK_H
#define STACK_H

#include "entity.h"

typedef class stack_node stack_node_t;

class stack_node {
    public:
        void *value;
        stack_node_t *next;
};

typedef class stack {
    public:
        stack_node_t *top;
        int size;
} stack_t;

int stack_init(stack_t *s);
int stack_delete(stack_t *s);
int stack_delete_characters(stack_t *s);
int stack_push(stack_t *s, void *value);
int stack_peek(stack_t *s, void **value);
int stack_pop(stack_t *s, void **value);
int stack_size(stack_t *s);

#endif
