uniform int mesh_id;

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} vertex_data;

out float mesh_id_color;


void main() {
    mesh_id_color = (mesh_id + 1) / 255.0;
}
