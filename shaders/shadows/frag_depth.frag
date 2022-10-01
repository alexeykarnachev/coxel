#version 460 core

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

uniform vec3 light_world_pos;
uniform float far;

void main() {
    float light_dist = length(fs_in.world_pos.xyz - light_world_pos); 
    light_dist = light_dist / far;
    gl_FragDepth = light_dist;
}

