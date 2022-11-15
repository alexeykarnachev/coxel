uniform int entity_id;

out float entity_id_color;


void main() {
    entity_id_color = (entity_id + 1) / 255.0;
}
