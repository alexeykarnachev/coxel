#version 460 core

layout(vertices = 3) out;

in vec3 vs_out_pos[];

out vec3 cs_out_pos[];

void main(void) {
    float level = 24.0;
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
    gl_TessLevelOuter[3] = level;
    gl_TessLevelInner[0] = level;
    gl_TessLevelInner[1] = level;

    cs_out_pos[gl_InvocationID] = vs_out_pos[gl_InvocationID];
}
