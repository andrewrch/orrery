#version 330

uniform sampler2D texture_unit;

in Data {
  vec2 tex_coord;
  vec3 eye_norm;
  vec4 eye_pos;
  vec4 sun_pos;
} data_in;

out vec4 output_colour;

void main()
{
  vec3 s = normalize((data_in.sun_pos - data_in.eye_pos).xyz);
  //vec4 r = reflect(-s, data_in.eye_norm);
  //vec4 v = normalize(data_in.eye_pos);
  //float spec = max(dot(v, r), 0.0);
  float diff = max(dot(data_in.eye_norm, s), 0.0);

  vec4 diff_colour = diff * texture(texture_unit, data_in.tex_coord);
  //vec3 spec_colour = pow(spec, 3) * vec3(1.0, 1.0, 1.0);
  vec4 ambient_colour = vec4(0.01, 0.01, 0.01, 1.0);

  output_colour = vec4(diff_colour + ambient_colour);
  // Set colour to the texture colour
  //output_colour = texture(texture_unit, data_in.tex_coord);
}
