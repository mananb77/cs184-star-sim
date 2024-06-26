#version 330

uniform mat4 u_view_projection;
uniform mat4 u_model;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 in_position;
in vec4 in_normal;
in vec4 in_tangent;
in vec2 in_uv;

out vec4 v_position;
out vec4 v_normal;
out vec2 v_uv;
out vec4 v_tangent;

float h(vec2 uv) {
  vec4 k = texture(u_texture_2, uv);
  return k.r;
}

void main() {
  // YOUR CODE HERE
  v_normal = normalize(u_model * in_normal);
  v_uv = in_uv;
  v_tangent = normalize(u_model * in_tangent);

  vec4 dis = h(v_uv) * v_normal * u_height_scaling;
  v_position = u_model * in_position + dis;
  gl_Position = u_view_projection * v_position;
}
