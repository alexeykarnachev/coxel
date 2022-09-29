#version 460 core

in vec3 a_pos;

out VertexData {
    vec4 pos;
} vs_out;


void main() {
    vs_out.pos = vec4(a_pos, 1.0);
    gl_Position = vs_out.pos;
}
