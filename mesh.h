#ifndef MESH_H
#define MESH_H

#include <GL/gl.h>

#include "boundingbox.h"

struct model;

struct mesh
{
    GLuint vao;
    GLuint vbo;
    GLsizei vertex_count;

    struct bounding_box bounding_box;
};

struct mesh * mesh_load(const char *path);
struct mesh * mesh_new(struct model *model);

void mesh_free(struct mesh *m);

#endif /* MESH_H */
