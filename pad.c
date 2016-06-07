#include <stdlib.h>
#include <GL/gl.h>

#include "pad.h"
#include "linmath.h"

struct pad *
pad_new(void)
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

void
pad_free(struct pad *t)
{
    glDeleteVertexArrays(1, &t->vao);
    glDeleteBuffers(1, &t->vbo);

    free(t);
}

void
draw_pad(void *object, GLuint shader_program)
{
    struct pad *pad = (struct pad *) object;

    mat4x4 model_matrix;
    mat4x4_translate(model_matrix, 0.0, 0.0, 0.0);

    GLint transform = glGetUniformLocation(shader_program, "model");
    printf("model: %d\n", transform);
    glUniformMatrix4fv(transform, 1, GL_FALSE, (const GLfloat *) model_matrix);

    //------------------------


    //XXX VIEW

    mat4x4 view_matrix;
    mat4x4_translate(view_matrix, 0.0, 0.0, 0.0);

    GLint view = glGetUniformLocation(shader_program, "view");
    printf("view: %d\n", view);
    glUniformMatrix4fv(view, 1, GL_FALSE, (const GLfloat *) view_matrix);

    //------------------------

    //XXX PROJECTION

    mat4x4 projection_matrix;
    mat4x4_perspective(projection_matrix, M_PI/4, 800/600, 0.1f, 100.f);
    //mat4x4_ortho(projection_matrix, 0.0, 800.0, 0.0, 600.0, 0.1, 100.0);

    GLint projection = glGetUniformLocation(shader_program, "projection");
    printf("projection: %d\n", projection);
    glUniformMatrix4fv(view, 1, GL_FALSE, (const GLfloat *) projection_matrix);

    //------------------------

    glBindVertexArray(pad->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

