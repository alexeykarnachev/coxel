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
uniform float shadow_max_dist;
uniform samplerCubeShadow cube_shadow_tex;

out vec4 frag_color;


vec2 poisson_disk64(int idx);
float sum(vec3 v) {
    return v.x + v.y + v.z;
}

float get_point_shadow(vec3 light_to_frag, int n_samples, float disk_radius, float bias) {
    float curr_depth = length(light_to_frag) / shadow_max_dist - bias;
    light_to_frag = normalize(light_to_frag);

    // vec3 stm;
    // vec3 a = abs(light_to_frag);
    // if (a.x >= max(a.y, a.z)) {
    //     stm = light_to_frag.zyx;
    // } if (a.y >= max(a.x, a.z)) {
    //     stm = light_to_frag.xzy;
    // } else {
    //     stm = light_to_frag.xyz;
    // }
    // vec2 st = 0.5 * (stm.st / stm.z + 1);

    float shadow = 0.0;
    for(int i = 0; i < n_samples; ++i) {
        vec2 rnd_vec = poisson_disk64(i) * disk_radius;
        // vec2 p = m * (2 * (rnd_vec + st) - 1);
        // vec3 v = p.s * s_mask + p.t * t_mask + m * m_mask;

        shadow += texture(cube_shadow_tex, vec4(light_to_frag + vec3(rnd_vec, 0.0), curr_depth));
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
    float shadow = get_point_shadow(light_to_frag, 64, 0.005, 0.001);
    // Combined:
    vec3 color = (ambient +  shadow * (diffuse + specular)) * diffuse_color;
    frag_color = vec4(color, 1.0);
}
