#ifndef EXPLOSION_H
#define EXPLOSION_H

struct particle_system;

struct explosion
{
    float x;
    float y;
    float z;

    struct particle_system *system;
};

struct explosion * explosion_new(float x, float y, float z);

void explosion_free(struct explosion *e);
void explosion_update(void *object);
void explosion_draw(void *object, GLuint shader_program);

int explosion_alive(struct explosion *e);

#endif /* EXPLOSION_H */
