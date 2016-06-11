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

    b->x = b->y = b->z = 0.0;
    b->mesh = ball_mesh();

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

    mat4x4 model_matrix;
    mat4x4_identity(model_matrix);
    mat4x4_translate_in_place(model_matrix, ball->x, ball->y, ball->z);
    mat4x4_scale_aniso(model_matrix, model_matrix, SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);

    mat4x4 normal_matrix;
    mat4x4_invert(normal_matrix, model_matrix);
    mat4x4_transpose(normal_matrix, normal_matrix);

    shader_set_uniform_m4(shader_program, "model", model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", normal_matrix);
    shader_set_uniform_i(shader_program, "objectType", OBJECT_BALL);

    glBindVertexArray(ball->mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, ball->mesh->vertex_count);
    glBindVertexArray(0);
}

void
ball_update(void *object)
{
    struct ball *p = (struct ball *) object;
    (void )p;

    /* FIXME */
}

