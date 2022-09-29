#version 330 core
in vec4 frag_pos;

uniform vec3 u_light_pos;
uniform float u_far;

void main() {
    float light_dist = length(frag_pos.xyz - u_light_pos);
    light_dist /= u_far;
    gl_FragDepth = light_distance;
}
