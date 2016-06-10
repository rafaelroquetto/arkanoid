#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"

#if 0
static void
dump_list(const struct list *l)
{
    struct node *n = l->first;
    while (n) {
        printf("n == %p\n", n);
        printf("n->data == %p\n", n->data);
        printf("n->prev == %p\n", n->prev);
        printf("n->next == %p\n", n->next);
        printf("-----\n");
        n = n->next;
    }

    putchar('\n');
}
#endif

struct list *
list_new(void)
{
    struct list *l;

    l = malloc(sizeof *l);

    l->size = 0;
    l->first = NULL;

    return l;
}

void
list_add(struct list *l, void *data)
{
    struct node *n;

    n = malloc(sizeof *n);
    n->data = data;

    n->next = l->first;
    n->prev = NULL;

    if (l->first)
        l->first->prev = n;

    l->first = n;
    l->size++;

}

void *
list_remove(struct list *l, struct node *n)
{
    void *data = n->data;
    if (n->prev) {
        n->prev->next = n->next;
    } else {
        l->first = n->next;
    }

    if (n->next) {
        n->next->prev = n->prev;
    }

    free(n);

    l->size--;

    assert(l->size >= 0);

    return data;
}


void
list_free(struct list *l, list_free_func free_func)
{
    struct node *current;
    struct node *next;
    void *data;

    if (free_func == NULL)
        free_func = free;

    current = l->first;

    while (current) {
        next = current->next;

        data = list_remove(l, current);

        free_func(data);

        current = next;
    }

    free(l);
}

int list_empty(const struct list *l)
{
    return (l->first == NULL) ? 1 : 0;
}

int list_size(const struct list *l)
{
    return l->size;
}

