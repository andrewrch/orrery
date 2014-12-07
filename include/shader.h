#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <stdbool.h>

typedef struct shader {
  GLuint id;
  char** src;
  unsigned int num_src;
  unsigned int current_src_len;
  bool compiled;
} Shader;

void create_frag_shader(Shader* s);
void create_geom_shader(Shader* s);
void create_vert_shader(Shader* s);

int compile_shader(Shader* s);
void delete_shader(Shader* s);

void shader_add_source(Shader* s, char* file);

#endif
