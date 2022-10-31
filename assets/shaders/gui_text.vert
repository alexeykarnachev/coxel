#define FONT_ASPECT 0.6153846153846154

uniform int gui_text_id;
uniform int screen_height;

struct GUIText {
    int char_inds[GUI_TEXT_MAX_N_CHARS];
    int n_chars;
    int height;
    float x;
    float y;
};

layout (std140, binding=GUI_TEXT_BINDING_IDX) uniform GUITexts {
    GUIText gui_texts[MAX_N_GUI_TEXTS];
    int n_gui_texts;
};

out VertexData {
    vec2 tex_pos;
    flat int char_id;
} vs_out;


void main() {
    int id = gl_VertexID;

    GUIText text = gui_texts[gui_text_id];
    float char_height = float(text.height) / float(screen_height);
    float char_width = char_height * FONT_ASPECT;

    int char_id = id / 6;
    int id_mod_6 = id % 6;
    int id_mod_2 = id % 2;
    vec2 tex_pos = vec2(float(id_mod_6 == 0 || id_mod_6 == 4 || id_mod_6 == 5), float(id_mod_2 == 1));

    float x = text.x + char_width * char_id + char_width * tex_pos.x + 0.0025 * char_id;
    float y = text.y + char_height * tex_pos.y;
    x = x * 2.0 - 1.0;
    y = y * 2.0 - 1.0;

    vs_out.tex_pos = vec2(1.0 - tex_pos.x, tex_pos.y);
    vs_out.char_id = text.char_inds[char_id];
    gl_Position = vec4(x, y, 0, 1);
}

