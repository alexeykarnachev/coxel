#version 460 core

in vec3 vs_in_pos;

out vec3 vs_out_pos;

void main() {
    vs_out_pos = vs_in_pos;
}
