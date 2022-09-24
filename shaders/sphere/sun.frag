#version 460 core

in vec4 es_m_pos;
in vec4 es_m_center_pos;

uniform vec3 u_color;

layout (location = 0) out vec4 f_color;
layout (location = 1) out vec4 f_brightness; 

float noise3d(float x, float y, float z);


void main() {
    float freq = 4.0;
    float ampl = 1.0;
    float n_levels = 6.0;
    float freq_mult = 0.5;
    float ampl_mult = 0.3;
    float ampl_sum = 0.0;
    float val = 0.0;

    for (int i = 0; i < n_levels; ++i) {
        vec3 p = es_m_pos.xyz * freq;
        float v = ampl * noise3d(p.x, p.y, p.z);
        val += v;
        ampl_sum += ampl;
        ampl *= ampl_mult;
        freq *= freq_mult;
    }

    val /= ampl_sum;

    vec3 color = u_color;
    color.g *= pow(val, 8.0);
    color.b *= pow(val, 8.0);
    f_color = vec4(color, 1.0);
    // f_brightness = f_color;
    f_brightness = vec4(0.0);
}

