#include "linear_algebra.h"

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

void mat4x4_add(mat4x4 r, mat4x4 const a, mat4x4 const b) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r[i][j] = a[i][j] + b[i][j];
    }
  }
}
