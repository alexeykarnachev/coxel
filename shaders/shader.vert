#version 460 core

layout (location=0) in vec3 vs_in_pos;

uniform mat4 u_mv;
uniform mat4 u_proj;

out vec3 vs_out_pos;

void main() {
    // vs_out_pos = vs_in_pos;
    gl_Position = u_proj * u_mv * vec4(vs_in_pos, 1.0);
}
