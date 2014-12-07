#include <stdlib.h>
#include <stdio.h>
#include "shader.h"

#define SRC_ARRAY_RESIZE_BY 5

void create_shader(Shader* s, GLenum type) {
  s->id = glCreateShader(type);
  s->src_len = 0;
  s->src = NULL;
  s->num_src = 0;
  s->compiled = false;
}

void create_frag_shader(Shader* s, char* file) {
  create_shader(s, GL_FRAGMENT_SHADER);
  add_shader_source(s, file);
}

void create_geom_shader(Shader* s, char* file) {
  create_shader(s, GL_GEOMETRY_SHADER);
  add_shader_source(s, file);
}

void create_vert_shader(Shader* s, char* file) {
  create_shader(s, GL_VERTEX_SHADER);
  add_shader_source(s, file);
}

void delete_shader(Shader* s) {
  glDeleteShader(s->id);
  for (unsigned int i = 0; i < s->num_src; i++) {
    free(s->src[i]);
  }
  free(s->src);
}

void resize_src_array(Shader* s) {
  unsigned int new_len = SRC_ARRAY_RESIZE_BY + s->src_len;
  char** new_src = (char**) malloc(new_len * sizeof(char*));
  for (unsigned int i = 0; s->num_src; i++) {
    new_src[i] = s->src[i];
  }
  free(s->src);
  s->src = new_src;
  s->src_len = new_len;
}

void shader_add_source_from_string(Shader* s, char* src) {
  if (s->num_src == s->src_len) {
    resize_src_array(s);
  }
  s->src[s->num_src++] = src;
}

char* load_string_from_file(char* filename) {
  FILE* f = fopen(filename, "rb");
  char* buf = NULL;
  if (!f) {
    fprintf(stderr, "Could not open shader file: %s\n", filename);
    return NULL;
  } else {
    fseek(f, 0, SEEK_END);
    unsigned int len = ftell(f);
    buf = (char*) malloc(1 + len * sizeof(char));
    fseek(f, 0, SEEK_SET);
    if (buf) {
      fread(buf, sizeof(char), len, f);
    } else {
      fprintf(stderr, "Could not allocate memory for shader\n");
      return NULL;
    }
    buf[len] = '\0';
  }
  return buf;
}

void add_shader_source(Shader* s, char* file) {
  char* str = load_string_from_file(file);
  shader_add_source_from_string(s, str);
}

int compile_shader(Shader* s) {
  glShaderSource(s->id, s->num_src, (const char**) s->src, NULL);
  glCompileShader(s->id);
  //check whether the shader compiled OK
  GLint status;
  glGetShaderiv(s->id, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetShaderiv(s->id, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar infoLog[infoLogLength];
    glGetShaderInfoLog (s->id, infoLogLength, NULL, infoLog);
    fprintf(stderr, "Shader compilation failure.\n%s", infoLog);
    // Error code
    return 0;
  }
  s->compiled = true;
  return 1;
}
