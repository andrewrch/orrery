#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <stdbool.h>

typedef struct shader {
  GLuint id;
  char** src;
  unsigned int num_src;
  unsigned int src_len;
  bool compiled;
} Shader;

void create_frag_shader(Shader* s, char* file);
void create_geom_shader(Shader* s, char* file);
void create_vert_shader(Shader* s, char* file);

int compile_shader(Shader* s);
void delete_shader(Shader* s);

void add_shader_source(Shader* s, char* file);

#endif
