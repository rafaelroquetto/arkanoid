#ifndef UTILS_H
#define UTILS_H

#include "linmath.h"
#include "constants.h"

struct bounding_box;

float deg_to_rad(float deg);

int fuzzy_compare(double a, double b);

void dump_matrix4(mat4x4 m, const char *message);
void dump_bounding_box(struct bounding_box *b, const char *message);

static inline void
mat4x4_mul_vec3(vec3 r, mat4x4 M, vec3 v)
{
    vec4 aux;
    vec4 res;

    aux[COORD_X] = v[COORD_X];
    aux[COORD_Y] = v[COORD_Y];
    aux[COORD_Z] = v[COORD_Z];
    aux[COORD_W] = 1.0f;

    mat4x4_mul_vec4(res, M, aux);

    r[COORD_X] = res[COORD_X];
    r[COORD_Y] = res[COORD_Y];
    r[COORD_Z] = res[COORD_Z];
}

#endif /* UTILS_H */
