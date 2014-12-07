#version 330

smooth in vec4 vertex_colour;
out vec4 output_colour;

void main()
{
  output_colour = vec4(1.0, 0.0, 1.0, 0.5);
  //output_colour = vertex_colour;
  /*gl_FragColor = vertex_colour;*/
  //gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
