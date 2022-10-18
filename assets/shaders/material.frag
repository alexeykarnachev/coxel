#version 460 core

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} fs_in;

uniform vec3 eye_world_pos;

// Material:
uniform vec3 diffuse_color;
uniform vec3 ambient_color;
uniform vec3 specular_color;
uniform float shininess;

// Point light:
uniform vec3 point_light_world_pos;
uniform vec3 point_light_color;
uniform float point_light_energy;

// Shadow:
uniform samplerCubeShadow cube_shadow_tex;
uniform float shadow_far_plane;
uniform int shadow_n_samples;
uniform float shadow_disk_radius;
uniform float shadow_bias;

out vec4 frag_color;


vec2 poisson_disk64(int idx);
float sum(vec3 v) {
    return v.x + v.y + v.z;
}

float get_point_shadow(vec3 light_to_frag, int n_samples, float disk_radius, float bias) {
    float curr_depth = length(light_to_frag) / shadow_far_plane - bias;
    light_to_frag = normalize(light_to_frag);

    float shadow = 0.0;
    for(int i = 0; i < n_samples; ++i) {
        vec2 rnd_vec = poisson_disk64(i) * disk_radius;
        shadow += texture(
            cube_shadow_tex,
            vec4(light_to_frag + vec3(rnd_vec, 0.0), curr_depth)
        );
    }
    shadow /= float(n_samples); 
    return shadow;
}

void main() {
    vec3 world_pos = fs_in.world_pos.xyz;
    vec3 normal = normalize(cross(dFdx(world_pos), dFdy(world_pos)));
    float point_light_dist = pow(length(world_pos - point_light_world_pos), 2.0);

    // Ambient:
    vec3 ambient = 0.005 * ambient_color;

    // Diffuse:
    vec3 point_light_dir = normalize(world_pos - point_light_world_pos);
    float diffuse_weight = max(dot(-point_light_dir, normal), 0.0);
    vec3 diffuse = diffuse_weight * point_light_energy * point_light_color / point_light_dist;

    // Specular:
    vec3 view_dir = normalize(world_pos - eye_world_pos);
    vec3 halfway_dir = normalize(-point_light_dir - view_dir);
    float specular_weight = pow(max(dot(normal, halfway_dir), 0.0), shininess);
    vec3 specular = specular_weight * specular_color * point_light_energy / point_light_dist;

    // Shadows:
    vec3 light_to_frag = fs_in.world_pos.xyz - point_light_world_pos;
    float shadow = get_point_shadow(
        light_to_frag,
        shadow_n_samples,
        shadow_disk_radius,
        shadow_bias
    );
    // Combined:
    vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuse_color;
    frag_color = vec4(color, 1.0);
}
