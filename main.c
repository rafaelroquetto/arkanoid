#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "pad.h"
#include "panic.h"

enum {
    WINDOW_WIDTH = 800,
    WINDOW_HEIGHT = 600
};

static struct pad *pad = NULL;

static void
handle_kbd(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    default:
        break;
    }
}

static void
init_glfw(void)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}

static GLFWwindow *
create_window(void)
{
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
            "Project X", NULL, NULL);

    if (window == NULL)
        panic("Failed to create window");

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, handle_kbd);

    return window;
}

typedef void (*draw_func)(void *object, GLuint shader_program);

struct draw_ctx
{
    draw_func func;
    void **object;
};

static draw_func draw_functions[] = { draw_pad, NULL };

static struct draw_ctx draw_contexts[] = {
    { draw_pad, (void **) &pad },
    NULL
};

static void
draw(GLuint shader_program)
{
    struct draw_ctx *ctx;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ctx = draw_contexts;

    while (ctx->func) {
        ctx->func(*ctx->object, shader_program);
        ++ctx;
    }
}

static void
init_objects(void)
{
    assert(pad == NULL);

    pad = pad_new();
}

static void
free_objects(void)
{
    assert(pad != NULL);

    pad_free(pad);
}

int main(int argc, char *argv[])
{
    init_glfw();

    GLFWwindow *window = create_window();

    init_objects();

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    GLuint shader_program = load_shaders();

    glUseProgram(shader_program);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        draw(shader_program);

        glfwSwapBuffers(window);
    }

    free_objects();

    glfwTerminate();

    return 0;
}
