#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <linmath.h>
#include <GL/glew.h>

struct bounding_box
{
    vec3 min;
    vec3 max;
};

int bb_intersects(struct bounding_box *a,
        struct bounding_box *b);
int bb_intersects_top(struct bounding_box *a,
        struct bounding_box *b);

void bb_draw(struct bounding_box *b, GLuint shader_program);

#endif /* BOUNDINGBOX_H */
