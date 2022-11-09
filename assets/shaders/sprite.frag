in VertexData {
    vec4 proj_pos;
    vec2 tex_pos;
} vertex_data;

layout(location=SPRITE_TEXTURE_LOCATION_IDX) uniform sampler2D sprite_texture;

out vec4 frag_color;


void main() {
    vec4 color = texture(sprite_texture, vertex_data.tex_pos);
    // TODO: Do a proper alpha-blending for sprites.
    if (color.a < 0.01) {
        discard;
    }
    frag_color = color;
}
