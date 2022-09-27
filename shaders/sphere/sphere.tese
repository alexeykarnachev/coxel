# version 460 core

layout(triangles, equal_spacing, ccw) in;
in vec3 cs_pos[];

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

uniform int   u_surface_noise_n_levels;
uniform float u_surface_noise_freq_mult;
uniform float u_surface_noise_ampl_mult;
uniform float u_surface_noise_freq_init;
uniform float u_surface_noise_mult;

out vec4 frag_pos;

float layered_perlin_noise3d(
    float x,
    float y,
    float z,
    int n_levels,
    float freq_mult,
    float ampl_mult,
    float freq_init
);

void main(void) {
    vec3 p0 = gl_TessCoord.x * cs_pos[0];
    vec3 p1 = gl_TessCoord.y * cs_pos[1];
    vec3 p2 = gl_TessCoord.z * cs_pos[2];

    vec3 p = normalize(p0 + p1 + p2);

    if (u_surface_noise_mult > 0 && u_surface_noise_n_levels > 0) {
        float f = layered_perlin_noise3d(
                p.x,
                p.y,
                p.z,
                u_surface_noise_n_levels,
                u_surface_noise_freq_mult,
                u_surface_noise_ampl_mult,
                u_surface_noise_freq_init
        );
        p += p * u_surface_noise_mult * ((f * 2.0) - 1.0);
    }

    vec4 pos = vec4(p, 1.0);
    frag_pos = u_model * pos; 
    gl_Position = u_proj * u_view * es_pos;
}
