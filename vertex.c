#include "vertex.h"

int vertex_equal(vertex_t v, vertex_t u)
{
    return v.xpos == u.xpos && v.ypos == u.ypos;
}
