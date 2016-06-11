#include "boundingbox.h"
#include "constants.h"

static const float THRESHOLD = 0.7;

int bb_intersects(struct bounding_box *a,
        struct bounding_box *b)
{
    return ((a->min[COORD_X] + THRESHOLD < b->max[COORD_X] && a->max[COORD_X] + THRESHOLD > b->min[COORD_X])
         && (a->min[COORD_Y] + THRESHOLD < b->max[COORD_Y] && a->max[COORD_Y] + THRESHOLD > b->min[COORD_Y])
         && (a->min[COORD_Z] + THRESHOLD < b->max[COORD_Z] && a->max[COORD_Z] + THRESHOLD > b->min[COORD_Z]));
}
