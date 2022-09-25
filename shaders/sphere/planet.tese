# version 460 core

layout(triangles, equal_spacing, ccw) in;
in vec3 cs_pos[];

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec4 es_pos;

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

    int n_levels = 6;
    float freq_mult = 2.0;
    float ampl_mult = 0.5;
    float freq_init = 4.0;
    float f = layered_perlin_noise3d(p.x, p.y, p.z, n_levels, freq_mult, ampl_mult, freq_init);
    f = (f * 2.0) - 1.0;
    p += (f * 0.2);

    vec4 pos = vec4(p, 1.0);
    es_pos = u_model * pos; 
    gl_Position = u_proj * u_view * es_pos;
}
