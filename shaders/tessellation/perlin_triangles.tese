# version 460 core

layout(triangles, equal_spacing, ccw) in;

in VertexData {
    vec4 pos;
} tese_in[];

uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec3 u_center;
uniform bool u_is_sphere;

uniform int   u_surface_noise_n_levels = 6;
uniform float u_surface_noise_freq_mult = 2.0;
uniform float u_surface_noise_ampl_mult = 0.1;
uniform float u_surface_noise_freq_init = 6.0;
uniform float u_surface_noise_mult = 0.8;

out VertexData {
    vec4 pos;
} tese_out;


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
    vec3 p0 = gl_TessCoord.x * tese_in[0].pos.xyz;
    vec3 p1 = gl_TessCoord.y * tese_in[1].pos.xyz;
    vec3 p2 = gl_TessCoord.z * tese_in[2].pos.xyz;
    vec3 p = p0 + p1 + p2;
    vec3 normal = normalize(p - u_center);

    if (u_is_sphere) {
        p = u_center + normal;
    }

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
        p += normal * u_surface_noise_mult * ((f * 2.0) - 1.0);
    }

    tese_out.pos = vec4(p, 1.0);
    gl_Position = u_proj * u_view * tese_out.pos;
}
