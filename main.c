#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "pad.h"
#include "panic.h"
#include "constants.h"

static GLFWwindow *window = NULL;

static struct pad *pad = NULL;

/* "joypad" state */
enum {
    PAD_UP = 1,
    PAD_DOWN = 2,
    PAD_LEFT = 4,
    PAD_RIGHT = 8,
    PAD_FIRE = 16,
    PAD_PULSE = 32
};

static unsigned pad_state;

static void
handle_kbd(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    switch (action) {
    case GLFW_PRESS:
    case GLFW_RESIZABLE:
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (key == GLFW_KEY_LEFT) {
            pad_state |= PAD_LEFT;
        } else if (key == GLFW_KEY_RIGHT) {
            pad_state |= PAD_RIGHT;
        }

        break;

    case GLFW_RELEASE:
        if (key == GLFW_KEY_LEFT) {
            pad_state &= ~PAD_LEFT;
        } else if (key == GLFW_KEY_RIGHT) {
            pad_state &= ~PAD_RIGHT;
        }

        break;
    }
}

static GLFWwindow *
create_window(void)
{
    GLFWwindow *window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
            "Project X", NULL, NULL);

    if (window == NULL)
        panic("Failed to create window");

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, handle_kbd);

    return window;
}

static void
init_glfw(void)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = create_window();
}

typedef void (*draw_func)(void *object, GLuint shader_program);

struct draw_ctx
{
    draw_func func;
    void **object;
};

static struct draw_ctx draw_contexts[] = {
    { pad_draw, (void **) &pad },
    NULL
};

static void
update(void)
{
    if (pad_state & PAD_LEFT)
        pad_throttle_left(pad);
    if (pad_state & PAD_RIGHT)
        pad_throttle_right(pad);

    pad_update(pad);
}

static void
draw(GLuint shader_program)
{
    struct draw_ctx *ctx;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

static void
handle_events(void)
{
    double prev_ticks;

    glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    GLuint shader_program = load_shaders();

    glUseProgram(shader_program);

    while (!glfwWindowShouldClose(window)) {
        int delay;

        prev_ticks = glfwGetTime();

        glfwPollEvents();

        update();

        draw(shader_program);

        glfwSwapBuffers(window);

        delay = 1000/FPS - (glfwGetTime() - prev_ticks);

        if (delay > 0)
            usleep(delay * 1000);
    }
}

int main(int argc, char *argv[])
{
    init_glfw();

    init_objects();

    handle_events();

    free_objects();

    glfwTerminate();

    return 0;
}
