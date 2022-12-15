uniform vec4 color;
uniform int entity_id;

layout(location=1) out vec4 gui_rect_tex;
layout(location=3) out uint entity_id_tex;

void main() {
    gui_rect_tex = color;
    entity_id_tex = uint(entity_id + 1);
}
