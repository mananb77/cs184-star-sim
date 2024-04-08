#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
  // You may want to use this helper function...
  vec4 k = texture(u_texture_2, uv);
  return k.r;
}

void main() {
  // YOUR CODE HERE
  vec3 n = normalize(v_normal.xyz);
  vec3 t = normalize(v_tangent.xyz);
  vec3 b = cross(n, t);
  mat3 otw = mat3(t, b, n);
  float width = u_texture_2_size[0];
  float height = u_texture_2_size[1];
  float du = 10 * (h(v_uv + vec2(1.0/width , 0.0)) - h(v_uv)) * u_height_scaling * u_normal_scaling;
  float dv = 10 * (h(v_uv + vec2(0.0 , 1.0/height)) - h(v_uv)) * u_height_scaling * u_normal_scaling;
  vec3 n0 = vec3(-du, -dv, 1);
  vec3 v_normal_2 = otw * normalize(n0);

  // Phong shading
  float ka = 0.6;
  float kd = 1.0;
  float ks = 0.8;
  float ia = 0.2;

  vec3 disL = u_light_pos - v_position.xyz;
  vec3 disC = u_cam_pos - v_position.xyz;
  vec3 incident = normalize(disL);
  vec3 outgoing = normalize(disC);
  vec3 h = normalize(incident + outgoing);

  float r2 = length(disL) * length(disL);
  vec3 ambient = vec3(ka * ia, ka * ia, ka * ia);
  vec3 diffuse = kd * (u_light_intensity / r2) * max(0.0, dot(v_normal_2.xyz, incident));
  vec3 specular = ks * (u_light_intensity / r2) * pow(max(0.0, dot(v_normal_2.xyz, h)), 100.0);
  vec3 color = ambient + diffuse + specular;
  
  // (Placeholder code. You will want to replace it.)
  out_color = vec4(color, 0.0);
  out_color.a = 1;
}

