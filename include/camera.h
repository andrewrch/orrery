#ifndef CAMERA_H
#define CAMERA_H

#include "linear_algebra.h"
#include <stdbool.h>

typedef struct {
  vec3 eye, up;
  bool free_move;
  float r_lat, r_lon;
} Camera;

void init_camera(Camera* c, vec3 pos, vec3 eye);
void get_view_matrix(Camera* c, mat4x4 view);

void move_forward(Camera* c, float step);
void move_backwards(Camera* c, float step);
void move_left(Camera* c, float step);
void move_right(Camera* c, float step);
void rotate_up(Camera* c, float d);
void rotate_down(Camera* c, float d);
void rotate_left(Camera* c, float d);
void rotate_right(Camera* c, float d);

#endif
