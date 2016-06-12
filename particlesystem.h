#ifndef PARTICLES_H
#define PARTICLES_H

#include <GL/glew.h>

enum { MAX_PARTICLES = 10000 };
enum { DATA_BUFFER_SIZE = MAX_PARTICLES * 4 };

enum { POSITION_BUFF_SIZE = MAX_PARTICLES * 4 * sizeof (GLfloat) };
enum { COLOR_BUFF_SIZE = MAX_PARTICLES * 4 * sizeof (GLfloat) };

struct particle
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat size;

    GLfloat r, g, b, a;

    float angle;
    float weight;
    float life;
    float ini_speed;
    float speed;
    float accel;
};

struct particle_system
{
    GLuint vertex_buffer;
    GLuint position_buffer;
    GLuint color_buffer;

    GLuint vertex_vao;

    int count;

    struct particle particles[MAX_PARTICLES];

    int last_dead;
    int alive;

    GLfloat position_data[DATA_BUFFER_SIZE];
    GLfloat color_data[DATA_BUFFER_SIZE];

    GLuint texture;
};

struct particle_system * particle_system_new(int count);

void particle_system_free(struct particle_system *p);
void particle_system_update(void *object);
void particle_system_draw(void *object, GLuint shader_program);

int particle_system_alive(struct particle_system *p);

#endif /* PARTICLES_H */
