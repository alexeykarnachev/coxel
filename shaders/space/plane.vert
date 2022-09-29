#version 460 core

out vec3 vs_pos;
out vec2 tex_pos;

void main() {
    int id = gl_VertexID;
    tex_pos = vec2(id & 1, (id >> 1) & 1);
    vs_pos = vec3(tex_pos * 2 - 1, 0.0);
    gl_Position = vec4(vs_pos, 1.0);
}
