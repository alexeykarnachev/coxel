in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

struct Material {
    vec3 diffuse_color;
    vec3 specular_color;
};

uniform int entity_id;
uniform Material material;

out vec3 world_pos_tex;
out vec3 diffuse_tex;
out vec3 specular_tex;
out float entity_id_tex;


void main() {
    entity_id_tex = (entity_id + 1) / 255.0;
    world_pos_tex = fs_in.world_pos.xyz;
    diffuse_tex = material.diffuse_color;
    specular_tex = material.specular_color;
}
