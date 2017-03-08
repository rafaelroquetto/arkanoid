#include <GLFW/glfw3.h>

#include "camera.h"
#include "utils.h"

static const float CAMERA_X = 0.0;
static const float CAMERA_Y = -15.0;
static const float CAMERA_Z = -40.0;
static const float CAMERA_ANGLE = 0.0;

void
reset_camera(struct camera *camera)
{
    camera->x = CAMERA_X;
    camera->y = CAMERA_Y;
    camera->z = CAMERA_Z;
    camera->angle = CAMERA_ANGLE;
}

int
update_camera(struct camera *camera)
{
    int ret = 1;

    if (!fuzzy_compare(camera->x, CAMERA_X))
        camera->x -= CAMERA_STEP;
    else if (!fuzzy_compare(camera->y, CAMERA_Y))
        camera->y -= CAMERA_STEP;
    else if (!fuzzy_compare(camera->z, CAMERA_Z))
        camera->z -= CAMERA_STEP;
    else
        ret = 0;

    return ret;
}
