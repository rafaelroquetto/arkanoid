#include <stdlib.h>
#include <GL/glew.h>

#include "level.h"
#include "constants.h"
#include "list.h"
#include "brick.h"

static const float H_DISTANCE = 6.0;
static const float V_DISTANCE = 4.0;
static const float DEPTH = 4.0;
static const float X_ORIG = -60.0;
static const float Y_ORIG = 60.0;

struct level *
level_new(void)
{
    int i;
    int j;

    struct brick *b;
    struct level *l;

    l = malloc(sizeof *l);
    l->bricks = list_new();


    for (i = 0; i < LEVEL_ROWS; ++i) {
        for (j = 0; j < LEVEL_COLUMNS; ++j) {
            b = brick_new();

            b->x = X_ORIG + j * H_DISTANCE;
            b->y = Y_ORIG + i * V_DISTANCE;
            b->z = DEPTH;

            list_add(l->bricks, (void *) b);
        }
    }

    return l;
}

void
level_free(struct level *l)
{
    list_free(l->bricks, (list_free_func) brick_free);
    free(l);
}

void
level_draw(void *object, GLuint shader_program)
{
    struct level *level;
    struct node *n;

    level = (struct level *) object;

    for (n = level->bricks->first; n; n = n->next)
            brick_draw((struct brick *) n->data, shader_program);
}

void
level_update(void *object)
{
    struct level *p = (struct level *) object;
    (void )p;

    /* FIXME */
}

