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
#include "texture.h"

static const GLfloat PAD_THROTTLE = 0.3f;
static const GLfloat MAX_SPEED = 1.7f;
static const GLfloat PAD_FRICTION = 0.19f;
static const GLfloat SCALE_FACTOR = 0.4;
static const GLfloat MAX_SPRING = 0.4;
static const GLfloat SPRING_FORCE = 0.1;

static int mesh_ref_count = 0;
static struct mesh *mesh = NULL;

static int texture_ref_count = 0;
static GLuint texture = 0;

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

static GLuint
pad_texture(void)
{
    if (texture_ref_count == 0) {
        texture = load_texture_from_png(
                "resource/pad_texture.png", NULL, NULL);
    }

    ++texture_ref_count;

    return texture;
}

static void
pad_texture_free(void)
{
    if (texture_ref_count == 0)
        return;

    --texture_ref_count;

    if (texture_ref_count == 0) {
        glDeleteTextures(1, &texture);
    }
}

struct pad *
pad_new(void)
{
    struct pad *p = malloc(sizeof *p);

    p->x = 0.0;
    p->speed = 0.0;
    p->spring_strength = 0.0;
    p->angle = -90.0;
    p->mesh = pad_mesh();
    p->texture = pad_texture();

    return p;
}

void
pad_free(struct pad *p)
{
    free(p);
    pad_mesh_free();
    pad_texture_free();
}

void
pad_draw(void *object, GLuint shader_program)
{
    struct pad *pad = (struct pad *) object;

    shader_set_uniform_m4(shader_program, "model", pad->model_matrix);
    shader_set_uniform_m4(shader_program, "normalModel", pad->normal_matrix);
    shader_set_uniform_i(shader_program, "objectType", OBJECT_PAD);

    glBindTexture(GL_TEXTURE_2D, pad->texture);
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

    if (fabs(p->speed) <= 0.1f) {
        p->speed = 0.0f;
        return;
    }

}

static void
recalculate_matrices(struct pad *p)
{
    mat4x4_identity(p->model_matrix);
    mat4x4_translate_in_place(p->model_matrix, p->x, -p->spring_strength, 0.0);
    mat4x4_scale_aniso(p->model_matrix, p->model_matrix, SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);

    mat4x4_invert(p->normal_matrix, p->model_matrix);
    mat4x4_transpose(p->normal_matrix, p->normal_matrix);
}

static void
update_bounding_box(struct pad *p)
{
    mat4x4 m;
    mat4x4_identity(m);
    mat4x4_translate_in_place(m, p->x, 0.0, 0.0);
    mat4x4_scale_aniso(m, m, SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
    p->box = p->mesh->bounding_box;

    mat4x4_mul_vec3(p->box.min, m, p->box.min);
    mat4x4_mul_vec3(p->box.max, m, p->box.max);
}

void
pad_update(void *object)
{
    int signal;

    struct pad *p = (struct pad *) object;

    p->x += p->speed;

    if (fuzzy_compare(p->spring_strength, 0.0))
        p->spring_strength = 0.0;
    else
        p->spring_strength -= SPRING_FORCE;

    signal = (p->speed > 0) ? 1 : -1;

    if (fabs(p->x) > WORLD_BOUNDARY_X) {
        p->x = signal * WORLD_BOUNDARY_X;
        p->speed = -p->speed * 1.5;
    }

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

void
pad_bump(struct pad *p)
{
    p->spring_strength = MAX_SPRING;
}
