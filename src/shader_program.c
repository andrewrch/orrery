#include "shader_program.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define SHADERS_RESIZE_FACTOR 2

void resize_shader_array(ShaderProgram* s) {
  unsigned int new_len = SHADERS_RESIZE_FACTOR * s->shaders_len;
  Shader** new_shaders = (Shader**) malloc(new_len * sizeof(Shader*));
  for (unsigned int i = 0; i < s->num_shaders; i++) {
    new_shaders[i] = s->shaders[i];
  }
  free(s->shaders);
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
  GLint linkStatus, validStatus;
  glLinkProgram(sp->id);
  glGetProgramiv(sp->id, GL_LINK_STATUS, &linkStatus);
  glGetProgramiv(sp->id, GL_VALIDATE_STATUS, &validStatus);
  if (validStatus == GL_FALSE || linkStatus == GL_FALSE) {
    GLint infoLogLength;
    glGetProgramiv (sp->id, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar infoLog[infoLogLength];
    glGetProgramInfoLog (sp->id, infoLogLength, NULL, infoLog);
    fprintf(stderr, "Shader link failure\n%s", infoLog);
  }
  for (unsigned int i = 0; i < sp->num_shaders; i++) {
    glDetachShader(sp->id, sp->shaders[i]->id);
  }
}

void create_shader_program(ShaderProgram* sp, Shader* s, ...)
{
  // Process shader args
  va_list ptr;
  Shader* current_s;
  va_start(ptr, s);
  // Loop through variadic args
  do {
    current_s = va_arg(ptr, Shader*);
    add_shader(sp, current_s);
    printf("%d\n", current_s->id);
    // Until end of list (NULL ptr)
  } while(current_s);
  compile_shaders(sp);
  link_program(sp);
}
