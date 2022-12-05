in VertexData {
    vec2 tex_pos;
    flat uint char_id;
} fs_in;

uniform sampler1D font_tex;

layout(location=1) out uint gui_tex;

void main() {
    float tex_pos = (float(fs_in.char_id - 32) + fs_in.tex_pos.y) / 95.0; 
    int byte = int(texture(font_tex, tex_pos).r * 255);
    int bit = ((byte >> int(fs_in.tex_pos.x * 8)) & 1);
    if (bit == 1) {
        gui_tex = 2;
    } else {
        discard;
    }
}



