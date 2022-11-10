in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} fs_in;

out vec4 frag_color;

void main() {
    frag_color = vec4(0.2);
}
