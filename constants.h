#ifndef CONSTANTS_H
#define CONSTANTS_H

enum {
    VIEWPORT_WIDTH = 1280,
    VIEWPORT_HEIGHT= 1024
};

enum { FPS = 60 };

enum coord_offset {
    COORD_X = 0,
    COORD_Y = 1,
    COORD_Z = 2,
    COORD_W = 3
};

enum offsets {
    COORD_STRIDE = 3,
    INDEX_STRIDE = 3,
    VERTEX_OFFSET = 0,
    TEXTURE_OFFSET = 1,
    NORMAL_OFFSET = 2
};


#endif /* CONSTANTS_H */
