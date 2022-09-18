#version 460 core

out vec2 tex_pos;

void main() {
    int id = gl_VertexID;
    tex_pos = vec2(id & 1, (id >> 1) & 1);
    gl_Position = vec4(tex_pos * 2 - 1, 0.0, 1.0);
}
