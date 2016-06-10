#include <stdlib.h>
#include <GL/glew.h>

#include "brick.h"
#include "linmath.h"
#include "constants.h"
#include "shader.h"
#include "model.h"
#include "mesh.h"
#include "objects.h"

static const float SCALE_FACTOR = 0.2;
static inline
float deg_to_rad(float deg)
{
    return M_PI*deg/180.f;
}

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

    mat4x4 model_matrix;
    mat4x4_identity(model_matrix);
    mat4x4_scale_aniso(model_matrix, model_matrix, SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
    mat4x4_translate_in_place(model_matrix, brick->x, brick->y, brick->z);

    mat4x4 normal_matrix;
    mat4x4_invert(normal_matrix, model_matrix);
    mat4x4_transpose(normal_matrix, normal_matrix);

    shader_set_uniform_m4(shader_program, "model", model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", normal_matrix);
    shader_set_uniform_i(shader_program, "objectType", OBJECT_BRICK);

    glBindVertexArray(brick->mesh->vao);
    glDrawArrays(GL_TRIANGLES, 0, brick->mesh->vertex_count);
    glBindVertexArray(0);
}

void
brick_update(void *object)
{
    struct brick *p = (struct brick *) object;
    (void )p;

    /* FIXME */
}

