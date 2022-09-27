#version 460 core

layout(vertices = 3) out;

in vec3 vs_pos[];

layout(location = 1) uniform float u_tess_lvl_inner;
layout(location = 2) uniform float u_tess_lvl_outer;

out vec3 cs_pos[];


void main(void) {
    cs_pos[gl_InvocationID] = vs_pos[gl_InvocationID];
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = u_tess_lvl_inner;
        gl_TessLevelOuter[0] = u_tess_lvl_outer;
        gl_TessLevelOuter[1] = u_tess_lvl_outer;
        gl_TessLevelOuter[2] = u_tess_lvl_outer;
    }
}
