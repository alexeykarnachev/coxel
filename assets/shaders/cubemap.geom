layout (triangles, invocations=MAX_N_POINT_SHADOW_CASTERS) in;
in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} gs_in[];

struct PointShadowCaster {
    float near_plane;
    float far_plane;
    float min_n_samples;
    float max_n_samples;
    float disk_radius;
    float bias_min;
    float bias_max;
    vec4 world_pos;
    mat4 view_proj_mats[6];
};

layout (std140, binding=POINT_SHADOW_CASTERS_BINDING_IDX) uniform PointShadowCasters {
    PointShadowCaster point_shadow_casters[MAX_N_POINT_SHADOW_CASTERS];
    int n_point_shadow_casters;
};

layout (triangle_strip, max_vertices=18) out;
out VertexData {
    float depth;
} gs_out;

void main() {
    int caster_idx = gl_InvocationID;
    if (caster_idx < n_point_shadow_casters) {
        PointShadowCaster caster = point_shadow_casters[caster_idx];
        for(int face = 0; face < 6; ++face) {
            gl_Layer = face + 6 * caster_idx;

            for(int i = 0; i < 3; ++i) {
                gl_Position = caster.view_proj_mats[face] * gs_in[i].world_pos;
                gs_out.depth = length(gs_in[i].world_pos.xyz - caster.world_pos.xyz) / caster.far_plane;
                EmitVertex();
            }    

            EndPrimitive();
        }
    }
} 

