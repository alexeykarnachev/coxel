# version 460 core

layout(triangles, equal_spacing, ccw) in;

in VertexData {
    vec4 model_pos;
} tese_in[];

uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 light_view_mat;
uniform mat4 light_proj_mat;
uniform vec3 center_model_pos;

uniform int   surface_noise_n_levels = 6;
uniform float surface_noise_freq_mult = 2.0;
uniform float surface_noise_ampl_mult = 0.2;
uniform float surface_noise_freq_init = 2.0;
uniform float surface_noise_mult = 0.5;

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec4 light_pos;
    vec2 tex_pos;
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
    vec3 p0 = gl_TessCoord.x * tese_in[0].model_pos.xyz;
    vec3 p1 = gl_TessCoord.y * tese_in[1].model_pos.xyz;
    vec3 p2 = gl_TessCoord.z * tese_in[2].model_pos.xyz;
    vec3 p = p0 + p1 + p2;

    vec3 normal = normalize(p - center_model_pos);
    p = center_model_pos + normal;

    if (surface_noise_mult > 0 && surface_noise_n_levels > 0) {
        float f = layered_perlin_noise3d(
                p.x,
                p.y,
                p.z,
                surface_noise_n_levels,
                surface_noise_freq_mult,
                surface_noise_ampl_mult,
                surface_noise_freq_init
        );
        p += normal * surface_noise_mult * ((f * 2.0) - 1.0);
    }

    tese_out.model_pos = vec4(p, 1.0);
    tese_out.world_pos = world_mat * tese_out.model_pos;
    tese_out.proj_pos = proj_mat * view_mat * tese_out.world_pos;
    tese_out.light_pos = light_proj_mat * light_view_mat * tese_out.world_pos;
    tese_out.tex_pos = 0.5 * (tese_out.proj_pos.xy / tese_out.proj_pos.w + 1.0);
    gl_Position = tese_out.proj_pos;
}
