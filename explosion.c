#include <stdlib.h>

#include "particlesystem.h"
#include "explosion.h"

enum { PARTICLE_COUNT = 400 };

static void
setup_particle(struct explosion *e, struct particle *p)
{
    p->x = e->x;
    p->y = e->y;
    p->z = 2.0;

    p->r = (rand() % 101) / 100.0;
    p->g = (rand() % 101) / 100.0;
    p->b = (rand() % 101) / 100.0;
    p->a = 1.0;

    p->angle = rand() % 365;
    p->speed = (rand() % 9) / 10.0;
    p->accel = -0.001;
}

static void
setup_particles(struct explosion *e)
{
    int i;
    struct particle_system *p;

    p = e->system;

    for (i = 0; i < p->count; ++i)
        setup_particle(e, &p->particles[i]);
}

struct explosion *
explosion_new(float x, float y, float z)
{
    struct explosion *e;

    e = malloc(sizeof *e);
    e->x = x;
    e->y = y;
    e->z = z;
    e->system = particle_system_new(PARTICLE_COUNT);

    setup_particles(e);

    return e;
}

void
explosion_free(struct explosion *e)
{
    particle_system_free(e->system);
    free(e);
}

void
explosion_update(void *object)
{
    struct explosion *e;

    e = (struct explosion *) object;

    particle_system_update(e->system);
}

void
explosion_draw(void *object, GLuint shader_program)
{
    struct explosion *e;

    e = (struct explosion *) object;

    particle_system_draw(e->system, shader_program);
}

int
explosion_alive(struct explosion *e)
{
    return particle_system_alive(e->system);
}
