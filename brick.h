#ifndef BRICK_H
#define BRICK_H

#include <GL/glew.h>

#include "linmath.h"
#include "boundingbox.h"

struct mesh;

typedef enum {
    NORMAL,
    HARD
} brick_type;

struct brick
{
    GLfloat x;
    GLfloat y;
    GLfloat z;

    GLboolean alive;

    struct mesh *mesh;

    mat4x4 model_matrix;
    mat4x4 normal_matrix;

    struct bounding_box box;

    brick_type type;
};

struct brick * brick_new(void);

void brick_free(struct brick *p);
void brick_draw(void *object, GLuint shader_program);
void brick_update(void *object);
void brick_set_alive(struct brick *b, GLboolean alive);

#endif /* BRICK_H */
