#include <stdlib.h>
#include <GL/gl.h>

#include "pad.h"
#include "linmath.h"
#include "constants.h"

static const GLfloat PAD_THROTTLE = 0.1f;
static const GLfloat MAX_SPEED = 0.3f;
static const GLfloat PAD_FRICTION = 0.01f;

static int
fuzzy_compare(double a, double b)
{
    static const double EPSILON = 0.00001;

    return fabs(a - b) < EPSILON;
}

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

    struct pad *p = malloc(sizeof *p);
    p->vao = vao;
    p->vbo = vbo;
    p->x = 0.0;
    p->speed = 0.0;

    return p;
}

void
pad_free(struct pad *p)
{
    glDeleteVertexArrays(1, &p->vao);
    glDeleteBuffers(1, &p->vbo);

    free(p);
}

void
pad_draw(void *object, GLuint shader_program)
{
    struct pad *pad = (struct pad *) object;

    mat4x4 model_matrix;
    mat4x4_translate(model_matrix, pad->x, 0.0, 0.0);

    GLint transform = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(transform, 1, GL_FALSE, (const GLfloat *) model_matrix);

    //------------------------


    //XXX VIEW

    mat4x4 view_matrix;
    mat4x4_translate(view_matrix, 0.0, -6.5, -20.0);

    GLint view = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view, 1, GL_FALSE, (const GLfloat *) view_matrix);

    //------------------------

    //XXX PROJECTION

    mat4x4 projection_matrix;
    mat4x4_perspective(projection_matrix, M_PI/4, VIEWPORT_WIDTH/VIEWPORT_HEIGHT, 0.1f, 100.f);
    //mat4x4_ortho(projection_matrix, 0.0, 800.0, 0.0, 600.0, 0.1, 100.0);

    GLint projection = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(projection, 1, GL_FALSE, (const GLfloat *) projection_matrix);

    //------------------------

    glBindVertexArray(pad->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

static void
pad_deaccel(struct pad *p)
{
    int signal;

    if (fuzzy_compare(p->speed, 0.0))
        return;

    signal = (p->speed > 0) ? 1 : -1;

    p->speed -= signal*PAD_FRICTION;

    if (fabs(p->speed) <= 0.0f)
        p->speed = 0.0f;
}

void
pad_update(void *object)
{
    struct pad *p = (struct pad *) object;

    p->x += p->speed;

    pad_deaccel(p);
}

void
pad_throttle_left(struct pad *p)
{
    p->speed -= PAD_THROTTLE;

    if (p->speed < -MAX_SPEED)
        p->speed = -MAX_SPEED;
}

void
pad_throttle_right(struct pad *p)
{
    p->speed += PAD_THROTTLE;

    if (p->speed > MAX_SPEED)
        p->speed = MAX_SPEED;
}

