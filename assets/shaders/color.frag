in vec2 tex_pos;

struct Camera {
    vec4 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

struct PointLight {
    float linear;
    float quadratic;
    vec3 color;
    vec4 world_pos;
};

uniform Camera camera;
uniform sampler2D world_pos_tex;
uniform sampler2D world_norm_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;
uniform usampler2D outline_tex;
uniform usampler2D gui_rect_tex;
uniform sampler2D gui_text_tex;

uniform PointLight point_lights[MAX_N_POINT_LIGHTS_TO_RENDER];
uniform int n_point_lights;

out vec4 frag_color;

const ivec2 OUTLINE_OFFSETS[4] = ivec2[4](
    ivec2(0, 2),
    ivec2(2, 0),
    ivec2(0, -2),
    ivec2(-2, 0)
);

void main() {
    vec3 world_norm = texture(world_norm_tex, tex_pos).xyz;
    float outline = texture(outline_tex, tex_pos).r;
    uvec4 outline_neighbors = textureGatherOffsets(
        outline_tex, tex_pos, OUTLINE_OFFSETS
    );
    uint outline_neighbors_sum = \
        outline_neighbors.x + \
        outline_neighbors.y + \
        outline_neighbors.z + \
        outline_neighbors.w;

    if (
        outline_neighbors_sum != 0
        && outline_neighbors_sum != 4
        && outline == 0
    ) {
        frag_color = vec4(1.0, 1.0, 0.0, 1.0);
    } else if (length(world_norm) == 0) {
        frag_color = vec4(texture(diffuse_tex, tex_pos).rgb, 1.0);
    } else {
        vec3 camera_world_pos = camera.world_pos.xyz;
        vec3 world_pos = texture(world_pos_tex, tex_pos).xyz;
        vec3 view_dir = normalize(world_pos - camera_world_pos);
        vec3 diffuse_color = texture(diffuse_tex, tex_pos).rgb;
        float specular_color = texture(specular_tex, tex_pos).r;

        vec3 combined = vec3(0.0);
        for (int i = 0; i < n_point_lights; ++i) {
            PointLight light = point_lights[i];

            float light_brightness = max(max(light.color.r, light.color.g), light.color.b);
            float light_radius = (-light.linear + sqrt(light.linear * light.linear - 4 * light.quadratic * (1.0 - (256.0f / 5.0f) * light_brightness))) / (2.0f * light.quadratic);
            float dist_to_light = length(light.world_pos.xyz - world_pos);
            if (dist_to_light < light_radius) {
                vec3 light_dir = normalize(light.world_pos.xyz - world_pos);
                vec3 halfway_dir = normalize(light_dir - view_dir);
                float diffuse_weight = max(dot(light_dir, world_norm), 0.0);
                float specular_weight = pow(max(dot(world_norm, halfway_dir), 0.0), 16.0);

                float attenuation = 1.0 / (
                    1.0 + \
                    light.linear * dist_to_light + \
                    light.quadratic * dist_to_light * dist_to_light
                );
                vec3 diffuse = diffuse_weight * diffuse_color * light.color * attenuation;
                vec3 specular = specular_weight * specular_color * light.color * attenuation;
                combined += diffuse + specular;
            }
        }

        frag_color = vec4(combined, 1.0);
    }

    float gui_rect = texture(gui_rect_tex, tex_pos).r / 255.0;
    float gui_text = texture(gui_text_tex, tex_pos).r;
    frag_color = gui_rect * vec4(0.0) + (1.0 - gui_rect) * frag_color;
    frag_color = gui_text * vec4(1.0) + (1.0 - gui_text) * frag_color;
}
