#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "shader.h"
#include "linear_algebra.h"

typedef struct shader_program {
  GLuint id;
  Shader** shaders;
  unsigned int num_shaders;
} ShaderProgram;

void create_shader_program(ShaderProgram* sp, Shader* s, ...);
void bind_program(ShaderProgram* s);
void unbind_program(ShaderProgram* s);

void add_vec4_to_program(ShaderProgram* s, char* n, vec4 v);
void add_uvec4_to_program(ShaderProgram* s, char* n, uvec4 v);
void add_mat4x4_to_program(ShaderProgram* s, char* n, mat4x4 m);

#endif
