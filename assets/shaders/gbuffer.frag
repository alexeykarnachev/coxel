in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec3 world_norm;
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

struct Material {
    vec3 diffuse_color;
    float specular;
};

uniform int entity_id;
uniform Material material;

layout(location=0) out vec3 world_pos_tex;
layout(location=1) out vec3 world_norm_tex;
layout(location=2) out vec3 diffuse_tex;
layout(location=3) out float specular_tex;
layout(location=4) out uint entity_id_tex;


void main() {
    world_pos_tex = fs_in.world_pos.xyz;
    world_norm_tex = fs_in.world_norm.xyz;
    diffuse_tex = material.diffuse_color;
    specular_tex = material.specular;
    entity_id_tex = uint(entity_id + 1);
}
