#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <linmath.h>

struct bounding_box
{
    vec3 min;
    vec3 max;
};

int bb_intersects(struct bounding_box *a,
        struct bounding_box *b);

#endif /* BOUNDINGBOX_H */
