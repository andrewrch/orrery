#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  GLuint id;
} Texture ;

void init_texture(Texture* t);
bool create_texture_from_file(Texture* t, char* file);
void delete_texture(Texture* t);
void bind_texture(Texture* t);
void unbind_texture();
void bind_texture_cube(Texture* t);
void unbind_texture_cube();

#endif
