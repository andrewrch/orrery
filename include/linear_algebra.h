#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#define M_PI 3.14159265358979323846264338327
#define M_PI_2 1.57079632679489661923132169164

#include <stdio.h>
#include <math.h>

// Type definitions
typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec4 mat4x4[4];

typedef unsigned int uvec4[4];

void vec3_print(vec3 const a);
void vec3_add(vec3 r, vec3 const a, vec3 const b);
void vec3_negate(vec3 r, vec3 const a);
void vec3_sub(vec3 r, vec3 const a, vec3 const b);
void vec3_scale(vec3 r, vec3 const a, float s);
void mat4x4_add(mat4x4 r, mat4x4 const a, mat4x4 const b);

#endif
