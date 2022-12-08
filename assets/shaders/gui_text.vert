uniform mat4 world_mat;
uniform int font_height;
uniform int viewport_width;
uniform int viewport_height;
uniform uint char_inds[GUI_TEXT_MAX_N_CHARS];

out vec2 tex_pos;


void main() {
    int id = gl_VertexID;

    float h = float(font_height);
    float w = h * GUI_FONT_ASPECT;
    h /= viewport_height;
    w /= viewport_width;

    int char_id = id / 6;
    int id_mod_6 = id % 6;
    int id_mod_2 = id % 2;
    tex_pos = vec2(float(id_mod_6 == 0 || id_mod_6 == 4 || id_mod_6 == 5), float(id_mod_2 == 1));

    float x = world_mat[3][0] / float(viewport_width);
    float y = world_mat[3][1] / float(viewport_height);
    float x_ = x + w * char_id + w * tex_pos.x;
    float y_ = y + h * tex_pos.y;
    x_ = x_ * 2.0 - 1.0;
    y_ = y_ * 2.0 - 1.0;

    tex_pos.x = GUI_FONT_GLYPH_WIDTH * (char_inds[char_id] + tex_pos.x - 32) / GUI_FONT_TEXTURE_WIDTH;
    tex_pos.y = 1 - tex_pos.y;
    gl_Position = vec4(x_, y_, 0.0, 1);
}

