#version 460 core

in VertexData {
    vec4 model_pos;
} tesc_in[];

layout(location = 1) uniform float u_tess_lvl_inner = 32.0;
layout(location = 2) uniform float u_tess_lvl_outer = 32.0;
layout(vertices = 3) out;

out VertexData {
    vec4 model_pos;
} tesc_out[];


void main(void) {
    tesc_out[gl_InvocationID].model_pos = tesc_in[gl_InvocationID].model_pos;
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = u_tess_lvl_inner;
        gl_TessLevelOuter[0] = u_tess_lvl_outer;
        gl_TessLevelOuter[1] = u_tess_lvl_outer;
        gl_TessLevelOuter[2] = u_tess_lvl_outer;
    }
}
