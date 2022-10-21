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

layout (triangle_strip, max_vertices=18) out;
out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec3 view_world_pos;
} gs_out;

void main() {
    int layer = gl_InvocationID;
    if (layer < min(n_views, MAX_N_VIEWS)) {
        for(int face = 0; face < 6; ++face) {
            int global_face = face + 6 * layer;

            for(int i = 0; i < 3; ++i) {
                gl_Layer = global_face;
                gl_Position = view_proj_mats[global_face] * gs_in[i].world_pos;
                gs_out.model_pos = gs_in[i].model_pos;
                gs_out.world_pos = gs_in[i].world_pos;
                gs_out.proj_pos = gs_in[i].proj_pos;
                gs_out.view_world_pos = world_pos[layer];
                EmitVertex();
            }    
            EndPrimitive();
        }
    }
} 
