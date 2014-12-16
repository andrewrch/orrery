#version 330

in Data {
  vec4 colour;
  vec2 tex_coord;
  vec3 normal;
} data_in;

out vec4 output_colour;

void main()
{
  vec2 coord = gl_PointCoord - vec2(0.5);  //from [0,1] to [-0.5,0.5]
  // Make points circular
  float len = length(coord);
  if(len > 0.5)
    discard;
  // And give colour alpha a soft fall off so it looks nicer
  output_colour = vec4(data_in.colour.xyz, 1.0 - 2*len);
}
