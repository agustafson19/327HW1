#include <stdlib.h>

#include "stack.h"

int stack_init(stack_t *s)
{
    s->top = NULL;
    s->size = 0;
    return 0;
}

int stack_delete(stack_t *s)
{
    stack_node_t *n;
    for (n = s->top; n; n = s->top) {
        s->top = n->next;
        free(n->value);
        free(n);
    }
    return 0;
}

int stack_delete_characters(stack_t *s)
{
    stack_node_t *n;
    for (n = s->top; n; n = s->top) {
        s->top = n->next;
        free(((character_t *)n->value)->inventory);
        free(((character_t *)n->value)->equipment);
        free(n->value);
        free(n);
    }
    return 0;
}

int stack_push(stack_t *s, void *value)
{
    stack_node_t *n;
    if (!(n = (stack_node_t *) malloc(sizeof(*n)))) {
        return 1;
    }
    n->value = value;
    n->next = s->top;
    s->top = n;
    s->size++;
    return 0;
}

int stack_peek(stack_t *s, void **value)
{
    if (!s->top) {
        return 1;
    }
    *value = s->top->value;
    return 0;
}

int stack_pop(stack_t *s, void **value)
{
    stack_node_t *n;
    if (!s->top) {
        return 1;
    }
    n = s->top;
    s->top = n->next;
    s->size--;
    *value = n->value;
    free(n);
    return 0;
}

int stack_size(stack_t *s)
{
    return s->size;
}
