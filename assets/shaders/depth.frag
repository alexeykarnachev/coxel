#version 460 core

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} fs_in;

uniform vec3 origin_world_pos;
uniform float max_dist;

void main() {
    float dist = length(fs_in.world_pos.xyz - origin_world_pos); 
    gl_FragDepth = dist / max_dist;
}

