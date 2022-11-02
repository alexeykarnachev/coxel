in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} fs_in;

uniform int camera_id;
uniform int material_id;
layout(location=POINT_SHADOW_TEXTURE_LOCATION_IDX) uniform samplerCubeArrayShadow point_shadow_tex;
layout(location=DEFERRED_TEXTURE_LOCATION_IDX) uniform sampler2D deferred_tex;

struct Camera {
    vec3 world_pos;
    mat4 view_mat;
    mat4 proj_mat;
};

struct Material {
    vec4 diffuse_color;
    vec4 ambient_color;
    vec4 specular_color;
    float shininess;
};

struct PointLight {
    vec4 world_pos;
    vec4 color;
    float energy;
};

struct PointShadowCaster {
    float near_plane;
    float far_plane;
    float min_n_samples;
    float max_n_samples;
    float disk_radius;
    float bias_min;
    float bias_max;
    vec4 world_pos;
    mat4 vew_proj_mats[6];
};

layout (std140, binding=CAMERA_BINDING_IDX) uniform Cameras {
    Camera cameras[MAX_N_CAMERAS];
    int n_cameras;
};

layout (std140, binding=MATERIAL_BINDING_IDX) uniform Materials {
    Material materials[MAX_N_MATERIALS];
    int n_materials;
};

layout (std140, binding=POINT_LIGHT_BINDING_IDX) uniform PointLights {
    PointLight point_lights[MAX_N_POINT_LIGHTS];
    int n_point_lights;
};

layout (std140, binding=POINT_SHADOW_CASTER_BINDING_IDX) uniform PointShadowCasters {
    PointShadowCaster point_shadow_casters[MAX_N_POINT_SHADOW_CASTERS];
    int n_point_shadow_casters;
};

out vec4 frag_color;

vec2 poisson_disk64(int idx);

float get_point_shadow(vec3 normal) {
    float shadow = 0.0;
    
    for(int i_caster = 0; i_caster < n_point_shadow_casters; ++i_caster) {
        PointShadowCaster caster = point_shadow_casters[i_caster];
        vec3 ray = fs_in.world_pos.xyz - caster.world_pos.xyz;
        float ray_len = length(ray);
        ray = normalize(ray);

        float bias = max(caster.bias_max * (1.0 - dot(normal, -ray)), caster.bias_min);
        float curr_depth = ray_len / caster.far_plane - bias;

        float curr_shadow = 0;
        int n_samples = 0;
        for(int i_sample = 0; i_sample < caster.max_n_samples; ++i_sample) {
            vec2 rnd_vec = poisson_disk64(i_sample) * caster.disk_radius;
            curr_shadow += texture(
                point_shadow_tex,
                vec4(ray + vec3(rnd_vec, 0.0), i_caster),
                curr_depth
            );
            n_samples += 1;

            if (
                (n_samples == caster.min_n_samples)
                && (abs(curr_shadow) < 0.001 || abs(curr_shadow) - 1.0 < 0.001)
            ) {
                break;
            }
        }
        shadow += curr_shadow / n_samples;
    }
    return shadow / n_point_shadow_casters;
}

void main() {
    Material material = materials[material_id];
    vec3 camera_world_pos = cameras[camera_id].world_pos.xyz;
    vec3 diffuse_color = material.diffuse_color.rgb;
    vec3 ambient_color = material.ambient_color.rgb;
    vec3 specular_color = material.specular_color.rgb;
    float shininess = material.shininess;

    vec3 world_pos = fs_in.world_pos.xyz;
    vec3 normal = normalize(cross(dFdx(world_pos), dFdy(world_pos)));
    vec3 view_dir = normalize(world_pos - camera_world_pos);

    // Ambient:
    vec3 ambient = 0.005 * ambient_color.rgb;

    // Shadows:
    float shadow = get_point_shadow(normal);

    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);
    for (int i = 0; i < n_point_lights; ++i) {
        vec3 point_light_world_pos = point_lights[i].world_pos.xyz;
        vec3 point_light_color = point_lights[i].color.rgb;
        float point_light_energy = point_lights[i].energy;

        float point_light_dist = pow(length(world_pos - point_light_world_pos), 2.0);
        vec3 point_light_dir = normalize(world_pos - point_light_world_pos);
        vec3 halfway_dir = normalize(-point_light_dir - view_dir);
        float diffuse_weight = max(dot(-point_light_dir, normal), 0.0);
        float specular_weight = pow(max(dot(normal, halfway_dir), 0.0), shininess);

        diffuse += diffuse_weight * point_light_energy * point_light_color / point_light_dist;
        specular += specular_weight * specular_color.rgb * point_light_energy / point_light_dist;
    }

    // Combined:
    vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuse_color.rgb;
    // frag_color = vec4(color, 1.0);

    vec2 tex_pos = (fs_in.proj_pos.xy / fs_in.proj_pos.z + 1.0) / 2.0;
    frag_color = texture(deferred_tex, tex_pos);
}
