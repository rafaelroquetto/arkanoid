#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

#include "pad.h"
#include "linmath.h"
#include "constants.h"
#include "shader.h"
#include "model.h"
#include "mesh.h"
#include "objects.h"
#include "utils.h"

static const GLfloat PAD_THROTTLE = 0.8f;
static const GLfloat MAX_SPEED = 2.0f;
static const GLfloat PAD_FRICTION = 0.1f;
static const GLfloat SCALE_FACTOR = 0.4;

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

    shader_set_uniform_m4(shader_program, "model", pad->model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", pad->normal_matrix);
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

static void
recalculate_matrices(struct pad *p)
{
    mat4x4_identity(p->model_matrix);
    mat4x4_rotate_X(p->model_matrix, p->model_matrix, deg_to_rad(p->angle));
    mat4x4_rotate_Y(p->model_matrix, p->model_matrix, deg_to_rad(90.0));
    mat4x4_translate_in_place(p->model_matrix, 0.0, 0.0, -p->x);
    mat4x4_scale_aniso(p->model_matrix, p->model_matrix, SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);

    mat4x4_invert(p->normal_matrix, p->model_matrix);
    mat4x4_transpose(p->normal_matrix, p->normal_matrix);
}

static void
mat4x4_mul_vec3(vec3 r, mat4x4 M, vec3 v)
{
    vec4 aux;

    aux[COORD_X] = v[COORD_X];
    aux[COORD_Y] = v[COORD_Y];
    aux[COORD_Z] = v[COORD_Z];
    aux[COORD_W] = 1.0f;

    mat4x4_mul_vec4(aux, M, aux);

    r[COORD_X] = aux[COORD_X];
    r[COORD_Y] = aux[COORD_Y];
    r[COORD_Z] = aux[COORD_Z];
}

static void
update_bounding_box(struct pad *p)
{
    p->box = p->mesh->bounding_box;

    mat4x4_mul_vec3(p->box.min, p->model_matrix, p->box.min);
    mat4x4_mul_vec3(p->box.max, p->model_matrix, p->box.max);
}

void
pad_update(void *object)
{
    struct pad *p = (struct pad *) object;

    p->x += p->speed;

    pad_deaccel(p);

    recalculate_matrices(p);
    update_bounding_box(p);
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
