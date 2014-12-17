#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 normal;

uniform mat4 WVP;
uniform mat4 WV;
uniform mat4 N;
uniform vec3 sun_pos;

out Data {
  vec2 tex_coord;
  vec3 eye_norm;
  vec4 eye_pos;
  vec4 sun_pos;
} data_out;

void main()
{
  data_out.tex_coord = tex_coord;
  // Take only top left of N matrix
  data_out.eye_norm = normalize(mat3(N) * normal);
  data_out.eye_pos = WV * vec4(position, 1.0);
  data_out.sun_pos = vec4(sun_pos, 1.0);
  // And assign vertex position
  gl_Position = WVP * vec4(position, 1.0);
}
