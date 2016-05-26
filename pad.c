#include <stdlib.h>
#include <GLES3/gl3.h>

#include "linmath.h"

struct pad
{
    GLuint vao;
    GLuint vbo;
};

static struct pad *pad = NULL;

static struct pad *
setup_pad(void)
{
    static GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    /* create vertex store buffer */
    GLuint vbo;
    glGenBuffers(1, &vbo);

    /* setup a vao */
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    /*  - location of position vertex attrib
     *  - size of vertex attrib (vec3)
     *  - data type
     *  - normalized? no
     *  - stride
     *  - data offset in the buffer
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof (GLfloat), (GLvoid *) 0);

    /* place data on shader location 0 */
    glEnableVertexAttribArray(0);

    /* unbind vao */
    glBindVertexArray(0);

    struct pad *t = malloc(sizeof *t);
    t->vao = vao;
    t->vbo = vbo;

    return t;
}

static void
free_pad(struct pad *t)
{
    glDeleteVertexArrays(1, &t->vao);
    glDeleteBuffers(1, &t->vbo);

    free(t);
}

void
draw_pad(GLuint shader_program)
{
    /* FIXME */
    if (pad == NULL)
        pad = setup_pad();

    //XXX FIXME
    static mat4x4 model_matrix;
    static int i = 0;

    if (i == 0) {
        mat4x4_identity(model_matrix);
        i = 1;
    }

    mat4x4_rotate_X(model_matrix, model_matrix, 0.1);

    GLint transform = glGetUniformLocation(shader_program, "transform");
    glUniformMatrix4fv(transform, 1, GL_FALSE, (const GLfloat *) model_matrix);

    //------------------------

    glBindVertexArray(pad->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

