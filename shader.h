#ifndef SHADER_H
#define SHADER_H

#include "linmath.h"

GLuint load_shaders(void);

void shader_set_uniform_m4(GLuint shader_program, const char *uniform, mat4x4 m);
void shader_set_uniform_i(GLuint shader_program, const char *uniform, GLint i);

#endif /* SHADER_H */
