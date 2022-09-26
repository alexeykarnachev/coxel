#version 460 core

out vec2 vs_tex_pos;

void main() {
    int id = gl_VertexID;
    vs_tex_pos = vec2(id & 1, (id >> 1) & 1);
    gl_Position = vec4(vs_tex_pos * 2 - 1, 0.0, 1.0);
}
