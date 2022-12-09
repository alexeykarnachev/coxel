uniform vec4 color;

layout(location=1) out vec4 gui_rect_tex;

void main() {
    gui_rect_tex = color;
}
