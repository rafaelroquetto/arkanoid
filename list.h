#ifndef LIST_H
#define LIST_H

struct node
{
    void *data;

    struct node *next;
    struct node *prev;
};

struct list
{
    size_t size;
    struct node *first;
};

typedef void (*list_free_func)(void *);

struct list *
list_new(void);

/* adds the contents pointed by
 * data to the list
 * the contents aren't copied.
 */
void list_add(struct list *l, void *data);

/* removes and destroys (frees) the node
 * pointed by n, from the list, and returns
 * its data contents for further manipulation
 */
void *list_remove(struct list *l, struct node *n);

/* removes and destroy all elements from the list,
 * including the list itself
 * for each element on the list, free_func
 * will be called in order to free their memory
 */
void list_free(struct list *l, list_free_func free_func);

int list_empty(const struct list *l);
int list_size(const struct list *l);

#endif /* LIST_H */
