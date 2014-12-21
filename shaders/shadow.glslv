#version 330

layout(location = 0) in vec3 position;

uniform mat4 WVP;

void main(){
  gl_Position =  WVP * vec4(position, 1);
}
