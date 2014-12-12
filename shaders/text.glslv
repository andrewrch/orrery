attribute vec3 position;
attribute vec2 tex_coord;

varying vec2 texpos;

void main(void) {
  gl_Position = vec4(position.xyz, 1.0);
  texpos = tex_coord;
}
