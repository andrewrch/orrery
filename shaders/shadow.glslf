#version 330

out float depth;

void main(){
  // Not really needed, OpenGL does it anyway
  depth = gl_FragCoord.z;
}
