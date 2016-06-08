#include <GL/gl.h>
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

#include "model.h"


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
    free(m->vertices);
    free(m->normals);
    free(m->indices);

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

    while(nbytes = read(fd, buf, sizeof buf))
        byte_array_append(b, buf, nbytes);

    close(fd);

    return b;
}

static int
byte_array_lines(struct byte_array *b)
{
    int nlines;
    char *ptr;

    nlines = 0;

    ptr = b->data;

    if (*ptr == 0)
        return 0;

    while (*ptr) {
        if (*ptr == '\n')
            ++nlines;
        ++ptr;
    }

    return nlines;
}

static char *
byte_array_line(struct byte_array *b, int index)
{
    char *ptr;
    char *begin;
    char *line;
    int count;
    int line_len;

    /* files start at line 1, but index is 0-based */
    ++index;

    begin = ptr = b->data;

    if (*ptr == 0)
        return NULL;

    count = 0;
    line_len = 0;

    while (*ptr) {
        ++line_len;

        if (*ptr == '\n') {
            ++count;

            if (count == index) {
                break;
            } else {
                begin = ptr + 1;
                line_len = 0;
            }
        }

        ++ptr;
    }

    line = (char *) malloc(line_len);
    memcpy(line, begin, line_len);
    line[line_len - 1] = '\0';

    return line;
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
parse_face(struct byte_array *faces)
{
    char *token;

    GLuint coord;

    token = strtok(NULL, " ");
    byte_array_append_int(faces, (GLuint) atoi(token));

    token = strtok(NULL, " ");
    byte_array_append_int(faces, (GLuint) atoi(token));

    token = strtok(NULL, " ");
    byte_array_append_int(faces, (GLuint) atoi(token));
}

static void
parse_line(char *line, struct byte_array *vertices,
        struct byte_array *normals, struct byte_array *faces)
{
    char *token;

    if (line[0] == '#' || line[0] == 'o')
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

struct model *
load_model(const char *path)
{
    struct model *model;
    struct byte_array *file;
    struct byte_array *vertices;
    struct byte_array *normals;
    struct byte_array *faces;

    int lines;
    int i;

    vertices = byte_array_new(1024);
    normals = byte_array_new(1024);
    faces = byte_array_new(1024);

    file = load_file(path);

    lines = byte_array_lines(file);

    for (i = 0; i < lines; ++ i) {
        char *line = byte_array_line(file, i);
        parse_line(line, vertices, normals, faces);

        /*FIXME use a static buffer instead, allocs are expensive*/
        free(line);
    }

    byte_array_free(file);

    model = model_new();

    model->vsize = byte_array_to_float_array(vertices, &model->vertices);
    model->nsize = byte_array_to_float_array(normals, &model->normals);
    model->isize = byte_array_to_int_array(faces, &model->indices);

    byte_array_free(vertices);
    byte_array_free(normals);
    byte_array_free(faces);

    return model;
}

