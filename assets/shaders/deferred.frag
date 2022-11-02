in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} vertex_data;

in PixelData {
    flat int mesh_id;
} pixel_data;

out float frag_color;


void main() {
    frag_color = 1.0 / (pixel_data.mesh_id + 1);
}
