#version 460 core

in vec2 tex_pos;

uniform vec2 u_resolution;
uniform float u_time;

out vec4 color;

float perlin_noise2d(float x, float y);


vec3 get_circle_color(float noise, vec2 p, vec2 c, float r, float w) {
    r *= noise;
    vec3 color = vec3(0.0);
    float diff = abs(length(p - c) - r);
    color.r += 1.0 - smoothstep(0.0, w, diff);
    color.r += pow(1.0 - diff, 6.0);
    color.g += pow(color.r, 8.0);
    return color;
}


void main(void) {

    int n_levels = 8;
    float ampl = 1.0;
    float freq = 4.0;
    float ampl_mult = 0.6;
    float freq_mult = 2.0;
    float ampl_sum = 0.0;
    float z = 0.0;

    for (int i = 0; i < n_levels; ++i) {
        float x = tex_pos.x * freq;
        float y = tex_pos.y * freq;
        z += ampl * perlin_noise2d(x, y);
        ampl_sum += ampl;
        ampl *= ampl_mult;
        freq *= freq_mult;
    }
    z /= ampl_sum;
    z *= (1.0 + sin(u_time * 4.0)) / 2.0;

    vec2 center = vec2(0.5, 0.5);
    float radius = 0.4;
    float line_width = 0.2;

    vec3 c = get_circle_color(z, tex_pos, center, radius, line_width);
    color = vec4(c, 1.0);
}

