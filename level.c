#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include "level.h"
#include "list.h"
#include "brick.h"
#include "panic.h"

static const float H_DISTANCE = 6.0;
static const float V_DISTANCE = 4.0;
static const float DEPTH = 0.0;
static const float X_ORIG = -60.0;
static const float Y_ORIG = 60.0;

static brick_type parse_brick_type(char ch)
{
    switch (ch) {
    case '1':
        return NORMAL;
    case '2':
        return HARD;
    }

    assert(0);
}

void load_level_matrix(const char *path,
        char matrix[LEVEL_ROWS][LEVEL_COLUMNS + 1])
{
    int i;
    int fd;

    fd = open(path, O_RDONLY);

    if (fd == -1)
        panic("Cannot open level file: %s", strerror(errno));

    for (i = 0; i < LEVEL_ROWS; ++i) {
        read(fd, &matrix[i][0], LEVEL_COLUMNS + 1);
    }

    close (fd);
}

struct level * level_from_file(const char *path)
{
    int i;
    int j;
    char type;

    struct brick *b;
    struct level *l;

    /* 21 accounts for '\n' */
    static char matrix[LEVEL_ROWS][LEVEL_COLUMNS + 1];
    load_level_matrix(path, matrix);


    l = malloc(sizeof *l);
    l->bricks = list_new();


    for (i = 0; i < LEVEL_ROWS; ++i) {
        for (j = 0; j < LEVEL_COLUMNS; ++j) {

            /* because the world origin is on the
             * bottom left corner, from the downmost
             * rows towards the upmost ones
             */
            type = matrix[LEVEL_ROWS - i - 1][j];

            if (type == '0')
                continue;

            b = brick_new();

            b->x = X_ORIG + j * H_DISTANCE;
            b->y = Y_ORIG + i * V_DISTANCE;
            b->z = DEPTH;
            b->type = parse_brick_type(type);

            list_add(l->bricks, (void *) b);
        }
    }

    return l;
}

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
    struct brick *brick;

    level = (struct level *) object;

    for (n = level->bricks->first; n; n = n->next) {
       brick = (struct brick *) n->data;

       if (!brick->alive)
           continue;

       brick_draw((struct brick *) n->data, shader_program);
    }
}

void
level_update(void *object)
{
    struct level *level;
    struct node *n;

    level = (struct level *) object;

    for (n = level->bricks->first; n; n = n->next)
            brick_update((struct brick *) n->data);
}

