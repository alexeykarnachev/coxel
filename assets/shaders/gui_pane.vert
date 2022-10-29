uniform int gui_pane_id;

struct GUIPane {
    float x;
    float y;
    float width;
    float height;
};

layout (std140, binding=GUI_PANE_BINDING_IDX) uniform GUIPanes {
    GUIPane gui_panes[MAX_N_GUI_PANES];
    int n_gui_panes;
};

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} vs_out;


void main() {
    int id = gl_VertexID;
    GUIPane gui_pane = gui_panes[gui_pane_id];

    float x0 = (gui_pane.x * 2.0) - 1.0;
    float y0 = (gui_pane.y * 2.0) - 1.0;
    float x1 = x0 + gui_pane.width * 2.0;
    float y1 = y0 + gui_pane.height * 2.0;

    if (id == 0) {
        vs_out.proj_pos = vec4(x0, y0, -1, 1);
    } else if (id == 1) {
        vs_out.proj_pos = vec4(x1, y0, -1, 1);
    } else if (id == 2) {
        vs_out.proj_pos = vec4(x0, y1, -1, 1);
    } else if (id == 3) {
        vs_out.proj_pos = vec4(x1, y1, -1, 1);
    }

    gl_Position = vs_out.proj_pos;
}
