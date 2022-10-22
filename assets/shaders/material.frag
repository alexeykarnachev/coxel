in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} fs_in;

uniform vec3 eye_world_pos;

layout (std140, binding=MATERIAL_BINDING_IDX) uniform Material {
    vec4 diffuse_color;
    vec4 ambient_color;
    vec4 specular_color;
    float shininess;
};

// Point light:
uniform int n_point_lights;
uniform vec3 point_light_world_pos[MAX_N_POINT_LIGHTS];
uniform vec3 point_light_color[MAX_N_POINT_LIGHTS];
uniform float point_light_energy[MAX_N_POINT_LIGHTS];

// Shadow:
uniform samplerCubeArrayShadow cube_shadow_tex;
uniform float shadow_far_plane;
uniform int shadow_max_n_samples;
uniform int shadow_min_n_samples;
uniform float shadow_disk_radius;
uniform float shadow_bias_min;
uniform float shadow_bias_max;

out vec4 frag_color;

vec2 poisson_disk64(int idx);

float get_point_shadow(vec3 normal) {
    float shadow = 0.0;
    
    for(int i_light = 0; i_light < min(n_point_lights, MAX_N_POINT_SHADOWS); ++i_light) {
        vec3 light_to_frag = fs_in.world_pos.xyz - point_light_world_pos[i_light];
        float light_to_frag_length = length(light_to_frag);
        light_to_frag = normalize(light_to_frag);

        float bias = max(shadow_bias_max * (1.0 - dot(normal, -light_to_frag)), shadow_bias_min);
        float curr_depth = light_to_frag_length / shadow_far_plane - bias;

        float curr_shadow = 0;
        int n_samples = 0;
        for(int i_sample = 0; i_sample < shadow_max_n_samples; ++i_sample) {
            vec2 rnd_vec = poisson_disk64(i_sample) * shadow_disk_radius;
            curr_shadow += texture(
                cube_shadow_tex,
                vec4(light_to_frag + vec3(rnd_vec, 0.0), i_light),
                curr_depth
            );
            n_samples += 1;

            if (
                (n_samples == shadow_min_n_samples)
                && (abs(curr_shadow) < 0.001 || abs(curr_shadow) - 1.0 < 0.001)
            ) {
                break;
            }
        }
        shadow += curr_shadow / n_samples;
    }
    return shadow / n_point_lights;
}

void main() {
    vec3 world_pos = fs_in.world_pos.xyz;
    vec3 normal = normalize(cross(dFdx(world_pos), dFdy(world_pos)));
    vec3 view_dir = normalize(world_pos - eye_world_pos);

    // Ambient:
    vec3 ambient = 0.005 * ambient_color.rgb;

    // Shadows:
    float shadow = get_point_shadow(normal);

    // Diffuse:
    vec3 diffuse = vec3(0);
    for (int i = 0; i < n_point_lights; ++i) {
        float point_light_dist = pow(length(world_pos - point_light_world_pos[i]), 2.0);
        vec3 point_light_dir = normalize(world_pos - point_light_world_pos[i]);
        float diffuse_weight = max(dot(-point_light_dir, normal), 0.0);
        diffuse += diffuse_weight * point_light_energy[i] * point_light_color[i] / point_light_dist;
    }

    // Specular:
    vec3 specular = vec3(0);
    for (int i = 0; i < n_point_lights; ++i) {
        float point_light_dist = pow(length(world_pos - point_light_world_pos[i]), 2.0);
        vec3 point_light_dir = normalize(world_pos - point_light_world_pos[i]);
        vec3 halfway_dir = normalize(-point_light_dir - view_dir);
        float specular_weight = pow(max(dot(normal, halfway_dir), 0.0), shininess);
        specular += specular_weight * specular_color.rgb * point_light_energy[i] / point_light_dist;
    }

    // Combined:
    vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuse_color.rgb;
    frag_color = vec4(color, 1.0);
}
