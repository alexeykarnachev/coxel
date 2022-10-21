#version 460 core

#define MAX_N_VIEWS 4

layout (triangles, invocations=MAX_N_VIEWS) in;
in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} gs_in[];

uniform mat4 view_proj_mats[6 * MAX_N_VIEWS];
uniform vec3 world_pos[MAX_N_VIEWS];
uniform int n_views;
uniform float max_dist;

layout (triangle_strip, max_vertices=18) out;
out VertexData {
    float depth;
} gs_out;

void main() {
    int layer = gl_InvocationID;
    if (layer < min(n_views, MAX_N_VIEWS)) {
        for(int face = 0; face < 6; ++face) {
            int global_face = face + 6 * layer;

            for(int i = 0; i < 3; ++i) {
                gl_Layer = global_face;
                gl_Position = view_proj_mats[global_face] * gs_in[i].world_pos;
                gs_out.depth = length(gs_in[i].world_pos.xyz - world_pos[layer]) / max_dist;

                EmitVertex();
            }    
            EndPrimitive();
        }
    }
} 
