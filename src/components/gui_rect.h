typedef struct GUIRect {
} GUIRect;

GUIRect _GUI_RECTS_ARENA[1 << 14];
size_t _N_GUI_RECTS = 0;


GUIRect* gui_rect_create() {
    GUIRect* gui_rect = &_GUI_RECTS_ARENA[_N_GUI_RECTS++];

    return gui_rect;
}

