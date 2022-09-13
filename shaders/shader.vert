#version 460 core

out vec4 v_ndc;

void main() {
    float z = -0.5;
    if (gl_VertexID == 0) {
        v_ndc = vec4(-0.9, -0.9, z, 1);
    } else if (gl_VertexID == 1) {
        v_ndc = vec4(-0.9, 0.9, z, 1);
    } else if (gl_VertexID == 2) {
        v_ndc = vec4(0.9, 0.9, z, 1);
    } else if (gl_VertexID == 3) {
        v_ndc = vec4(0.9, -0.9, z, 1);
    }
}
