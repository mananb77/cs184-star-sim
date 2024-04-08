#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
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
  vec3 diffuse = kd * (u_light_intensity / r2) * max(0.0, dot(v_normal.xyz, incident));
  vec3 specular = ks * (u_light_intensity / r2) * pow(max(0.0, dot(v_normal.xyz, h)), 100.0);
  vec3 color = ambient + diffuse + specular;

  // (Placeholder code. You will want to replace it.)
  out_color = vec4(color, 0.0);
  out_color.a = 1.0;
}

