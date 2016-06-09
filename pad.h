#ifndef PAD_H
#define PAD_H

#include <GL/gl.h>

struct pad
{
    GLfloat x;
    GLfloat speed;
    GLint angle;

    GLuint vao;
    GLuint vbo;
    GLsizei vertex_count;
};

struct pad * pad_new(void);

void pad_free(struct pad *p);
void pad_draw(void *object, GLuint shader_program);
void pad_update(void *object);
void pad_throttle_left(struct pad *p);
void pad_throttle_right(struct pad *p);
void pad_rotate_x(struct pad *p);

#endif /* PAD_H */
