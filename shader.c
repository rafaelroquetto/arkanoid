#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "panic.h"

static GLuint
install_shader(const GLchar **source, GLenum type, GLint size)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, source, &size);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        static GLchar log[512];

        glGetShaderInfoLog(shader, sizeof log, NULL, log);

        panic("Shader compilation failed: %s", log);
    }

    return shader;
}

static GLuint
load_shader(const char *path, GLenum type)
{
    GLchar *buf;
    GLuint rc;
    int fd;
    ssize_t nbytes;
    ssize_t bytes_read;
    struct stat s;

    if ((fd = open(path, O_RDONLY)) == -1)
        panic("Cannot open shader file: %s", strerror(errno));

    if (fstat(fd, &s) == -1)
        panic("Cannot get file size: %s", strerror(errno));

    buf = malloc(s.st_size);
    memset(buf, 0 , s.st_size);

    bytes_read = 0;

    do {
        nbytes = read(fd, (buf + bytes_read), s.st_size);
        bytes_read += nbytes;
    } while (nbytes > 0 && bytes_read < s.st_size);

    assert(bytes_read == s.st_size);

    close(fd);

    rc = install_shader((const GLchar **)&buf, type, s.st_size);

    free(buf);

    return rc;
}

static inline GLuint
load_vertex_shader(void)
{
    return load_shader("vertex.glsl", GL_VERTEX_SHADER);
}

static inline GLuint
load_fragment_shader(void)
{
    return load_shader("fragment.glsl", GL_FRAGMENT_SHADER);
}

GLuint
load_shaders(void)
{
    GLuint vertex_shader = load_vertex_shader();
    GLuint fragment_shader = load_fragment_shader();

    GLuint shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if (!success) {
        static GLchar log[512];

        glGetProgramInfoLog(shader_program, sizeof log, NULL, log);

        panic("Shader program linking failed: %s", log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

