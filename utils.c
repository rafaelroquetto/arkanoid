#include <stdio.h>

#include "utils.h"
#include "boundingbox.h"

float
deg_to_rad(float deg)
{
    return M_PI*deg/180.f;
}

int
fuzzy_compare(double a, double b)
{
    static const double EPSILON = 0.1;

    return (int)((float)fabs(a - b) < EPSILON);
}

void
dump_matrix4(mat4x4 m, const char *message)
{
    int i;
    int j;

    if (message != NULL)
        printf("%s\n", message);

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            printf("%f ", m[i][j]);
        }

        putchar('\n');
    }
    putchar('\n');

}

void
dump_bounding_box(struct bounding_box *b, const char *message)
{
    printf("%s (%f, %f, %f, %f, %f, %f)\n", message,
            b->min[COORD_X], b->min[COORD_Y], b->min[COORD_Z],
            b->max[COORD_X], b->max[COORD_Y], b->max[COORD_Z]);

}
