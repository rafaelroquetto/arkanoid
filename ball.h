#ifndef BALL_H
#define BALL_H

#include <GL/glew.h>

#include "linmath.h"
#include "boundingbox.h"

struct mesh;

struct ball
{
    GLfloat x;
    GLfloat y;
    GLfloat z;

    GLfloat angle;

    vec2 direction;

    GLfloat speed;

    struct mesh *mesh;

    mat4x4 model_matrix;
    mat4x4 normal_matrix;

    struct bounding_box box;
};

struct ball * ball_new(void);

void ball_free(struct ball *p);
void ball_draw(void *object, GLuint shader_program);
void ball_update(void *object);
void ball_set_direction(struct ball *b, float angle);
void ball_set_speed(struct ball *b, float speed);
void ball_reset_direction(struct ball *b);
#endif /* BALL_H */
