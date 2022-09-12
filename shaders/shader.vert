#version 460 core

out vec4 v_ndc;

uniform mat4 u_view;
uniform mat4 u_proj;

void main() {
    vec4 ndc;
    float z = -0.5;
    if (gl_VertexID == 0) {
        ndc = vec4(-0.9, -0.9, z, 1);
    } else if (gl_VertexID == 1) {
        ndc = vec4(-0.9, 0.9, z, 1);
    } else if (gl_VertexID == 2) {
        ndc = vec4(0.9, 0.9, z, 1);
    } else if (gl_VertexID == 3) {
        ndc = vec4(0.9, -0.9, z, 1);
    }

    v_ndc = u_proj * u_view * ndc;
    gl_Position = v_ndc;
}
