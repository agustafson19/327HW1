#ifndef VERTEX_H
#define VERTEX_H

typedef class vertex {
    public:
        uint8_t xpos;
        uint8_t ypos;
} vertex_t;

int vertex_equal(vertex_t v, vertex_t u);

#endif
