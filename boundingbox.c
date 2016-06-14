#include <GL/glew.h>

#include "boundingbox.h"
#include "constants.h"
#include "shader.h"
#include "objects.h"

static const float THRESHOLD = 0.0;

int bb_intersects(struct bounding_box *a,
        struct bounding_box *b)
{
    return ((a->min[COORD_X] + THRESHOLD < b->max[COORD_X] && a->max[COORD_X] + THRESHOLD > b->min[COORD_X])
         && (a->min[COORD_Y] + THRESHOLD < b->max[COORD_Y] && a->max[COORD_Y] + THRESHOLD > b->min[COORD_Y])
         && (a->min[COORD_Z] + THRESHOLD < b->max[COORD_Z] && a->max[COORD_Z] + THRESHOLD > b->min[COORD_Z]));
}

int bb_intersects_top(struct bounding_box *a,
        struct bounding_box *b)
{
    return ((a->min[COORD_X] + THRESHOLD < b->max[COORD_X] && a->max[COORD_X] + THRESHOLD > b->min[COORD_X])
         && (a->min[COORD_Y] + THRESHOLD < b->max[COORD_Y] && a->max[COORD_Y] + THRESHOLD > b->max[COORD_Y])
         && (a->min[COORD_Z] + THRESHOLD < b->max[COORD_Z] && a->max[COORD_Z] + THRESHOLD > b->min[COORD_Z]));
}

float bb_width(struct bounding_box *b)
{
    return (b->max[COORD_X] - b->min[COORD_X]);
}

float bb_horizontal_center(struct bounding_box *b)
{
    return b->min[COORD_X] + (b->max[COORD_X] - b->min[COORD_X])/2;
}

void
bb_draw(struct bounding_box *b, GLuint shader_program)
{
    GLfloat vertices[] = {
        b->min[COORD_X], b->min[COORD_Y], b->max[COORD_Z], /* 0 */
        b->min[COORD_X], b->max[COORD_Y], b->max[COORD_Z], /* 1 */
        b->max[COORD_X], b->max[COORD_Y], b->max[COORD_Z], /* 2 */
        b->max[COORD_X], b->min[COORD_Y], b->max[COORD_Z], /* 3 */
        b->max[COORD_X], b->min[COORD_Y], b->min[COORD_Z], /* 4 */
        b->max[COORD_X], b->max[COORD_Y], b->min[COORD_Z], /* 5 */
        b->min[COORD_X], b->max[COORD_Y], b->min[COORD_Z], /* 6 */
        b->min[COORD_X], b->min[COORD_Y], b->min[COORD_Z] /* 7 */
    };

    GLuint elements[] = {
        0, 1, 3,
        1, 2, 3,
        3, 2, 4,
        2, 5, 4,
        4, 7, 5,
        5, 7, 6,
        0, 7, 1,
        1, 7, 6,
        1, 2, 6,
        2, 6, 5,
        0, 7, 3,
        7, 3, 4
    };

    GLuint vbo;
    GLuint ebo;
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (elements), elements, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (GLfloat), (GLvoid *) 0);

    glEnableVertexAttribArray(VERTEX_VAO);

    glUseProgram(shader_program);

    shader_set_uniform_i(shader_program, "objectType", OBJECT_BOUNDING_BOX);

    glDrawElements(GL_TRIANGLES, sizeof (elements) / sizeof (GLuint), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}
