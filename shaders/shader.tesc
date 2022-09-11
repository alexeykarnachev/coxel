#version 460 core

layout(vertices = 4) out;

in vec4 v_ndc[];

out vec4 c_ndc[];

void main(void) {
    float level = 24.0;
    gl_TessLevelOuter[0] = level;
    gl_TessLevelOuter[1] = level;
    gl_TessLevelOuter[2] = level;
    gl_TessLevelOuter[3] = level;
    gl_TessLevelInner[0] = level;
    gl_TessLevelInner[1] = level;

    c_ndc[gl_InvocationID] = v_ndc[gl_InvocationID];
}
