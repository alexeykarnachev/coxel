in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec3 world_norm;
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

out vec4 frag_color;

void main() {
    frag_color = vec4(0.2);
}
