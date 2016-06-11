#include <stdlib.h>
#include <GL/glew.h>
#include <stdio.h>

#include "mesh.h"
#include "linmath.h"
#include "constants.h"
#include "shader.h"
#include "model.h"

static void
calculate_bounding_box(struct model *m, struct bounding_box *box)
{
    int i;
    int ncoords;
    vec3 *coord;

    coord = (vec3 *) m->coords;

    ncoords = m->csize / sizeof *coord;

    box->min[COORD_X] = coord[0][COORD_X];
    box->min[COORD_Y] = coord[0][COORD_Y];
    box->min[COORD_Z] = coord[0][COORD_Z];

    box->max[COORD_X] = coord[0][COORD_X];
    box->max[COORD_Y] = coord[0][COORD_Y];
    box->max[COORD_Z] = coord[0][COORD_Z];

    /* i += 2 to skip normals */
    for (i = 0; i < ncoords; i += 2) {
        if (coord[i][COORD_X] < box->min[COORD_X])
            box->min[COORD_X] = coord[i][COORD_X];

        if (coord[i][COORD_Y] < box->min[COORD_Y])
            box->min[COORD_Y] = coord[i][COORD_Y];

        if (coord[i][COORD_Z] < box->min[COORD_Z])
            box->min[COORD_Z] = coord[i][COORD_Z];

        if (coord[i][COORD_X] > box->max[COORD_X])
            box->max[COORD_X] = coord[i][COORD_X];

        if (coord[i][COORD_Y] > box->max[COORD_Y])
            box->max[COORD_Y] = coord[i][COORD_Y];

        if (coord[i][COORD_Z] > box->max[COORD_Z])
            box->max[COORD_Z] = coord[i][COORD_Z];
    }
}

struct mesh *
mesh_load(const char *path)
{
    struct model *model;
    struct mesh *mesh;

    model = load_model(path);

    mesh = mesh_new(model);

    model_free(model);

    return mesh;
}

struct mesh *
mesh_new(struct model *model)
{
    /* create vertex store buffer */
    GLuint vbo;
    glGenBuffers(1, &vbo);

    /* setup a vao */
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, model->csize, model->coords, GL_STATIC_DRAW);

    /*  - location of position vertex attrib
     *  - size of vertex attrib (vec3)
     *  - data type
     *  - normalized? no
     *  - stride
     *  - data offset in the buffer
     */

    /* vertices */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof (GLfloat), (GLvoid *) 0);

    /* place data on shader location 0 */
    glEnableVertexAttribArray(0);

    /* normals */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof (GLfloat), (GLvoid *) (3 * sizeof (GLfloat)));

    glEnableVertexAttribArray(1);

    /* unbind vao */
    glBindVertexArray(0);

    struct mesh *m = malloc(sizeof *m);
    m->vao = vao;
    m->vbo = vbo;
    m->vertex_count = model->nvertex;

    calculate_bounding_box(model, &m->bounding_box);

    return m;
}

void
mesh_free(struct mesh *m)
{
    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vbo);

    free(m);
}

