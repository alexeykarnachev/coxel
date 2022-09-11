#version 460 core

layout(quads, equal_spacing, ccw) in;
in vec4 c_ndc[];

void main(void) {
    float x = mix(c_ndc[0].x, c_ndc[2].x, gl_TessCoord.x);
    float y = mix(c_ndc[0].y, c_ndc[1].y, gl_TessCoord.y);
    float z = mix(c_ndc[0].z, c_ndc[2].z, gl_TessCoord.z);
    float w = mix(c_ndc[0].w, c_ndc[2].w, gl_TessCoord.z);

    gl_Position = vec4(x, y, z, w);
}
