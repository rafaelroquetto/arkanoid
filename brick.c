#include <stdlib.h>
#include <GL/glew.h>

#include "brick.h"
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
brick_mesh(void)
{

    if (mesh != NULL) {
        ++mesh_ref_count;
        return mesh;
    }

    mesh = mesh_load("resource/brick.obj");

    return mesh;
}

static void
brick_mesh_free(void)
{
    if (mesh == NULL)
        return;

    --mesh_ref_count;

    if (mesh_ref_count == 0) {
        mesh_free(mesh);
        mesh = NULL;
    }
}

struct brick *
brick_new(void)
{
    struct brick *b = malloc(sizeof *b);

    b->x = b->y = b->z = 0.0;
    b->mesh = brick_mesh();
    b->alive = GL_TRUE;

    return b;
}

void
brick_free(struct brick *b)
{
    free(b);
    brick_mesh_free();
}

void
brick_draw(void *object, GLuint shader_program)
{
    struct brick *brick = (struct brick *) object;

    shader_set_uniform_m4(shader_program, "model", brick->model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", brick->normal_matrix);
    shader_set_uniform_i(shader_program, "objectType", OBJECT_BRICK);
    shader_set_uniform_i(shader_program, "explosionFactor", 1000);

    glBindVertexArray(brick->mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, brick->mesh->vertex_count);
    glBindVertexArray(0);
}

static void
recalculate_matrices(struct brick *b)
{
    mat4x4_identity(b->model_matrix);
    mat4x4_scale_aniso(b->model_matrix, b->model_matrix,
            SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);

    mat4x4_translate_in_place(b->model_matrix, b->x, b->y, b->z);

    mat4x4_invert(b->normal_matrix, b->model_matrix);
    mat4x4_transpose(b->normal_matrix, b->normal_matrix);
}

static void
update_bounding_box(struct brick *b)
{
    b->box = b->mesh->bounding_box;

    mat4x4_mul_vec3(b->box.min, b->model_matrix, b->box.min);
    mat4x4_mul_vec3(b->box.max, b->model_matrix, b->box.max);
}

void
brick_update(void *object)
{
    struct brick *b = (struct brick *) object;

    recalculate_matrices(b);
    update_bounding_box(b);
}

void
brick_set_alive(struct brick *b, GLboolean alive)
{
    b->alive = alive;
}
