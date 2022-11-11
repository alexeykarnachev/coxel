uniform mat4 world_mat;
uniform int font_height;
uniform int viewport_width;
uniform int viewport_height;
uniform uint char_inds[GUI_TEXT_MAX_N_CHARS];

out VertexData {
    vec2 tex_pos;
    flat uint char_id;
} vs_out;


void main() {
    int id = gl_VertexID;

    float h = float(font_height) / float(viewport_height);
    float w = h * GUI_FONT_ASPECT;
    float x = world_mat[3][0] / float(viewport_width);
    float y = world_mat[3][1] / float(viewport_height);

    float font_aspect = w / h;

    int char_id = id / 6;
    int id_mod_6 = id % 6;
    int id_mod_2 = id % 2;
    vec2 tex_pos = vec2(float(id_mod_6 == 0 || id_mod_6 == 4 || id_mod_6 == 5), float(id_mod_2 == 1));

    float x_ = x + w * char_id + w * tex_pos.x + GUI_FONT_TRACKING * char_id;
    float y_ = y + h * tex_pos.y;
    x_ = x_ * 2.0 - 1.0;
    y_ = y_ * 2.0 - 1.0;

    vs_out.tex_pos = vec2(1.0 - tex_pos.x, tex_pos.y);
    vs_out.char_id = char_inds[char_id];
    gl_Position = vec4(x_, y_, -0.5, 1);
}

