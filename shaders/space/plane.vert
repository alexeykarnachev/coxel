#version 460 core

out VertexData {
    vec4 proj_pos;
    vec2 tex_pos;
} vs_out;

void main() {
    int id = gl_VertexID;
    vs_out.tex_pos = vec2(id & 1, (id >> 1) & 1);
    vs_out.proj_pos = vec4(vs_out.tex_pos * 2 - 1, 0.0, 1.0);
    gl_Position = vs_out.proj_pos;
}
