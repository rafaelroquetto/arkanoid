#ifndef PAD_H
#define PAD_H

struct pad
{
    GLuint vao;
    GLuint vbo;
};

struct pad * pad_new(void);

void pad_free(struct pad *t);
void draw_pad(void *object, GLuint shader_program);

#endif /* PAD_H */
