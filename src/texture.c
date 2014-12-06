#include "texture.h"

void bind_texture(Texture* t) {
  glBindTexture(GL_TEXTURE_2D, t->id);
}

void unbind_texture() {
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture* create_texture() {
  Texture* t = (Texture*) malloc(sizeof(Texture));
  glGenTextures(1, &t->id);
  return t;
}

