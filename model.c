#define _GNU_SOURCE /* for strchrnul */

#include <GL/glew.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

#include "panic.h"
#include "bytearray.h"
#include "linmath.h"

#include "model.h"
#include "constants.h"


static struct model *
model_new(void)
{
    struct model *m;

    m = malloc(sizeof *m);

    return m;
}

void
model_free(struct model *m)
{
    free(m->coords);
    free(m);
}

static struct byte_array *
load_file(const char *path)
{
    static char buf[1024];
    struct byte_array *b;
    int fd;
    int nbytes;

    b = byte_array_new(1024);

    bzero(buf, sizeof buf);

    if ((fd = open(path, O_RDONLY)) == -1)
        panic("Cannot load model '%s': %s", path, strerror(errno));

    while ((nbytes = read(fd, buf, sizeof buf)))
        byte_array_append(b, buf, nbytes);

    close(fd);

    return b;
}

static char *
byte_array_get_next_line(struct byte_array *b)
{
    static char *ptr = NULL;

    if (b != NULL)
        ptr = b->data;

    char *newline;
    char *ret;

    if (*ptr == '\0')
        return NULL;

    newline = strchrnul(ptr, '\n');

    *newline = '\0';

    ret = ptr;

    if (newline == '\0')
        ptr = newline;
    else
        ptr = newline + 1;

    return ret;
}

static void
byte_array_append_float(struct byte_array *b, GLfloat number)
{
    byte_array_append(b, (const char *) &number, sizeof number);
}

static void
byte_array_append_int(struct byte_array *b, GLuint number)
{
    byte_array_append(b, (const char *) &number, sizeof number);
}

static void
parse_vertex(struct byte_array *dest)
{
    char *token;

    token = strtok(NULL, " ");
    byte_array_append_float(dest, (GLfloat) atof(token));

    token = strtok(NULL, " ");
    byte_array_append_float(dest, (GLfloat) atof(token));

    token = strtok(NULL, " ");
    byte_array_append_float(dest, (GLfloat) atof(token));
}

static void
face_to_indices(const char *face, GLuint *ivertex,
        GLuint *itexture, GLuint *inormal)
{
    *ivertex = *itexture = *inormal = 0;

    const char *ptr = face;

    *ivertex = atoi(ptr);

    ptr = strchrnul(face, '/');

    if (ptr == NULL)
        return;

    ++ptr;

    *itexture = atoi(ptr);

    ptr = strchrnul(ptr, '/');

    if (ptr == NULL)
        return;

    ++ptr;

    *inormal = atoi(ptr);
}

static void
parse_face_component(struct byte_array *faces)
{
    char *token;

    GLuint ivertex;
    GLuint itexture;
    GLuint inormal;

    token = strtok(NULL, " ");
    face_to_indices(token, &ivertex, &itexture, &inormal);
    byte_array_append_int(faces, ivertex);
    byte_array_append_int(faces, itexture);
    byte_array_append_int(faces, inormal);
}

static void
parse_face(struct byte_array *faces)
{
    parse_face_component(faces); /* first */
    parse_face_component(faces); /* second */
    parse_face_component(faces); /* third */
}

static void
parse_line(char *line, struct byte_array *vertices,
        struct byte_array *normals, struct byte_array *faces)
{
    char *token;

    if (line[0] == '#' || line[0] == 'o')
        return;

    while (*line == ' ')
        line++;

    if (strlen(line) == 0)
        return;

    token = strtok(line, " ");

    if (strcmp(token, "v") == 0) {
        parse_vertex(vertices);
    } else if (strcmp(token, "vn") == 0) {
        parse_vertex(normals);
    } else if (strcmp(token, "f") == 0) {
        parse_face(faces);
    }

}

static size_t
byte_array_to_float_array(struct byte_array *b, GLfloat **float_array)
{
    *float_array = (GLfloat *) malloc(b->size);

    memcpy(*float_array, b->data, b->size);

    return b->size / sizeof (GLfloat);
}

static size_t
byte_array_to_int_array(struct byte_array *b, GLuint **int_array)
{
    *int_array = (GLuint *) malloc(b->size);

    memcpy(*int_array, b->data, b->size);

    return b->size / sizeof (GLuint);
}

static GLfloat
vertex_coord(const GLfloat *vertex_data, const GLuint *index_data,
        int coord, int vertex)
{
    const unsigned int offset = vertex * INDEX_STRIDE + VERTEX_OFFSET;
    const unsigned int vertex_index = index_data[offset] - 1; /* 0-based indices */
    const unsigned int vertex_offset = vertex_index * COORD_STRIDE + coord;

    return vertex_data[vertex_offset];
}

static GLfloat
normal_coord(const GLfloat *normal_data, const GLuint *index_data,
        int coord, int vertex)
{
    const unsigned int offset = vertex * INDEX_STRIDE + NORMAL_OFFSET;
    const unsigned int normal_index = index_data[offset] - 1; /* 0-based indices */
    const unsigned int normal_offset = normal_index * COORD_STRIDE + coord;

    return normal_data[normal_offset];
}

static void
setup_model(struct model *m, struct byte_array *vertices,
        struct byte_array *normals, struct byte_array *indices)
{
    size_t icount;
    size_t buffer_size;
    int i;
    int nvertex;
    int current_coord;

    GLfloat *vertex_data;
    GLfloat *normal_data;
    GLuint *index_data;
    GLfloat *buffer;

    byte_array_to_float_array(vertices, &vertex_data);
    byte_array_to_float_array(normals, &normal_data);
    icount = byte_array_to_int_array(indices, &index_data);

    /* icount holds the total number of components of each index
     * i.e. icount = number of vertex * 3 (vertex index, texture index and
     * normal index).
     */
    buffer_size = icount * 2 * sizeof (GLfloat);
    nvertex = icount / 3;

    /* size of vertices + size of normals */
    buffer = (GLfloat *) malloc(buffer_size);

    for (current_coord = i = 0; i < nvertex; ++i) {
        buffer[current_coord++] = vertex_coord(vertex_data, index_data, COORD_X, i);
        buffer[current_coord++] = vertex_coord(vertex_data, index_data, COORD_Y, i);
        buffer[current_coord++] = vertex_coord(vertex_data, index_data, COORD_Z, i);

        buffer[current_coord++] = normal_coord(normal_data, index_data, COORD_X, i);
        buffer[current_coord++] = normal_coord(normal_data, index_data, COORD_Y, i);
        buffer[current_coord++] = normal_coord(normal_data, index_data, COORD_Z, i);
    }

    free(vertex_data);
    free(normal_data);
    free(index_data);

    m->coords = buffer;
    m->csize = buffer_size;
    m->nvertex = nvertex;
}

struct model *
load_model(const char *path)
{
    struct model *model;
    struct byte_array *file;
    struct byte_array *vertices;
    struct byte_array *normals;
    struct byte_array *faces;

    char *line;

    vertices = byte_array_new(1024);
    normals = byte_array_new(1024);
    faces = byte_array_new(1024);

    file = load_file(path);

    line = byte_array_get_next_line(file);

    while (line) {
        parse_line(line, vertices, normals, faces);
        line = byte_array_get_next_line(NULL);
    }

    byte_array_free(file);

    model = model_new();

    setup_model(model, vertices, normals, faces);

    byte_array_free(vertices);
    byte_array_free(normals);
    byte_array_free(faces);

    return model;
}

