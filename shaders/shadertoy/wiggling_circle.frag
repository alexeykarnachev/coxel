#version 460 core

in vec2 tex_pos;

uniform vec2 u_resolution;
uniform float u_time;

out vec4 color;

float perlin_noise3d(float x, float y, float z);

vec3 get_circle_color(vec2 p, vec2 c, float r, float w) {
    float diff = abs(length(p - c) - r);
    if (diff < w) {
        return vec3(1.0, 0.1, 0.1);
    } else {
        return vec3(0.2);
    }
}

void main(void) {
    int n_levels = 6;
    float ampl = 1.0;
    float freq = 2.0;
    float ampl_mult = 0.5;
    float freq_mult = 2.0;
    float ampl_sum = 0.0;
    float w = 0.0;

    float z = u_time * 2.0;
    for (int i = 0; i < n_levels; ++i) {
        float x = tex_pos.x * freq;
        float y = tex_pos.y * freq;
        w += ampl * perlin_noise3d(x, y, z);
        ampl_sum += ampl;
        ampl *= ampl_mult;
        freq *= freq_mult;
    }
    vec2 center = vec2(0.5);
    float radius = 0.2 + w * 0.1;
    float line_width = 0.003;
    color = vec4(get_circle_color(tex_pos, center, radius, line_width), 1.0);
}

