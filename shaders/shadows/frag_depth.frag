#version 460 core

in VertexData {
    vec4 pos;
} fs_in;

uniform vec3 u_light_pos;
uniform float u_far;

void main() {
    gl_FragDepth = length(fs_in.pos.xyz - u_light_pos) / u_far;
}
