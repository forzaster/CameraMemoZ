//
// Created by n-naka on 2017/01/21.
//

#ifndef Z_VERTEX_H
#define Z_VERTEX_H

#include <vector>
#include "gl_lib.h"

struct Vertex {
    GLfloat pos[2];
    GLubyte rgba[4];
    GLfloat uv[2];
};

#endif //Z_VERTEX_H
