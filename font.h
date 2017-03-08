#ifndef FONT_H
#define FONT_H

#include <GL/glew.h>
#include "list.h"

struct glyph
{
    GLuint texture;

    int width;
    int height;
    char character;
};

struct font
{
    struct list *glyphs;
    int spacing;
};

struct font *
font_new(const char *name);

struct glyph *
font_get_glyph(struct font *f, char ch);

void font_destroy(struct font *f);
#endif /* FONT_H */
