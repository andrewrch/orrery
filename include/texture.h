#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <stdlib.h>

typedef struct {
  GLuint id;
} Texture ;

int create_texture(Texture* t, char* file);
void delete_texture(Texture* t);
void bind_texture(Texture* t);
void unbind_texture();

#endif
