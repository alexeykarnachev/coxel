in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec3 world_norm;
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

struct Material {
    vec4 diffuse_color;
    vec4 ambient_color;
    vec4 specular_color;
    vec4 constant_color;
    float shininess;
};

uniform Material material;
uniform sampler2D sprite_texture;

out vec4 frag_color;


void main() {
    vec4 color = texture(sprite_texture, fs_in.tex_pos);
    // TODO: Do a proper alpha-blending for sprites.
    if (color.a < 0.01) {
        discard;
    }
    frag_color = color + material.constant_color;
}
