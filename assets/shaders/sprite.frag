in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec3 world_norm;
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

uniform int entity_id;
uniform sampler2D sprite_texture;

layout(location=0) out vec3 world_pos_tex;
layout(location=1) out vec3 world_norm_tex;
layout(location=2) out vec3 diffuse_tex;
layout(location=4) out uint entity_id_tex;


// TODO: This shader could be combined with gbuffer.frag, e.g
// use the same render logic for meshes and for sprites
void main() {
    vec4 color = texture(sprite_texture, fs_in.tex_pos);
    // TODO: Do a proper alpha-blending for sprites.
    if (color.a < 0.01) {
        discard;
    }
    world_pos_tex = fs_in.world_pos.xyz;
    world_norm_tex = vec3(0.0);
    diffuse_tex = color.rgb;
    entity_id_tex = uint(entity_id + 1);
}
