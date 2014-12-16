#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 normal;

uniform mat4 WVP;

out Data {
  vec4 colour;
  vec2 tex_coord;
  vec3 normal;
} data_out;

void main()
{
  data_out.colour = colour;
  data_out.normal = normal;
  data_out.tex_coord = tex_coord;
  gl_Position = WVP * vec4(position, 1.0);
}
