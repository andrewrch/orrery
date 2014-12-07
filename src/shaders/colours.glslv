#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 normal;

uniform mat4 WVP;

smooth out vec4 vertex_colour;

void main()
{
  vertex_colour = colour;
  gl_Position = WVP * vec4(position, 1.0);
}
