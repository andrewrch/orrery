#include "camera.h"

void init_camera(Camera* c, vec3 eye, vec3 up) {
  // Set position
  for (unsigned int i = 0; i < 3; i++) {
    c->eye[i] = eye[i];
    c->up[i] = up[i];
  }
  c->free_move = false;
  c->r_lat = 0;
  c->r_lon = 0;
}

static inline double deg_to_rad(double deg) {
  return deg * 0.017453293;
}

void get_view_matrix(Camera* c, mat4x4 view) {
  // Calculate target
  vec3 target;
  target[0] = c->eye[0] + cos(c->r_lat) * cos(c->r_lon);
  target[1] = c->eye[1] + sin(c->r_lat);
  target[2] = c->eye[2] + sin(c->r_lon) * cos(c->r_lat);
  mat4x4_look_at(view, c->eye, target, c->up);
}

void move_forward(Camera* c, float step) {
  c->eye[0] += step * cos(c->r_lon);
  c->eye[1] += 0;
  c->eye[2] += step * sin(c->r_lon);
}
void move_backwards(Camera* c, float step) {
  c->eye[0] -= step * cos(c->r_lon);
  c->eye[1] -= 0;
  c->eye[2] -= step * sin(c->r_lon);
}

void move_left(Camera* c, float step) {
  c->eye[0] += step * sin(c->r_lon);
  c->eye[1] += 0;
  c->eye[2] -= step * cos(c->r_lon);
}

void move_right(Camera* c, float step) {
  c->eye[0] -= step * sin(c->r_lon);
  c->eye[1] += 0;
  c->eye[2] += step * cos(c->r_lon);
}

float check_bounds(float r) {
  if (r <= deg_to_rad(-89)) {
    return deg_to_rad(-89);
  } else if (r >= deg_to_rad(89)) {
    return deg_to_rad(89);
  } else {
    return r;
  }
}

void rotate_up(Camera* c, float d) {
  c->r_lat += deg_to_rad(d);
  c->r_lat = check_bounds(c->r_lat);
}
void rotate_down(Camera* c, float d) {
  c->r_lat -= deg_to_rad(d);
  c->r_lat = check_bounds(c->r_lat);
}
void rotate_left(Camera* c, float d) {
  c->r_lon -= deg_to_rad(d);
}
void rotate_right(Camera* c, float d) {
  c->r_lon += deg_to_rad(d);
}
