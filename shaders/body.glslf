#version 330

uniform sampler2D surface;
uniform samplerCubeShadow shadow;
//uniform mat4 light_proj;
uniform mat4 camera_view_matrix_inv;

in Data {
  vec2 tex_coord;
  vec3 eye_norm;
  vec4 eye_pos;
  vec4 sun_pos;
  vec4 world_pos;
} data_in;

out vec4 output_colour;

void main()
{
 //vec4 position_ls = light_view_matrix * camera_view_matrix_inv * position_cs;
 vec4 position_ls = data_in.world_pos;
 vec4 abs_position = abs(position_ls);
 float fs_z = max(abs_position.x, max(abs_position.y, abs_position.z));

 const float f = 250000.0;
 const float n = 0.1;
 float unproj_z = (f + n) / (f - n) - ( 2 * f * n) / ( f - n ) / fs_z;
 float depth = (unproj_z + 1.0) * 0.5;
 float result = texture(shadow, vec4(position_ls.xyz, depth));

 /*vec3 lvector = light_position - position_cs.xyz;*/
 /*float ldistance = length(lvector);*/
 /*float lintensity = max(dot(normal_cs, normalize(lvector)), 0.0) * 10.0;*/
 /*lintensity /= ldistance * ldistance;*/
 /*lintensity /= lintensity + 0.5;*/
 /*vec3 diffuse = lintensity * result.xyz * color;*/
 /*gl_FragColor = vec4(diffuse,1);*/

  vec3 s = normalize((data_in.sun_pos - data_in.eye_pos).xyz);
  //vec4 r = reflect(-s, data_in.eye_norm);
  //vec4 v = normalize(data_in.eye_pos);
  //float spec = max(dot(v, r), 0.0);
  float diff = max(dot(data_in.eye_norm, s), 0.0);

  //vec4 diff_colour = diff * result * texture(surface, data_in.tex_coord);
  vec4 diff_colour = diff * texture(surface, data_in.tex_coord);
  //vec3 spec_colour = pow(spec, 3) * vec3(1.0, 1.0, 1.0);
  vec4 ambient_colour = vec4(0.01, 0.01, 0.01, 1.0);

  //output_colour = vec4(result, 0, 0, 1);
  output_colour = vec4(diff_colour + ambient_colour);
  // Set colour to the texture colour
  //output_colour = texture(texture_unit, data_in.tex_coord);
}
