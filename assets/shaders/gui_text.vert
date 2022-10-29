uniform int gui_text_id;

struct FontGlyph {
    float x;
    float y;
    float width;
    float height;
};

struct TextGlyph {
    int font_glyph_id;
    float x;
    float y;
};

struct Text {
    TextGlyph glyphs[MAX_N_TEXT_GLYPHS];
};

struct Font {
    FontGlyph glyphs[MAX_N_FONT_GLYPHS]; 
};

layout (std140, binding=GUI_TEXT_BINDING_IDX) uniform Texts {
    Text texts[MAX_N_GUI_TEXTS];
    int n_texts;
};

layout (std140, binding=GUI_FONT_BINDING_IDX) uniform Fonts {
    Font fonts[1];
    int n_fonts;
};

out VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec2 tex_pos;
} vs_out;


void main() {
    int id = gl_VertexID;

    Text text = texts[gui_text_id];
    Font font = fonts[0];
    // TODO: calculate real text_glyph_id based on vertex id from a triangle strip.
    int text_glyph_id = id / 6;
    TextGlyph text_glyph = text.glyphs[text_glyph_id];
    FontGlyph font_glyph = font.glyphs[text_glyph.font_glyph_id];

    id %= 6;
    float x = float(id == 2 || id == 4 || id == 5);
    float y = float(id % 2 == 1);

    vs_out.tex_pos = vec2(font_glyph.x, font_glyph.y) + vec2(x, y) * vec2(font_glyph.width, font_glyph.height);
    vec2 pos = vec2(text_glyph.x, text_glyph.y) + vec2(x, 0) * vec2(19);
    gl_Position = vec4(pos.x, pos.y, 0, 1);
}

