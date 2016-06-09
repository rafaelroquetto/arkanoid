#include <stdlib.h>
#include <GL/glew.h>

#include "pad.h"
#include "linmath.h"
#include "constants.h"
#include "shader.h"
#include "model.h"
#include "mesh.h"
#include "objects.h"

static const GLfloat PAD_THROTTLE = 0.8f;
static const GLfloat MAX_SPEED = 2.0f;
static const GLfloat PAD_FRICTION = 0.1f;
static const GLfloat SCALE_FACTOR = 0.4;

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

static int mesh_ref_count = 0;
static struct mesh *mesh = NULL;

static struct mesh *
pad_mesh(void)
{

    if (mesh != NULL) {
        ++mesh_ref_count;
        return mesh;
    }

    mesh = mesh_load("resource/pad.obj");

    return mesh;
}

static void
pad_mesh_free(void)
{
    if (mesh == NULL)
        return;

    --mesh_ref_count;

    if (mesh_ref_count == 0) {
        mesh_free(mesh);
        mesh = NULL;
    }
}

struct pad *
pad_new(void)
{
    struct pad *p = malloc(sizeof *p);

    p->x = 0.0;
    p->speed = 0.0;
    p->angle = -90.0;
    p->mesh = pad_mesh();

    return p;
}

void
pad_free(struct pad *p)
{
    free(p);
    pad_mesh_free();
}

void
pad_draw(void *object, GLuint shader_program)
{
    struct pad *pad = (struct pad *) object;

    mat4x4 model_matrix;
    mat4x4_identity(model_matrix);
    mat4x4_scale_aniso(model_matrix, model_matrix, SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
    mat4x4_rotate_X(model_matrix, model_matrix, deg_to_rad(pad->angle));
    mat4x4_rotate_Y(model_matrix, model_matrix, deg_to_rad(90.0));
    mat4x4_translate_in_place(model_matrix, 0.0, 0.0, -pad->x);

    mat4x4 normal_matrix;
    mat4x4_invert(normal_matrix, model_matrix);
    mat4x4_transpose(normal_matrix, normal_matrix);

    shader_set_uniform_m4(shader_program, "model", model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", normal_matrix);
    shader_set_uniform_i(shader_program, "objectType", OBJECT_PAD);

    glBindVertexArray(pad->mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, pad->mesh->vertex_count);
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
