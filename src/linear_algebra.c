#include "linear_algebra.h"

/* vec3 functions */
void vec3_print(vec3 const a) {
  printf("{ %f, %f, %f }", a[0], a[1], a[2]);
}

void vec3_add(vec3 r, vec3 const a, vec3 const b) {
  for (int i = 0; i < 3; i++) {
    r[i] = a[i] + b[i];
  }
}

void vec3_negate(vec3 r, vec3 const a) {
  for (int i = 0; i < 3; i++) {
    r[i] = -a[i];
  }
}

void vec3_sub(vec3 r, vec3 const a, vec3 const b) {
  for (int i = 0; i < 3; i++) {
    r[i] = a[i] - b[i];
  }
}

void vec3_scale(vec3 r, vec3 const a, float s) {
  for (int i = 0; i < 3; i++) {
    r[i] = a[i] * s;
  }
}

float vec3_dot(vec3 const a, vec3 const b) {
  float r = 0;
  for (int i = 0; i < 3; i++) {
    r += a[i] * b[i];
  }
  return r;
}

/* vec4 functions */
void vec4_add(vec4 r, vec4 const a, vec4 const b) {
  for (int i = 0; i < 4; i++) {
    r[i] = a[i] + b[i];
  }
}

void vec4_negate(vec4 r, vec4 const a) {
  for (int i = 0; i < 4; i++) {
    r[i] = -a[i];
  }
}

void vec4_sub(vec4 r, vec4 const a, vec4 const b) {
  for (int i = 0; i < 4; i++) {
    r[i] = a[i] - b[i];
  }
}

void vec4_scale(vec4 r, vec4 const a, float s) {
  for (int i = 0; i < 4; i++) {
    r[i] = a[i] * s;
  }
}

float vec4_dot(vec4 const a, vec4 const b) {
  float r = 0;
  for (int i = 0; i < 4; i++) {
    r += a[i] * b[i];
  }
  return r;
}

void mat4x4_identity(mat4x4 a) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      a[i][j] = i==j ? 1.0 : 0.0;
    }
  }
}

void mat4x4_add(mat4x4 r, mat4x4 const a, mat4x4 const b) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r[i][j] = a[i][j] + b[i][j];
    }
  }
}

void mat4x4_sub(mat4x4 r, mat4x4 const a, mat4x4 const b) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r[i][j] = a[i][j] - b[i][j];
    }
  }
}

void mat4x4_translate(mat4x4 r, float x, float y, float z) {
  at4x4_identity(r);
  r[3][0] = x;
  r[3][1] = y;
  r[3][2] = z;
}

