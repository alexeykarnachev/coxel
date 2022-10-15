#version 460 core

in vec3 model_pos;

uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec2 tex_pos;
} vs_out;


void main() {
    vs_out.model_pos = vec4(model_pos, 1.0);
    vs_out.world_pos = world_mat * vec4(model_pos, 1.0);
    vs_out.proj_pos = proj_mat * view_mat * vs_out.world_pos;
    gl_Position = vs_out.proj_pos;
}

