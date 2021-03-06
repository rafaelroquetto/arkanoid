#ifndef LABEL_H
#define LABEL_H

#include "font.h"

struct label
{
    struct font *font;
    char *text;
    int visible;

    GLfloat x;
    GLfloat y;
};

struct label *
label_new(const char *text, GLfloat x, GLfloat y,
        struct font *f);

void label_draw(void *object, GLuint shader_program);
void label_destroy(struct label *l);
void label_set_text(struct label *l, const char *text);
void label_set_visible(struct label *l, int visible);

#endif /* LABEL_H */
