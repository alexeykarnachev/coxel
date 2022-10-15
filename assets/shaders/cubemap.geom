#version 460 core

layout (triangles) in;
in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} gs_in[];

uniform mat4 view_proj_mats[6];

layout (triangle_strip, max_vertices=18) out;
out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} gs_out;

void main() {
    for(int face = 0; face < 6; ++face) {
        for(int i = 0; i < 3; ++i) {
            gl_Layer = face;
            gs_out.model_pos = gs_in[i].model_pos;
            gs_out.world_pos = gs_in[i].world_pos;
            gs_out.proj_pos = gs_in[i].proj_pos;
            gl_Position = view_proj_mats[face] * gs_out.world_pos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 
