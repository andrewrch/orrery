#version 330

in vec4 colour;
out vec4 gl_fragColour;

void main()
{
  gl_fragColour = vsVertexColourOut;
}
