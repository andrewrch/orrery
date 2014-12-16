#version 330

in Data {
  vec4 colour;
  vec2 tex_coord;
  vec3 normal;
} data_in;

out vec4 output_colour;

void main()
{
  // Set colour to the texture colour
  output_colour = data_in.colour;
  //output_colour = vec4(1.0, 1.0, 1.0, 1.0);
}
