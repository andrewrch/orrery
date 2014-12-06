#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>
#include <stdlib.h>

typedef struct texture {
  GLuint id;
} Texture ;

Texture* create_texture();
void bind_texture(Texture* t);
void unbind_texture();

#endif
