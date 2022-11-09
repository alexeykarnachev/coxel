uniform vec4 gui_text; // x, y, font_width, font_height
uniform uint char_inds[GUI_TEXT_MAX_N_CHARS];

out VertexData {
    vec2 tex_pos;
    flat uint char_id;
} vs_out;


void main() {
    int id = gl_VertexID;

    float x = gui_text.x;
    float y = gui_text.y;
    float font_width = gui_text.z;
    float font_height = gui_text.w;
    float font_aspect = font_width / font_height;

    int char_id = id / 6;
    int id_mod_6 = id % 6;
    int id_mod_2 = id % 2;
    vec2 tex_pos = vec2(float(id_mod_6 == 0 || id_mod_6 == 4 || id_mod_6 == 5), float(id_mod_2 == 1));

    float x_ = x + font_width * char_id + font_width * tex_pos.x + GUI_FONT_TRACKING * char_id;
    float y_ = y + font_height * tex_pos.y;
    x_ = x_ * 2.0 - 1.0;
    y_ = y_ * 2.0 - 1.0;

    vs_out.tex_pos = vec2(1.0 - tex_pos.x, tex_pos.y);
    vs_out.char_id = char_inds[char_id];
    gl_Position = vec4(x_, y_, -0.5, 1);
}

