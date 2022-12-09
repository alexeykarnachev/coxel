typedef struct GUIRect {
    size_t width;
    size_t height;
    Vec4 color;
} GUIRect;

GUIRect _GUI_RECTS_ARENA[1 << 14];
size_t _N_GUI_RECTS = 0;


GUIRect* gui_rect_create(size_t width, size_t height, Vec4 color) {
    GUIRect* gui_rect = &_GUI_RECTS_ARENA[_N_GUI_RECTS++];

    gui_rect->width = width;
    gui_rect->height = height;
    gui_rect->color = color;

    return gui_rect;
}

