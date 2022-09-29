#version 460 core

layout (triangles) in;
in VertexData {
    vec4 pos;
} gs_in[];

uniform mat4 u_shadow_transforms[6];

layout (triangle_strip, max_vertices=18) out;
out VertexData {
    vec4 pos;
} gs_out;

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face;
        for(int i = 0; i < 3; ++i) {
            // gs_out.pos = gl_in[i].gl_Position;
            gs_out.pos = gs_in[i].pos;
            gl_Position = u_shadow_transforms[face] * gs_out.pos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 
