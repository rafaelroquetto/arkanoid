#include "panic.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void
panic(const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "FATAL: ");

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fputc('\n', stderr);

    glfwTerminate();

    exit(1);
}

