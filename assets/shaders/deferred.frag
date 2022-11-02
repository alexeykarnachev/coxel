uniform int mesh_id;

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} vertex_data;

out float frag_color;


void main() {
    frag_color = mesh_id / 255.0;
}
