#ifndef CAMERA_H
#define CAMERA_H

struct camera
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat angle;
};

static const float CAMERA_STEP = 0.5;

void reset_camera(struct camera *camera);

int update_camera(struct camera *camera);

#endif /* CAMERA_H */
