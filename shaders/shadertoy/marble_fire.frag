#version 460 core

in VertexData {
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

uniform vec2 u_resolution;
uniform float u_time;

out vec4 color;

float noise3d(float x, float y, float z);
#define PI 3.14159265359

void main(void) {

    int n_levels = 6;
    float freq = 10;
    float ampl = 1;
    float total_ampl = 0;
    float w = 0;
    for (int i = 0; i < n_levels; ++i) {
        float x = fs_in.tex_pos.x * freq;
        float y = fs_in.tex_pos.y * freq;
        float z = u_time / 4.0;
        w += ampl * noise3d(x, y, z);
        total_ampl += ampl;
        freq *= 2.0;
        ampl *= 0.5;
    }

    w /= total_ampl;
    w = ((1.0 + sin(fs_in.tex_pos.x * 2.0 * PI + w * 4.0 + fs_in.tex_pos.y * w * 5.0)) * 0.5 + (1.0 + sin(u_time * 2.0)) * 0.5) / 2.0;
    float r = (1.0 - fs_in.tex_pos.y) * w + w * (1.0 - fs_in.tex_pos.y) + 0.2 * (1.0 + sin(u_time)) * (1.0 - fs_in.tex_pos.y);
    float g = sign(w) * pow(w, 2.0) - fs_in.tex_pos.y;
    float b = (fs_in.tex_pos.y) * (1.0 - w);
    color = vec4(r, g, b, 1.0);
}
