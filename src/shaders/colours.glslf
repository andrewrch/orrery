#version 330

uniform sampler2D texture_unit;

in Data {
  vec4 colour;
  vec2 tex_coord;
  vec3 normal;
} data_in;

out vec4 output_colour;

void main()
{
  // Set colour to the texture colour
  output_colour = texture(texture_unit, data_in.tex_coord);
}
