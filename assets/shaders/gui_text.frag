in vec2 tex_pos;

uniform sampler2D font_tex;

layout(location=2) out float gui_text_tex;

void main() {
    float alpha = texture(font_tex, tex_pos).r;
    gui_text_tex = alpha;
}



