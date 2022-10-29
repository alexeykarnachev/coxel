uniform int gui_text_id;
uniform int 

struct GUIText {
};

struct GUIGlyph {
};

layout (std140, binding=GUI_TEXT_BINDING_IDX) uniform GUITexts {
    GUIText gui_texts[MAX_N_GUI_TEXTS];
    int n_gui_texts;
};


