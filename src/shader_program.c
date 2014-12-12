#include "shader_program.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define SHADERS_RESIZE_BY 5

void resize_shader_array(ShaderProgram* s) {
  unsigned int new_len = SHADERS_RESIZE_BY + s->shaders_len;
  Shader** new_shaders = (Shader**) malloc(new_len * sizeof(Shader*));
  for (unsigned int i = 0; i < s->num_shaders; i++) {
    new_shaders[i] = s->shaders[i];
  }
  if (s->shaders) {
    free(s->shaders);
  }
  s->shaders = new_shaders;
  s->shaders_len = new_len;
}

void add_shader(ShaderProgram* sp, Shader* s) {
  if (sp->num_shaders == sp->shaders_len) {
    resize_shader_array(sp);
  }
  sp->shaders[sp->num_shaders++] = s;
}

void compile_shaders(ShaderProgram* sp) {
  for (unsigned int i = 0; i < sp->num_shaders; i++) {
    if (!sp->shaders[i]->compiled) {
      compile_shader(sp->shaders[i]);
    }
  }
}

void bind_program(ShaderProgram* s) {
  glUseProgram(s->id);
}

void link_program(ShaderProgram* sp) {
  sp->id = glCreateProgram();
  // Attach shaders
  for (unsigned int i = 0; i < sp->num_shaders; i++) {
    glAttachShader(sp->id, sp->shaders[i]->id);
  }
  //link and check whether the program links fine
  GLint status;
  glLinkProgram(sp->id);
  glGetProgramiv(sp->id, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint len;
    glGetProgramiv (sp->id, GL_INFO_LOG_LENGTH, &len);
    GLchar info_log[len];
    glGetProgramInfoLog (sp->id, len, NULL, info_log);
    fprintf(stderr, "Shader link failure\n%s", info_log);
  }

  // Validate that it'll all work fine with this GL profile
  glValidateProgram(sp->id);
  glGetProgramiv(sp->id, GL_VALIDATE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint len;
    glGetProgramiv (sp->id, GL_INFO_LOG_LENGTH, &len);
    GLchar info_log[len];
    glGetProgramInfoLog (sp->id, len, NULL, info_log);
    fprintf(stderr, "Shader validation failed\n%s", info_log);
  }
  for (unsigned int i = 0; i < sp->num_shaders; i++) {
    glDetachShader(sp->id, sp->shaders[i]->id);
  }
}

void create_program_from_files(ShaderProgram* sp, unsigned int num, ...)
{
  sp->shaders = NULL;;
  sp->shaders_len = 0;
  sp->num_shaders = 0;
  // Process shader args
  va_list ptr;
  char* current_f;
  va_start(ptr, num);
  // Loop through variadic args
  for (unsigned int i = 0; i < num; i++) {
    current_f = va_arg(ptr, char*);

    Shader* s = malloc(sizeof(Shader));
    if (strstr(current_f, ".glslf")) {
      create_frag_shader(s, current_f);
    } else if (strstr(current_f, ".glslv")) {
      create_vert_shader(s, current_f);
    }
    add_shader(sp, s);
  }
  compile_shaders(sp);
  link_program(sp);
}

void create_program_from_shaders(ShaderProgram* sp, unsigned int num, ...)
{
  sp->shaders = NULL;;
  sp->shaders_len = 0;
  sp->num_shaders = 0;
  // Process shader args
  va_list ptr;
  Shader* current_s;
  va_start(ptr, num);
  // Loop through variadic args
  for (unsigned int i = 0; i < num; i++) {
    current_s = va_arg(ptr, Shader*);
    add_shader(sp, current_s);
  }
  compile_shaders(sp);
  link_program(sp);
}

void delete_program(ShaderProgram* sp) {
  for (unsigned int i = 0; i < sp->num_shaders; i++) {
    delete_shader(sp->shaders[i]);
    free(sp->shaders[i]);
  }
  free(sp->shaders);
  glDeleteProgram(sp->id);
}

void add_float_uniform(ShaderProgram* s, char* n, float f) {
  GLint loc = glGetUniformLocation(s->id, n);
  glUniform1f(loc, f);
}

void add_int_uniform(ShaderProgram* s, char* n, int i) {
  GLint loc = glGetUniformLocation(s->id, n);
  glUniform1i(loc, i);
}

void add_vec4_uniform(ShaderProgram* s, char* n, vec4 v) {
  GLint loc = glGetUniformLocation(s->id, n);
  glUniform4fv(loc, 1, v);
}

void add_mat4x4_uniform(ShaderProgram* s, char* n, mat4x4 m) {
  GLint loc = glGetUniformLocation(s->id, n);
  glUniformMatrix4fv(loc , 1, false, (GLfloat*)m);
}

unsigned int get_attrib_location(ShaderProgram* s, char* n) {
  return glGetAttribLocation(s->id, n);
}
