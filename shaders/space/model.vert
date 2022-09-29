#version 460 core

in vec3 a_pos;

uniform mat4 u_model;

out VertexData {
    vec4 pos;
} vs_out;


void main() {
    vs_out.pos = u_model * vec4(a_pos, 1.0);
    gl_Position = vs_out.pos;
}
