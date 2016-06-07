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

typedef void (*draw_func)(GLuint shader_program);

static draw_func draw_functions[] = { draw_pad, NULL };

static void
draw(GLuint shader_program)
{
    draw_func *f;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    f = draw_functions;

    while (*f) {
        (*f)(shader_program);
        ++f;
    }
}

int main(int argc, char *argv[])
{
    init_glfw();

    GLFWwindow *window = create_window();

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    GLuint shader_program = load_shaders();

    glUseProgram(shader_program);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        draw(shader_program);

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}
