uniform int entity_id;

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} vertex_data;

out float entity_id_color;


void main() {
    entity_id_color = (entity_id + 1) / 255.0;
}
