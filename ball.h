#ifndef BALL_H
#define BALL_H

#include <GL/gl.h>

#include "linmath.h"
#include "boundingbox.h"

struct mesh;

struct ball
{
    GLfloat x;
    GLfloat y;
    GLfloat z;

    struct mesh *mesh;

    mat4x4 model_matrix;
    mat4x4 normal_matrix;

    struct bounding_box box;
};

struct ball * ball_new(void);

void ball_free(struct ball *p);
void ball_draw(void *object, GLuint shader_program);
void ball_update(void *object);

#endif /* BALL_H */
