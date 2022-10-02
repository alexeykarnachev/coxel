#version 460 core

in VertexData {
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

uniform vec2 u_resolution;
uniform float u_time;

out vec4 color;

float noise3d(float x, float y, float z);

void main(void) {
    int n_levels = 10;
    float freq = 5;
    float ampl = 1;
    float total_ampl = 0;
    float w = 0;
    for (int i = 0; i < n_levels; ++i) {
        float x = (fs_in.tex_pos.x + (u_time / 10.0) - sin(u_time) / 10.0) * freq;
        float y = (fs_in.tex_pos.y + (1 + sin(u_time)) / 10.0) * freq;
        float z = u_time / 4.0;
        w += ampl * noise3d(x, y, z);
        total_ampl += ampl;
        freq *= 2.0;
        ampl *= 0.5;
    }

    w /= total_ampl;
    color = vec4(w * 0.8, w * 0.1, w * 0.1, 1.0);
}
