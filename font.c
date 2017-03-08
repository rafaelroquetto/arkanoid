#include <stdlib.h>
#include <stdio.h>
#include "font.h"
#include "texture.h"

static struct glyph *
load_glyph(const char *name, char ch)
{
    struct glyph *g;
    char path[256];

    g = malloc(sizeof *g);
    g->character = ch;

    snprintf(path, sizeof path, "fonts/%s/%c.png", name, ch);

    g->texture = load_texture_from_png(path, &g->width, &g->height);

    return g;
}

static void
load_glyph_range(const char *name, int i, int j, struct list *glyphs)
{
    struct glyph *g;
    int ch;

    for (ch = i; ch <= j; ch++) {
        g = load_glyph(name, ch);
        list_add(glyphs, (void *) g);
    }
}

static struct list *
load_glyphs(const char *name)
{
    struct list *glyphs;

    glyphs = list_new();

    load_glyph_range(name, '0', '9', glyphs);
    load_glyph_range(name, 'a', 'z', glyphs);
    load_glyph_range(name, 'A', 'Z', glyphs);

    return glyphs;
}

static void
glyph_destroy(struct glyph *g)
{
    glDeleteTextures(1, &g->texture);

    free(g);
}

struct font *
font_new(const char *name)
{
    struct font *f;

    f = malloc(sizeof *f);

    f->glyphs = load_glyphs(name);
    f->spacing = 10;

    return f;
}

struct glyph *
font_get_glyph(struct font *f, char ch)
{
    struct node *n;

    for (n = f->glyphs->first; n; n = n->next) {
        struct glyph *g = (struct glyph *) n->data;

        if (g->character == ch)
            return g;
    }

    return NULL;
}

void font_destroy(struct font *f)
{
    list_free(f->glyphs, (list_free_func) glyph_destroy);

    free(f);
}
