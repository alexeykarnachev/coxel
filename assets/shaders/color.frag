in vec2 tex_pos;

struct Camera {
    vec4 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

struct PointLight {
    vec4 world_pos;
    vec4 color;
    float energy;
};

uniform Camera camera;
uniform sampler2D world_pos_tex;
uniform sampler2D world_norm_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

uniform PointLight point_lights[MAX_N_POINT_LIGHTS_TO_RENDER];
uniform int n_point_lights;

out vec4 frag_color;


void main() {
    vec3 camera_world_pos = camera.world_pos.xyz;
    vec3 world_pos = texture(world_pos_tex, tex_pos).xyz;
    vec3 world_norm = texture(world_norm_tex, tex_pos).xyz;
    vec3 view_dir = normalize(world_pos - camera_world_pos);
    vec3 diffuse_color = texture(diffuse_tex, tex_pos).rgb;
    vec3 specular_color = texture(specular_tex, tex_pos).rgb;

    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);
    for (int i = 0; i < n_point_lights; ++i) {
        vec3 point_light_world_pos = point_lights[i].world_pos.xyz;
        vec3 point_light_color = point_lights[i].color.rgb;
        float point_light_energy = point_lights[i].energy;

        float point_light_dist = pow(length(world_pos - point_light_world_pos), 2.0);
        vec3 point_light_dir = normalize(world_pos - point_light_world_pos);
        vec3 halfway_dir = normalize(-point_light_dir - view_dir);
        float diffuse_weight = max(dot(-point_light_dir, world_norm), 0.0);
        float specular_weight = pow(max(dot(world_norm, halfway_dir), 0.0), 64.0);

        diffuse += diffuse_weight * point_light_energy * point_light_color / point_light_dist;
        specular += specular_weight * specular_color * point_light_energy / point_light_dist;
    }

    vec3 combined = (0.3 + diffuse + specular) * diffuse_color;
    frag_color = vec4(combined, 1.0);
}
