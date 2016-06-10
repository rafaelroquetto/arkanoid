#ifndef UTILS_H
#define UTILS_H

#include "linmath.h"

float deg_to_rad(float deg);

int fuzzy_compare(double a, double b);

void dump_matrix4(mat4x4 m, const char *message);

#endif /* UTILS_H */
