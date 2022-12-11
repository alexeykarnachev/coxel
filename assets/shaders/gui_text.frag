in vec2 tex_pos;

uniform sampler2D font_tex;
uniform vec3 color;

layout(location=2) out vec4 gui_text_tex;

void main() {
    float alpha = texture(font_tex, tex_pos).r;
    gui_text_tex = vec4(color, alpha);
}



