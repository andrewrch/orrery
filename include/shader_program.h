#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "shader.h"
#include "linear_algebra.h"

typedef struct {
  GLuint id;
  Shader** shaders;
  unsigned int num_shaders;
  unsigned int shaders_len;
} ShaderProgram;

void create_program_from_files(ShaderProgram* sp, unsigned int num, ...);
void create_program_from_shaders(ShaderProgram* sp, unsigned int num, ...);
void delete_program(ShaderProgram* sp);
void bind_program(ShaderProgram* s);
void validate_program(ShaderProgram* s);

void add_float_uniform(ShaderProgram* s, char* n, float f);
void add_int_uniform(ShaderProgram* s, char* n, int i);
void add_vec4_uniform(ShaderProgram* s, char* n, vec4 v);
void add_vec3_uniform(ShaderProgram* s, char* n, vec3 v);
void add_mat4x4_uniform(ShaderProgram* s, char* n, mat4x4 m);
unsigned int get_attrib_location(ShaderProgram* s, char* n);

#endif
