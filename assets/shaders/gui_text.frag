in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

layout(location=GUI_FONT_TEXTURE_LOCATION_IDX) uniform sampler2D font_tex;

out vec4 frag_color;

void main() {
    vec4 color = texture(font_tex, fs_in.tex_pos);
    // if (color.a > 0.001) {
    //     frag_color = vec4(0.0);
    // } else {
    //     discard;
    // }
    frag_color = vec4(color.a);
}



