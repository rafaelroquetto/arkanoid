#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "pad.h"
#include "panic.h"
#include "constants.h"
#include "linmath.h"
#include "level.h"
#include "ball.h"

static GLFWwindow *window = NULL;

static struct pad *pad = NULL;

static struct level *level = NULL;

static struct ball *ball = NULL;

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
        } else if (key == GLFW_KEY_UP) {
            pad_state |= PAD_UP;
        }

        break;

    case GLFW_RELEASE:
        if (key == GLFW_KEY_LEFT) {
            pad_state &= ~PAD_LEFT;
        } else if (key == GLFW_KEY_RIGHT) {
            pad_state &= ~PAD_RIGHT;
        } else if (key == GLFW_KEY_UP) {
            pad_state &= ~PAD_UP;
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

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
        panic("Cannot initialize GLEW");

    glEnable(GL_DEPTH_TEST);
}

typedef void (*update_func)(void *object);

struct update_ctx
{
    update_func func;
    void **object;
};

static struct update_ctx update_contexts[] = {
    { pad_update, (void **) &pad },
    { level_update, (void **) &level },
    { ball_update, (void **) &ball },
    { NULL }
};

static void
update(void)
{
    struct update_ctx *ctx;

    if (pad_state & PAD_LEFT)
        pad_throttle_left(pad);
    if (pad_state & PAD_RIGHT)
        pad_throttle_right(pad);
    if (pad_state & PAD_UP)
        pad_rotate_x(pad);

    ball->x = pad->x;
    ball->y = 0.55;
    ball->z = 0.0;

    ctx = update_contexts;

    while (ctx->func) {
        ctx->func(*ctx->object);
        ++ctx;
    }
}

typedef void (*draw_func)(void *object, GLuint shader_program);

struct draw_ctx
{
    draw_func func;
    void **object;
};

static struct draw_ctx draw_contexts[] = {
    { pad_draw, (void **) &pad },
    { level_draw, (void **) &level },
    { ball_draw, (void **) &ball },
    { NULL }
};

static void
setup_uniforms(GLuint shader_program)
{
    /* FIXME place this in a proper place */

    /* view matrix */
    mat4x4 view_matrix;
    mat4x4_translate(view_matrix, 0.0, -15.0, -40.0);

    shader_set_uniform_m4(shader_program, "view", view_matrix);

    /* projection matrix */
    mat4x4 projection_matrix;
    mat4x4_perspective(projection_matrix, M_PI/4, VIEWPORT_WIDTH/VIEWPORT_HEIGHT, 0.1f, 100.f);

    shader_set_uniform_m4(shader_program, "projection", projection_matrix);
}

static void
draw(GLuint shader_program)
{
    struct draw_ctx *ctx;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setup_uniforms(shader_program);

    for (ctx = draw_contexts; ctx->func; ++ctx) {
        ctx->func(*ctx->object, shader_program);
    }
}

static void
init_objects(void)
{
    assert(pad == NULL);

    pad = pad_new();

    level = level_new();

    ball = ball_new();
}

static void
free_objects(void)
{
    assert(pad != NULL);

    pad_free(pad);

    level_free(level);

    ball_free(ball);
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
