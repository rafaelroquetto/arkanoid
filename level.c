#include <stdlib.h>
#include <GL/glew.h>

#include "level.h"
#include "linmath.h"
#include "constants.h"
#include "shader.h"
#include "bytearray.h"
#include "brick.h"
#include "objects.h"

struct level *
level_new(void)
{
    int i;
    int j;
    struct brick *b;

    struct level *l = malloc(sizeof *l);

    float init_x = -60.0;
    float init_y = 60.0;

    //l->map = { 1 };

    l->bricks = byte_array_new(LEVEL_ROWS * LEVEL_COLUMNS * sizeof (*b));

    for (i = 0; i < LEVEL_ROWS; ++i) {
        for (j = 0; j < LEVEL_COLUMNS; ++j) {
            b = brick_new();
            b->x = init_x + j * 6.0;
            b->y = init_y + i * 4.0;
            b->z = 5.0;
            /* FIXME use a linked list to avoid 
             * unecessary allocs
             */
            byte_array_append(l->bricks, (const char *) b, sizeof *b);
            /* FIXME memory leak: cannot free brick or mesh will be freed */
        }
    }

    return l;
}

void
level_free(struct level *l)
{
    byte_array_free(l->bricks);
    free(l);
}

void
level_draw(void *object, GLuint shader_program)
{
    int i;
    struct level *level = (struct level *) object;
    struct brick *brick;

    brick = (struct brick *) byte_array_data(level->bricks);

    for (i = 0; i < LEVEL_ROWS * LEVEL_COLUMNS; ++i) {
            brick_draw(brick, shader_program);
            ++brick;
    }
}

void
level_update(void *object)
{
    struct level *p = (struct level *) object;
    (void )p;

    /* FIXME */
}

