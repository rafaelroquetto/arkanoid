#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H

#include <GL/glew.h>

struct list;

struct list * explosions_new(void);

void explosions_free(struct list *l);
void explosions_update(void *object);
void explosions_draw(void *object, GLuint shader_program);
void explosions_create(struct list *l, float x, float y, float z);

#endif /* EXPLOSIONS_H */
