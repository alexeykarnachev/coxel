#version 460 core

in vec3 frag_pos;

uniform vec3 u_light_pos;
uniform float u_far;

void main() {
    gl_FragDepth = length(frag_pos - light_pos) / u_far;
}
