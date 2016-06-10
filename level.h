#ifndef LEVEL_H
#define LEVEL_H

#include <GL/gl.h>

enum {
    LEVEL_COLUMNS = 20,
    LEVEL_ROWS = 20
};

struct level
{
    GLfloat x;
    GLfloat y;

    int map[LEVEL_ROWS][LEVEL_COLUMNS];

    struct list *bricks;
};

struct level * level_new(void);

void level_free(struct level *p);
void level_draw(void *object, GLuint shader_program);
void level_update(void *object);

#endif /* LEVEL_H */
