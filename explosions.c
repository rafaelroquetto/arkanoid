#include "explosions.h"
#include "explosion.h"
#include "list.h"

struct list *
explosions_new(void)
{
    return list_new();
}

void
explosions_free(struct list *l)
{
    list_free(l, (list_free_func) explosion_free);
}

void
explosions_update(void *object)
{
    struct list *explosions;
    struct node *n;
    struct explosion *explosion;

    explosions = (struct list *) object;

    for (n = explosions->first; n; n = n->next) {
        explosion_update(n->data);

        explosion = n->data;

        if (!explosion_alive(explosion)) {
            list_remove(explosions, n);
            explosion_free(explosion);
            break;
        }
    }
}

void
explosions_draw(void *object, GLuint shader_program)
{
    struct list *explosions;
    struct node *n;

    explosions = (struct list *) object;

    for (n = explosions->first; n; n = n->next) {
        explosion_draw(n->data, shader_program);
    }
}

void
explosions_create(struct list *l, float x, float y, float z)
{
    struct explosion *e;

    e = explosion_new(x, y, z);

    list_add(l, (void *) e);
}
