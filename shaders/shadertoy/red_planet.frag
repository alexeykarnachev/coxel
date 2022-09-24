#version 460 core

in vec2 tex_pos;

uniform vec2 u_resolution;
uniform float u_time;

out vec4 color;

float noise4d(float x, float y, float z, float u);
#define PI 3.14159265359

bool is_in_circle(vec2 pos, float radius) {
    if (length(pos) < radius) {
        return true;
    }
    return false;
}

float get_z(vec2 pos, float radius) {
    vec2 p = pos + radius;
    float z = sqrt(pow(radius, 2) - pow(radius - p.x, 2) - pow(radius - p.y, 2));
    return z;
}

void main(void) {
    
    vec2 p = tex_pos - 0.5;
    float r = 0.4;
    float z = get_z(p, r);
    float u = (1.0 + sin(u_time)) * 0.5;

    if (is_in_circle(p, r)) {
        float freq = 2.0;
        float ampl = 1.0;
        float freq_mult = 2.0;
        float ampl_mult = 0.9;
        float ampl_sum = 0.0;
        float n_levels = 7;
        float w = 0.0;

        for (int i = 0; i < n_levels; ++i) {
            w += ampl * noise4d(p.x * freq, p.y * freq, z * freq, u);
            ampl_sum += ampl;
            freq *= freq_mult;
            ampl *= ampl_mult;
        }

        w /= ampl_sum;
        w = fract(w * 4.0);

        vec3 red = vec3(1.0, 0.0, 0.0);
        vec3 yellow = vec3(1.0, 1.0, 0.0); 
        vec3 white = vec3(1.0, 1.0, 1.0); 

        vec3 c = (1.5 * w * red + 0.3 * w * yellow + 0.5 * w * white);
        color = vec4(c, 1.0);
    } else {
        color = vec4(0.05);
    }
    


}
