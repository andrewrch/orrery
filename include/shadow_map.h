#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include <GL/glew.h>
#include <stdbool.h>
#include "texture.h"
#include "shader_program.h"
#include "drawable_object.h"
#include "linear_algebra.h"

typedef struct {
  GLuint fb;
  Texture shadow_map;
  ShaderProgram shader;
  int width, height;
  mat4x4 light_proj;
  mat4x4 view_mats[6];
  vec3 up_directions[6];
  vec3 light_directions[6];
  vec3 light_pos;
} ShadowMap;

bool init_omnidirectional_shadow_map(ShadowMap* s, int w, int h, vec3 pos);
void draw_to_omnidirectional_shadow_map(ShadowMap* s,
                                        unsigned int face,
                                        DrawableObject* o,
                                        mat4x4 world);
void bind_shadow_map(ShadowMap* s, unsigned int face);
void unbind_shadow_map();

#endif
