#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

#include "shader.h"
#include "constants.h"
#include "objects.h"
#include "particlesystem.h"
#include "utils.h"
#include "texture.h"

static const float LIFE_FACTOR = 1e-2;

static int texture_ref_count = 0;
static GLuint texture = 0;

static GLuint
particle_texture(void)
{
    if (texture_ref_count == 0) {
        texture = load_texture_from_png(
                "resource/particle.png", NULL, NULL);
    }

    ++texture_ref_count;

    return texture;
}

static void
particle_texture_free(void)
{
    if (texture_ref_count == 0)
        return;

    --texture_ref_count;

    if (texture_ref_count == 0) {
        glDeleteTextures(1, &texture);
    }
}

static void
setup_system(struct particle_system *p)
{
    static const GLfloat vertices[] = {
        -0.1f, -0.1f, 0.0f, 0.0, 0.0,
         0.1f, -0.1f, 0.0f, 1.0, 0.0,
        -0.1f,  0.1f, 0.0f, 0.0, 1.0,
         0.1f,  0.1f, 0.0f, 1.0, 1.0
    };

    glGenVertexArrays(1, &p->vertex_vao);

    glBindVertexArray(p->vertex_vao);

    /* particle mesh */
    glGenBuffers(1, &p->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, p->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices),  vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(PARTICLE_VERTEX_VAO, 3, GL_FLOAT, GL_FALSE,
            5 * sizeof (GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(PARTICLE_VERTEX_VAO);

    glVertexAttribPointer(TEXTURE_VAO, 2, GL_FLOAT, GL_TRUE,
            5 * sizeof (GLfloat), (GLvoid *) (3 * sizeof (GLfloat)));
    glEnableVertexAttribArray(TEXTURE_VAO);

    /* particles positions and sizes
     * initialize it as a NULL buffer
     */
    glGenBuffers(1, &p->position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, p->position_buffer);
    glBufferData(GL_ARRAY_BUFFER, POSITION_BUFF_SIZE, NULL, GL_STREAM_DRAW);

    glVertexAttribPointer(PARTICLE_POSITION_VAO, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);
    glEnableVertexAttribArray(PARTICLE_POSITION_VAO);

    /* color buffer */
    glGenBuffers(1, &p->color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, p->color_buffer);
    glBufferData(GL_ARRAY_BUFFER, COLOR_BUFF_SIZE, NULL, GL_STREAM_DRAW);

    glVertexAttribPointer(PARTICLE_COLOR_VAO, 4, GL_FLOAT, GL_TRUE, 0, (GLvoid *) 0);
    glEnableVertexAttribArray(PARTICLE_COLOR_VAO);

    glBindVertexArray(0);
}

struct particle_system *
particle_system_new(int count)
{
    struct particle_system *p;

    p = malloc(sizeof *p);

    setup_system(p);

    p->count = count;
    p->last_dead = 0;
    p->alive = 1;
    p->texture = particle_texture();

    return p;
}

void
particle_system_free(struct particle_system *p)
{
    particle_texture_free();

    glDeleteVertexArrays(1, &p->vertex_vao);
    glDeleteBuffers(1, &p->vertex_buffer);
    glDeleteBuffers(1, &p->position_buffer);
    glDeleteBuffers(1, &p->color_buffer);
    free(p);
}

static int
particle_alive(struct particle *p)
{
    return (int) ((float) p->speed > LIFE_FACTOR);
}

static void
update_particle(struct particle *p)
{
    float rad;


    if (p->speed == 0)
        return;

    rad = deg_to_rad(p->angle);

    p->x += cos(rad)*p->speed;
    p->y += sin(rad)*p->speed;

    p->speed += p->accel;

    if (p->speed <= LIFE_FACTOR)
        p->speed = 0;
}

static void
update_particles(struct particle_system *p)
{
    int i;
    int alive;

    if (!p->alive)
        return;

    alive = 0;

    for (i = 0; i < p->count; ++i) {
        update_particle(&p->particles[i]);
        alive |= particle_alive(&p->particles[i]);
    }

    p->alive = alive;
}

static void
fill_data_buffers(struct particle_system *p)
{
    int i;
    int current_coord;

    for (current_coord = i = 0; i < p->count; ++i) {
        p->position_data[current_coord] = p->particles[i].x;
        p->color_data[current_coord++] = p->particles[i].r;

        p->position_data[current_coord] = p->particles[i].y;
        p->color_data[current_coord++] = p->particles[i].g;

        p->position_data[current_coord] = p->particles[i].z;
        p->color_data[current_coord++] = p->particles[i].b;

        p->position_data[current_coord] = p->particles[i].size;
        p->color_data[current_coord++] = p->particles[i].a;
    }
}

static inline void
update_buffer_data(GLuint vbo, size_t size, void *data)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    /* orphaning */
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data); /* FIXME */
}

void
particle_system_update(void *object)
{
    struct particle_system *p;

    p = (struct particle_system *) object;

    update_particles(p);

    fill_data_buffers(p);

    update_buffer_data(p->position_buffer, POSITION_BUFF_SIZE, p->position_data);
    update_buffer_data(p->color_buffer, COLOR_BUFF_SIZE, p->color_data);
}

void
particle_system_draw(void *object, GLuint shader_program)
{
    struct particle_system *p;

    p = (struct particle_system *) object;

    shader_set_uniform_i(shader_program, "objectType", OBJECT_PARTICLE);

    glBindTexture(GL_TEXTURE_2D, p->texture);

    glBindVertexArray(p->vertex_vao);

    glVertexAttribDivisor(PARTICLE_VERTEX_VAO, 0);
    glVertexAttribDivisor(PARTICLE_POSITION_VAO, 1);
    glVertexAttribDivisor(PARTICLE_COLOR_VAO, 1);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, p->count);

}

int
particle_system_alive(struct particle_system *p)
{
    return p->alive;
}
