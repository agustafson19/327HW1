#ifndef VERTEX_H
#define VERTEX_H

class vertex_t {
    public:
        uint8_t xpos;
        uint8_t ypos;
};

int vertex_equal(vertex_t v, vertex_t u);

#endif
