#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_shadow_transforms[6];

out vec3 model_pos;

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face;
        for(int i = 0; i < 3; ++i) {
            frag_pos = gl_in[i].gl_Position;
            gl_Position = u_shadow_transforms[face] * frag_pos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 
