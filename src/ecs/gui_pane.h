typedef struct GUIPane {
} GUIPane;

GUIPane _GUI_PANES_ARENA[1 << 14];
size_t _N_GUI_PANES = 0;


GUIPane* gui_pane_create() {
    GUIPane* gui_pane = &_GUI_PANES_ARENA[_N_GUI_PANES++];

    return gui_pane;
}

