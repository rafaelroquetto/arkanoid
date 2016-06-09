#include <stdlib.h>
#include <GL/gl.h>

#include "pad.h"
#include "linmath.h"
#include "constants.h"
#include "shader.h"
#include "model.h"

static const GLfloat PAD_THROTTLE = 0.1f;
static const GLfloat MAX_SPEED = 0.3f;
static const GLfloat PAD_FRICTION = 0.01f;

static inline
float deg_to_rad(float deg)
{
    return M_PI*deg/180.f;
}

static int
fuzzy_compare(double a, double b)
{
    static const double EPSILON = 0.00001;

    return fabs(a - b) < EPSILON;
}

static struct model *
pad_model(void)
{
    static struct model *m = NULL;

    if (m != NULL)
        return m;

    m = load_model("resource/pad.obj");

    return m;
}

struct pad *
pad_new(void)
{
    struct model *model = pad_model();

    /* create vertex store buffer */
    GLuint vbo;
    glGenBuffers(1, &vbo);

    /* setup a vao */
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, model->csize, model->coords, GL_STATIC_DRAW);

    /*  - location of position vertex attrib
     *  - size of vertex attrib (vec3)
     *  - data type
     *  - normalized? no
     *  - stride
     *  - data offset in the buffer
     */

    /* vertices */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            6 * sizeof (GLfloat), (GLvoid *) 0);

    /* place data on shader location 0 */
    glEnableVertexAttribArray(0);

    /* normals */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            6 * sizeof (GLfloat), (GLvoid *) (3 * sizeof (GLfloat)));
    glEnableVertexAttribArray(1);

    /* unbind vao */
    glBindVertexArray(0);

    struct pad *p = malloc(sizeof *p);
    p->vao = vao;
    p->vbo = vbo;
    p->x = 0.0;
    p->speed = 0.0;
    p->vertex_count = model->nvertex;
    p->angle = 0.0;

    return p;
}

void
pad_free(struct pad *p)
{
    glDeleteVertexArrays(1, &p->vao);
    glDeleteBuffers(1, &p->vbo);
    glDeleteBuffers(1, &p->ebo);

    free(p);
}

void
pad_draw(void *object, GLuint shader_program)
{
    struct pad *pad = (struct pad *) object;

    mat4x4 model_matrix;
    //mat4x4_translate(model_matrix, pad->x, 0.0, 0.0);
    mat4x4_translate(model_matrix, -0.5, 0.0, 0.0);
    mat4x4_rotate_X(model_matrix, model_matrix, deg_to_rad(pad->angle));
    //mat4x4_rotate_Y(model_matrix, model_matrix, deg_to_rad(90.0));

    mat4x4 normal_matrix;
    mat4x4_invert(normal_matrix, model_matrix);
    mat4x4_transpose(normal_matrix, normal_matrix);

    shader_set_uniform_m4(shader_program, "model", model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", normal_matrix);

    glBindVertexArray(pad->vao);
    glDrawArrays(GL_TRIANGLES, 0, pad->vertex_count);
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

void
pad_rotate_x(struct pad *p)
{
    p->angle += 10;

    p->angle = p->angle % 360;
}
