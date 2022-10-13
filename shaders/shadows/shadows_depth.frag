#version 460 core

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec4 light_pos;
    vec2 tex_pos;
} fs_in;

uniform vec3 light_world_pos;
uniform float far;
layout(location = 0) out vec4 depth;

void main() {
    float light_dist = length(fs_in.world_pos.xyz - light_world_pos); 
    depth = vec4(light_dist / far, 0.0, 0.0, 1.0);
}

