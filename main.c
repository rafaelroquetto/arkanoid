#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "pad.h"
#include "panic.h"
#include "constants.h"
#include "linmath.h"
#include "level.h"
#include "ball.h"
#include "utils.h"
#include "boundingbox.h"
#include "list.h"
#include "brick.h"
#include "explosion.h"

static GLFWwindow *window = NULL;

static struct pad *pad = NULL;

static struct level *level = NULL;

static struct ball *ball = NULL;

struct list *explosions = NULL;

enum { DEBUG_BOXES = 0 };

/* "joypad" state */
enum {
    PAD_UP = 1,
    PAD_DOWN = 2,
    PAD_LEFT = 4,
    PAD_RIGHT = 8,
    PAD_FIRE = 16,
    PAD_PULSE = 32
};

enum state {
    INTRO,
    RESET,
    RUNNING
};

static unsigned pad_state;

static unsigned game_state;

struct camera
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat angle;
};

static struct camera camera;

static const float CAMERA_X = 0.0;
static const float CAMERA_Y = -15.0;
static const float CAMERA_Z = -40.0;
static const float CAMERA_ANGLE = 0.0;
static const float CAMERA_STEP = 0.5;

static void
reset_game(void)
{
    game_state = RESET;

    ball_set_speed(ball, 0.0);
    ball_reset_direction(ball);

    pad->x = 0.0;
    ball->x = pad->x;
    ball->y = 0.7;
    ball->z = 0.0;

    camera.x = CAMERA_X;
    camera.y = CAMERA_Y;
    camera.z = CAMERA_Z;
    camera.angle = CAMERA_ANGLE;
}

static void
start_game(void)
{
    game_state = RUNNING;
    ball_set_speed(ball, 0.2);
}

static void
reset_to_intro(void)
{
    game_state = INTRO;

    camera.x = 10.0;
    camera.y = -10.0;
    camera.z = 1.0;
}

static void
update_camera(void)
{
    if (!fuzzy_compare(camera.x, CAMERA_X))
        camera.x -= CAMERA_STEP;
    else if (!fuzzy_compare(camera.y, CAMERA_Y))
        camera.y -= CAMERA_STEP;
    else if (!fuzzy_compare(camera.z, CAMERA_Z))
        camera.z -= CAMERA_STEP;
    else
        game_state = RESET;
}

static void
create_explosion(float x, float y, float z)
{
    struct explosion *e;

    e = explosion_new(x, y, z);

    list_add(explosions, (void *) e);
}

static void
explosions_update(void *object)
{
    struct list *explosions;
    struct node *n;
    struct explosion *explosion;

    explosions = (struct list *) object;

    for (n = explosions->first; n; n = n->next) {
        explosion_update(n->data);

        explosion = n->data;

        if (!explosion_alive(explosion)) {
            list_remove(explosions, n);
            explosion_free(explosion);
            break;
        }
    }
}

static void
explosions_draw(void *object, GLuint shader_program)
{
    struct list *explosions;
    struct node *n;

    explosions = (struct list *) object;

    for (n = explosions->first; n; n = n->next) {
        explosion_draw(n->data, shader_program);
    }
}

static void
handle_kbd(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    switch (action) {
    case GLFW_PRESS:
    case GLFW_REPEAT:
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (key == GLFW_KEY_LEFT) {
            pad_state |= PAD_LEFT;
        } else if (key == GLFW_KEY_RIGHT) {
            pad_state |= PAD_RIGHT;
        } else if (key == GLFW_KEY_UP) {
            pad_state |= PAD_UP;
        } else if (key == GLFW_KEY_SPACE) {
            if (game_state == RUNNING)
                reset_game();
            else
                start_game();
        } else if (key == GLFW_KEY_W) {
            camera.z += CAMERA_STEP;
        } else if (key == GLFW_KEY_S) {
            camera.z -= CAMERA_STEP;
        } else if (key == GLFW_KEY_A) {
            camera.x += CAMERA_STEP;
        } else if (key == GLFW_KEY_D) {
            camera.x -= CAMERA_STEP;
        } else if (key == GLFW_KEY_Q) {
            camera.y += CAMERA_STEP;
        } else if (key == GLFW_KEY_E) {
            camera.y -= CAMERA_STEP;
        } else if (key == GLFW_KEY_R) {
            camera.angle += CAMERA_STEP;
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    { explosions_update, (void **) &explosions },
    { NULL }
};

static inline float
calculate_new_angle(struct ball *b, struct pad *p)
{
    static const float MAX_ANGLE = 75.0;
    float pad_half_width;
    float angle_factor;

    pad_half_width = bb_width(&p->box) * 0.5;

    angle_factor =
        (bb_horizontal_center(&b->box)
         - bb_horizontal_center(&p->box)) / pad_half_width;

    return MAX_ANGLE * (1 - angle_factor);
}

static void
check_ball_pad_collision(struct ball *b, struct pad *p)
{
    if (game_state == RESET)
        return;

    if (bb_intersects_top(&b->box, &p->box)) {
        ball_set_direction(ball, calculate_new_angle(b, p));
    }
}


static void
check_ball_brick_collision(struct ball *b, struct level *l)
{
    struct node *n;
    struct brick *brick;

    for (n = level->bricks->first; n; n = n->next) {
        brick = (struct brick *) n->data;

        if (!brick->alive)
            continue;

        if (bb_intersects(&b->box, &brick->box)) {
            ball_set_direction(ball, -b->angle);
            brick_set_alive(brick, GL_FALSE);
            create_explosion(b->x, b->y, 2.0);
            break;
        }
    }
}

static void
check_collisions()
{
    check_ball_pad_collision(ball, pad);
    check_ball_brick_collision(ball, level);
}

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

    if (game_state == RESET)
        ball->x = pad->x;

    check_collisions();

    if (game_state == INTRO)
        update_camera();

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
    { explosions_draw, (void **) &explosions },
    { NULL }
};

static void
setup_uniforms(GLuint shader_program)
{
    /* FIXME place this in a proper place */

    /* view matrix */
    mat4x4 view_matrix;
    mat4x4_translate(view_matrix, camera.x, camera.y, camera.z);
    mat4x4_rotate_X(view_matrix, view_matrix, deg_to_rad(camera.angle));

    shader_set_uniform_m4(shader_program, "view", view_matrix);

    /* projection matrix */
    mat4x4 projection_matrix;
    mat4x4_perspective(projection_matrix, M_PI/4, VIEWPORT_WIDTH/VIEWPORT_HEIGHT, 0.1f, 100.f);

    shader_set_uniform_m4(shader_program, "projection", projection_matrix);
    shader_set_uniform_3f(shader_program, "viewPos", camera.x, camera.y, camera.z);
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

    if (DEBUG_BOXES) {
        bb_draw(&ball->box, shader_program);
        bb_draw(&pad->box, shader_program);
    }
}

static void
init_objects(void)
{
    assert(pad == NULL);

    pad = pad_new();

    level = level_new();

    ball = ball_new();

    explosions = list_new();
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
    srand(time(NULL));

    init_glfw();

    init_objects();

    reset_game();

    reset_to_intro();

    handle_events();

    free_objects();

    glfwTerminate();

    return 0;
}
