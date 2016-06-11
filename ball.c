#include <stdlib.h>
#include <GL/glew.h>
#include <stdio.h>

#include "ball.h"
#include "linmath.h"
#include "constants.h"
#include "shader.h"
#include "model.h"
#include "mesh.h"
#include "objects.h"
#include "utils.h"

static const float SCALE_FACTOR = 0.2;

static int mesh_ref_count = 0;
static struct mesh *mesh = NULL;

static struct mesh *
ball_mesh(void)
{

    if (mesh != NULL) {
        ++mesh_ref_count;
        return mesh;
    }

    mesh = mesh_load("resource/sphere.obj");

    return mesh;
}

static void
ball_mesh_free(void)
{
    if (mesh == NULL)
        return;

    --mesh_ref_count;

    if (mesh_ref_count == 0) {
        mesh_free(mesh);
        mesh = NULL;
    }
}

struct ball *
ball_new(void)
{
    struct ball *b = malloc(sizeof *b);

    b->speed = b->x = b->y = b->z = 0.0;
    b->mesh = ball_mesh();

    ball_set_direction(b, 120.0);

    return b;
}

void
ball_free(struct ball *b)
{
    free(b);
    ball_mesh_free();
}

void
ball_draw(void *object, GLuint shader_program)
{
    struct ball *ball = (struct ball *) object;

    shader_set_uniform_m4(shader_program, "model", ball->model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", ball->normal_matrix);
    shader_set_uniform_i(shader_program, "objectType", OBJECT_BALL);

    glBindVertexArray(ball->mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, ball->mesh->vertex_count);
    glBindVertexArray(0);
}

static void
recalculate_matrices(struct ball *b)
{
    mat4x4_identity(b->model_matrix);
    mat4x4_translate_in_place(b->model_matrix, b->x, b->y, b->z);
    mat4x4_scale_aniso(b->model_matrix, b->model_matrix, SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);

    mat4x4_invert(b->normal_matrix, b->model_matrix);
    mat4x4_transpose(b->normal_matrix, b->normal_matrix);

}

static void
update_bounding_box(struct ball *b)
{
    b->box = b->mesh->bounding_box;

    mat4x4_mul_vec3(b->box.min, b->model_matrix, b->box.min);
    mat4x4_mul_vec3(b->box.max, b->model_matrix, b->box.max);
}

void
ball_update(void *object)
{
    int signal;

    struct ball *b = (struct ball *) object;

    b->x += b->direction[COORD_X];
    b->y += b->direction[COORD_Y];

    signal = (b->x) < 0.0 ? -1 : 1;

    if (fabs(b->x) > WORLD_BOUNDARY_X) {
        ball_set_direction(b, 180 - b->angle);
        b->x = signal * WORLD_BOUNDARY_X;
    }

    if (b->y > WORLD_BOUNDARY_Y) {
        ball_set_direction(b, -b->angle);
        b->y = WORLD_BOUNDARY_Y;
    }

    recalculate_matrices(b);
    update_bounding_box(b);
}

void
ball_set_direction(struct ball *b, float angle)
{
    float rad;

    rad = deg_to_rad(angle);

    b->angle = angle;
    b->direction[COORD_X] = cos(rad) * b->speed;
    b->direction[COORD_Y] = sin(rad) * b->speed;
}

void
ball_set_speed(struct ball *b, float speed)
{
    b->speed = speed;
    ball_set_direction(b, b->angle);
}

void
ball_reset_direction(struct ball *b)
{
    float angle;

    angle = 45.0 + rand() % 90;

    ball_set_direction(b, angle);
}
