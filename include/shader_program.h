#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "shader.h"
#include "linear_algebra.h"

typedef struct shader_program {
  GLuint id;
  Shader** shaders;
  unsigned int num_shaders;
  unsigned int shaders_len;
} ShaderProgram;

void create_shader_program(ShaderProgram* sp, unsigned int num, ...);
void bind_program(ShaderProgram* s);

void add_float_uniform(ShaderProgram* s, char* n, float f);
void add_int_uniform(ShaderProgram* s, char* n, int i);
void add_vec4_uniform(ShaderProgram* s, char* n, vec4 v);
void add_mat4x4_uniform(ShaderProgram* s, char* n, mat4x4 m);

#endif
