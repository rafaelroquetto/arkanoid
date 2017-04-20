#include <string.h>
#include <stdlib.h>

#include "panic.h"
#include "label.h"
#include "linmath.h"
#include "shader.h"
#include "objects.h"
#include "constants.h"

static GLuint glyphVAO(void)
{
    static int loaded = 0;
    static GLuint vao;

    static GLfloat vertices[] = {
         0.5f,  0.5f, 0.0f,     1.0f, 0.0f, /* top-right */
         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, /* bottom-right */
        -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, /* bottom-left */
        -0.5f,  0.5f, 0.0f,     0.0f, 0.0f  /* top-left */
    };

    static GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    if (loaded)
        return vao;

    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLuint ebo;
    glGenBuffers(1, &ebo);

    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(VERTEX_VAO, 3, GL_FLOAT, GL_FALSE,
            5 * sizeof (GLfloat), (GLvoid *) 0);

    glEnableVertexAttribArray(VERTEX_VAO);

    glVertexAttribPointer(TEXTURE_VAO, 2, GL_FLOAT, GL_FALSE,
            5 * sizeof (GLfloat), (GLvoid *) (3 * sizeof (GLfloat)));

    glEnableVertexAttribArray(TEXTURE_VAO);

    glBindVertexArray(0);

    loaded = 1;

    return vao;
}

static struct glyph * find_glyph(const struct list *glyphs, char ch)
{
    for (struct node *n = glyphs->first; n; n = n->next) {
        struct glyph *g = (struct glyph *) n->data;

        if (g->character == ch)
            return g;
    }

    return NULL;
}

static void glyph_draw(const struct glyph *g, GLfloat x, GLfloat y, GLuint shader_program)
{
    mat4x4 model_matrix;
    mat4x4 normal_matrix;

    mat4x4_identity(model_matrix);
    mat4x4_translate_in_place(model_matrix, x, y, -1.0);
    //mat4x4_scale_aniso(model_matrix, model_matrix, 3 , 3 , 3 );

    mat4x4_invert(normal_matrix, model_matrix);
    mat4x4_transpose(normal_matrix, normal_matrix);

    shader_set_uniform_m4(shader_program, "model", model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", normal_matrix);
    shader_set_uniform_i(shader_program, "objectType", OBJECT_GLYPH);

    glBindTexture(GL_TEXTURE_2D, g->texture);
    glBindVertexArray(glyphVAO());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

struct label *
label_new(const char *text, GLfloat x, GLfloat y,
        struct font *f)
{
    struct label *l;

    l = malloc(sizeof *l);

    l->x = x;
    l->y = y;
    l->font = f;
    l->text = strdup(text);

    return l;
}

void label_draw(void *object, GLuint shader_program)
{
    struct label *l = (struct label *) object;
    char *ch = l->text;
    float x = l->x;

    while (*ch) {
        struct glyph *g;
        g = find_glyph(l->font->glyphs, *ch);
        glyph_draw(g, x, l->y, shader_program);
        ++ch;
        x += 1.2f;
    }
}

void label_destroy(struct label *l)
{
    free(l->text);
    free(l);
}

void label_set_text(struct label *l, const char *text)
{
    free(l->text);
    l->text = strdup(text);
}
