#include <stdio.h>
#include "shadow_map.h"
#include "linear_algebra.h"

void init_up_directions(vec3 up_directions[6]) {
  up_directions[0][0] = 0.0;
  up_directions[0][1] = -1.0;
  up_directions[0][2] = 0.0;

  up_directions[1][0] = 0.0;
  up_directions[1][1] = -1.0;
  up_directions[1][2] = 0.0;

  up_directions[2][0] = 0.0;
  up_directions[2][1] = 0.0;
  up_directions[2][2] = 1.0;

  up_directions[3][0] = 0.0;
  up_directions[3][1] = 0.0;
  up_directions[3][2] = -1.0;

  up_directions[4][0] = 0.0;
  up_directions[4][1] = -1.0;
  up_directions[4][2] = 0.0;

  up_directions[5][0] = 0.0;
  up_directions[5][1] = -1.0;
  up_directions[5][2] = 0.0;
}

void init_light_directions(vec3 light_directions[6], vec3 light_pos) {
  // +X
  light_directions[0][0] = light_pos[0] + 1;
  light_directions[0][1] = light_pos[1];
  light_directions[0][2] = light_pos[2];
  // -X
  light_directions[1][0] = light_pos[0] - 1;
  light_directions[1][1] = light_pos[1];
  light_directions[1][2] = light_pos[2];
  // +Y
  light_directions[2][0] = light_pos[0];
  light_directions[2][1] = light_pos[1] + 1;
  light_directions[2][2] = light_pos[2];
  // -Y
  light_directions[3][0] = light_pos[0];
  light_directions[3][1] = light_pos[1] - 1;
  light_directions[3][2] = light_pos[2];
  // +Z
  light_directions[4][0] = light_pos[0];
  light_directions[4][1] = light_pos[1];
  light_directions[4][2] = light_pos[2] + 1;
  // -Z
  light_directions[5][0] = light_pos[0];
  light_directions[5][1] = light_pos[1];
  light_directions[5][2] = light_pos[2] - 1;
}

void init_view_matrices(ShadowMap* s) {
  for (unsigned int i = 0; i < 6; i++) {
  mat4x4_look_at(s->view_mats[i], s->light_pos,
                 s->light_directions[i], s->up_directions[i]);
  }
}

bool init_omnidirectional_shadow_map(ShadowMap* s, int width, int height,
                                     vec3 light_pos) {
  s->width = width;
  s->height = height;
  // Generate the framebuffer
  glGenFramebuffers(1, &s->fb);
  // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
  init_texture(&s->shadow_map);
  glActiveTexture(GL_TEXTURE0 + 1);
  bind_texture_cube(&s->shadow_map);
  for (unsigned int i = 0; i < 6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                 GL_DEPTH_COMPONENT, width, height,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  unbind_texture_cube();
  // Initialise the shaders
  create_program_from_files(&s->shader, 1, "../shaders/shadow.glslv");

  vec3_dup(s->light_pos, light_pos);
  init_up_directions(s->up_directions);
  init_light_directions(s->light_directions, s->light_pos);
  init_view_matrices(s);

  mat4x4_perspective(s->light_proj, deg_to_rad(90),
                     s->width / (float) s->height,
                     0.1, 200000.f);
  return true;
}

void bind_shadow_map(ShadowMap* s, unsigned int face) {
  glBindFramebuffer(GL_FRAMEBUFFER, s->fb);
  glViewport(0, 0, s->width, s->height);
  glCullFace(GL_FRONT);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                         s->shadow_map.id, 0);
  // Check framebuffer
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Something went wrong making shadow cube\n");
  }
}

void unbind_shadow_map() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glCullFace(GL_BACK);
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
}

void draw_to_omnidirectional_shadow_map(ShadowMap* s,
                                        unsigned int face,
                                        DrawableObject* o,
                                        mat4x4 world) {
  bind_program(&s->shader);
  mat4x4 wvp;
  mat4x4_mul(wvp, s->view_mats[face], world);
  mat4x4_mul(wvp, s->light_proj, wvp);
  add_mat4x4_uniform(&s->shader, "WVP", wvp);
  draw_triangles(o);
}
