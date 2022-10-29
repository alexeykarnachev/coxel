uniform int text_id;

struct FontGlyph {
    float x;
    float y;
    float width;
    float height;
}

struct TextGlyph {
    int font_glyph_id;
    float x;
    float y;
};

struct Text {
    TextGlyph glyphs[MAX_N_TEXT_GLYPHS];
}

struct Font {
    FontGlyph glyphs[MAX_N_FONT_GLYPHS]; 
}

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
    vec4 tex_pos;
} vs_out;


void main() {
    Text text = texts[text_id];
    // TODO: calculate real text_glyph_id based on vertex id from a triangle strip.
    int text_glyph_id = 0;
    TextGlyph text_glyph = text.glyphs[text_glyph_id];
    FontGlyph font_flyph = font_glyphs[text_glyph.font_glyph_id];
}


