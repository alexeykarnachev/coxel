#version 460 core

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec3 view_world_pos;
} fs_in;

uniform float max_dist;

void main() {
    // float dist = length(fs_in.world_pos.xyz - fs_in.view_world_pos); 
    // float dist = length(fs_in.world_pos.xyz - vec3(0.0, 8.0, -20.0)); 
    // gl_FragDepth = dist / max_dist;

    float dist = length(fs_in.world_pos.xyz - fs_in.view_world_pos); 
    gl_FragDepth = dist / max_dist;
}

