#define GUI_PANE_PACK_SIZE 16

typedef struct GUIPane {
    float x;
    float y;
    float width;
    float height;
} GUIPane;

GUIPane gui_pane_create(
    float x,
    float y,
    float width,
    float height
) {
    GUIPane gui_pane;
    gui_pane.x = x;
    gui_pane.y = y;
    gui_pane.width = width; 
    gui_pane.height = height;

    return gui_pane;
}

void gui_pane_pack(GUIPane* gui_pane, float dst[]) {
    dst[0] = gui_pane->x;
    dst[1] = gui_pane->y;
    dst[2] = gui_pane->width;
    dst[3] = gui_pane->height;
}
