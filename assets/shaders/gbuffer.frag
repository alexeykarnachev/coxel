in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec3 world_norm;
    vec4 proj_pos;
    vec2 tex_pos;
} fs_in;

struct Material {
    vec3 diffuse_color;
    vec3 specular_color;
};

uniform int entity_id;
uniform Material material;

layout(location=0) out vec3 world_pos_tex;
layout(location=1) out vec3 world_norm_tex;
layout(location=2) out vec3 diffuse_tex;
layout(location=3) out vec3 specular_tex;
layout(location=4) out float entity_id_tex;


void main() {
    world_pos_tex = fs_in.world_pos.xyz;
    world_norm_tex = vec3(0.8, 0.3, 0.2);
    diffuse_tex = material.diffuse_color;
    specular_tex = material.specular_color;
    entity_id_tex = (entity_id + 1) / 255.0;
}
