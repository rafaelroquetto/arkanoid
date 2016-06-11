#ifndef UTILS_H
#define UTILS_H

#include "linmath.h"
#include "constants.h"

float deg_to_rad(float deg);

int fuzzy_compare(double a, double b);

void dump_matrix4(mat4x4 m, const char *message);

static inline void
mat4x4_mul_vec3(vec3 r, mat4x4 M, vec3 v)
{
    vec4 aux;

    aux[COORD_X] = v[COORD_X];
    aux[COORD_Y] = v[COORD_Y];
    aux[COORD_Z] = v[COORD_Z];
    aux[COORD_W] = 1.0f;

    mat4x4_mul_vec4(aux, M, aux);

    r[COORD_X] = aux[COORD_X];
    r[COORD_Y] = aux[COORD_Y];
    r[COORD_Z] = aux[COORD_Z];
}

#endif /* UTILS_H */
