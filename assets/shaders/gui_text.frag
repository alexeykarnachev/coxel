in VertexData {
    vec2 tex_pos;
    flat uint char_id;
} fs_in;

uniform sampler2D font_tex;

layout(location=1) out uint gui_tex;

void main() {
    int byte = int(texture(font_tex, vec2(100, 100)).r * 255);
    int bit = ((byte >> int(fs_in.tex_pos.x * 8)) & 1);
    if (bit == 1) {
        gui_tex = 2;
    } else {
        discard;
    }
}



