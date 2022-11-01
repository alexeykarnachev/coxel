uniform vec4 gui_pane; // {x, y, width, height}

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} vs_out;


void main() {
    int id = gl_VertexID;

    float x0 = (gui_pane.x * 2.0) - 1.0;
    float y0 = (gui_pane.y * 2.0) - 1.0;
    float x1 = x0 + gui_pane.z * 2.0;
    float y1 = y0 + gui_pane.w * 2.0;

    if (id == 0) {
        vs_out.proj_pos = vec4(x0, y0, 0, 1);
    } else if (id == 1) {
        vs_out.proj_pos = vec4(x1, y0, 0, 1);
    } else if (id == 2) {
        vs_out.proj_pos = vec4(x0, y1, 0, 1);
    } else if (id == 3) {
        vs_out.proj_pos = vec4(x1, y1, 0, 1);
    }

    gl_Position = vs_out.proj_pos;
}
